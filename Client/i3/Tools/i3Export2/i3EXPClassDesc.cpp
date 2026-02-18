#include "stdafx.h"
#include "i3EXPClassDesc.h"
#include "Util.h"
#include "resource.h"
#include "i3Export.h"

i3EXPClassDesc::i3EXPClassDesc(void)
{
}

i3EXPClassDesc::~i3EXPClassDesc(void)
{
}

const TCHAR* i3EXPClassDesc::ClassName(void)
{
	return GetString( IDS_CLASSNAME);
}

SClass_ID i3EXPClassDesc::SuperClassID(void)
{
	return GUP_CLASS_ID;
}

Class_ID i3EXPClassDesc::ClassID(void)
{
	return I3_EXPORT2_CLASS_ID;
}

const TCHAR* i3EXPClassDesc::Category(void)   // primitive/spline/loft/ etc
{
	return GetString( IDS_CATEGORY);
}

void * i3EXPClassDesc::Create(BOOL loading)
{
	return new i3Export;
}

/////////////////////////////////////////////////////////////////////
static i3EXPClassDesc		s_ClassDesc;

ClassDesc * GetExportedClassDesc(void)
{
	return &s_ClassDesc;
}
