#include "stdafx.h"
#include "i3ImageClassDesc.h"
#include "Util.h"
#include "resource.h"
#include "i3Image.h"

i3ImageClassDesc::i3ImageClassDesc(void)
{
}

i3ImageClassDesc::~i3ImageClassDesc(void)
{
}

const TCHAR* i3ImageClassDesc::ClassName(void)
{
	return GetString( IDS_IMAGE_CLASSNAME);
}

SClass_ID i3ImageClassDesc::SuperClassID(void)
{
	return BMM_IO_CLASS_ID;
}

Class_ID i3ImageClassDesc::ClassID(void)
{
	return I3_IMAGE_CLASS_ID;
}

const TCHAR* i3ImageClassDesc::Category(void)   // primitive/spline/loft/ etc
{
	return GetString( IDS_IMAGE_CATEGORY);
}

void * i3ImageClassDesc::Create(BOOL loading)
{
	return new i3ImageEXP;
}

/////////////////////////////////////////////////////////////////////
static i3ImageClassDesc		s_ClassDesc;

ClassDesc * GetImageClassDesc(void)
{
	return &s_ClassDesc;
}
