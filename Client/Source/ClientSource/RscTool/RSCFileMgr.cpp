#include "stdafx.h"

#include "RSCFileMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3RscPartialFile.h"
#include "PreprocessMessageCallback.h"

RSCFileMgr::RSCFileMgr()
{
	

}

RSCFileMgr::~RSCFileMgr()
{


}

void RSCFileMgr::SetMediaPath(const i3::wstring strMediaFolder, PreprocessMessageCallback* cb)
{
	m_rscfile_set.m_map.clear();
	m_anipack_map.m_map.clear();
	m_aicontext_animpath_map.m_map.clear();

	if (strMediaFolder.empty())
		return;

	i3::string full_path;	
	
	i3::utf16_to_mb(strMediaFolder, full_path);
	
	cb->OnTextMessage(L"Reading Effects_High.RSC");
	LoadRSCFile_Acc(full_path, "Effect/Define/Effects_High.RSC", m_rscfile_set);

	cb->OnTextMessage(L"Reading EffectImage.RSC");
	LoadRSCFile_Acc(full_path, "Effect/Define/EffectImage.RSC", m_rscfile_set);


	cb->OnTextMessage(L"Reading Weapon/Weapon.RSC");
	LoadRSCFile_Acc(full_path, "Weapon/Weapon.RSC"  ,m_rscfile_set);

	{
		RscFileInfoList animpack_list;

		cb->OnTextMessage(L"Reading Weapon/WeaponAnimPack.rsc");
		LoadRSCFile_Acc(full_path, "Weapon/WeaponAnimPack.rsc", m_rscfile_set, animpack_list);

		cb->OnTextMessage(L"Reading Chara/AnimPack.rsc");
		LoadRSCFile_Acc(full_path, "Chara/AnimPack.rsc", m_rscfile_set, animpack_list);

		cb->OnTextMessage(L"Making Weapon & Chara AnimPack Map");
		UpdateAnimPackInfoMapFromRscFileInfoList(full_path, animpack_list, m_anipack_map);
	}


	{
		cb->OnTextMessage(L"Reading Chara/Character.RSC");

		RscFileInfoList chara_list;
		LoadRSCFile_Acc(full_path, "Chara/Character.RSC", m_rscfile_set, chara_list);
		
		i3::utf16_to_mb(strMediaFolder, full_path);
		{
			const size_t num = chara_list.m_path_list.size();
			
			i3::wliteral_formatter fmt(L"Reading %s...");
			i3::stack_wstring wstr;

			for (size_t i = 0; i < num ; ++i )
			{
				const i3::rc_string& path = chara_list.m_path_list[i];
				i3::sprintf(wstr, fmt, path);
				cb->OnTextMessage(wstr.c_str());
				MakeAnimPathNodeMap_Acc(full_path, path, m_aicontext_animpath_map);
			}

		}
	}

	cb->OnTextMessage(L"Reading Chara/CharaTexture.RSC");
	LoadRSCFile_Acc(full_path, "Chara/CharaTexture.RSC", m_rscfile_set);
	
	cb->OnTextMessage(L"Reading Equip/Equip.RSC");
	LoadRSCFile_Acc(full_path, "Equip/Equip.RSC", m_rscfile_set);	//없을수 있음..

	cb->OnTextMessage(L"Reading Equip/EquipTexture.RSC");
	LoadRSCFile_Acc(full_path, "Equip/EquipTexture.RSC", m_rscfile_set);	// 없을수 있음..


	cb->OnTextMessage(L"Reading Sound/Weapon.rsc");
	LoadRSCFile_Acc(full_path, "Sound/Weapon.rsc", m_rscfile_set);

	cb->OnTextMessage(L"Reading Sound/Character.RSC");
	LoadRSCFile_Acc(full_path, "Sound/Character.RSC", m_rscfile_set);

	cb->OnTextMessage(L"Reading Sound/Dinosaur.RSC");
	LoadRSCFile_Acc(full_path, "Sound/Dinosaur.RSC", m_rscfile_set);

	cb->OnTextMessage(L"Reading Sound/Interface.RSC");
	LoadRSCFile_Acc(full_path, "Sound/Interface.RSC", m_rscfile_set);

	cb->OnTextMessage(L"Reading Sound/Object.RSC");
	LoadRSCFile_Acc(full_path, "Sound/Object.RSC", m_rscfile_set);

	cb->OnTextMessage(L"Reading Sound/Voice.RSC");
	LoadRSCFile_Acc(full_path, "Sound/Voice.RSC", m_rscfile_set);


	
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

	iterator it = m_aicontext_animpath_map.m_map.find(node_name);
	
	if (it == m_aicontext_animpath_map.m_map.end())
		return false;
	
	out = it->second;			// 원소 복사가 있긴 한데..아직 심각한 수준은 아니므로..일단 놔둔다..

	return true;
}
