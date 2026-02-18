#include <stdio.h>
#include <string.h>

#define MAX_STRING 255

// DEBUG 실행시에는 경로를 바꾸어 주어야 합니다.
// 아래 경로들 앞에 ../../ 을 추가하면 됩니다. ( 2014.06.13 폴더구조 기준 )

char CommonDefPath[MAX_STRING] = {"../CommonSource/CommonDef.h"};
char BuildVerInfoPath[MAX_STRING] = {"../ClientSource/Source/BuildVerInfo.h"};

FILE* fCommonDef = fopen(CommonDefPath,"r");

enum
{
	VER_GAME,
	VER_BUILD,
	VER_SC_PROTOCOL,
	VER_MAX
};

bool	ConditionCheck[VER_MAX];
int		VersionNum[VER_MAX];
int		PastVersionNum[VER_MAX];
const char VersionChar[VER_MAX][MAX_STRING] = 
{
	{"VER_GAME"},
	{"VER_BUILD"},
	{"VER_SC_PROTOCOL"},
};

bool	isNeedFindMore(void);
int	ReadAndWriteVer(void);
bool	IsDiffVer(void);