#include "StdAfx.h"

#include <sstream>

#include "PropEditorProfile.h"

CPropEditorProfile::CPropEditorProfile(void)
{
	m_bComplexUse = false;
	m_GapSize = 3;
	Load();
}

CPropEditorProfile::~CPropEditorProfile(void)
{
	Save();
}

void _LoadRegistryRect(CRect & Rect, HKEY hKey, const char* PreName)
{
	DWORD dwSize = MAX_PATH;
	char Name[MAX_PATH];

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "X");
	RegQueryValueEx(hKey, Name, NULL, NULL , (LPBYTE)&Rect.left, &dwSize);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "Y");
	RegQueryValueEx(hKey, Name, NULL, NULL , (LPBYTE)&Rect.top, &dwSize);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "CX");
	RegQueryValueEx(hKey, Name, NULL, NULL , (LPBYTE)&Rect.right, &dwSize);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "CY");
	RegQueryValueEx(hKey, Name, NULL, NULL , (LPBYTE)&Rect.bottom, &dwSize);
}

void	CPropEditorProfile::Load( void)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	HKEY hKey;
	TCHAR szBuffer[MAX_PATH] = {'\0' ,};

	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE , 
		"SOFTWARE\\JuuL\\PefEditor" , 0 , KEY_READ , &hKey);
	if(ERROR_SUCCESS == lResult)
	{
		RegQueryValueEx(hKey , "WorkDir" , NULL , &dwType , (LPBYTE)szBuffer , &dwSize);
	}
	m_WorkDirectoryPath = szBuffer;
	RegCloseKey( hKey );

//	m_SplitFactor = 20;
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE , 
		"SOFTWARE\\JuuL\\PefEditor\\MainWnd" , 0 , KEY_READ , &hKey);
	if(ERROR_SUCCESS == lResult)
	{
		_LoadRegistryRect(m_MainWnd, hKey, "Main");

		_LoadRegistryRect(m_SimpleTree, hKey, "SimpleTree");
		_LoadRegistryRect(m_SimpleList, hKey, "SimpleList");

		_LoadRegistryRect(m_Tree1, hKey, "Tree1");
		_LoadRegistryRect(m_Tree2, hKey, "Tree2");
		_LoadRegistryRect(m_List1, hKey, "List1");
		_LoadRegistryRect(m_List2, hKey, "List2");
		_LoadRegistryRect(m_Image, hKey, "Image");
	}
	RegCloseKey( hKey );

	DefaultRect();
	UpdateGapRect();
}

void _SaveRegistryRect(CRegKey & RegKey, const char* PreName, CRect & Rect)
{
	char Name[MAX_PATH];

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "X");
	RegKey.SetValue(Rect.left, Name);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "Y");
	RegKey.SetValue(Rect.top, Name);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "CX");
	RegKey.SetValue(Rect.right, Name);

	sprintf_s(Name, MAX_PATH, "%s%s", PreName, "CY");
	RegKey.SetValue(Rect.bottom, Name);
}

void	CPropEditorProfile::Save( void)
{
	CRegKey RegKey;
	RegKey.Create(HKEY_LOCAL_MACHINE , "SOFTWARE\\JuuL\\PefEditor");
	RegKey.SetValue(m_WorkDirectoryPath , "WorkDir");
	RegKey.Close();

	RegKey.Create(HKEY_LOCAL_MACHINE , "SOFTWARE\\JuuL\\PefEditor\\MainWnd");
	
	_SaveRegistryRect(RegKey, "Main", m_MainWnd);

	_SaveRegistryRect(RegKey, "SimpleTree", m_SimpleTree);
	_SaveRegistryRect(RegKey, "SimpleList", m_SimpleList);

	_SaveRegistryRect(RegKey, "Tree1", m_Tree1);
	_SaveRegistryRect(RegKey, "Tree2", m_Tree2);
	_SaveRegistryRect(RegKey, "List1", m_List1);
	_SaveRegistryRect(RegKey, "List2", m_List2);
	_SaveRegistryRect(RegKey, "Image", m_Image);

	RegKey.Close();
}


void	CPropEditorProfile::DefaultRect( void)
{
	CRect	rc;
	GetClientRect(GetDesktopWindow(), rc);

	//전체 크기
	if ( m_MainWnd.left < 0 || m_MainWnd.left >= rc.right )
		m_MainWnd.left = 10;

	if ( m_MainWnd.top < 0 || m_MainWnd.top >= rc.bottom )
		m_MainWnd.top = 10;

	if( m_MainWnd.right <= 0)
		m_MainWnd = CRect(10, 10, 680, 480);

	//  |-----------------------------------------------------
	//  |  Tree1  |    Tree2         2      Image           |
	//  |              1                       |                              |
	//  |               |--------3-------+-----------3---------|
	//  |               |                        |                             |
	//  |               |      List1         5       List2             |
	//  |-----------------------------------------------------

	int Left = 0;
	int Top = 0;
	int Width = 654;
	int Height = 369;


	int VerBar1 = 130;
	int VerBar2 = (int)( (Width - VerBar1) * 0.5f);
	int HorBar3 = (int)(Height * 0.5f);
	int VerBar5 = (int)( (Width - VerBar1) * 0.5f);

	//Simple Tree 위치(Main Tree)
	if ( m_SimpleTree.right <= 0)
	{
		m_SimpleTree = CRect(Left, Top, VerBar1-m_GapSize, Height );
	}

	//Simple List 위치(Main List)
	if ( m_SimpleList.right <= 0)
	{
		m_SimpleList = CRect(VerBar1+m_GapSize, Top, Width, Height );
	}

	//Tree1 위치(Main Tree)
	if ( m_Tree1.right <= 0)
	{
		m_Tree1 = CRect(Left, Top, VerBar1-m_GapSize, Height );
	}

	//Tree2 위치(Sub Tree)
	if ( m_Tree2.right <= 0)
	{
		m_Tree2 = CRect(VerBar1+m_GapSize, Top, VerBar1 + VerBar2 - m_GapSize, HorBar3 - m_GapSize );
	}

	//List1 위치(ItemList)
	if ( m_List1.right <= 0)
	{
		m_List1 = CRect(VerBar1 + m_GapSize, HorBar3 + m_GapSize, VerBar1 + VerBar5 - m_GapSize, Height );
	}

	//List2 위치(WeaponList)
	if ( m_List2.right <= 0)
	{
		m_List2 = CRect(VerBar1 + VerBar5 + m_GapSize, HorBar3 + m_GapSize, Width, Height );
	}

	//Image
	if ( m_Image.right <= 0)
	{
		m_Image = CRect(VerBar1 + VerBar2 + m_GapSize, Top, Width, HorBar3 - m_GapSize );
	}
}

void	CPropEditorProfile::UpdateGapRect( void)
{
	m_GapRect = CRect(m_SimpleTree.right, m_SimpleTree.top,
		m_SimpleList.left, m_SimpleTree.bottom);
	m_GapRect.OffsetRect(2, 0);

	//  |-----------------------------------------------------
	//  |  Tree1  |    Tree2         2      Image           |
	//  |              1                       |                              |
	//  |               |--------3-------+-----------4---------|
	//  |               |                        |                             |
	//  |               |      List1         5       List2             |
	//  |-----------------------------------------------------

	m_GapRect1 = CRect(m_Tree1.right, m_Tree1.top,
		m_Tree2.left, m_Tree1.bottom);
	m_GapRect1.OffsetRect(2, 0);

	m_GapRect2 = CRect( m_Tree2.right, m_Tree2.top,
		m_Image.left, m_Tree2.bottom);
	m_GapRect2.OffsetRect(2, 0);

	m_GapRect5 = CRect( m_List1.right, m_List1.top,
		m_List2.left, m_List1.bottom);
	m_GapRect5.OffsetRect(2, 0);

	m_GapRect3 = CRect( m_Tree2.left, m_Tree2.bottom,
		m_List2.right, m_List2.top);
	m_GapRect3.OffsetRect(0, 2);
}

int CPropEditorProfile::MousePointOnSizeBar( POINT pt, int & BarIndex )
{
	if( !m_bComplexUse )
	{
		if( m_GapRect.PtInRect(pt) )
		{
			BarIndex = 1;
			return SBT_VER;
		}

		BarIndex = 0;
		return SBT_NONE;
	}

	if( m_GapRect1.PtInRect(pt) )
	{
		BarIndex = 1;
		return SBT_VER;
	}
	if( m_GapRect2.PtInRect(pt) )
	{
		BarIndex = 2;
		return SBT_VER;
	}
	if( m_GapRect5.PtInRect(pt) )
	{
		BarIndex = 5;
		return SBT_VER;
	}

	if( m_GapRect3.PtInRect(pt) )
	{
		BarIndex = 3;
		return SBT_HOR;
	}

	BarIndex = 0;
	return SBT_NONE;
}