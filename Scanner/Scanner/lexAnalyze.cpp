#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexAnalyze.h"

//全局字符变量，存放最新字符
char character;

//全局字符数组，存放读入的字符序列，只存同一类型数据
char token[25];

//全局文件指针，用于数据读入、输出与出错处理
FILE *fp;
FILE *fp_out;
FILE *fp_err;

//计数变量i
unsigned int i;

//计数行数
int row_num = 1;

//关键字表，对应助记符表
char *keyWord[RESERVE_COUNT] = { "begin","end","integer","if","then",
								 "else","function","read","write" };

//读取字符函数
int lex_getchar()
{
	if ((character = getc(fp)) != EOF)
	{
		return TRUE;
	}

	return FALSE;
}


//读取非空字符串
void lex_getnbc()
{
	while (character == ' '  ||
		   character == '\t' ||
		   character == 'LF' ||
		   character == 'CR')
	{
		character = getc(fp);
	}
}


//连接字符为字符串
void lex_concat()
{
	token[i] = character;
	i++;
}


//判断字符为字母
int lex_letter()
{
	if ((character >= 'a' && character <= 'z') ||
		(character >= 'A' && character <= 'Z'))
	{
		return TRUE;
	}

	return FALSE;
}


//判断字符为常数
int lex_digit()
{
	if (character >= '0' && character <= '9')
	{
		return TRUE;
	}

	return FALSE;
}


//查关键字表
int lex_reserve()
{
	int k;
	for (k = 0; k < RESERVE_COUNT; k++)
	{
		if (strcmp(keyWord[k], token) == 0)
		{
			return (k + 1);
		}
	}

	return 0;
}


//退回字符到字节流
void lex_retract()
{
	ungetc(character, fp);
}


//返回值打印
void lex_return(char *word_ch,int type,FILE *stream)
{
	printf("%s		%d\n", word_ch, type);
	fprintf(stream, "%s		%d\n", word_ch, type);
	fflush(stream);
}


//错误处理
void lex_error(FILE *stream)
{
	fprintf(stream, "***LINE: %d  ", row_num);
	fprintf(stream, "'%c' is illegal!\n", token[0]);
	//printf("***LINE: %d  '%c' is illegal!\n", row_num, token[0]);
}


//词法分析
int lexAnalyze()
{
	memset(token, 0, sizeof(token));
	int num;

	i = 0;

	if (lex_getchar() == FALSE)
	{
		return FALSE;
	}
	lex_getnbc();

	if (character == -1)
	{
		return FALSE;
	}

	
	switch (character)
	{
	case'a':
	case'b':
	case'c':
	case'd':
	case'e':
	case'f':
	case'g':
	case'h':
	case'i':
	case'j':
	case'k':
	case'l':
	case'm':
	case'n':
	case'o':
	case'p':
	case'q':
	case'r':
	case's':
	case't':
	case'u':
	case'v':
	case'w':
	case'x':
	case'y':
	case'z':
		while (lex_letter() || lex_digit())
		{
			lex_concat();
			lex_getchar();
		}

		lex_retract();
		num = lex_reserve();

		if (num != 0)
		{
			lex_return(token, num, fp_out);
		}
		else
		{
			lex_return(token, $SYMBOL, fp_out);
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
		while (lex_digit())
		{
			lex_concat();
			lex_getchar();
		}
		
		lex_retract();

		lex_return(token, $CONSTANT, fp_out);
		break;
	case '<':
		lex_concat();
		lex_getchar();
		if (character == '=')
		{
			lex_concat();
			lex_return(token, $LE, fp_out);
		}
		else if (character == '>')
		{
			lex_concat();
			lex_return(token, $LG, fp_out);
		}
		else
		{
			lex_retract();
			lex_return(token, $L, fp_out);
		}
		break;
	case '>':
		lex_concat();
		lex_getchar();

		if (character == '=')
		{
			lex_concat();
			lex_return(token, $GE, fp_out);
		}
		else
		{
			lex_retract();
			lex_return(token, $G, fp_out);
		}
		break;
	case '=':
		lex_concat();
		lex_return(token, $E, fp_out);
		break;
	case ':':
		lex_concat();
		lex_getchar();
		if (character == '=')
		{
			lex_concat();
			lex_return(token, $ME, fp_out);
		}
		else
		{
			lex_retract();
			lex_error(fp_err);
		}
		break;
	case '-':
		lex_concat();
		lex_return(token, $SUB, fp_out);
		break;
	case '*':
		lex_concat();
		lex_return(token, $MUL, fp_out);
		break;
	case '(':
		lex_concat();
		lex_return(token, $LPAR, fp_out);
		break;
	case ')':
		lex_concat();
		lex_return(token, $RPAR, fp_out);
		break;
	case ';':
		lex_concat();
		lex_return(token, $SEG, fp_out);
		break;
	case '\n':
		token[0] = 'E';
		token[1] = 'O';
		token[2] = 'L';
		token[3] = 'N';
		row_num++;
		lex_return(token, $N, fp_out);
		break;
	default:
		lex_concat();
		lex_error(fp_err);
		break;
	}

	return TRUE;
}

int main()
{
	char *path1 = "D:\\test.pas";
	char *path2 = "D:\\test.dyd";
	char *path3 = "D:\\test.err";

	fp	   = fopen(path1, "rb");
	fp_out = fopen(path2, "wb");
	fp_err = fopen(path3, "wb");

	if (!fp)
	{
		fprintf(stderr, "cann't open this file!");
		exit(-1);
	}

	if (!fp_out)
	{
		fprintf(stderr, "cann't find this file!");
		exit(-1);
	}

	if (!fp_err)
	{
		fprintf(stderr, "cann't find this file!");
		exit(-1);
	}


	while (lexAnalyze())
	{
		;
	}

	if (feof(fp))
	{
		memset(token, 0, sizeof(token));
		token[0] = 'E';
		token[1] = 'O';
		token[2] = 'F';

		lex_return(token, $EOF, fp_out);
	}

	fclose(fp);
	fclose(fp_out);
	fclose(fp_err);

	printf("\n");
	printf("Compiling Finished!\n");

	return 0;
}


