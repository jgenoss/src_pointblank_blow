#include "StdAfx.h"
#include "GlobalVariable.h"

CPropEditorProfile * g_pProfile = NULL;

static UINT32 g_nSelCodePage = 0;

const INT32 g_nCodePageCount = 14;

const UINT32 g_nCodePage[ g_nCodePageCount] = 
{
	I3_LANG_CODE_ANSI,
	I3_LANG_CODE_KOREAN,
	I3_LANG_CODE_JAPANESE,
	I3_LANG_CODE_CHINESE_SIMPLIFIED,
	I3_LANG_CODE_CHINESE_TRADITIONAL,
	I3_LANG_CODE_THAI,
	I3_LANG_CODE_RUSSIA,
	I3_LANG_CODE_GREEK,
	I3_LANG_CODE_TURKISH,
	I3_LANG_CODE_HEBREW,
	I3_LANG_CODE_ARABIC,
	I3_LANG_CODE_BALTIC,
	I3_LANG_CODE_EASTEUROPE,
	I3_LANG_CODE_VIETNAMESE,
};

INT32 GetCodePageCount(void)
{
	return g_nCodePageCount;
}

UINT32 FindCodePageByIdx( INT32 idx)
{
	I3ASSERT( 0 <= idx && g_nCodePageCount > idx);

	return g_nCodePage[idx];
}

INT32 FindIdxByCodePage(UINT32 nCodePage)
{
	for(INT32 i=0; i<g_nCodePageCount; i++)
	{
		if( g_nCodePage[i] == nCodePage)
			return i;
	}

	return -1;
}

void GetCodePageName(UINT32 nCodePage, char * pOut)
{
	I3ASSERT( pOut != NULL);

	sprintf( pOut, "%s (%d)", i3Language::GetLanguage( nCodePage), nCodePage);
}

void SetSelCodePage(UINT32 nCodePage)
{
	g_nSelCodePage = nCodePage;
}

INT32 GetSelCodePage(void)
{
	return g_nSelCodePage;
}