#include "stdafx.h"
#include "OptionInfo.h"
#include "i3Base/i3StringListBuffer.h"
#include "i3PackTool.h"
#include "i3Base/string/ext/remove_all_blank.h"
#include "i3Base/string/ext/is_matched_wildcard.h"


COptionInfo::COptionInfo(void)
{
	m_szWorkPath[0] = 0;
	m_szLivePath[0] = 0;
	m_szScriptPath[0] = 0;
// 	m_szWorkHistory[0][0] = 0;
// 	m_szWorkHistory[1][0] = 0;
// 	m_szWorkHistory[2][0] = 0;
	Load();
}

COptionInfo::~COptionInfo(void)
{
	Save();
}

bool COptionInfo::isExceptFile( char * pszPath)
{
	
	char szFile[MAX_PATH] = "";

	for(size_t i = 0; i < m_ExceptList.size(); i++)
	{
		i3::safe_string_copy( szFile, m_ExceptList[i].c_str(), MAX_PATH );
		i3::remove_all_blank(szFile);

//		if( i3String::IsMatchedWildcard( pszPath, szFile))
		if (i3::is_matched_wildcard(pszPath, szFile))
			return true;
	}

	return false;
}

void COptionInfo::Load(void)
{
	CString str;

	// Working Directory
	str = AfxGetApp()->GetProfileString( "Option", "WorkPath", "");
	strcpy( m_szWorkPath, LPCTSTR( str));

	// Working History0
// 	str = AfxGetApp()->GetProfileString( "Option", "WorkHistory0", "");
// 	strcpy( m_szWorkHistory[0], LPCTSTR( str));
// 
// 	// Working History1
// 	str = AfxGetApp()->GetProfileString( "Option", "WorkHistory1", "");
// 	strcpy( m_szWorkHistory[1], LPCTSTR( str));
// 
// 	// Working History2
// 	str = AfxGetApp()->GetProfileString( "Option", "WorkHistory2", "");
// 	strcpy( m_szWorkHistory[2], LPCTSTR( str));

	// Live Directory
	str = AfxGetApp()->GetProfileString( "Option", "LivePath", "");
	strcpy( m_szLivePath, LPCTSTR( str));

	// Script Directory
	str = AfxGetApp()->GetProfileString( "Option", "ScriptPath", "");
	strcpy( m_szScriptPath, LPCTSTR( str));

	i3ResourceFile::SetWorkingFolder( m_szWorkPath);

	// Exception Rulls
	str = AfxGetApp()->GetProfileString( "Option", "Except", "");
//	m_ExceptList.CopyFromBuffer( str.GetBuffer());
	CopyFromBufferToStringList(m_ExceptList, str.GetBuffer());

	char temp[ 4096] = {};
	CopyFromStringListToBuffer(m_ExceptList, temp, sizeof(temp));
	if( temp[0] == 0)
	{
		str = "*.sif;*.log;*.i3CharaEditor;*.i3LevelDesign;*.dump;*.svn;*.RSG;*.exe;*.i3Reg;*.ini;*.xml;*.i3RegXML";
		theApp.ParseException(str.GetBuffer(), m_ExceptList);

	}
	
}

void COptionInfo::Save(void)
{
	// Working Directory
	AfxGetApp()->WriteProfileString( "Option", "WorkPath", m_szWorkPath);
	i3ResourceFile::SetWorkingFolder( m_szWorkPath);

// 	AfxGetApp()->WriteProfileString( "Option", "WorkHistory0", m_szWorkHistory[0]);
// 	AfxGetApp()->WriteProfileString( "Option", "WorkHistory1", m_szWorkHistory[1]);
// 	AfxGetApp()->WriteProfileString( "Option", "WorkHistory2", m_szWorkHistory[2]);

	AfxGetApp()->WriteProfileString( "Option", "LivePath", m_szLivePath);
	AfxGetApp()->WriteProfileString( "Option", "ScriptPath", m_szScriptPath);

	// Exception Rulls
	{
	//	INT32 sz =	GetStringListBufferSize(m_ExceptList);		// m_ExceptList.GetBufferSize();
		char temp[ 4096] = {};

		CopyFromStringListToBuffer(m_ExceptList, temp, sizeof(temp));		//m_ExceptList.CopyToBuffer( pTemp, sz);
		AfxGetApp()->WriteProfileString( "Option", "Except", temp);

	}
}

