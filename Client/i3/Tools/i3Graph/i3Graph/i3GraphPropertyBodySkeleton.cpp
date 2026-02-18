#include "stdafx.h"
#include "i3GraphPropertyBodySkeleton.h"
#include "SkeletonSpecDialog.h"

CPropertyBodySkeleton::CPropertyBodySkeleton()
{
	m_Style = I3_BIG_DIALOG; 
}

CPropertyBodySkeleton::~CPropertyBodySkeleton()
{

}
void CPropertyBodySkeleton::Get( char * pszValueStr)
{
	strcpy(pszValueStr,"Click This line");
}

void CPropertyBodySkeleton::Set( const char * pszValueStr)
{
}

void CPropertyBodySkeleton::PopUpDialog(void)
{
	CSkeletonSpecDialog dlg;

	dlg.Execute( m_pLOD);
}