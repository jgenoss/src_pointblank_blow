#include "stdafx.h"
#include "WeaponFileCrawlerImpl.h"
#include "RscTool/RSCFileMgr.h"
#include "RscTool/i3RscPartialFile.h"
#include "i3sCrawler.h"
#include "i3chrCrawler.h"
#include <tuple>
#include <array>
#include "FolderFilesCrawler.h"

// rsc tool 에서 그대로 가져옴.
void CollectAnimPathRecursive(const i3Rsc_AnimPathNode* n, i3::vector<i3::rc_string>& out)
{
	const size_t num_path = n->m_pathlist.size();

	for (size_t i = 0; i < num_path; ++i)
	{
		const i3::rc_string& strPath = n->m_pathlist[i];
		out.push_back(strPath);
	}

	const size_t num_child = n->m_childlist.size();

	for (size_t i = 0; i < num_child; ++i)
	{
		CollectAnimPathRecursive(n->m_childlist[i].get(), out);
	}
}

namespace rsc
{
	const i3::rc_string RetrieveRegStringData(int idxNation, i3RscPartialFile* rscfilePef
		, i3ResourceFileObjectInfo2* registryKey)
	{

		i3::rc_wstring wstrValue = _T("");
		i3::rc_string strValue = "";

		if (rscfilePef->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue))
		{
			i3::utf16_to_mb(wstrValue, strValue);
		}

		return strValue;
	}

	bool PushbackRegStringData(int idxNation, const char* regString
		, i3RscPartialFile* rscfilePef
		, i3ResourceFileObjectInfo2* currRegistryKey
		, std::vector<PathPtr>& out)
	{
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		registryKey = rscfilePef->Findi3RegData(currRegistryKey, regString);

		if (registryKey)
		{
			const i3::rc_string& strValue = RetrieveRegStringData(idxNation, rscfilePef, registryKey);

			if (true != strValue.empty())
			{
				out.push_back(std::make_shared<Path>(strValue.c_str()));
				return true;
			}
		}

		return false;
	}

	WeaponFileCrawlerImpl::WeaponFileCrawlerImpl(ContentsDataPtr contentsData, 
												 const Path& workdir,
												 const Path& PEFRelpath,
												 const std::string& strCountryName)
		: IPefable(workdir, PEFRelpath, strCountryName), m_contentsData(contentsData), m_workdir(workdir)
	{}

	WeaponFileCrawlerImpl::~WeaponFileCrawlerImpl() {}

	bool WeaponFileCrawlerImpl::LoadPef()
	{
		if (m_contentsData.get() == nullptr)
			return false;

		if (!IPefable::OpenPef())
			return false;

		if (IPefable::GetPefNationIdx() == -1)
			return false;

		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegKeyRoot();
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, "WeaponList");
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, "Weapon");
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strSubcategoryName.c_str());
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strContentsName.c_str());
		if (registryKey == nullptr)
			return false;

		m_currRegistryKey = registryKey;

		return true;
	}

	void WeaponFileCrawlerImpl::GatherPefFiles(std::vector<PathPtr>& out)
	{
		Path pefRelPath(IPefable::GetRelativePath());

		out.push_back(std::make_shared<Path>(pefRelPath));	// weapon pef 파일.
		out.push_back(std::make_shared<Path>(pefRelPath.replace_extension(".i3RegXML")));	// weapon.i3RegXML 파일.
	}

	void WeaponFileCrawlerImpl::GatherMeshFiles(std::vector<PathPtr>& out)
	{
		i3ResourceFileObjectInfo2* registryKey = nullptr;
		i3::rc_wstring wstrFileName;
		i3::rc_string strFileName;

		const int idxNation = GetPefNationIdx();

		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_ResName");
		if (registryKey)
		{
			if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrFileName))
			{
				i3::utf16_to_mb(wstrFileName, strFileName);

				strFileName += ".i3CHR";

				RscFileInfo* outRscFileInfo = nullptr;
				if (RSCFileMgr::i()->FindRscFilePath(strFileName, outRscFileInfo))
				{
					out.push_back(std::make_shared<Path>(outRscFileInfo->m_Path.c_str()));

					i3chrCrawler i3chrCwl;
					i3chrCwl.SetWorkdir(m_workdir);
					i3chrCwl.SetRelativePath(outRscFileInfo->m_Path.c_str());
					i3chrCwl.operator()(out);
				}
			}
		}

		// 메시 공유 파일. pef에 i3s 확장자 파일명이 명시적으로 적혀있다.
		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_ResName_I3S");
		if (registryKey)
		{
			if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrFileName))
			{
				strFileName.clear();
				i3::utf16_to_mb(wstrFileName, strFileName);

				GatherMeshSharingAssocFiles(strFileName.c_str(), out);
			}
		}
		else // pef에 없지만 공유 가능성 있음. 폴더 내 i3s 파일 이름 참조하여 공유하는 케이스.
		{
			if ((registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_ResName")))
			{
				if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrFileName) && !wstrFileName.empty())
				{
					strFileName.clear();
					i3::utf16_to_mb(wstrFileName, strFileName);
					strFileName += ".I3S";

					GatherMeshSharingAssocFiles(strFileName.c_str(), out);
				}
			}
		}
	}

	void WeaponFileCrawlerImpl::GatherSoundFiles(std::vector<PathPtr>& out)
	{
		std::array<std::tuple<const char*, const char*>, 3> registryDataList =
		{
			std::make_tuple("_FireSound",		".wav"),
			std::make_tuple("_FireSoundExt",	".wav"),
			std::make_tuple("_SwapSound",		".wav")
		};

		RscFileInfo* outRscFileInfo = nullptr;
		i3ResourceFileObjectInfo2* registryKey = nullptr;
		i3::rc_wstring wstrFileName;
		i3::rc_string strFileName;

		const int idxNation = GetPefNationIdx();

		Path tempRelPath;
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetIncludeSubFolder(false);

		for (const auto& registryData : registryDataList)
		{
			const char* registryDataName = std::get<0>(registryData);
			if ((registryKey = GetPef()->Findi3RegData(m_currRegistryKey, registryDataName)) == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrFileName))
				continue;

			strFileName.clear();
			i3::utf16_to_mb(wstrFileName, strFileName);

			const char* ext = std::get<1>(registryData);
			strFileName += ext;

			if (!RSCFileMgr::i()->FindRscFilePath(strFileName, outRscFileInfo))
				continue;

			out.push_back(std::make_shared<Path>(outRscFileInfo->m_Path.c_str()));

			// Secondary 사운드 파일과 같은 경우 PB 소스에 하드코딩되어 있으나 대부분 위에서 추출된
			// 파일과 동일한 경로에 존재하므로 폴더 내 .wav 파일들을 전부 긁어 모은다.
			tempRelPath = outRscFileInfo->m_Path.c_str();
			folderCwl.SetRelativePath(tempRelPath.parent_path());
			folderCwl.operator()(out);
		}
	}

	void WeaponFileCrawlerImpl::GatherCharaAssocFiles(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		i3ResourceFileObjectInfo2* info = nullptr;

		if ((info = GetPef()->Findi3RegData(m_currRegistryKey, "LinkedToCharaAI")) == nullptr)	// 무기 연관 캐릭터 애니메이션 관련.
			return;

		const int idxNation = GetPefNationIdx();
		if (!GetPef()->Readi3RegStringArrayElement(info, idxNation, wstrValue))
			return;

		i3::rc_string value;
		i3::utf16_to_mb(wstrValue, value);

		vector_i3Rsc_AnimPathNode_Filename animpathNodes;
		if (!RSCFileMgr::i()->FindLinkedToCharaAI(value, animpathNodes))
			return;

		for (const auto& apnode : animpathNodes)
		{
			const i3Rsc_AnimPathNode_Filename* nodeptr = apnode.get();

			// i3CHR 검색.
			RscFileInfo* RSCInfo = nullptr;
			if (RSCFileMgr::i()->FindRscFilePath(nodeptr->m_filename.c_str(), RSCInfo))
			{
				const Path i3RSCRelPath(RSCInfo->m_RSCFileName.c_str());		// ".RSC" 파일 상대 경로.
				
				Path i3RSCGenRelPath(RSCInfo->m_RSCFileName.c_str());
				i3RSCGenRelPath.replace_extension(".RSG");	// ".RSG" 파일 상대 경로.

				i3RSCGen* i3RSCGenPtr = RSCFileMgr::i()->FindOrLoadRSCGen(GetWorkdir().string().c_str(), i3RSCGenRelPath.string());
				
				i3::vector<RSCNode*> RSCNodeList;
				if (i3RSCGenPtr && i3RSCGenPtr->FindByFileName(nodeptr->m_filename.c_str(), RSCNodeList))
				{
					for (const auto& RSCNode : RSCNodeList)
					{
						out.push_back(std::make_shared<Path>(RSCNode->GetRelativePath().c_str()));
					}
				}
			}

			// i3Animpack 검색.
			i3::vector<i3::rc_string> animFileList;
			for (auto &node : nodeptr->m_nodelist)
			{
				CollectAnimPathRecursive(node.get(), animFileList);

				for (auto &animFile : animFileList)
				{
					out.push_back(std::make_shared<Path>(animFile.c_str()));

					i3::vector<i3::rc_string> animpackRelPathList;
					if (!RSCFileMgr::i()->FindAnimPackFile(animFile, animpackRelPathList))
						continue;

					for (auto &animpackRelPath : animpackRelPathList)
						out.push_back(std::make_shared<Path>(animpackRelPath.c_str()));
				}
			}
		}
	}

	void WeaponFileCrawlerImpl::GatherRSCFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("Weapon/Weapon.RSC"));
		out.push_back(std::make_shared<Path>("Weapon/Weapon.RSG"));
		out.push_back(std::make_shared<Path>("Weapon/WeaponAnimPack.RSC"));
		out.push_back(std::make_shared<Path>("Weapon/WeaponAnimPack.RSG"));
		out.push_back(std::make_shared<Path>("Chara/AnimPack.RSC"));
		out.push_back(std::make_shared<Path>("Chara/AnimPack.RSG"));
		out.push_back(std::make_shared<Path>("Sound/Weapon.RSC"));
		out.push_back(std::make_shared<Path>("Sound/Weapon.RSG"));
	}

	bool WeaponFileCrawlerImpl::GatherMeshSharingAssocFiles(const std::string& stri3sRelPath, std::vector<PathPtr>& out)
	{
		const i3SGTexChangeInfo* texChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(stri3sRelPath.c_str());
		if (texChangeInfo)
		{
			i3::rc_string strOrigi3sRelPath = i3SGTexChangeManager::i()->GetRefi3sFilename(texChangeInfo);	// 원본 메시 파일.
			out.push_back(std::make_shared<Path>(strOrigi3sRelPath.c_str()));

			i3::vector<i3::rc_string> strTexRelPathList;
			i3SGTexChangeManager::GatherAllTargetTexturePaths(texChangeInfo, strTexRelPathList);

			for (const auto& strTexRelPath : strTexRelPathList)
				out.push_back(std::make_shared<Path>(strTexRelPath.c_str()));

			// 원본 i3s 파일 대상 재검색.
			i3sCrawler i3sCwl;	
			i3sCwl.SetWorkdir(GetWorkdir());
			i3sCwl.SetRelativePath(strOrigi3sRelPath.c_str());
			i3sCwl.operator()(out);

			// pef 파일.
			Path PEFRelPath("config/texture_change_Weapon.pef");
			out.push_back(std::make_shared<Path>(PEFRelPath));
			out.push_back(std::make_shared<Path>(PEFRelPath.replace_extension(".i3RegXML")));

			return true;
		}

		return false;
	}

	void WeaponFileCrawlerImpl::GatherCommonUIFiles(std::vector<PathPtr>& out)
	{
	}

}