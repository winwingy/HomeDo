// ClassSize.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
class AB
{
	int m;
	float b;
	static float c;
	static const int t;
	enum opo
	{
		pss = 1,
		puu = 2,
	};
};

class KKK
{
	int m;
	enum opo	//ö����Ϊһ����������, û�б���, Ҳ��ռ��С
	{
		pss = 1,
		puu = 2,
	};
};

class KEE
{
	enum opo	//ö����Ϊһ����������, û�б���, Ҳ��ռ��С
	{
		pss = 1,
		puu = 2,
	};
	opo eOpo;	//ö�ٱ������4���ֽ�	
};

class KEEBIG
{
	enum opo	//ö����Ϊһ����������, û�б���, Ҳ��ռ��С
	{
		pss = 1,
		puu = 2,
		paa = 3,
		pbb = 4,
		pcc = 5,
		pdd = 6000,
		pee = 90000
	};
	opo eOpo;	//ö�ٱ������4���ֽ�	�ٴ��Ҳ��4���ֽ�, ��!!!
};



class ZZZ
{
	int m;
	static float c;	//��ľ�̬��Ա��ռ��Ĵ�С
	static const int t;

};




int _tmain(int argc, _TCHAR* argv[])
{
	int iSize = sizeof(AB);
	printf("%d\n", iSize);
	iSize = sizeof(KKK);
	printf("%d\n", iSize);
	iSize = sizeof(ZZZ);
	printf("%d\n", iSize);
	
	iSize = sizeof(KEE);
	printf("%d\n", iSize);

	iSize = sizeof(KEEBIG);
	printf("%d\n", iSize);

	system("pause");
	return 0;
}

