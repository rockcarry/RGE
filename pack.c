#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "pack.h"

/* 函数实现 */
int travel(char *path, int depth, PFN_TRAVEL_FUNC func, void *param)
{
    DIR   *dir;
    struct dirent *ent;
    struct stat sb;
    char   pathbase[MAX_PATH];
    char   pathnew [MAX_PATH];
    int    retv = FALSE;;
    int    len;

    /* 去掉可能存在的反斜杠 */
    strncpy(pathbase, path, sizeof(pathbase));
    len = strlen(pathbase);
    if (len > 0 && pathbase[len - 1] == '/') {
        pathbase[len - 1] = '\0';
    }

    dir = opendir(pathbase);
    if (!dir) return FALSE;

    /* start */
    if (depth == 0) {
        if (!func(NULL, pathbase, depth, TDT_START, param)) {
            goto done;
        }
    }

    while (1)
    {
        ent = readdir(dir);
        if (!ent) break;

        if (  strcmp(ent->d_name, "." ) == 0
           || strcmp(ent->d_name, "..") == 0 )
        {
            continue;
        }

        snprintf(pathnew, sizeof(pathnew), "%s/%s", pathbase, ent->d_name);
        if (stat(pathnew, &sb) >= 0)
        {
            if (!S_ISDIR(sb.st_mode)) {
                /* file */
                if (!func(ent->d_name, pathnew, depth, TDT_FILE, param)) {
                    goto done;
                }
            }
            else {
                /* enter dir */
                if (!func(ent->d_name, pathnew, depth, TDT_DIR_ENTER, param)) {
                    goto done;
                }

                depth++; // depth inc
                travel(pathnew, depth, func, param);
                depth--; // depth dec

                /* exit dir */
                if (!func(ent->d_name, pathnew, depth, TDT_DIR_EXIT, param)) {
                    goto done;
                }
            }
        }
    }

    /* success */
    retv = TRUE;

done:
    /* done */
    if (depth == 0) {
        func(NULL, pathbase, depth, TDT_DONE, param);
    }

    closedir(dir);
    return retv;
}

typedef struct {
    char *file;
    FILE *fp;
} PACK_CONTEXT;

static BOOL dopackfunc1(char *name, char *path, int depth, int type, void *param)
{
    PACK_CONTEXT *ctxt = (PACK_CONTEXT*)param;
    struct stat sb;
    long   len;
    char  *startdir;

    switch (type) {
    case TDT_START:
        ctxt->fp = fopen(ctxt->file, "wb");
        if (!ctxt->fp) return FALSE;
        fputc('s', ctxt->fp); /* start flag */
        fputc(0, ctxt->fp); /* reserved 4 bytes */
        fputc(0, ctxt->fp);
        fputc(0, ctxt->fp);
        fputc(0, ctxt->fp);

        /* + enter start dir + */
        startdir = path + strlen(path);
        while (startdir > path) {
            if (*--startdir == '/') {
                startdir++;
                break;
            }
        }
        fputc('+', ctxt->fp);
        len = strlen(startdir);
        fputc((BYTE)len, ctxt->fp);
        fwrite(startdir, len, 1, ctxt->fp);
        /* - enter start dir - */
        break;

    case TDT_FILE:
        /* file flag */
        fputc('f', ctxt->fp);

        /* name len & name */
        len = strlen(name);
        fputc((BYTE)len, ctxt->fp);
        fwrite(name, len, 1, ctxt->fp);

        /* file size */
        stat(path, &sb);
        len = sb.st_size;
        fwrite(&len, sizeof(len), 1, ctxt->fp);
        break;

    case TDT_DIR_ENTER:
        fputc('+', ctxt->fp);
        /* name len & name */
        len = strlen(name);
        fputc((BYTE)len, ctxt->fp);
        fwrite(name, len, 1, ctxt->fp);
        break;

    case TDT_DIR_EXIT:
        fputc('-', ctxt->fp);
        break;

    case TDT_DONE:
        fputc('-', ctxt->fp); /* exit start dir */
        fputc('d', ctxt->fp); /* done flag */
        fclose(ctxt->fp);
        break;
    }
    return TRUE;
}

static BOOL dopackfunc2(char *name, char *path, int depth, int type, void *param)
{
    PACK_CONTEXT *ctxt = (PACK_CONTEXT*)param;
    FILE         *fp   = NULL;
    BYTE          byte = 0;
    struct stat   sb;
    long          len;

    switch (type) {
    case TDT_START:
        stat(ctxt->file, &sb);
        len = sb.st_size;
        ctxt->fp = fopen(ctxt->file, "r+");
        if (!ctxt->fp) return FALSE;
        fseek(ctxt->fp, 1, SEEK_SET);
        fwrite(&len, sizeof(len), 1, ctxt->fp);
        fclose(ctxt->fp);

        ctxt->fp = fopen(ctxt->file, "ab");
        if (!ctxt->fp) return FALSE;
        break;

    case TDT_FILE:
        fp = fopen(path, "rb");
        if (!fp) return FALSE;
        while (1) {
            byte = fgetc(fp);
            if (feof(fp)) break;
            fputc(byte, ctxt->fp);
        }
        fclose(fp);
        break;

    case TDT_DONE:
        fclose(ctxt->fp);
        break;
    }
    return TRUE;
}

BOOL dirdopack(char *dir, char *file)
{
    PACK_CONTEXT context;
    context.file = file;
    context.fp   = NULL;

    if (!travel(dir, 0, dopackfunc1, &context)) return FALSE;
    if (!travel(dir, 0, dopackfunc2, &context)) return FALSE;

    return TRUE;
}

BOOL dirunpack(char *dir, char *file)
{
    FILE *fpin    = NULL;
    FILE *fpout   = NULL;
    int   type    = 0;
    long  length  = 0;
    long  poslast = 0;
    long  poscur  = 0;
    char  fbase[MAX_PATH];
    char  fpath[MAX_PATH];
    char  fname[MAX_PATH];
    long  i;

    fpin = fopen(file, "rb");
    if (!fpin) return FALSE;

    while (!feof(fpin))
    {
        type = fgetc(fpin);
        switch (type)
        {
        case 's':
            /* get data poscur */
            fread(&poscur, sizeof(poscur), 1, fpin);

            /* make base dir */
            strncpy(fbase, dir, sizeof(fbase));
            length = strlen(fbase);
            if (length > 0 && fbase[length - 1] == '/') {
                fbase[length - 1] = '\0';
            }
            mkdir(fbase);
            break;

        case 'd':
            goto done;
            break;

        case '+':
            length = fgetc(fpin);
            fread(fname, length, 1, fpin);
            fname[length] = '\0';
            strncat(fbase, "/",   sizeof(fbase));
            strncat(fbase, fname, sizeof(fbase));
            mkdir(fbase);
            break;

        case '-':
            length = strlen(fbase);
            while (length > 0 && fbase[--length] != '/');
            fbase[length] = '\0';
            break;

        case 'f':
            length = fgetc(fpin);
            fread(fname, length, 1, fpin);
            fname[length] = '\0';
            fread(&length, sizeof(length), 1, fpin);

            poslast = ftell(fpin);
            fseek(fpin, poscur , SEEK_SET);
            snprintf(fpath, sizeof(fpath), "%s/%s", fbase, fname);
            fpout = fopen(fpath, "wb");
            if (fpout) {
                for (i=0; i<length; i++) {
                    fputc(fgetc(fpin), fpout);
                }
                poscur += length;
                fclose(fpout);
            }
            fseek(fpin, poslast, SEEK_SET);
            break;
        }
    }

done:
    fclose(fpin);
    return TRUE;
}

#else
#include "pack.h"
#include "log.h"

static BOOL print_dir_info(char *name, char *path, int depth, int type, void *param)
{
    int i;
    for (i=0; i<depth; i++) {
        log_printf("    ");
    }

    switch (type) {
    case TDT_START:
        log_printf("++ start ++\n", name);
        break;
    case TDT_DONE:
        log_printf("-- done --\n", name);
        break;
    case TDT_FILE:
        log_printf("%s\n", name);
        break;
    case TDT_DIR_ENTER:
        log_printf("%s {\n", name);
        break;
    case TDT_DIR_EXIT:
        log_printf("}\n");
        break;
    }
    return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    log_init("log.txt");

    if (*lpszCmdLine == '\0') lpszCmdLine = ".";
    travel(lpszCmdLine, 0, print_dir_info, NULL);

    dirdopack("../src"   , "../pack.dat");
    dirunpack("../unpack", "../pack.dat");

    log_done();
    return 0;
}
#endif

