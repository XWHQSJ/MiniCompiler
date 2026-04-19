#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include "paraser.h"

int main()
{
	var1 = vhead;
	char ch;


	fp_in  = fopen(path1, "rb");	//.dyd
	fp_var = fopen(path2, "wb");	//.var
	fp_pro = fopen(path3, "wb");	//.pro
	fp_out = fopen(path4, "wb");	//.out
	fp_err = fopen(path5, "wb");	//.err


	program();


	//.pro文件和.err文件在函数中处理
	//输出.var文件
	while (var1 != NULL)
	{
		fprintf(fp_var, "vname: %s\n", var1->vname);
		fprintf(fp_var, "vproc: %s\n", var1->vproc);
		fprintf(fp_var, "vkind: %d\n", var1->vkind);
		fprintf(fp_var, "vtype: %s\n", var1->vtype);
		fprintf(fp_var, "vlev:  %d\n", var1->vlev);
		fprintf(fp_var, "vadr:  %d\n\n", var1->vadr);
		var1 = var1->next;
	}

	//输出.dys文件
	fseek(fp_in, 0, SEEK_SET);
	while ((ch = fgetc(fp_in)) != EOF)
	{
		fputc(ch, fp_out);
	}


	fclose(fp_in);
	fclose(fp_var);
	fclose(fp_pro);
	fclose(fp_out);
	fclose(fp_err);
	return 0;
}

void program()
{
	advance();
	partProgram();
}

void advance()
{
	char buffer[MAX_COUNT];
	int types;

	fscanf(fp_in, "%s%d", buffer, &types);

	if (types == 24)
	{
		lineNum++;
		fscanf(fp_in, "%s%d", buffer, &types);
	}

	name = (char *)malloc(sizeof(char) * (strlen(buffer) + 1));
	strcpy(name, buffer);
	symbols = types;
}


void partProgram()
{
	proc_name = "main";  //最外层过程名取名为main
	if (symbols == 1)	     //begin
	{
		advance();

		pnode = (struct PRONODE*)malloc(sizeof(struct PRONODE));
		pnode->pname = (char *)malloc(sizeof(char) * strlen(proc_name));
		pnode->ptype = (char *)malloc(sizeof(char) * strlen("void"));
		strcpy(pnode->pname, proc_name);
		strcpy(pnode->ptype, "void");
		pnode->plevNum = plevNum;
		pnode->fadr = varlocation;
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'begin'\n", lineNum);
	}

	declareTable();
	executeTable();
	
	if (symbols != 2)  //end
	{
		fprintf(fp_err, "***LINE:%d 缺少'end'\n", lineNum);
	}
	else
	{
		//输出.pro文件
		fprintf(fp_pro, "pname: %s\n", pnode->pname);  
		fprintf(fp_pro, "ptype: %s\n", pnode->ptype);
		fprintf(fp_pro, "plevNum: %d\n", pnode->plevNum);
		fprintf(fp_pro, "fadr: %d\n", pnode->fadr);
		fprintf(fp_pro, "ladr: %d\n\n", pnode->ladr);
	}
}

void declareTable()
{
	declareStatement();
	MdeclareTable();
}

void MdeclareTable()
{
	if (symbols == 23)
	{
		advance();
		if (symbols == 3) 
		{
			declareStatement();
			MdeclareTable();
		}
	}
	else
		fprintf(fp_err, "***LINE:%d 说明语句与执行语句之间无';'\n", lineNum);
}

void declareStatement()
{
	if (symbols == 3)  
	{
		strcpy(types, name);
		advance();
	}
	else
		fprintf(fp_err, "***LINE:%d 缺少类型\n", lineNum);

	MdeclareStatement();
}

void MdeclareStatement()
{
	if (symbols == 7) 
	{
		advance();
		plevNum++;
		pstack = (struct STACK *)malloc(sizeof(struct STACK)); 
		if (pstack != NULL)
		{
			pstack->top = NULL;
			pstack->size = 0;
		}
		pnode->next = pstack->top;
		pstack->size++;
		pstack->top = pnode;
		pnode = (struct PRONODE *)malloc(sizeof(struct PRONODE));  
		functionDelcare();
	}
	else if (symbols == 10) 
	{
		varTable(0); 
		advance();
	}
	else
		fprintf(fp_err, "***LINE:%d 不是函数说明或变量定义\n", lineNum);
}

void executeTable()
{
	executeStatement();
	MexecuteTable();
}

void MexecuteTable()
{
	if (symbols == 23)
	{
		advance();
		executeStatement();
		MexecuteTable();
	}
}

void executeStatement()
{
	if (symbols == 8)
	{
		advance();
		readStatement();
	}
	else if (symbols == 9)
	{
		advance();
		writeStatement();
	}
	else if (symbols == 10)
	{
		variates();
		assignStatement();
	}
	else if (symbols == 4)
	{
		advance();
		conditionStatement();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 不是执行语句\n", lineNum);
	}
}


void varTable(int kind)   
{
	//重定义查询
	var1 = vhead;
	while (var1 != NULL)
	{
		if (!strcmp(var1->vname, name) && 
			!strcmp(var1->vproc, proc_name) && 
			 var1->vkind == kind)
		{
			fprintf(fp_err, "***LINE:%d 变量 '%s' 重定义\n", lineNum, name);
			return;
		}
		else
		{
			var1 = var1->next;
		}
	}

	var1 = (struct VARNODE*)malloc(sizeof(struct VARNODE));
	var1->vname = (char *)malloc(sizeof(char)* (strlen(name) + 1));
	strcpy(var1->vname, name);

	var1->vproc = (char *)malloc(sizeof(char)* (strlen(proc_name) + 1));
	strcpy(var1->vproc, proc_name);

	var1->vtype = (char *)malloc(sizeof(char)* (strlen(types) + 1));
	strcpy(var1->vtype, types);

	var1->vkind = kind;
	var1->vlev = plevNum;
	var1->vadr = varlocation;
	pnode->ladr = varlocation;  

	varlocation++;
	var1->next = NULL;

	if (vhead == NULL)
	{
		vhead = var1;
		var2 = vhead;
	}
	else
	{
		var2->next = var1;
		var2 = var1;
	}
}

void variates()
{
	int flag = 0;
	if (symbols == 10)
	{
		var1 = vhead;

		while (var1 != NULL)
		{
			if ((*(var1->vname) == *name) && 
				(*(var1->vproc) == *(pnode->pname))) 
			{
				flag = 1;
			}

			var1 = var1->next;
		}

		if (!flag)
		{
			f1 = fhead;
			while (f1 != NULL)
			{
				if (!strcmp(f1->fname, name))
				{
					flag = 1;
				}

				f1 = f1->next;
			}
		}

		if (!flag)
		{
			fprintf(fp_err, "***LINE:%d 变量 %s 未定义\n", lineNum, name);
		}

		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少参数\n", lineNum);
	}
}


void identifier()
{
	if (symbols == 10)
	{
		proc_name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
		pnode->pname = (char *)malloc(sizeof(char)* strlen(proc_name));
		pnode->ptype = (char *)malloc(sizeof(char)* strlen(types));
		strcpy(proc_name, name);
		pnode->pname = proc_name;
		strcpy(pnode->ptype, types);
		pnode->plevNum = plevNum;

		f1 = (struct FUNCNODE*)malloc(sizeof(struct FUNCNODE));  
		f1->fname = (char *)malloc(sizeof(char)* strlen(pnode->pname));
		strcpy(f1->fname, pnode->pname);
		f1->next = NULL;
		if (fhead == NULL)
		{
			fhead = f1;
			f2 = fhead;
		}
		else
		{
			f2->next = f1;
			f2 = f1;
		}

		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少函数名\n", lineNum);
	}
}


void parameters()
{
	varTable(1); 
	variates();
}


void functions()
{
	if (symbols == 1)
	{
		pnode->fadr = varlocation - 1; 
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'begin'\n", lineNum);
	}

	declareTable();
	executeTable();

	if (symbols == 2)
	{
		fprintf(fp_pro, "pname: %s\n", pnode->pname);  
		fprintf(fp_pro, "ptype: %s\n", pnode->ptype);
		fprintf(fp_pro, "plevNum: %d\n", pnode->plevNum);
		fprintf(fp_pro, "fadr: %d\n", pnode->fadr);
		fprintf(fp_pro, "ladr: %d\n\n", pnode->ladr);

		pstack->size--;
		pnode = pstack->top;
		pstack->top = pnode->next;
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'end'\n", lineNum);
	}
}

void functionDelcare()
{
	identifier();  

	if (symbols == 21) 
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'('\n", lineNum);
	}
	
	parameters();
	
	if (symbols == 22)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少')'\n", lineNum);
	}

	if (symbols == 23)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少';'\n", lineNum);
	}

	functions();
}


void readStatement()
{
	if (symbols == 21)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'('\n", lineNum);
	}
	
	variates();
	
	if (symbols == 22)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少')'\n", lineNum);
	}
}

void writeStatement()
{
	if (symbols == 21)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'('\n", lineNum);
	}

	variates();
	
	if (symbols == 22)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少')'\n", lineNum);
	}
}

void assignStatement()
{
	if (symbols == 20)
	{
		advance();
		algorithm();
	}
	else
	{
		advance();
		fprintf(fp_err, "***LINE:%d 缺少赋值符号\n", lineNum);
		algorithm();
	}
}

void algorithm()
{
	item();
	Malgorithm();
}

void Malgorithm()
{
	if (symbols == 18)
	{
		advance();
		item();
		Malgorithm();
	}
}

void item()
{
	factor();
	Mitem();
}

void Mitem()
{
	if (symbols == 19)
	{
		advance();
		factor();
		Mitem();
	}
}

void factor()
{
	if (symbols == 10)
	{
		variates();

		if (symbols == 21)
		{
			callFunction();
		}
		else
		{
			return;
		}
	}
	else if (symbols == 11) 
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 不是标识符/常数\n", lineNum);
	}
}

void callFunction()
{
	if (symbols == 21)
	{
		advance();
		algorithm();

		if (symbols == 22)
		{
			advance();
		}
		else
		{
			fprintf(fp_err, "***LINE:%d 缺少')'\n", lineNum);
		}
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'('\n", lineNum);
	}
}

void conditionStatement()
{
	expression();
	
	if (symbols == 5)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'then'\n", lineNum);
	}

	executeStatement();
	
	if (symbols == 6)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少'else'\n", lineNum);
	}

	executeStatement();
}

void expression()
{
	algorithm();
	relationOperator();
	algorithm();
}

void relationOperator()
{
	if (symbols == 12 || symbols == 13 ||
		symbols == 14 || symbols == 15 ||
		symbols == 16 || symbols == 17)
	{
		advance();
	}
	else
	{
		fprintf(fp_err, "***LINE:%d 缺少关系运算符\n", lineNum);
	}
}




