#include "stdafx.h"
#include "OptionInfo.h"

COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;

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

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

void COptionInfo::Save(void)
{
	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);
}

