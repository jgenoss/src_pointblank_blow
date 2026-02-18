#include "BuildCounter.h"
#include <time.h>

bool isNeedFindMore(void)
{
	for ( int i = 0 ; i < VER_MAX ; i ++ )
	{
		if ( ConditionCheck[i] == false )
			return true;
	}
	return false;
}

int ReadAndWriteVer(void)
{
	int tVer = 0;
	// Read 용 파일 오픈
	{
		FILE* fReadVerInfo = fopen(BuildVerInfoPath,"r");
		if (fReadVerInfo == NULL) return -1;

		char tempString[MAX_STRING];
		for ( int i = 0 ; i < VER_MAX ; i ++ )
		{
			sprintf(tempString,"#define BUILD_%s %%d\n",VersionChar[i]);
			fscanf_s(fReadVerInfo,tempString,&PastVersionNum[i]);
		}

		fscanf_s(fReadVerInfo,"#define BUILD_COUNT %d",&tVer);
		fclose(fReadVerInfo);
	}
	// Read 완료

	//마지막 버전 날자로 변경함. 
	{
		#define COUNT_DIGIT	1000
		
		int iOldYYMMDD	= (int)(tVer / COUNT_DIGIT); 
		int iCount		= (int)(tVer % COUNT_DIGIT); 
		int iNewYYMMDD; 	

		//오늘 날자 가지고 오기 
		time_t now = time(NULL); //시스템 시각을 얻어옴
		struct tm *lt = localtime(&now); //tm 구조체로 변환
		iNewYYMMDD = ((lt->tm_year -100) * 10000)+ ((lt->tm_mon+1) * 100)+ lt->tm_mday; 

		if(iNewYYMMDD != iOldYYMMDD)
		{
			iCount = 0; 
		}
		iCount ++;
		tVer = (iNewYYMMDD * COUNT_DIGIT) + iCount; 
	}
	
	// Write 용 파일 오픈
	{
		FILE* fWrietVerInfo = fopen(BuildVerInfoPath,"w");
		if (fWrietVerInfo == NULL) return -1;

		char tempString[MAX_STRING];
		for ( int i = 0 ; i < VER_MAX ; i ++ )
		{
			sprintf(tempString,"#define BUILD_%s %%d\n",VersionChar[i]);
			fprintf_s(fWrietVerInfo,tempString,VersionNum[i]);
		}

		fprintf_s(fWrietVerInfo,"#define BUILD_COUNT %d",tVer);

		fclose(fWrietVerInfo);
	}

	return tVer;

	// Write 완료
}

// VER_BUILD 가 다를 때만 초기화 합니다.
bool IsDiffVer(void)
{
	if ( VersionNum[VER_BUILD] != PastVersionNum[VER_BUILD] )
		return true;
	else
		return false;
}

int main(void)
{
	while( isNeedFindMore() )
	{
		char tempString[MAX_STRING];
		fscanf_s(fCommonDef,"%s",tempString,MAX_STRING);
		for ( int i = 0 ; i < VER_MAX  ; i ++ )
		{
			if ( ConditionCheck[i] == true ) continue;
			if ( strcmp(tempString,VersionChar[i]) == 0 )
			{
				int tempNum = 0;
				fscanf_s(fCommonDef,"%d",&tempNum);
				VersionNum[i] = tempNum;
				ConditionCheck[i] = true;
				break;
			}
		}
	}

	int Rv = ReadAndWriteVer();

	fclose(fCommonDef);
	return Rv;
}