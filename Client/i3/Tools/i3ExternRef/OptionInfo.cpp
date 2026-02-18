#include "stdafx.h"
#include "OptionInfo.h"

COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;
	m_szExportResFolder[0] = 0;
	m_bExternTexture = TRUE;

	Load();
}

COptionInfo::~COptionInfo(void)
{
	Save();
}

void COptionInfo::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "Option", "WorkPath", "");
	strcpy( m_szWorkPath, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "Option", "ExportResFolder", "");
	strcpy( m_szExportResFolder, LPCTSTR( str));

	m_bExternTexture	= AfxGetApp()->GetProfileInt( "Option", "ExternTexture", TRUE);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

void COptionInfo::Save(void)
{
	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);
	AfxGetApp()->WriteProfileString( "Option", "ExportResFolder", m_szExportResFolder);

	AfxGetApp()->WriteProfileInt( "Option", "ExternTexture", m_bExternTexture);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

