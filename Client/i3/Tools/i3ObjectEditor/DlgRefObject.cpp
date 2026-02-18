// DlgRefObject.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "DlgRefObject.h"
#include ".\dlgrefobject.h"
#include "i3Base/string/ext/extract_fileext.h"


// CDlgRefObject dialog

IMPLEMENT_DYNAMIC(CDlgRefObject, CDialog)
CDlgRefObject::CDlgRefObject(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRefObject::IDD, pParent)
{
}

CDlgRefObject::~CDlgRefObject()
{
}

void CDlgRefObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJ_LIST, m_ObjListCtrl);
}

bool CDlgRefObject::Execute( i3GameObj * pObj)
{
	m_pObj = pObj;

	DoModal();

	return true;
}

BEGIN_MESSAGE_MAP(CDlgRefObject, CDialog)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
END_MESSAGE_MAP()


// CDlgRefObject message handlers
void CDlgRefObject::_UpdateObj( INT32 idx, i3ResourceObject * pObj)
{
	m_ObjListCtrl.SetItemData( idx, (DWORD_PTR) pObj);

	m_ObjListCtrl.SetItemText( idx, 1, pObj->meta()->class_name().c_str());
}

void CDlgRefObject::_UpdateAllRef(void)
{
	INT32 i;

	m_ObjListCtrl.DeleteAllItems();

	for( i = 0; i < m_pObj->getRefObjectCount(); i++)
	{
		i3ResourceObject * pRef = m_pObj->getRefObject( i);

		INT32 idx = m_ObjListCtrl.InsertItem( i, pRef->GetName(), 0);

		_UpdateObj( idx, pRef);
	}
}

BOOL CDlgRefObject::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ObjListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 180);
		m_ObjListCtrl.InsertColumn( 1, "Type", LVCFMT_LEFT, 120);
		m_ObjListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	// Reference Object List
	{
		_UpdateAllRef();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRefObject::_AddAnim( const char * pszPath)
{
	i3Animation * pAnim;

	pAnim = i3Animation::LoadFromFile( pszPath);

	pAnim->SetExternRes( TRUE);

	m_pObj->addRefObject( pAnim);

	INT32 idx = m_ObjListCtrl.InsertItem( m_ObjListCtrl.GetItemCount(), pAnim->GetName(), 0);

	_UpdateObj( idx, pAnim);
}

void CDlgRefObject::_AddTex( const char * pszPath)
{
	i3ImageFile file;
	i3Texture * pTex;

	pTex = file.Load( pszPath);

	pTex->SetExternRes( TRUE);

	m_pObj->addRefObject( pTex);

	INT32 idx = m_ObjListCtrl.InsertItem( m_ObjListCtrl.GetItemCount(), pTex->GetName(), 0);

	_UpdateObj( idx, pTex);
}

void CDlgRefObject::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
//	char szTempPath[MAX_PATH]; //, * pPath;
//	char szName[MAX_PATH];
//	bool bProcessed = false;

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

//		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3::extract_fileext(TempName, Ext);
/*
		i3::extract_directoryname( TempName, szTempPath);
*/
/*
		i3::safe_string_copy( szTempPath, TempName, MAX_PATH );
		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;
*/
		if( i3::generic_is_iequal( Ext, "I3A") )
		{
			_AddAnim( (const char *) TempName);
		}
		else if( i3::generic_is_iequal( Ext, "I3I") )
		{
			_AddTex( (const char *) TempName);
		}
	}

	DragFinish( hDropInfo);
}

void CDlgRefObject::OnBnClickedBtnDelete()
{
	POSITION pos;
	INT32 idx;
	i3::vector<i3ResourceObject*> list;

	// 우선 선택된 Object들의 리스트를 만든다.
	pos = m_ObjListCtrl.GetFirstSelectedItemPosition();

	while( pos != NULL)
	{
		idx = m_ObjListCtrl.GetNextSelectedItem( pos);

		list.push_back( reinterpret_cast<i3ResourceObject*>(m_ObjListCtrl.GetItemData( idx)));
	}

	for(size_t i = 0; i < list.size(); i++)
	{
		i3ResourceObject * pRef = list[i];

		m_pObj->removeRefObject( pRef);
	}

	_UpdateAllRef();
}
