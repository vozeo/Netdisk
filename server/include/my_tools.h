// my_tools.h
#ifndef HEHEPIG_MY_TOOLS
#define HEHEPIG_MY_TOOLS


//==============================================================
// �� Linux ϵͳ��صĺ���
//==============================================================


/*-----------------------------------------------------
 * ʹ��ǰ���̱�Ϊ�ػ�����
 *
 * nochdir�� 	��Ϊ0���򽫸ý��̹���Ŀ¼��Ϊ"/"��
 * 		���򲻸ı乤��Ŀ¼��
 *
 * noclose��	��Ϊ0�����ض���0/1/2�����"/dev/null"��
 * 		���򲻱䡣
 *
 * nosetid��	��Ϊ0�������setsid()�����»Ựʹ�����
 * 		������ÿ���̨�Ľ��̣�
 * 		���򲻵���setsid()���˳���ǰ����̨���
 * 		����Ҳ���Ž�����
 *
 * �ɹ�����0�����򷵻�-1
 * ---------------------------------------------------*/
int my_daemon(int nochdir, int noclose, int nosetsid);


/*-----------------------------------------------------
 * ̽��ĳ�������Ƿ����
 * procName��	Ҫ�ҵĽ�����
 *
 * pids��	�ҵ��Ľ��̺Ŵ�������
 * 		�����ҵ������������һ���������
 *
 * maxcnt��	�������ô�������ֹpidsԽ��
 *
 * �ɹ��򷵻��ҵ��ĸ��������򷵻�-1
 *
 * ʵ�ַ����֣�̰���㣬�õ� popen ֱ����shell��pidof
 * ��ʵpidof�ڲ�ԭ����ȥ /proc �ļ��б�������
 * ---------------------------------------------------*/
int my_find_pids_by_name(const char *procName, int *pids, int maxcnt);



//==============================================================
// ���ַ�����صĺ���
//==============================================================

/*-----------------------------------------
 * �ַ���תint���б߽�����
 * ��������scanf��������Ҫ�ģ����治����
 * �ɹ�����0�����򷵻�-1����ʱ num ������
 * ---------------------------------------*/
int string_to_int(const char *str, int *num, int minval, int maxval);


/*-----------------------------------------
 * ����ָ�� str ��ʼ��һ�����ǿո��λ��
 * �� str �� NULL �򷵻� NULL
 * ---------------------------------------*/
char *strip_space(const char *str);




//==============================================================
// �������ļ���صĺ���
//==============================================================

/*-----------------------------------------------------
 * �� infile ���ҵ���һ�з��� key=xxx �ģ�
 * ��=���濪ʼ������ʣ�µ������buf���棬�����ո�
 * � maxlen������β0��
 *
 * ���ı� infile ��״ֱ̬�� fscanf
 *
 * �ɹ����ش�����ַ�����������0����ʧ�ܷ���-1
 * ---------------------------------------------------*/
//int conf_getval(FILE *infile, const char *key, char *buf, 
//		int maxlen);



#endif