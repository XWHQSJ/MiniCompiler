#ifndef LEXANALYZE_H
#define LEXANALYZE_H

#include <stdio.h>
#include <stdlib.h>

//单词符号的编码
#define $SYMBOL		1
#define $CONSTANT	2
#define $INT		3
#define $IF			4
#define $ELSE		5
#define $WHILE		6
#define $FOR		7
#define $READ		8
#define $WRITE		9
#define $ADD		10
#define $SUB		11
#define $MUL		12
#define $DIV		13
#define $L			14
#define $LE			15
#define $G			16
#define $GE			17
#define $NE			18
#define $E			19
#define $ASSIGN		20
#define $LPAR		21
#define $RPAR		22
#define $COM		23
#define $SEM		24

//空白字符
#define NUL		    0		//空白符   '\0'
#define BS			8		//退格     '\b'
#define HT			9		//横向制表 '\t'
#define LF			10		//换行     '\n'
#define CR			13		//回车     '\r'


const static int RESERVED_COUNT = 7;	//关键字数
const static int LETTER_COUNT = 26;     //字母数
const static int DIGIT_COUNT = 10;      //数字数


//二元数结构体
typedef struct Word_Struct
{
	int number;				//字符编码
	int position;			//字符位置
}WORDS;


//关键字表
char *keyWords[RESERVED_COUNT] = { "int","if","else","while","for","read","write" };

//字母表
char *letters[LETTER_COUNT] = { 'a','b','c','d','e','f','g','h','i','j','k','l',
									'm','n','o','p','q','r','s','t','u','v','w',
									'x','y','z','A','B','C','D','E','F','G','H',
									'I','J','K','L','M','N','O','P','Q','R','S',
									'U','V','W','X','Y','Z' };

//数字表
char *digits[DIGIT_COUNT] = { '0','1','2','3','4','5','6','7','8','9};


//判断是否为空
bool is_space(char );

//读入一个字符函数
void lex_getchar();

//读入非空白字符函数
void lex_getnbc();

//连接字符串函数
void lex_concat();

//判断字母函数
bool is_letter();

//判断数字函数
bool is_digit();

//回退字符函数
void lex_retract();

//处理保留字函数
int lex_reserve();

//处理标识符函数
int lex_symbol();

//常数存入常数表函数
int lex_constant();

//返回二元数函数
WORDS word_return(int ,int );

//出错处理函数
void lex_error();

//词法分析器函数
WORDS lexAnalyze(WORDS words);

#endif // !LEXANALYZE_H