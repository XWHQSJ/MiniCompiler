#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntaxAnalyze.h"


//主函数
int main()
{
	init();
	syntaxA();
	finals();

	return 0;
}


bool init()
{
	inFile  = fopen(path1, "rb"); //.dyd
	outFile = fopen(path2, "wb"); //.dys
	errFile = fopen(path3, "wb"); //.err
	varFile = fopen(path4, "wb"); //.var
	proFile = fopen(path5, "wb"); //.pro

	if (inFile  && 
		outFile && 
		errFile && 
		varFile &&
		proFile)
	{
		//初始化单词指针、字符指针、行号
		inputCount = 0;
		pToken = 0;
		pChar = 0;
		lineNum = 1;
		strcpy(currentPro.pname,"");
		currentPro.plev = 0;
		currentPro.varNum = 0;
		currentPro.parameter = -1;
		varCount = 0;
		proCount = 0;

		//读取输入文件内容，初始化input数组
		while (!feof(inFile))
		{
			char stringOfLine[MAX_COUNT];
			if (fgets(stringOfLine, MAX_COUNT, inFile))
			{
				char lineString[20] = "";
				strncpy(lineString, stringOfLine, 19);
				char *kindString = strrchr(lineString, ' ');
				kind[inputCount] = atoi(kindString + 1);

				char string[17] = "";
				strncpy(string, stringOfLine, 16);
				char *lastString = strrchr(string, ' ');
				strcpy(input[inputCount], lastString + 1);

				inputCount++;
			}
		}

		return true;
	}
	else
	{
		fclose(inFile);
		fclose(outFile);
		fclose(errFile);
		fclose(varFile);
		fclose(proFile);

		return false;
	}
}

bool finals()
{
	for (int i = 0; i < varCount; i++)
	{
		int vkind = var[i].vkind ? 1 : 0;
		char *vtype = (var[i].vtype == integer) ? "integer" : "";
		fprintf(varFile, "%16s %16s %d %s %d %d\n", 
				var[i].vname, var[i].vproc, vkind, vtype, var[i].vlev, var[i].vadr);
	}

	for (int i = 0; i < proCount; i++)
	{
		char *ptype = (pro[i].ptype == integer) ? "integer" : "";
		fprintf(proFile, "%16s %s %d %d %d\n",
				pro[i].pname, ptype, pro[i].plev, pro[i].fadr, pro[i].ladr);
	}

	if (fseek(inFile, 0, 0) == 0)
	{
		while (!feof(inFile))
		{
			fputc(fgetc(inFile), outFile);
		}
	}

	bool val;
	val = fclose(inFile);
	val = fclose(outFile);
	val = fclose(errFile);
	val = fclose(varFile);
	val = fclose(proFile);

	return val;
}

bool error(int errNum, const char *symbol)
{
	char *errInfo;
	switch (errNum)
	{
	case SIGN_UNDEFINED_ERR:
		fprintf(errFile, "***LINE:%d  %s符号无定义\n", lineNum, input[pToken]);
		break;
	case SIGN_REDEFINED_ERR:
		fprintf(errFile, "***LINE:%d  %s符号重定义\n", lineNum, input[pToken]);
		break;
	case SIGN_EXECUTE_ERR:
		fprintf(errFile, "***LINE:%d  %s处不能匹配执行语句\n", lineNum, input[pToken]);
		break;
	case NO_SIGN_ERR:
		fprintf(errFile, "***LINE:%d  %s处缺少%s\n", lineNum, input[pToken], symbol);
		break;
	case SIGN_RESERVE_ERR:
		errInfo = "以保留字开头";
		break;
	case NO_PARA_ERR:
		fprintf(errFile, "***LINE:%d  缺少形参%s的声明\n", lineNum, symbol);
		break;
	default:
		errInfo = "未知错误！";
	}

	return true;
}

bool nextToken()
{
	pToken++;
	pChar = 0;
	if (strcmp(input[pToken], "EOF") == 0)
	{
		return true;
	}
	if(strcmp(input[pToken],"EOLN") == 0)
	{
		pToken++;
		lineNum++;
	}

	return false;
}

bool nextChar()
{
	if (input[pToken][pChar] == '\0')
	{
		return true;
	}
	pChar++;
	return false;
}

bool isVarExisted(char* vname, char* vproc, bool vkind)
{
	for (int i = 0; i < varCount; i++)
	{
		if ((strcmp(vname, var[i].vname) == 0) && 
			(strcmp(vproc, var[i].vproc) == 0) && 
			(var[i].vkind == vkind))
			return true;
	}
	for (int i = 0; i < proCount; i++)
	{
		if (strcmp(vname, pro[i].pname) == 0)
			return true;
	}
	return false;
}

bool isProExisted(char* vname)
{
	for (int i = 0; i < varCount; i++)
	{
		if (strcmp(vname, var[i].vname) == 0)
			return true;
	}
	for (int i = 0; i < proCount; i++)
	{
		if (strcmp(vname, pro[i].pname) == 0)
			return true;
	}
	return false;
}

int getNextToken()
{
	int pNextToken = pToken + 1;
	if(strcmp(input[pNextToken], "EOLN") == 0)
	{
		pNextToken++;
	}
	return pNextToken;
}

void syntaxA()
{
	syntaxB();
}

void syntaxB()
{
	if (strcmp(input[pToken], "begin") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "begin");
		if (strcmp(input[pToken], "integer") != 0)
		{
			nextToken();
		}
	}
	syntaxC();
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if ((strcmp(input[pToken], "integer") != 0) && 
			(strcmp(input[pToken], "read") != 0)    && 
			(strcmp(input[pToken], "write") != 0)   && 
			(kind[pToken] != 10))
		{
			nextToken();
		}
	}
	syntaxM();
	if (strcmp(input[pToken], "end") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "end");
	}
}

void syntaxC()
{
	syntaxD();
	syntaxC_();
}

void syntaxC_()
{
	if (strcmp(input[pToken], ";") == 0					&& 
		strcmp(input[getNextToken()], "integer") == 0)
	{
		nextToken();
		syntaxD();
		syntaxC_();
	}
	else
	{
		if (strcmp(input[pToken], "integer") == 0)
		{
			error(NO_SIGN_ERR, ";");
			syntaxD();
			syntaxC_();
		}
	}

}

void syntaxD()
{
	if (strcmp(input[pToken + 1], "function") == 0)
	{
		syntaxJ();
	}
	else
	{
		syntaxE();
	}
}

void syntaxE()
{
	if (strcmp(input[pToken], "integer") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "integer");
		nextToken();
	}
	strcpy(currentVar.vname, input[pToken]);
	strcpy(currentVar.vproc, currentPro.pname);
	if (pToken == currentPro.parameter)
	{
		currentVar.vkind = true;
		currentPro.parameterIsDefined = true;
	}
	else
	{
		currentVar.vkind = false;
	}
	currentVar.vtype = integer;
	currentVar.vlev = currentPro.plev;
	currentVar.vadr = varCount;
	if (isVarExisted(input[pToken], currentPro.pname, currentVar.vkind))	//如果存在变量
	{
		error(SIGN_REDEFINED_ERR, NULL);
	}
	else
	{
		if (currentPro.varNum == 0)											//如果当前过程中变量数为0，则当前变量是当前过程的第一个变量
		{
			currentPro.fadr = currentVar.vadr;
		}
		currentPro.ladr = currentVar.vadr;									//过程中最后一个变量在变量表中的位置
		currentPro.varNum++;												//过程中变量数++
		var[varCount] = currentVar;											//当前变量存入var数组
		varCount++;															//变量数++
	}
	syntaxF();
}

void syntaxF()
{
	syntaxG();
}

void syntaxG()
{

	if (kind[pToken] == 10)
	{
		nextToken();
	}
}

void syntaxJ()
{
	proRecord proBackup = currentPro;										//备份当前过程，在匹配完G过程后恢复
	if (strcmp(input[pToken], "integer") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "integer");
		if (strcmp(input[pToken], "function") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "function") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "function");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	strcpy(currentPro.pname, input[pToken]);
	currentPro.ptype = integer;
	currentPro.plev++;
	currentPro.varNum = 0;
	currentPro.parameterIsDefined = false;
	if (isProExisted(input[pToken]))
	{
		error(SIGN_REDEFINED_ERR, NULL);
	}
	syntaxG();
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	
	currentPro.parameter = pToken;
	syntaxK();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if (strcmp(input[pToken], ";") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if (strcmp(input[pToken], "begin") != 0)
		{
			nextToken();
		}
	}
	syntaxL();
	currentPro = proBackup;										//匹配完G过程后恢复原过程
}

void syntaxK()
{
	syntaxF();
}

void syntaxL()
{
	if (strcmp(input[pToken], "begin") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "begin");
		if (strcmp(input[pToken], "integer") != 0)
		{
			nextToken();
		}
	}
	syntaxC();
	if (!currentPro.parameterIsDefined)
	{
		error(NO_PARA_ERR, input[currentPro.parameter]);
	}
	pro[proCount] = currentPro;									//在这里而不是在syntaxJ()函数最后把currentPro加入pro数组是因为M中可能会使用当前过程(递归)
	proCount++;
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ";");
		if ((strcmp(input[pToken], "integer") != 0) && 
			(strcmp(input[pToken], "read") != 0)    && 
			(strcmp(input[pToken], "write") != 0)   && 
			(kind[pToken] != 10))
		{
			nextToken();
		}
	}
	syntaxM();
	if (strcmp(input[pToken], "end") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "end");
		if ((strcmp(input[pToken], ";") != 0) && 
			(strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}
}

void syntaxM()
{
	syntaxN();
	syntaxM_();
}

void syntaxM_()
{
	if (strcmp(input[pToken], ";") == 0)
	{
		nextToken();
		syntaxN();
		syntaxM_();
	}
	else
	{
		if ((strcmp(input[pToken], "end") != 0) && 
			(strcmp(input[pToken], "EOF") != 0))
		{
			error(NO_SIGN_ERR, ";");
			syntaxN();
			syntaxM_();
		}
	}
}

void syntaxN()
{
	if (strcmp(input[pToken], "read") == 0)
	{
		syntaxO();
	}
	else if (strcmp(input[pToken], "write") == 0)
	{
		syntaxP();
	}
	else if (strcmp(input[pToken], "if") == 0)
	{
		syntaxW();
	}
	else if (kind[pToken] == 10)
	{
		syntaxQ();
	}
	else
	{
		error(SIGN_EXECUTE_ERR, NULL);
		nextToken();
	}
}

void syntaxO()
{
	if (strcmp(input[pToken], "read") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "read");
		if (strcmp(input[pToken], "(") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	if (!isVarExisted(input[pToken], currentPro.pname, false) && 
		!isVarExisted(input[pToken], currentPro.pname, true))
	{
		error(SIGN_UNDEFINED_ERR, NULL);
	}
	syntaxF();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], ";") != 0) && 
			(strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}
}

void syntaxP()
{
	if (strcmp(input[pToken], "write") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "write");
		if (strcmp(input[pToken], "(") != 0)
		{
			nextToken();
		}
	}
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if (kind[pToken] != 10)
		{
			nextToken();
		}
	}
	if (!isVarExisted(input[pToken], currentPro.pname, false) && 
		!isVarExisted(input[pToken], currentPro.pname, true))
	{
		error(SIGN_UNDEFINED_ERR, NULL);
	}
	syntaxF();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], ";") != 0) && 
			(strcmp(input[pToken], "end") != 0))
		{
			nextToken();
		}
	}
}

void syntaxQ()
{
	if (!isVarExisted(input[pToken], currentPro.pname, false) && 
		!isVarExisted(input[pToken], currentPro.pname, true)  && 
		!isProExisted(input[pToken]))
	{
		error(SIGN_UNDEFINED_ERR, NULL);
	}
	syntaxF();
	if (strcmp(input[pToken], ":=") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ":=");
		if ((kind[pToken] != 10) && 
			(kind[pToken] != 11))
		{
			nextToken();
		}
	}
	syntaxR();
}

void syntaxR()
{
	syntaxS();
	syntaxR_();
}

void syntaxR_()
{
	if (strcmp(input[pToken], "-") == 0)
	{
		nextToken();
		syntaxS();
		syntaxR_();
	}
	else
	{
		if ((kind[pToken] == 10) || 
			(kind[pToken] == 11))
		{
			syntaxS();
			syntaxR_();
		}
	}
}

void syntaxS()
{
	syntaxT();
	syntaxS_();
}

void syntaxS_()
{
	if (strcmp(input[pToken], "*") == 0)
	{
		nextToken();
		syntaxT();
		syntaxS_();
	}
	else
	{
		if ((kind[pToken] == 10) || 
			(kind[pToken] == 11))
		{
			syntaxT();
			syntaxS_();
		}
	}
}

void syntaxT()
{
	if (input[pToken][pChar] >= '0' && 
		input[pToken][pChar] <= '9')
	{
		syntaxU();
	}
	else if (strcmp(input[getNextToken()], "(") == 0)				//pToken+1
	{
		syntaxZ();
	}
	else
	{
		if (!isVarExisted(input[pToken], currentPro.pname, false) && 
			!isVarExisted(input[pToken], currentPro.pname, true))
		{
			error(SIGN_UNDEFINED_ERR, NULL);
		}
		syntaxF();
	}
}

void syntaxU()
{
	if (kind[pToken-1] == 11)
	{
		nextToken();
	}
}

void syntaxW()
{
	if (strcmp(input[pToken], "if") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "if");
		if ((kind[pToken] != 10) && 
			(kind[pToken] != 11))
		{
			nextToken();
		}
	}
	syntaxX();
	if (strcmp(input[pToken], "then") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "then");
		if ((strcmp(input[pToken], "integer") != 0) && 
			(strcmp(input[pToken], "read") != 0)    && 
			(strcmp(input[pToken], "write") != 0)   && 
			(kind[pToken] != 10))
		{
			nextToken();
		}
	}
	syntaxN();
	if (strcmp(input[pToken], "else") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "else");
		if ((strcmp(input[pToken], "integer") != 0) && 
			(strcmp(input[pToken], "read") != 0)    && 
			(strcmp(input[pToken], "write") != 0)   && 
			(kind[pToken] != 10))
		{
			nextToken();
		}
	}
	syntaxN();
}

void syntaxX()
{
	syntaxR();
	syntaxY();
	syntaxR();
}

void syntaxY()
{
	if (strcmp(input[pToken], "<") == 0  || 
		strcmp(input[pToken], "<=") == 0 || 
		strcmp(input[pToken], ">") == 0  || 
		strcmp(input[pToken], ">=") == 0 || 
		strcmp(input[pToken], "=") == 0  || 
		strcmp(input[pToken], "<>") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "关系运算符");
		if ((kind[pToken] != 10) && 
			(kind[pToken] != 11))
		{
			nextToken();
		}
	}
}

void syntaxZ()
{
	if (!isProExisted(input[pToken]))
	{
		error(SIGN_UNDEFINED_ERR, NULL);
	}
	syntaxG();
	if (strcmp(input[pToken], "(") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, "(");
		if ((kind[pToken] != 10) && 
			(kind[pToken] != 11))
		{
			nextToken();
		}
	}
	syntaxR();
	if (strcmp(input[pToken], ")") == 0)
	{
		nextToken();
	}
	else
	{
		error(NO_SIGN_ERR, ")");
		if ((strcmp(input[pToken], "-") != 0) && 
			(strcmp(input[pToken], "*") != 0) && 
			(strcmp(input[pToken], ";") != 0) && 
			(strcmp(input[pToken], "end")))
		{
			nextToken();
		}
	}
}


