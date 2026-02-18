#include "i3FrameworkPCH.h"
#include "i3CharaSGChangeManager.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/make_unix_path.h"

struct i3CharaSGChangeInfo
{
	i3::rc_string tgt_i3chr_filename;		// 타겟 i3chr파일 이름..외부에서 들어온것이 아닌 내부 생성 이름이다..
	i3::rc_string ref_i3chr_filename;		// 레퍼런스 i3chr파일이름 (외부입력값)

	i3::rc_string i3s_filename;				// 교환할 i3s값.. 
};											// 실제 i3s파일의 경우 : 상대경로만큼은 확보된 파일이름이어야함.
											// 가짜 i3s파일의 경우 : texture_change_i3s.pef에 
											// 등록된 이름이면 된다.

i3CharaSGChangeManager::~i3CharaSGChangeManager()
{
	i3::cu::for_each_delete(m_clonemap);
}

bool	i3CharaSGChangeManager::AddData(const i3::wliteral_range& wstrCharaFile, 
				const i3::wliteral_range& wstrSGFileToChange, i3::rc_string& outFakeCharaFile)
{
	i3::rc_string strCharaFile, strSGFileToChange;
	i3::utf16_to_mb(wstrCharaFile, strCharaFile);
	i3::utf16_to_mb(wstrSGFileToChange, strSGFileToChange);
	
	return i3CharaSGChangeManager::AddData(strCharaFile, strSGFileToChange, outFakeCharaFile);
}

bool	i3CharaSGChangeManager::AddData(const i3::rc_string& strCharaFile,
				const i3::rc_string& strSGFileToChange, i3::rc_string& outFakeCharaFile)
{
	// 먼저 타겟 i3chr파일을 구해둔다.
	
	//a) i3s파일의 타이틀을 먼저 얻고
	i3::stack_string strSGFileTitle; i3::extract_filetitle(strSGFileToChange, strSGFileTitle);


	{
		//b) i3chr파일의 타이틀 범위를 얻어서 i3s파일 타이틀과 replace ...
		i3::stack_string strCharaFileDir; i3::extract_directoryname(strCharaFile, strCharaFileDir);
		i3::make_unix_path(strCharaFileDir);

		i3::stack_string strCharaExt; i3::extract_fileext(strCharaFile, strCharaExt);

		i3::stack_string strFakeCharaFile;

		if (!strCharaFileDir.empty())
		{
			strFakeCharaFile = strCharaFileDir;	strFakeCharaFile += '/';
		}
		
		strFakeCharaFile += strSGFileTitle;
		
		if (!strCharaExt.empty())
		{
			strFakeCharaFile += '.';	strFakeCharaFile += strCharaExt;
		}
	
		outFakeCharaFile = strFakeCharaFile;
	}
	
	i3::rc_string strLowerSGFileTitle; 
	i3::to_lower_copy(strLowerSGFileTitle, strSGFileTitle);
		
	i3::unordered_map<i3::rc_string, i3CharaSGChangeInfo*>::iterator it = m_clonemap.find(strLowerSGFileTitle);
	
	if ( it != m_clonemap.end() )		// 기존에 존재...에러는 아닐수 있음..I3TRACE로 표시..
	{
		I3TRACE("Redundant [ Ref i3Chara : %s ] [ Change i3SceneGraph : %s ] File Exists From Pef\n", 
			strCharaFile.c_str(), strSGFileToChange.c_str());
		return true;
	}
	
	// 새 것이면 삽입처리..
	i3::stack_string strRefCHRFileName; 
	i3::extract_filetitle(strCharaFile, strRefCHRFileName);	strRefCHRFileName += ".I3CHR";
	i3::stack_string strTgtCHRFilename = strSGFileTitle;	strTgtCHRFilename += ".I3CHR";

	i3::stack_string strTgtI3SFilename;
	const i3SGTexChangeInfo* texChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(strSGFileTitle.c_str());
	
	if (texChangeInfo != nullptr)
	{
		strTgtI3SFilename = strSGFileTitle;	strTgtI3SFilename += ".I3S";
	}
	else	// 실제 존재하는 것으로 일단 가정.. (이경우에는 상대경로와 확장자를 다 들고 있어야한다...)
	{
		strTgtI3SFilename =	strSGFileToChange;
		i3::make_unix_path(strTgtI3SFilename);

		size_t pos = strTgtI3SFilename.find_last_of("./");
		
		if (pos == i3::stack_string::npos)		// 이런 경우...실패한 것으로 간주해야한다..
		{
			I3TRACE("This Real %s SceneGraph File needs RelativePath and FileExt.\n", strSGFileToChange.c_str() );
			return false;
		}
		else
		if ( strTgtI3SFilename[pos] == '/')
		{
			strTgtI3SFilename += ".I3S";
		}
		else	// '.'
		{
			pos = strTgtI3SFilename.rfind('/');
			if (pos == i3::stack_string::npos)
			{
				I3TRACE("This Real %s SceneGraph File needs RelativePath.\n", strSGFileToChange.c_str() );
				return false;
			}
		}
	}
			
	i3CharaSGChangeInfo* new_info = new i3CharaSGChangeInfo;
	new_info->tgt_i3chr_filename = strTgtCHRFilename;
	new_info->ref_i3chr_filename = strRefCHRFileName;
	new_info->i3s_filename = strTgtI3SFilename;
	
	m_clonemap.insert( i3::unordered_map<i3::rc_string, i3CharaSGChangeInfo*>::value_type(
		strLowerSGFileTitle, new_info ) );

	return true;
}

const i3CharaSGChangeInfo*	i3CharaSGChangeManager::FindSGChangeInfo(const i3::literal_range& strFakeCharaFile) const
{
	i3::stack_string lower_title;
	i3::extract_filetitle(strFakeCharaFile, lower_title);
	i3::to_lower(lower_title);
	i3::rc_string key_name = lower_title;

	i3::unordered_map<i3::rc_string, i3CharaSGChangeInfo*>::const_iterator it = m_clonemap.find(key_name);	
	return (it == m_clonemap.end()) ? nullptr : it->second;
}

i3::rc_string		i3CharaSGChangeManager::GetRefi3CharaFileName(const i3CharaSGChangeInfo* info)
{
	return info->ref_i3chr_filename;
}

i3::rc_string		i3CharaSGChangeManager::GetTgti3CharaFileName(const i3CharaSGChangeInfo* info)
{
	return info->tgt_i3chr_filename;
}

i3::rc_string		i3CharaSGChangeManager::GetTgtI3SFileName(const i3CharaSGChangeInfo* info)
{
	return info->i3s_filename;
}



