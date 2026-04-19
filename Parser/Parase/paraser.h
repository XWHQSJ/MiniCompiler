#pragma once
#ifndef _INCLUDE_PARASER_H_
#define _INCLUDE_PARASER_H_

#include <stdio.h>

#define MAX_COUNT 24


//记录变量信息结构体
typedef struct VARNODE  
{
	char *vname;
	char *vproc;
	char *vtype;
	int   vkind;
	int   vlev;
	int   vadr;
	struct VARNODE *next;
}*vNode;


//记录过程信息结构体
typedef struct PRONODE 
{
	char *pname;
	char *ptype;
	int plevNum;
	int fadr;
	int ladr;
	struct PRONODE *next;
}*pNode;


//堆栈保存节点信息
typedef struct STACK
{
	pNode top;
	int size;
}*Stack;


//保存函数结构体
typedef struct FUNCNODE  
{
	char *fname;
	struct FUNCNODE *next;
}funcNode;


//全局变量声明
pNode pnode;
Stack pstack;
funcNode *fhead, *f1, *f2;
vNode vhead, var1, var2;

//各输出输入文件指针
FILE *fp_in, *fp_var, *fp_pro, *fp_out, *fp_err;

//各输出输入文件路径
char *path1 = "D:\\test.dyd";
char *path2 = "D:\\test.var";
char *path3 = "D:\\test.pro";
char *path4 = "D:\\test.dys";
char *path5 = "D:\\test.err";

char *proc_name;			 //变量过程名
char *name;				     //变量名
int symbols;				 //单词符号种别
int lineNum = 1;			 //行号
int plevNum = 1;			 //过程层次
char types[MAX_COUNT];	     //类型
int varlocation = 1;		 //变量位置


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
J ：函数说明			J->integer functions G(K); L
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


//与产生式对应的函数声明
//
//语法分析入口  
void program();

//字符指针后移
void advance();				 		 

//语法分析分程序
void partProgram();			

//说明语句表
void declareTable();		 

//消除左递归后新的说明语句表
void MdeclareTable();

//说明语句
void declareStatement();

//消除左递归后后的说明语句
void MdeclareStatement();

//执行语句表
void executeTable();	

//消除左递归后的执行语句
void MexecuteTable();

//执行语句
void executeStatement();

//将变量/形参写入变量链表
void varTable(int kind);

//变量
void variates();				 

//标识符
void identifier();			 

//函数说明
void functionDelcare();		 

//参数
void parameters();			 

//函数体
void functions();		  

//读语句
void readStatement();		 

//写语句
void writeStatement();		 

//赋值语句
void assignStatement();		 

//算术表达式
void algorithm();	

//消除左递归后的算术表达式
void Malgorithm();

//项
void item();	

//消除左递归后的项
void Mitem();

//因子
void factor();				 

//函数调用
void callFunction();		

//条件语句
void conditionStatement();	 

//条件表达式
void expression();			 

//关系运算符
void relationOperator();	


#endif // !_INCLUDE_PARASER_H_