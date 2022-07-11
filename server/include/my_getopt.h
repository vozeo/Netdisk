// my_getopt.h

#ifndef HEHEPIG_MY_GETOPT
#define HEHEPIG_MY_GETOPT


#define myopt_no_argument 			(0)
#define myopt_required_argument 	(1)
#define myopt_optional_argument 	(2)

#define myopt_argtype_charstar		(0)
#define myopt_argtype_int			(1)


typedef struct my_option {
	const char *name;	// ѡ������--name
	int need_opt;		// �Ƿ��Ǳ����е�ѡ�1�Ǳ���ģ�0�ǲ������
	int has_arg;		// ��Ҫ��Ӳ����������0�ǲ��ܸ�������1�Ǳ����������2�ǿ��Ը�Ҳ���Բ���
	int *flag;			// ���и�ѡ���*flag=1������*flag=0����flag��NULL�򲻸�ֵ
	int type_arg;		// Ҫ����ת�ɵ����ͣ�0��ԭchar*��1��int
	void *target;		// Ҫ�������浽��λ�ã�NULL�򲻴棬��û�в�����������Ϸ����򲻲���target
	int minval;
	int maxval;
} my_option;

// �������� 0�����򷵻� -1��
// opts ���һ��� name ��Ϊ NULL�������� name ����Ϊ NULL
// type_arg ����0���ǽ� *target = argv[xxx]������ strcpy
int my_getopt(int argc, char **argv, my_option *opts);


#endif
