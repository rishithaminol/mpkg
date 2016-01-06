#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    char file_type[4];

    switch (tflag) {
    case FTW_D:
        strcpy(file_type, "d");
        break;
    case FTW_DNR:
        strcpy(file_type, "dnr");
        break;
    case FTW_DP:
        strcpy(file_type, "dp");
        break;
    case FTW_F:
        strcpy(file_type, "f");
        break;
    case FTW_NS:
        strcpy(file_type, "ns");
        break;
    case FTW_SL:
        strcpy(file_type, "sl");
        break;
    case FTW_SLN:
        strcpy(file_type, "sln");
        break;
    default:
        strcpy(file_type, "???");
        break;
    }

    printf("%-3s %2d %7jd   %-40s %d %s\n", file_type,
        ftwbuf->level, (intmax_t) sb->st_size,
        fpath, ftwbuf->base, fpath + ftwbuf->base);

    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;

   if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;

   if (nftw((argc < 2) ? "." : argv[1], display_info, 30, 0)
            == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}