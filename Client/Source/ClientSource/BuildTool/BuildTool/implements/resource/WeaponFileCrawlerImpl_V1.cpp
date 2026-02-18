#include "stdafx.h"
#include "WeaponFileCrawlerImpl_V1.h"
#include <boost/lexical_cast.hpp>
#include "RscTool/RSCFileMgr.h"
#include "i3sCrawler.h"

namespace rsc
{

	WeaponFileCrawlerImpl_V1::WeaponFileCrawlerImpl_V1(ContentsDataPtr contentsData,
													   const Path& workdir,
													   const Path& pefRelPath, 
													   const std::string& countryName)
		: WeaponFileCrawlerImpl(contentsData, workdir, pefRelPath, countryName)
	{}

	WeaponFileCrawlerImpl_V1::~WeaponFileCrawlerImpl_V1()
	{}

	bool WeaponFileCrawlerImpl_V1::LoadPef()
	{
		return WeaponFileCrawlerImpl::LoadPef();
	}

	void WeaponFileCrawlerImpl_V1::GatherPefFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherPefFiles(out);
	}

	void WeaponFileCrawlerImpl_V1::GatherMeshFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherMeshFiles(out);
	}

	void WeaponFileCrawlerImpl_V1::GatherEffectFiles(std::vector<PathPtr>& out)
	{
		std::array<const char*, 6> registryDataNameList =
		{
			"_FireEffect", "_FireEffect1", "_FireEffect2", "_FireEffectExt", "CharaHitEffect", "IdleEffect"
		};

		RscFileInfo* outRscFileInfo = nullptr;
		i3ResourceFileObjectInfo2* registryKey = nullptr;
		i3::rc_wstring wstrFileName;
		i3::rc_string strFileName;

		i3sCrawler i3sCwl;
		i3sCwl.SetWorkdir(m_workdir);

		const int idxNation = GetPefNationIdx();
		bool findOK = false;

		for (auto &registryDataName : registryDataNameList)
		{
			if ((registryKey = GetPef()->Findi3RegData(m_currRegistryKey, registryDataName)) == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrFileName))
				continue;

			strFileName.clear();
			i3::utf16_to_mb(wstrFileName, strFileName);

			strFileName += ".i3S";

			if (!RSCFileMgr::i()->FindRscFilePath(strFileName, outRscFileInfo))
				continue;

			findOK = true;

			out.push_back(std::make_shared<Path>(outRscFileInfo->m_Path.c_str()));

			i3sCwl.SetRelativePath(outRscFileInfo->m_Path.c_str());
			i3sCwl(out);
		}

		if (findOK)	// 이펙트 i3s 파일 출력을 위해서는 아래 파일이 필요함.
		{
			out.push_back(std::make_shared<Path>("Effect\\Define\\Default.i3FxList"));
		}
	}

	void WeaponFileCrawlerImpl_V1::GatherSoundFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherSoundFiles(out);
	}

	void WeaponFileCrawlerImpl_V1::GatherUIFiles(std::vector<PathPtr>& out)
	{
		i3RscPartialFile* rscfile = GetPef();

		const int idxNation = IPefable::GetPefNationIdx();		

		// UiPath
		PushbackRegStringData(idxNation, "UiPath"
			, rscfile, m_currRegistryKey, out);

		// _UIShapeIndex : 상점 shape.
		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_UIShapeIndex");
		if (registryKey)
		{
			INT32 shapeIdx = 0;
			if (rscfile->Readi3RegIntArrayElement(registryKey, idxNation, shapeIdx))
			{
				const int i3iShapeCapacity = 88;
				const int idx = shapeIdx / i3iShapeCapacity;

				// Weapon_select, Weapon_kill i3i 파일.
				std::array<const char*, 2> arri3iBasePath =
				{
					"Gui/WeaponShape/weapon_select",
					"Gui/WeaponShape/Weapon_Kill",
				};

				std::string postfix(std::to_string(idx));	postfix += ".i3i";

				std::string stri3iRelPath;
				for (auto &curri3iBasePath : arri3iBasePath)
				{
					stri3iRelPath = curri3iBasePath;
					stri3iRelPath += postfix;

					out.push_back(std::make_shared<Path>(stri3iRelPath.c_str()));
				}
			}
		}

		// ScopeType : Scope 자동화에 따라 pef에서 스코프 경로 추가
		PushbackRegStringData(idxNation, "ScopeType"
			, rscfile, m_currRegistryKey, out);		

		// _SpecialDamageHud 
		PushbackRegStringData(idxNation, "_SpecialDamageHud"
			, rscfile, m_currRegistryKey, out);	

		// _SpecialDieHud
		PushbackRegStringData(idxNation, "_SpecialDieHud"
			, rscfile, m_currRegistryKey, out);
		
		GatherCommonUIFiles(out);
	}

	void WeaponFileCrawlerImpl_V1::GatherCharaAssocFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherCharaAssocFiles(out);
	}

	void WeaponFileCrawlerImpl_V1::GatherRSCFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherRSCFiles(out);
	}
}