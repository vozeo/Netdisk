#include <stdio.h>
#include <string.h>
#include "my_getopt.h"
#include "my_tools.h"

static int is_optname(const char *str)
{
    if (!str || strlen(str)<=2 || str[0]!='-' || str[1]!='-') return 0;
    return 1;
}

static int same_optname(const char *str, const char *name)
{
    if (!is_optname(str) || !name) return 0;
    return (strcmp(str+2, name)==0);
}

int my_getopt(int argc, char **argv, my_option *opts)
{
    if (!opts) return -1;

    int cnt_used_argv=1;    // 操作完必须是刚好等于 argc

    for (my_option *cur=opts; cur->name; cur++) {
        // 检查 name 是否存在
        int nameind=0;
        if (cur->flag) (*(cur->flag)) = 0;
        for (int i=1; i<argc; i++) {
            if (same_optname(argv[i], cur->name)) {
                if (nameind) {
                    printf("opterr: more than one option <%s>\n", cur->name);
                    return -1;
                }
                nameind = i;
                cnt_used_argv++;
            }
        }
        if (!nameind) {
            if (cur->need_opt) {
                printf("opterr: need option <%s>\n", cur->name);
                return -1;
            }
            continue;
        }
        if (cur->flag) (*(cur->flag)) = 1;

        // 检查后接的参数
        int argind=nameind+1;
        if (argind>=argc || is_optname(argv[argind])) argind--;
        else cnt_used_argv++;
        int cntarg=argind-nameind;

        switch(cur->has_arg) {
            case 0:
                if (cntarg!=0) {
                    printf("opterr: find arg <%s> in no-arg option <%s>\n", argv[argind], argv[nameind]);
                    return -1;
                }
                continue;   // 重要
                break;
            case 1:
                if (cntarg!=1) {
                    printf("opterr: no arg in require-arg option <%s>\n", argv[nameind]);
                    return -1;
                }
                break;
            default:
                printf("opterr: unknown has_arg in <%s>\n", argv[nameind]);
                return -1;
        }

        // 转换
        int tmp;
        switch(cur->type_arg) {
            case 0:
                (*((const char**)(cur->target))) = argv[argind];
                break;
            case 1:
                if (string_to_int(argv[argind], &tmp, cur->minval, cur->maxval) != 0) {
                    printf("opterr: unacceptable arg in option <%s>\n", argv[nameind]);
                    return -1;
                }
                (*((int*)(cur->target))) = tmp;
                break;
            default:
                printf("opterr: unknown type_arg in option <%s>\n", argv[nameind]);
                return -1;
        }
    }

    if (cnt_used_argv!=argc) {
        printf("opterr: found unknown option or multi-arg option\n");
        return -1;
    }

    return 0;
}