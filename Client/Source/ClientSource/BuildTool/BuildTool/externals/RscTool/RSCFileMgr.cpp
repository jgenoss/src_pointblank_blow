#include "stdafx.h"

#include "RSCFileMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3RscPartialFile.h"
#include "PreprocessMessageCallback.h"

#include <boost/filesystem.hpp>
#include <array>

namespace
{
	enum RSC_TYPE
	{
		RSC_TYPE_GENERAL_I3CHR,
		RSC_TYPE_I3ANIMPACK,
		RSC_TYPE_CHARACTER_I3CHR,
		RSC_MAX_COUNT,
	};

	class RSCFilePaths final
	{
	public:
		RSCFilePaths() 
		{
			Regist(RSC_TYPE_GENERAL_I3CHR, "Effect/Define/Effects_High.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Effect/Define/EffectImage.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Weapon/Weapon.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Chara/CharaTexture.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Equip/Equip.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Equip/EquipTexture.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Weapon.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Character.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Dinosaur.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Interface.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Object.RSC");
			Regist(RSC_TYPE_GENERAL_I3CHR, "Sound/Voice.RSC");

			Regist(RSC_TYPE_I3ANIMPACK, "Weapon/WeaponAnimPack.RSC");
			Regist(RSC_TYPE_I3ANIMPACK, "Chara/AnimPack.RSC");

			Regist(RSC_TYPE_CHARACTER_I3CHR, "Chara/Character.RSC");
		}

		void Regist(const RSC_TYPE type, const char* relPath)
		{
			auto it = m_tbl.find(type);
			if (it == m_tbl.end())
			{
				auto pr = m_tbl.insert(std::map<RSC_TYPE, RelPathListPtr>::value_type(type, std::make_unique<RelPathList>()));
				if (pr.second == false)
					return;

				it = pr.first;
			}

			(it->second)->push_back(relPath);
		}

		using RelPathList = std::vector<std::string>;

		const RelPathList* GetRelPathList(const RSC_TYPE type)
		{
			auto it = m_tbl.find(type);
			if (it == m_tbl.end())
				return false;

			return (it->second).get();
		}

		void GetRelPathAll(RelPathList& out)
		{
			for (int i = 0; i < RSC_MAX_COUNT; i++)
			{
				const RelPathList* list = GetRelPathList((RSC_TYPE)i);
				if (list)
					std::copy(list->begin(), list->end(), std::back_inserter(out));
			}
		}

	private:
		using RelPathListPtr = std::unique_ptr<RelPathList>;
		std::map<RSC_TYPE, RelPathListPtr> m_tbl;
	} g_RSCFilePaths;

	void ReplaceExtension(const std::vector<std::string>& src, const char* ext, std::vector<std::string>& out)
	{
		Path temp;
		out.reserve(src.size());
		for (size_t i = 0; i < src.size(); i++)
		{
			temp = src[i];
			temp.replace_extension(ext);
			out.push_back(temp.string());
		}
	}
}

RSCFileMgr::RSCFileMgr()
{
	

}

RSCFileMgr::~RSCFileMgr()
{


}

i3RSCGen* RSCFileMgr::_LoadRSCGen(const i3::rc_string& workdir, const i3::rc_string& i3RSCGenRelPath)
{
	i3::string stri3RSCGenAbsPath(workdir.c_str());	stri3RSCGenAbsPath += "\\";	stri3RSCGenAbsPath += i3RSCGenRelPath;

	i3RSCGenPtr rscGenerator = std::make_unique<i3RSCGen>();

	if (rscGenerator->OpenRSCFile(stri3RSCGenAbsPath.c_str()))
	{
		i3RSCGen* ptr = rscGenerator.get();

		i3::to_upper(stri3RSCGenAbsPath);
		Path registerKey(stri3RSCGenAbsPath.c_str());	registerKey.make_preferred();

		auto pr = 
			m_i3RSCGenMap.insert(i3RSCGenMap::value_type(registerKey.string(), std::move(rscGenerator)));
		
		if (pr.second == true)
			return ptr;
	}

	return nullptr;
}

void RSCFileMgr::SetMediaPath(const i3::wstring strMediaFolder)//, PreprocessMessageCallback* cb)
{
	m_rscfile_set.m_map.clear();
	m_anipack_map.m_map.clear();
	m_aicontext_animpath_map.m_map.clear();
	m_i3RSCGenMap.clear();

	if (strMediaFolder.empty())
		return;

	i3::string full_path;	
	
	i3::utf16_to_mb(strMediaFolder, full_path);

	const std::vector<std::string>* i3chrRSCList = g_RSCFilePaths.GetRelPathList(RSC_TYPE_GENERAL_I3CHR);
	if (i3chrRSCList)
	{
		for (size_t i = 0; i < i3chrRSCList->size(); i++)
			LoadRSCFile_Acc(full_path, (*i3chrRSCList)[i], m_rscfile_set);
	}

	RscFileInfoList animpack_list;
	const std::vector<std::string>* i3AnimpackRSCList = g_RSCFilePaths.GetRelPathList(RSC_TYPE_I3ANIMPACK);
	if (i3AnimpackRSCList)
	{
		for (size_t i = 0; i < i3AnimpackRSCList->size(); i++)
			LoadRSCFile_Acc(full_path, (*i3AnimpackRSCList)[i], m_rscfile_set, animpack_list);
	}
	UpdateAnimPackInfoMapFromRscFileInfoList(full_path, animpack_list, m_anipack_map);

	RscFileInfoList chara_list;
	const std::vector<std::string>* i3CharaRSCList = g_RSCFilePaths.GetRelPathList(RSC_TYPE_CHARACTER_I3CHR);
	if (i3CharaRSCList)
	{
		for (size_t i = 0; i < i3CharaRSCList->size(); i++)
			LoadRSCFile_Acc(full_path, (*i3CharaRSCList)[i], m_rscfile_set, chara_list);
	}

	i3::utf16_to_mb(strMediaFolder, full_path);
	{
		const size_t num = chara_list.m_path_list.size();

		i3::wliteral_formatter fmt(L"Reading %s...");
		i3::stack_wstring wstr;

		for (size_t i = 0; i < num; ++i)
		{
			const i3::rc_string& path = chara_list.m_path_list[i];
			i3::sprintf(wstr, fmt, path);

			MakeAnimPathNodeMap_Acc(full_path, path, m_aicontext_animpath_map);
		}
	}
}

void RSCFileMgr::LoadRSCGenFiles(const i3::string& strWorkdir)
{
	if (!fs::exists(Path(strWorkdir.c_str())))
		return;

	std::vector<std::string> RSCList;
	g_RSCFilePaths.GetRelPathAll(RSCList);

	std::vector<std::string> RSGList;
	ReplaceExtension(RSCList, ".RSG", RSGList);

	for (size_t i = 0; i < RSGList.size(); i++)
		_LoadRSCGen(strWorkdir, RSGList[i].c_str());
}

bool RSCFileMgr::FindRscFilePath(const i3::string& name, RscFileInfo*& result)
{
	return FindRscFileInfo(m_rscfile_set, name, result);
}

bool RSCFileMgr::FindRscFilePath_UpperName(const i3::fixed_string& upper_name, RscFileInfo*& result)
{
	return FindRscFileInfo_UpperName(m_rscfile_set, upper_name, result);
}

bool RSCFileMgr::FindAnimPackFile( const i3::rc_string& i3a_name, i3::vector<i3::rc_string>& vec_animpackfile)
{
	AnimPackInfos infos;
	bool res = ::FindAnimPackFile(m_anipack_map, i3a_name, infos);
	
	const size_t num = infos.m_vecInfo.size();
	
	vec_animpackfile.reserve( vec_animpackfile.size() + num );

	for (size_t i = 0 ; i < num ; ++i )
	{
		vec_animpackfile.push_back(infos.m_vecInfo[i].m_PackFileName);
	}

	return res;
}

bool RSCFileMgr::FindAnimPackFile_UpperUnix( const i3::fixed_string& i3a_upper_name, i3::vector<i3::rc_string>& vec_animpackfile)
{
	
	AnimPackInfos infos;
	bool res = ::FindAnimPackFile_UpperUnix(m_anipack_map, i3a_upper_name, infos);

	const size_t num = infos.m_vecInfo.size();

	for (size_t i = 0 ; i < num ; ++i )
	{
		vec_animpackfile.push_back(infos.m_vecInfo[i].m_PackFileName);
	}

	return res;
	
}

bool RSCFileMgr::FindLinkedToCharaAI( const i3::fixed_string& node_name, vector_i3Rsc_AnimPathNode_Filename& out)
{
	typedef i3::unordered_map<i3::fixed_string, vector_i3Rsc_AnimPathNode_Filename>::iterator iterator;

	i3::string node_name_upper(node_name);
	i3::to_upper(node_name_upper);
	iterator it = m_aicontext_animpath_map.m_map.find(node_name_upper);
	
	if (it == m_aicontext_animpath_map.m_map.end())
		return false;
	
	out = it->second;			// 원소 복사가 있긴 한데..아직 심각한 수준은 아니므로..일단 놔둔다..

	return true;
}

i3RSCGen* RSCFileMgr::FindOrLoadRSCGen(const i3::rc_string& workdir, const i3::rc_string& i3RSCGenRelPath)
{
	i3::string i3RSCGenAbsUpperPath(workdir);	i3RSCGenAbsUpperPath += "\\";	i3RSCGenAbsUpperPath += i3RSCGenRelPath;
	
	i3::to_upper(i3RSCGenAbsUpperPath);

	Path registerKey(i3RSCGenAbsUpperPath.c_str());
	registerKey.make_preferred();
	registerKey.replace_extension(".RSG");
	
	auto it = m_i3RSCGenMap.find(registerKey.string());
	if (it != m_i3RSCGenMap.end())
		return (it->second).get();

	// 로드가 안 되어 있으므로 로드 시도.
	return _LoadRSCGen(workdir, i3RSCGenRelPath);
}

void RSCFileMgr::GetAllRSCGenRelPaths(std::vector<std::string>& out) const
{
	std::vector<std::string> RSCGenRelPathList;
	g_RSCFilePaths.GetRelPathAll(RSCGenRelPathList);

	for (auto& path : RSCGenRelPathList)
	{
		Path RSCGenPath(path);
		RSCGenPath.replace_extension(".RSG");

		path = RSCGenPath.string();
	}

	out.assign(RSCGenRelPathList.begin(), RSCGenRelPathList.end());
}