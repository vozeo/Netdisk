// my_getopt.h

#ifndef HEHEPIG_MY_GETOPT
#define HEHEPIG_MY_GETOPT


#define myopt_no_argument 			(0)
#define myopt_required_argument 	(1)
#define myopt_optional_argument 	(2)

#define myopt_argtype_charstar		(0)
#define myopt_argtype_int			(1)


typedef struct my_option {
	const char *name;	// 选项名，--name
	int need_opt;		// 是否是必须有的选项，1是必须的，0是不必须的
	int has_arg;		// 需要后接参数的情况，0是不能跟参数，1是必须跟参数，2是可以跟也可以不跟
	int *flag;			// 若有该选项，则*flag=1，否则*flag=0，若flag是NULL则不赋值
	int type_arg;		// 要将答案转成的类型，0是原char*，1是int
	void *target;		// 要将参数存到的位置，NULL则不存，若没有参数或参数不合法，则不操作target
	int minval;
	int maxval;
} my_option;

// 正常返回 0，否则返回 -1。
// opts 最后一项的 name 设为 NULL，其他项 name 不能为 NULL
// type_arg 若是0，是将 *target = argv[xxx]，不是 strcpy
int my_getopt(int argc, char **argv, my_option *opts);


#endif
