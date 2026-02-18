#pragma once

#include "LoadRSCFile.h"
#include "i3RscPartialFile.h"
#include "i3RSCGen/i3RSCGen.h"


// const 처리가 약간 미비하다...정리되는대로 처리할것..
struct PreprocessMessageCallback;

class RSCFileMgr : public i3::shared_ginst<RSCFileMgr>
{
public:
	
	RSCFileMgr();
	~RSCFileMgr();

	void SetMediaPath(const i3::wstring strMediaFolder);// , PreprocessMessageCallback* cb);
	void LoadRSCGenFiles(const i3::string& strWorkdir);
		
	bool FindRscFilePath(const i3::string& name, RscFileInfo*& result);
	bool FindRscFilePath_UpperName(const i3::fixed_string& upper_name, RscFileInfo*& result);
	
	bool FindAnimPackFile( const i3::rc_string& i3a_name, i3::vector<i3::rc_string>& vec_animpackfile);
	bool FindAnimPackFile_UpperUnix( const i3::fixed_string& i3a_upper_name, i3::vector<i3::rc_string>& vec_animpackfile);
	
	bool FindLinkedToCharaAI( const i3::fixed_string& node_name, vector_i3Rsc_AnimPathNode_Filename& out);

	i3RSCGen* FindOrLoadRSCGen(const i3::rc_string& workdir, const i3::rc_string& i3RSCGenRelPath);

	void GetAllRSCGenRelPaths(std::vector<std::string>& out) const;
		
private:
	i3RSCGen* _LoadRSCGen(const i3::rc_string& workdir, const i3::rc_string& i3RSCGenRelPath);

	RscFileInfoSet m_rscfile_set;
	
	AnimPackInfoMap m_anipack_map;
	
	i3Rsc_AnimPathNodeMap	m_aicontext_animpath_map;

	using i3RSCGenPtr = std::unique_ptr<i3RSCGen>;
	using i3RSCGenMap = std::map<std::string, i3RSCGenPtr>;
	i3RSCGenMap m_i3RSCGenMap;

};


