#include "stdafx.h"
#include "i3GraphPropertyShapeSet.h"
#include "ShapeSetSpecDialog.h"

CPropertyShapeSet::CPropertyShapeSet()
{
	m_Style = I3_BIG_DIALOG; 
}

CPropertyShapeSet::~CPropertyShapeSet()
{

}
void CPropertyShapeSet::Get( char * pszValueStr)
{
	strcpy(pszValueStr,"Click This line");
}

void CPropertyShapeSet::Set( const char * pszValueStr)
{
}

void CPropertyShapeSet::PopUpDialog(void)
{
	CShapeSetSpecDialog dlg;

	dlg.Execute( m_pShapeSet);
}