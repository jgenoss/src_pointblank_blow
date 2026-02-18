#include "StdAfx.h"

//#include "ListSubCtrl_Impl.h"
#include "NationEditorDlg.h"

CListSubCtrl_Impl::CListSubCtrl_Impl(void)
{
}

CListSubCtrl_Impl::~CListSubCtrl_Impl(void)
{
}

void CListSubCtrl_Impl::NotifyHeaderColumn()
{
	CListSubCtrl::NotifyHeaderColumn();

	Clear_Cells();
	ResetComboString();

	CNationEditorDlg* pParent = (CNationEditorDlg*)GetParent();

	const PefContextBank* pPefContextBank = pParent->GetPefContextBankPtr();
	const StringVector* pNationBank = pParent->GetNationListPtr();

	//Combo control string setting
	size_t nationSize = pNationBank->size();
	AddComboString("_Blank_");
	for(size_t i=0;i<nationSize;++i)
	{
		AddComboString( (*pNationBank)[i].c_str() );
	}

	//List control header column setting
	Set_Column( "Pef", 100, CST_NONE);
	Set_Column( "Nation", 100, CST_COBMO);

	//Insert pef item
	std::string Name;
	size_t Index = 0;

	Insert_Cell( 0, "All" );
	Set_Cell(0, 1, "_Blank_");

	size_t size = pPefContextBank->size();
	for(size_t i=0;i<size;++i)
	{
		Name = (*pPefContextBank)[i]->Name;
		Index = Name.rfind( "." );
		Name.erase( Index );

		Insert_Cell( i+1, (LPTSTR)Name.c_str() );
		Set_Cell(i+1, 1, "_Blank_");
	}
}

void CListSubCtrl_Impl::EndSubControl(int nRow, int nCol, LPTSTR String)
{
	CListSubCtrl::EndSubControl(nRow, nCol, String);

	if( nRow == 0 )
	{
		CNationEditorDlg* pParent = (CNationEditorDlg*)GetParent();
		const PefContextBank* pPefContextBank = pParent->GetPefContextBankPtr();

		size_t size = pPefContextBank->size();
		for(size_t i=0;i<size;++i)
		{
			Set_Cell(i+1, 1, String);
		}
	}	
}

String CListSubCtrl_Impl::GetNationName(size_t Index)
{
	String Ret;

	//All이 있기 때문에 하나 증가 한다.
	if( (int)Index < Get_CellCount() )
		Ret = Get_CellText( Index + 1, 1 ).GetString();

	return Ret;
}