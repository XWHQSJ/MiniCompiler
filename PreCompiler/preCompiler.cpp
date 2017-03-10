#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdio.h>

using namespace std;

static const char delim = '#';
static const char delim0 = '"';
static string delim1 = "\n\t\v\r ";


void error(int i)
{
	switch(i)
	{
		case 1:
			cout << "file error" << endl;
			break;
		case 2:
			cout << "xxx error" << endl;
			break;
		case 3:
			cout << "xxx error" << endl;
			break;
		case 4:
			cout << "argc error!" << endl;
			break;
	}
}


bool isSpace(string::iterator it)
{
	string::iterator st;
	for(st = delim1.begin(); st != delim1.end(); st++)
	{
		if (*it == *st)
		{
			return true;
		}
	}

	return false;
}


string readFile(string fileName)
{
	ifstream tmpFile;
	tmpFile.open(fileName.c_str(), fstream::in);

	return string(istreambuf_iterator<char>(tmpFile), istreambuf_iterator<char>());
}


bool isInclude(string str)
{
	string strInclude("#include");

	if (str == strInclude)
	{
		return true;
	}

	return false;
}


string doInclude(string str,string::iterator st)
{
	string hName;

	if (isInclude(str))
	{
		st += 9;
		if (*st == delim0)
		{
			while (*(++st) != delim0)
			{
				hName.push_back(*st);
			}

		}	
		return hName;
	}

	return 0;
}


bool isDefine(string str)
{
	string strDefine("#define");

	if (str == strDefine)
	{
		return true;
	}

	return false;
}


string doDefine(string str, string input, string::iterator st)
{
	string str0;
	string str1;

	if (isDefine(str))
	{
		st += 8;
		while (!isSpace(st))
		{
			str0.push_back(*st);
			st++;
		}
		st++;
		while (!isSpace(st))
		{
			str1.push_back(*st);
			st++;
		}

		string::size_type tmp = input.find(str0);

		int z = (int)str1.length();
		for (int n = 0; n < z; n++)
		{
			input[tmp + n] = str1[n];
			str0[n] = str1[n];
		}
		return input;
	}
	return NULL;
}


string strTmp(string::iterator st)
{
	string str;

	while (!isSpace(st))
	{
		str.push_back(*st);
		st++;
	}
	return str;
}


int main(int argc,char** argv)
{
	string hFile;
	string mInput;
	string hInput;

	string inArcFile = argv[1];
	string outArcFile = argv[2];

	if(argc != 3)
	{
		error(4);
	}

	mInput = readFile(inArcFile);
	string::iterator it = find(mInput.begin(), mInput.end(), delim);
	string tmpStr = strTmp(it);

	cout << tmpStr << endl;
	hFile = doInclude(tmpStr, it);
	hInput = readFile(hFile);
	
	string::iterator i = find(it + 1, mInput.end(), delim);
	tmpStr = strTmp(i);

	string tmpInput;
	string::iterator r = i + 16;

	while (isSpace(r))
	{
		++r;
	}

	while (r != mInput.end())
	{
		tmpInput.push_back(*r);
		r++;
	}

	mInput = doDefine(tmpStr,tmpInput,i);

	ofstream outFile;
	outFile.open(outArcFile.c_str(), ofstream::out);
	outFile << hInput;
	outFile << mInput;

	return 0;
}

