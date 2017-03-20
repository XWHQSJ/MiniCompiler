#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <string>

#include "lexAnalyze.h"

char lex_ch = ' ';								//全局字符变量用于存放最新存入的字符
std::string lex_token = NULL;					//字符数组，用于存放已读入的字符序列
std::vector<std::string> symbols = NULL;		//符号表
std::vector<int> constants = NULL;				//常数表
int lex_i = 0;									//字符计数


FILE *filePtr;



//判断字符是否为空白字符
bool is_space()
{
	if(lex_ch == NUL || lex_ch == BS || lex_ch == HT || lex_ch == LF)
	{
		return true;
	}

	return false;
}

//读入一个字符函数
void lex_getchar()
{
	lex_ch = getc(filePtr);

	if (lex_ch == EOF)
	{
		printf("File is end.\n");
		exit(1);
	}
}

//读入非空白字符函数
void lex_getnbc()
{
	while (is_space())
	{
		lex_ch = getc(filePtr);
	}
}

//连接字符串函数
void lex_concat()
{
	lex_token.push_back(lex_i);
	lex_i++;
}

//判断字母函数
bool is_letter()
{
	int num = 0;
	while (num < LETTER_COUNT)
	{
		if (lex_ch == letters[num])
		{
			return true;
		}

		num++;
	}

	return false;
}

//判断数字函数
bool is_digit()
{
	int num = 0;
	while (num < DIGIT_COUNT)
	{
		if (lex_ch == digits[num])
		{
			return true;
		}

		num++;
	}

	return false;
}

//回退字符函数
void lex_retract()
{
	if (lex_ch != -1)
	{
		fseek(filePtr, -1, SEEK_CUR);
		lex_ch = ' ';
	}
}

//处理保留字函数
int lex_reserve()
{
	int num = 0;
	while (num < RESERVED_COUNT)
	{
		if (strcmp(keyWords[num], lex_token) == 0)
		{
			return (num + 3);
		}

		num++;
	}

	return 0;
}

//处理标识符函数
int lex_symbol()
{

}

//常数存入常数表函数
int lex_constant()
{

}

//返回二元数函数
WORDS word_return(int lex_number,int lex_value)
{
	WORDS words;
	words.number = lex_number;
	words.position = lex_value;

	return words;
}

//出错处理函数
void lex_error(int count)
{
	switch (count)
	{
	case 0:				//character is valid
		printf("Your input character is invalid.\n");
		break;
	case 1:				//file is empty
		fseek(filePtr, 0, SEEK_END);
		if (ftell(filePtr) == 0)
		{
			printf("File is empty.\n");
			exit(1);
		}
		else
		{
			fseek(filePtr, 0, SEEK_CUR);
		}
		break;
	case 2:				//load file error
		printf("File can't open.\n");
		break;
	case 3:				//close file error
		printf("File can't close.\n");
		break;
	}
}

//词法分析器函数
WORDS lexAnalyze(WORDS words)
{
	lex_token = NULL;
	int lex_number = 0;
	int lex_value = 0;
	lex_i = 0;

	//读入第一个字符
	lex_getchar();
	//读入非空白字符
	lex_getnbc();

	switch (lex_ch)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
		while (is_letter() || is_digit())
		{
			lex_concat();
			lex_getchar();
		}
		
		lex_retract();

		lex_number = lex_reserve();

		if (lex_number != 0)
		{
			words = word_return(lex_number, 0);
		}
		else
		{
			lex_value = lex_symbol();
			words = word_return($SYMBOL,lex_value);
		}
		break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		while (is_digit())
		{
			lex_concat();
			lex_getchar();
		}

		lex_retract();
		lex_value = lex_constant();

		words = word_return($CONSTANT, lex_value);
		break;

	case '<':
		lex_getchar();
		if (lex_ch == '=')
		{
			words = word_return($LE, 0);
		}
		else
		{
			lex_retract();
			words = word_return($L, 0);  //返回'<'符号
		}
		break;
	case '>':
		lex_getchar();
		if (lex_ch == '=')
		{
			words = word_return($GE, 0);
		}
		else
		{
			lex_retract();
			words = word_return($GE, 0);  //返回'>'符号
		}
		else
		{
			lex_retract();
			words = word_return($G, 0);   //返回'>='符号
		}
		break;
	case '=':
		lex_getchar();
		if (lex_ch == '=')
		{
			words = word_return($E, 0);
		}
		else
		{
			lex_retract();
			words = word_return($ASSIGN, 0);
		}
		break;
	case '!':
		lex_getchar();
		if (lex_ch == '=')
		{
			words = word_return($NE, 0);
		}
		else
		{
			lex_error();
		}
		break;
	case '+':
		words = word_return($ADD, 0);
		break;
	case '-':
		words = word_return($SUB, 0);
		break;
	case '*':
		words = word_return($MUL, 0);
		break;
	case '/':
		words = word_return($DIV, 0);
		break;
	case '(':
		words = word_return($LPAR, 0);
		break;
	case ')':
		words = word_return($RPAR, 0);
		break;
	case ',':
		words = word_return($COM, 0);
		break;
	case ';':
		words = word_return($SEM, 0);
		break;
	default:
		lex_error();
	}

}

int main(int agrc, char *argv[])
{


	return 0;
}