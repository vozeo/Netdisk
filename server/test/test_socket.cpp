#include <stdio.h>
#include <unistd.h>         // for sleep ...
#include <sys/socket.h>     // for ...
#include <fcntl.h>          // for fcntl
#include <arpa/inet.h>      // for htonl
#include <errno.h>          // for errno
#include <netdb.h>          // for getnameinfo
#include <string.h>         // for strcpy ...
#include <sys/time.h>       // for gettimeofday
#include <signal.h>

#include <vector>
#include <string>

#include "my_tools.h"
#include "my_getopt.h"
#include "hehepig_socket.h"
#include "hehepig_ufile.h"
#include "hehepig_user.h"
#include "hehepig_file.h"
#include "hehepig_parameter.h"



static int DAEMON;
int PORT;
const char *LOGNAME;
int get_main_opt(int argc, char **argv)
{
    struct my_option opts[] = {
        {"port",        1,  1,  NULL,   1,  &PORT       ,   0, 65535    },
        {"logname",     0,  1,  NULL,   0,  &LOGNAME                    },
        {"daemon",      0,  0,  &DAEMON                                 },
        {"sql_user",    0,  1,  NULL,   0,  &SQL_USER                   },
        {"sql_passwd",  0,  1,  NULL,   0,  &SQL_PASSWD                 },
        {"sql_database",0,  1,  NULL,   0,  &SQL_PASSWD                 },
        {NULL}
    };
    return my_getopt(argc, argv, opts);
}
void print_cur_time()
{
    struct tm nowtime;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &nowtime);
    printf("@%04d-%02d-%02d %02d:%02d:%02d ",
        nowtime.tm_year+1900,
        nowtime.tm_mon+1,
        nowtime.tm_mday,
        nowtime.tm_hour,
        nowtime.tm_min,
        nowtime.tm_sec
        //(int)(tv.tv_usec/1000)
    );
}



std::vector<std::string> fun_analyze(const char* msg)
{
    std::vector<std::string> ret;
    std::string tmpstr;
    const char *p=msg;
    for (; *p!='\n'; p++) {
        if (*p=='\t') {
            ret.push_back(tmpstr);
            tmpstr="";
            continue;
        }
        tmpstr+=*p;
    }
    if (tmpstr!="") ret.push_back(tmpstr);
    return ret;
}


void fun_response(int task_id, int accept_sockfd, std::vector<std::string> req)
{
        int tmp;
        char buf[10086];
        hehepig_sqlres res;
        if (!req.size()) {return;}
        std::string username, tmpstr;
        
    while (1) {

        // ls dir username
        if (req[0]=="10" && req.size()==3 && ufile_ls(res, atoi(req[1].c_str()))==0) {

            username = req[2];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "[]");
                break;
            }

            std::string tmpstr;
            std::string tmp_title[4]={"ufile_id", "is_dir", "name", "upload_time"};
            tmpstr+="[";

            int rows=res.size();
            for (int i=0; i<rows; i++) {
                if (i>0) tmpstr+=",";
                tmpstr+="{";
                tmpstr+="\"size\":";
                tmpstr+=file_get_size(res[i][4]);
                for (int j=0; j<4; j++) {
                    tmpstr+=",";
                    tmpstr+="\"";
                    tmpstr+=tmp_title[j];
                    tmpstr+="\":";
                    if (j>=2) tmpstr+="\"";
                    tmpstr+=res[i][j];
                    if (j>=2) tmpstr+="\"";
                }
                tmpstr+="}";
            }
            tmpstr+="]";

            sprintf(buf, tmpstr.c_str());
        }

        // mkdir curdir_id name username
        else if (req[0]=="11" && req.size()==4) {

            username = req[3];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            int ret, new_id=-1;
            ret = ufile_mkdir(
                atoi(req[1].c_str()),   // curdir_id
                req[2],                 // name
                &new_id                 // ufile_id for newdir
            );

            sprintf(buf, "{\"ret_code\":%d,\"ufile_id\":%d}", ret, new_id);
        }

        // touch curdir_id name md5 username
        else if (req[0]=="12" && req.size()==5) {

            username = req[4];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, "{\"ret_code\":%d}", ufile_touch(
                atoi(req[1].c_str()),   // curdir_id
                req[2],                 // fname
                req[3]                  // md5
            ));
        }

        // cp dir_from dir_to username
        else if (req[0]=="13" && req.size()==4) {

            username = req[3];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[2].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, "{\"ret_code\":%d}", ufile_cp(
                atoi(req[1].c_str()),   // dir_from
                atoi(req[2].c_str())    // dir_to
            ));
        }

        // mv dir_from dir_to username
        else if (req[0]=="14" && req.size()==4) {

            username = req[3];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[2].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, "{\"ret_code\":%d}", ufile_mv(
                atoi(req[1].c_str()),   // dir_from
                atoi(req[2].c_str())    // dir_to
            ));
        }

        // rm dir username
        else if (req[0]=="15" && req.size()==3) {

            username = req[2];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, "{\"ret_code\":%d}", ufile_rm(
                atoi(req[1].c_str())    // dir
            ));
        }

        // rename dir new_name username
        else if (req[0]=="16" && req.size()==4) {

            username = req[3];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, "{\"ret_code\":%d}", ufile_safe_rename(
                atoi(req[1].c_str()),   // dir
                req[2]                  // new_name
            ));
        }

        // getdetail_f dir username
        else if (req[0]=="17" && req.size()==3) {

            username = req[2];
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, ufile_get_detail(atoi(req[1].c_str())).c_str());
        }

        // getdetail_d dir usrename
        else if (req[0]=="18" && req.size()==3) {

            username = req[2];
            fflush(stdout);
            if (ufile_dirname(tmpstr, ufile_get_rootid(atoi(req[1].c_str())))!=0 || tmpstr!=username) {
                sprintf(buf, "err");
                break;
            }

            sprintf(buf, ufile_get_dfsdetail(atoi(req[1].c_str())).c_str());
        }

        else if (req[0]=="21" && req.size()==3) {
            const std::string &md5=req[1];
            long long size=strtoll(req[2].c_str(), NULL, 0);
            int file_status;

            if ((file_status=file_check_md5(md5))>=0 || (
                    !file_init_file(md5, size) &&
                    !file_init_slices(md5, size)
                )
            ) {
                // done
                if (file_status==1 || (file_get_one_undone_slice(md5, res)==0 && !res.size())) {
                    sprintf(buf, "{\"ret_code\":0}");
                }
                else {
                    sprintf(buf, "{\"ret_code\":1,\"slice_id\":%s,\"start\":%s,\"slice_size\":%s}",
                        res[0][0].c_str(),
                        res[0][1].c_str(),
                        res[0][2].c_str()
                    );
                }
            }
            else {
                sprintf(buf, "err");
            }
        }
        else if (req[0]=="22" && req.size()==3) {
            const std::string &md5=req[1];
            int file_status;

            file_modify_slice(req[1], atoi(req[2].c_str()), 2);

            if ((file_status=file_check_md5(md5))>=0) {
                // done
                if (file_status==1 || (file_get_one_undone_slice(md5, res)==0 && !res.size())) {
                    sprintf(buf, "{\"ret_code\":0,\"progress\":1}");
                }
                else {
                    sprintf(buf, "{\"ret_code\":1,\"progress\":%.3lf,\"slice_id\":%s,\"start\":%s,\"slice_size\":%s}",
                        file_get_progress(md5),
                        res[0][0].c_str(),
                        res[0][1].c_str(),
                        res[0][2].c_str()
                    );
                }
            }
            else {
                sprintf(buf, "err");
            }
        }

        else if (req[0]=="31" && req.size()==3) {
            sprintf(buf, "{\"rootdir\":%d}", user_register(req[1].c_str(), req[2].c_str()));
        }
        else if (req[0]=="32" && req.size()==3) {
            sprintf(buf, "{\"rootdir\":%d}", user_login(req[1].c_str(), req[2].c_str()));
        }
        else {
            sprintf(buf, "err");
        }

        break;
    }

        printf("<%d> sending...\n", task_id);
        for (int i=0; i<10086 && buf[i]; i++) {
            if ((buf[i]>=' ' && buf[i]<='~') || isspace(buf[i]))
                printf("%c", buf[i]);
            else
                printf("\'\\%d\'", (int)(buf[i]));
        }
        tmp = send(accept_sockfd, buf, strlen(buf), 0);
        printf("\n<%d> end send, return %d\n\n", task_id, tmp);
}


int main(int argc, char **argv)
{
    int listen_sockfd;
    int accept_sockfd;
    char buf[1024];
    int tmp;
    std::vector<std::string> anares;

    signal(SIGCHLD, SIG_IGN);

    if (get_main_opt(argc, argv) != 0) {
        printf("example: %s --port 20166 --logname 1.log --daemon --sql_user --sql_passwd --sql_database\n", argv[0]);
        return 0;
    }
    printf("port:           %d\n", PORT);
    printf("logname:        %s\n", LOGNAME);
    printf("daemon:         %d\n", DAEMON);
    puts("=====================");
    fflush(stdout);
    if (DAEMON) my_daemon(1, 1, 0);

    if (LOGNAME && (freopen(LOGNAME, "w", stdout) == NULL)) {
        printf("failed to open %s, err %d %s\n", LOGNAME, errno, strerror(errno));
        return -1;
    }


    listen_sockfd = open_listen_sockfd(NULL, PORT, 50);
    if (listen_sockfd < 0) {
        printf("failed to open listen_sockfd.\n");
        return -1;
    }
    print_cur_time(); printf("\n");
    printf("listening on localhost:%d at [%d]...\n", PORT, listen_sockfd);

    int task_id=0;
    while (1) {
        task_id++;
        fflush(stdout);
        //puts("\n=============================");

        //printf("start to accept...\n");
        accept_sockfd = tmpsocket_accept(listen_sockfd, buf, &tmp);
        print_cur_time(); printf("\n");
        printf("<%d> end accept, [%d], %s:%d\n", task_id, accept_sockfd, buf, tmp);

        //printf("start to recv...\n");
        tmp = recv(accept_sockfd, buf, 1024, 0);
        buf[tmp>=0?tmp:0]=0;
        for (int i=0; i<tmp; i++) {
            if ((buf[i]>=' ' && buf[i]<='~') || isspace(buf[i]))
                printf("%c", buf[i]);
            else
                printf("\'\\%d\'", (int)(buf[i]));
        }
        printf("<%d> end recv, return %d\n\n", task_id, tmp);
        fflush(stdout);

        int pid=fork();
        if (pid==0) {
            fun_response(task_id, accept_sockfd, fun_analyze(buf));
            close(accept_sockfd);
            break;
        }

        close(accept_sockfd);
    }

    close(listen_sockfd);
    return 0;
}
