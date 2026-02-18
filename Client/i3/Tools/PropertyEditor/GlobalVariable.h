#pragma once

#include "PropEditorProfile.h"

extern CPropEditorProfile * g_pProfile;

extern INT32 GetCodePageCount(void);
extern UINT32 FindCodePageByIdx( INT32 idx);
extern INT32 FindIdxByCodePage(UINT32 nCodePage);
extern void GetCodePageName(UINT32 nCodePage, char * pOut);

extern void SetSelCodePage(UINT32 nCodePage);
extern INT32 GetSelCodePage(void);