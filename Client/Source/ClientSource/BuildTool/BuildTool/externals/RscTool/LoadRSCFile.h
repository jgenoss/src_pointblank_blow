#pragma once

class PackScriptFile;

struct RscFileInfo
{
	RscFileInfo() : m_IsNameUnique(true) {}
	i3::rc_string	m_Name;
	i3::rc_string	m_Path;
	i3::rc_string	m_RSCFileName;		// Media기준 상대경로로..저장..

	bool			m_IsNameUnique;		// 유니크한 이름인지 아닌지 여부..처리..
};										// 유니크한 이름이 아니라면, Path를 key로 하여 재검색해야한다..


typedef i3::shared_ptr<RscFileInfo>		RscFileInfoPtr;

struct RscFileInfoSet
{
	i3::unordered_map<i3::fixed_string, RscFileInfoPtr>	m_map;		// 대문자화 이름 기준으로 검색..
};

struct RscFileInfoList
{
	i3::vector<i3::rc_string>	m_path_list;
};

struct AnimPackInfo
{
	i3::rc_string	m_AniFileName;
	i3::rc_string	m_PackFileName;
};

struct AnimPackInfos
{
	i3::vector<AnimPackInfo>	m_vecInfo;					// 같은 i3a파일인데도 복수팩파일 중복이 있을수 있음..
};

struct AnimPackInfoMap
{
	i3::unordered_map< i3::fixed_string, AnimPackInfos> m_map;		// 대문자화된 .i3a파일 조회로 .i3Animpack파일 추적...
};

// 결과가 누적된다..  unordered_map이므로 중복된 이름의 경우는 존재하면 안된다..

bool LoadRSCFile_Acc(const i3::string& media_base_path,
	const i3::rc_string& rel_path, RscFileInfoSet& result);
bool LoadRSCFile_Acc(const i3::string& media_base_path,
	const i3::rc_string& rel_path, RscFileInfoSet& result1, RscFileInfoList& result2);


void UpdateAnimPackInfoMapFromRscFileInfoSet(const i3::string& mediaPath, const RscFileInfoSet& set, AnimPackInfoMap& result);

void UpdateAnimPackInfoMapFromRscFileInfoList(const i3::string& mediaPath, const RscFileInfoList& list, AnimPackInfoMap& result);

bool LoadAnimPackRscFile_Acc(const i3::string& mediaPath, const i3::string& rel_path, AnimPackInfoMap& result);



bool FindRscFileInfo(const RscFileInfoSet& set, const i3::string& name, RscFileInfo*& result);
bool FindRscFileInfo_UpperName(const RscFileInfoSet& set, const i3::fixed_string& upper_name, RscFileInfo*& result);


bool FindAnimPackFile(const AnimPackInfoMap& map, const i3::rc_string& i3a_filepath, AnimPackInfos& result);
bool FindAnimPackFile_UpperUnix(const AnimPackInfoMap& map, const i3::fixed_string& i3a_upper_path, AnimPackInfos& result);


////
bool LoadRscFileList_Acc(const i3::string& media_base_path, const i3::rc_string& rel_path, RscFileInfoList& result);
bool LoadAnimationPack_Acc(const i3::string& mediaPath, const i3::rc_string& rel_path, AnimPackInfoMap& result);


