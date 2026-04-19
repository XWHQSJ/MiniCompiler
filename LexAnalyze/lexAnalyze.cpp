#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include "lexAnalyze.h"

using namespace std;

char lex_ch;								//ȫ���ַ��������ڴ�����´�����ַ�
std::string lex_token;							//�ַ����飬���ڴ���Ѷ�����ַ�����
std::map<std::string, int> symbols;		        //���ű�
std::map<std::string, int> constants;		    //������
int lex_i = 0;									//�ַ�����

FILE *filePtr;



//�ж��ַ��Ƿ�Ϊ�հ��ַ�
bool is_space()
{
	if (lex_ch == NUL || lex_ch == BS || lex_ch == HT || lex_ch == LF || lex_ch == CR)
	{
		return true;
	}

	return false;
}

//����һ���ַ�����
void lex_getchar()
{
	lex_ch = getc(filePtr);

	if (lex_ch == EOF)
	{
		std::cout << "File is end.\n" << std::endl;
		exit(1);
	}
}

//����ǿհ��ַ�����
void lex_getnbc()
{
	while (is_space())
	{
		lex_ch = getc(filePtr);
	}
}

//�����ַ�������
void lex_concat()
{
	lex_token.push_back(lex_ch);
}

//�ж���ĸ����
bool is_letter()
{
	if ((lex_ch >= 'a' && lex_ch <= 'z') ||
		(lex_ch >= 'A' && lex_ch <= 'Z'))
	{
		return true;
	}

	return false;
}

//�ж����ֺ���
bool is_digit()
{
	if (lex_ch >= '0' && lex_ch <= '9')
	{
		return true;
	}

	return false;
}

//�����ַ�����
void lex_retract()
{
	if (lex_ch != -1)
	{
		fseek(filePtr, -1, SEEK_CUR);
		lex_ch = ' ';
	}
}

//���������ֺ���
int lex_reserve()
{
	int num = 0;
	while (num < RESERVED_COUNT)
	{
		if (keyWords[num] == lex_token)
		{
			return (num + 3);
		}

		num++;
	}

	return 0;
}

//������ʶ������
int lex_symbol()
{
	//WORDS words;

	std::map<std::string, int>::iterator sym_iter;
	sym_iter = symbols.find(lex_token);

	if (sym_iter != symbols.end())
	{
		return sym_iter->second;
	}
	else
	{
		symbols.insert(std::pair<std::string, int>(lex_token, symbols.size() + 1));

		return (symbols.size() + 1);
	}
}

//�������볣��������
int lex_constant()
{
	std::map<std::string, int>::iterator const_iter;
	const_iter = constants.find(lex_token);

	if (const_iter != constants.end())
	{
		return const_iter->second;
	}
	else
	{
		constants.insert(std::pair<std::string, int>(lex_token, constants.size() + 1));

		return (constants.size() + 1);
	}
}

//���ض�Ԫ������
WORDS word_return(int lex_number, int lex_value)
{
	WORDS words;
	words.chCode = lex_number;
	words.chPost = lex_value;

	return words;
}

//������������
void lex_error(int errCode)
{
	switch (errCode)
	{
	case 0:				//character is valid
		std::cout << "Your input character is invalid.\n" << std::endl;
		break;
	case 1:				//file is empty
		fseek(filePtr, 0, SEEK_END);
		if (ftell(filePtr) == 0)
		{
			std::cout << "File is empty.\n" << std::endl;;
			exit(1);
		}
		else
		{
			fseek(filePtr, 0, SEEK_CUR);
		}
		break;
	case 2:				//load file error
		std::cout << "File can't open.\n" << std::endl;
		break;
	case 3:				//close file error
		std::cout << "Syntax: lexAnalyze [file]\n" << std::endl;
		break;
	}
}

//�ʷ�����������
WORDS lex_analyze()
{
	WORDS words;
	lex_token = " ";
	int lex_number = 0;
	int lex_value = 0;

	//�����һ���ַ�
	lex_getchar();
	//����ǿհ��ַ�
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
			words = word_return($SYMBOL, lex_value);
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
			words = word_return($L, 0);  //����'<'����
		}
		break;
	case '>':
		lex_getchar();
		if (lex_ch == '=')
		{
			words = word_return($GE, 0);  //����'>='����
		}
		else
		{
			lex_retract();
			words = word_return($G, 0);   //����'>'����
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
			lex_error(0);
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
		lex_error(0);
	}

	return words;
}

int main(int argc,char *argv[])
{
	WORDS words;

	const char* file = argv[1];
	filePtr = fopen(file, "rb");

	//���������ļ�
	switch (argc)
	{
	case 1:              
		filePtr = stdin;
		break;
	case 2:
		if (filePtr == NULL)
		{
			lex_error(2);
			exit(1);
		}
		break;
	case 3:
		lex_error(3);
		exit(0);
		break;
	}

	while ((lex_ch = fgetc(filePtr)) != EOF)
	{
		words = lex_analyze();
		std::cout << words.chCode << " : " << words.chPost << std::endl;
	}


	fclose(filePtr);

	return 0;
}