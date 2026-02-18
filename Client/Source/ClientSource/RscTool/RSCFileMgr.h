#pragma once

#include "LoadRSCFile.h"
#include "i3RscPartialFile.h"


// const 처리가 약간 미비하다...정리되는대로 처리할것..
struct PreprocessMessageCallback;

class RSCFileMgr : public i3::shared_ginst<RSCFileMgr>
{
public:
	
	RSCFileMgr();
	~RSCFileMgr();

	void SetMediaPath(const i3::wstring strMediaFolder, PreprocessMessageCallback* cb);
		
	bool FindRscFilePath(const i3::string& name, RscFileInfo*& result);
	bool FindRscFilePath_UpperName(const i3::fixed_string& upper_name, RscFileInfo*& result);
	
	bool FindAnimPackFile( const i3::rc_string& i3a_name, i3::vector<i3::rc_string>& vec_animpackfile);
	bool FindAnimPackFile_UpperUnix( const i3::fixed_string& i3a_upper_name, i3::vector<i3::rc_string>& vec_animpackfile);
	
	bool FindLinkedToCharaAI( const i3::fixed_string& node_name, vector_i3Rsc_AnimPathNode_Filename& out);
		
private:
	
	RscFileInfoSet m_rscfile_set;
	
	AnimPackInfoMap m_anipack_map;
	
	i3Rsc_AnimPathNodeMap	m_aicontext_animpath_map;

};


