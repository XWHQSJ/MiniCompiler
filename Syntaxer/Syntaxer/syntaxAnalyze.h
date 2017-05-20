#pragma once
#ifndef _INCLUDE_SYNTAXANALYZE_H_
#define _INCLUDE_SYNTAXANALYZE_H_

#include <stdio.h>

#define MAX_COUNT			1024   
#define SIGN_UNDEFINED_ERR	1     //符号未定义错误
#define SIGN_REDEFINED_ERR	2	  //符号重复定义错误
#define SIGN_EXECUTE_ERR	3     //符号执行错误
#define NO_SIGN_ERR			4     //无符号错误
#define SIGN_RESERVE_ERR	5     //符号匹配错误
#define NO_PARA_ERR			6     //括号匹配错误

//支持类型集合types
typedef enum {integer}types;

//记录变量信息
typedef struct VARRECORD
{
	int vlev;
	int vadr;
	char vname[17];
	char vproc[17];
	bool vkind;
	types vtype;
}varRecord;

//记录过程信息
typedef struct PRORECORD
{
	int plev;
	int varNum;
	int ladr;
	int fadr;
	int parameter;
	char pname[17];
	bool parameterIsDefined;
	types ptype;
}proRecord;

//文法产生式
/************************************************************************
A ：程序				A->B
B ：分程序			B->begin C; M end
C ：说明语句表		C->DC'
C'：新说明语句表		C'->;DC' | ε
D ：说明语句			D->E | J
E ：变量说明			E->integer F
F ：变量				F->G
G ：标识符			G->HG'
G'：新标志符			G'->HG' | IG'|ε
H ：字母				H->a | ... | z | A | ... | Z
I ：数字				I->0 | 1 | ... | 9
J ：函数说明			J->integer function G(K); L
K ：参数				K->F
L ：函数体			L->begin C; M end
M ：执行语句表		M->NM'
M'：新执行语句表		M'->;NM' | ε
N ：执行语句			N->O | P | Q | W
O ：读语句			O->read(F)
P ：写语句			P->write(F)
Q ：赋值语句			Q->F: = R
R ：算术表达式		R->SR'
R'：新算术表达式		R'->-SR' | ε
S ：项				S->TS'
S'：新项				S'->*TS' | ε
T ：因子				T->F | U | Z
U ：常数				U->V
V ：无符号整数		V->IV'
V'：新无符号整数		V'->IV' | ε
W ：条件语句			W->if X then N else N
X ：条件表达式		X->RYR
Y ：关系运算符		Y->< | <= | > | >= |= | <>
Z ：函数调用			Z->G(R)
************************************************************************/

//函数声明
//处理产生式函数
void syntaxA();
void syntaxB();
void syntaxC();
void syntaxC_();
void syntaxD();
void syntaxE();
void syntaxF();
void syntaxG();
void syntaxJ();
void syntaxK();
void syntaxL();
void syntaxM();
void syntaxM_();
void syntaxN();
void syntaxO();
void syntaxP();
void syntaxQ();
void syntaxR();
void syntaxR_();
void syntaxS();
void syntaxS_();
void syntaxT();
void syntaxU();
void syntaxW();
void syntaxX();
void syntaxY();
void syntaxZ();

//初始化函数
bool init();

//结束处理函数
bool finals();

//错误处理函数
bool error(int, const char*);

//判断获取下一符号函数
bool nextToken();

//判断获取当前符号的下一字符函数
bool nextChar();

//判断变量存在函数
bool isVarExisted(char*, char*, bool);

//判断过程存在函数
bool isProExisted(char*);

//获得下一符号函数
int getNextToken();

//全局变量
char input[MAX_COUNT][17];		//存放输入文件的所有符号数组
int kind[MAX_COUNT];
int inputCount;					//输入符号的数量
int pToken;						//指向当前输入字符
int pChar;						//指向当前输入字符的当前字符

varRecord currentVar;			//存放当前变量的信息
proRecord currentPro;			//存放当前过程的信息
varRecord var[MAX_COUNT];		//存放变量名表项
proRecord pro[MAX_COUNT];		//存放过程名表项

int varCount;					//变量数量
int proCount;					//过程数量
int lineNum;					//存放当前行号

FILE *inFile;					//输入文件指针
FILE *outFile;					//输出文件指针
FILE *errFile;					//错误文件指针
FILE *varFile;					//变量文件指针
FILE *proFile;					//过程文件指针

char *path1 = "D:\\test.dyd";	//输入文件路径
char *path2 = "D:\\test.dys";	//输出文件路径
char *path3 = "D:\\test.err";	//错误文件路径
char *path4 = "D:\\test.var";	//变量文件路径
char *path5 = "D:\\test.pro";	//过程文件路径

#endif // !_INCLUDE_SYNTAXANALYZE_H_


