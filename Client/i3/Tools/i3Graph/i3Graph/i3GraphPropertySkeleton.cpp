#include "stdafx.h"
#include "i3GraphPropertySkeleton.h"
#include "SkeletonSpecDialog.h"

CpropertySkeleton::CpropertySkeleton()
{
	m_Style = I3_BIG_DIALOG; 
}

CpropertySkeleton::~CpropertySkeleton()
{

}
void CpropertySkeleton::Get( char * pszValueStr)
{
	strcpy(pszValueStr,"Click This line");
}

void CpropertySkeleton::Set( const char * pszValueStr)
{
}

void CpropertySkeleton::PopUpDialog(void)
{
}