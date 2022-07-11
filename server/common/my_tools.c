// my_tools.c
#include "my_tools.h"
#include <unistd.h>
#include <fcntl.h>

#include <string.h>    /* for strlen ... */
#include <ctype.h>     /* for isspace ... */

#include <stdio.h>


//=====================================================================
// 系统相关
//=====================================================================

int my_daemon(int nochdir, int noclose, int nosetsid)
{
	pid_t pid;

	if (!nochdir && (chdir("/") != 0)) {
		return -1;
	}

	if (!noclose) {
		int fd = open("/dev/null", O_RDWR);
		if (fd < 0) {return -1;}
		dup2(fd, 0);
		dup2(fd, 1);
		dup2(fd, 2);
		close(fd);
	}

	pid = fork();

	if (pid < 0) {
		return -1;
	}

	if (pid > 0) {
		_exit(0);
	}

	if (!nosetsid && (setsid() < 0)) {
		return -1;
	}

	return 0;
}


int my_find_pids_by_name(const char *procName, int *pids, int maxcnt)
{
	if (!procName || !pids || maxcnt<0) return -1;
	
	int ret=0;
	int tmp;
	char buf[255];

	FILE *fp;
	sprintf(buf, "pidof %s", procName);
	if ((fp=popen(buf, "r"))==NULL) {
		return -1;
	}
	
	for (; maxcnt; maxcnt--, pids++) {
		if (fscanf(fp, "%d", &tmp) ==1) {
			*pids = tmp;
			ret++;
		}
		else {
			break;
		}
	}
	fclose(fp);

	return ret;
}



//=====================================================================
// 字符串处理相关
//=====================================================================

int string_to_int(const char *str, int *num, int minval, int maxval)
{
	static int sig;
	static long long ret;
	if (!str || !num || minval>maxval)
		return -1;
	while (isspace(*str))
		str++;
	sig = (*str == '-' && ++str) ? -1 : 1;
	ret = 0;
	if (!isdigit(*str) || strlen(str) > 10)
		return -1;
	for (; isdigit(*str); ++str)
		ret = ret*10 + *str - '0';
	ret *= sig;
	if (ret < minval || ret > maxval)
		return -1;
	*num = ret;
	return 0;
}
