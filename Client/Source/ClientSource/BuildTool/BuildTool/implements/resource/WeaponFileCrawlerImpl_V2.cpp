#include "stdafx.h"
#include "WeaponFileCrawlerImpl_V2.h"
#include <boost/lexical_cast.hpp>
#include <array>
#include "RscTool/RSCFileMgr.h"
#include "i3sCrawler.h"
#include <boost/format.hpp>

namespace rsc
{
	WeaponFileCrawlerImpl_V2::WeaponFileCrawlerImpl_V2(ContentsDataPtr contentsData,
													   const Path& workdir, 
													   const Path& PEFRelPath, 
													   const std::string& strCountryName)
		: WeaponFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	WeaponFileCrawlerImpl_V2::~WeaponFileCrawlerImpl_V2()
	{}

	bool WeaponFileCrawlerImpl_V2::LoadPef()
	{
		return WeaponFileCrawlerImpl::LoadPef();
	}

	void WeaponFileCrawlerImpl_V2::GatherPefFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherPefFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherMeshFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherMeshFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherEffectFiles(std::vector<PathPtr>& out)
	{
		i3ResourceFileObjectInfo2* effectRegRootKey = GetPef()->FindChildi3TreeElement(m_currRegistryKey, "Effect");
		if (effectRegRootKey == nullptr)
			return;

		const int idxNation = IPefable::GetPefNationIdx();

		std::array<const char*, 5> arrRegDataName =
		{
			"BulletMarkExt", "CharaHitEffect", "ObjHitEffect", "_FireEffect", "IdleEffect"
		};

		i3ResourceFileObjectInfo2* currRegistryKey = nullptr;
		i3::rc_wstring wstrValue;
		i3::rc_string strValue;
		RscFileInfo* outRscFileInfo = nullptr;

		i3sCrawler i3sCwl;
		dynamic_cast<IFilePathable*>(&i3sCwl)->SetWorkdir(m_workdir);

		bool findOK = false;

		for (auto &currDataName : arrRegDataName)
		{
			currRegistryKey = GetPef()->Findi3RegData(effectRegRootKey, currDataName);
			if (currRegistryKey == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(currRegistryKey, idxNation, wstrValue))
				continue;

			i3::utf16_to_mb(wstrValue, strValue);
			if (strValue.empty())
				continue;

			strValue += ".i3s";

			if (!RSCFileMgr::i()->FindRscFilePath(strValue, outRscFileInfo))
				continue;

			findOK = true;

			out.push_back(std::make_shared<Path>(outRscFileInfo->m_Path.c_str()));

			dynamic_cast<IFilePathable*>(&i3sCwl)->SetRelativePath(outRscFileInfo->m_Path.c_str());
			i3sCwl.operator()(out);
		}

		if (findOK)	// 이펙트 i3s 파일 출력을 위해서는 아래 파일이 필요함.
		{
			out.push_back(std::make_shared<Path>("Effect\\Define\\Default.i3FxList"));
		}
	}

	void WeaponFileCrawlerImpl_V2::GatherSoundFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherSoundFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherUIFiles(std::vector<PathPtr>& out)
	{
		i3ResourceFileObjectInfo2* registryKey = nullptr;
		i3RscPartialFile* rscfile = GetPef();
		const int idxNation = IPefable::GetPefNationIdx();

		// UiPath
		registryKey = rscfile->Findi3RegData(m_currRegistryKey, "UiPath");
		if (registryKey)
		{
			const i3::rc_string strValue = RetrieveRegStringData(idxNation, rscfile, registryKey);

			if (!strValue.empty())
			{
				i3::rc_string strUIRelPath("UIRe/Image/Weapon_detail/");
				strUIRelPath += strValue;

				out.push_back(std::make_shared<Path>(strUIRelPath.c_str()));
			}			
		}

		// _UIShapeIndex : 상점 이미지, 인게임 킬마크.
		registryKey = rscfile->Findi3RegData(m_currRegistryKey, "_UIShapeIndex");
		if (registryKey)
		{
			INT32 shapeIdx = 0;
			if (rscfile->Readi3RegIntArrayElement(registryKey, idxNation, shapeIdx))
			{
				const int shapeCapacity_TGA = 60;

				const int idx_TGA = shapeIdx / (shapeCapacity_TGA*2);

				const int nth = shapeIdx % (shapeCapacity_TGA * 2);

				const char* suffix = (nth < shapeCapacity_TGA) ? "a" : "b";

				std::string str = boost::io::str(boost::format("UIRe/Image/WeaponShape/weapon_select%d_%s.tga") % idx_TGA % suffix);
				out.push_back(std::make_shared<Path>(str.c_str()));

				str = boost::io::str(boost::format("UIRe/Image/Weapon_Kill/Weapon_Kill%d.i3i") % idx_TGA);
				out.push_back(std::make_shared<Path>(str.c_str()));
			}
		}
			
		GatherCommonUIFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherCharaAssocFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherCharaAssocFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherRSCFiles(std::vector<PathPtr>& out)
	{
		WeaponFileCrawlerImpl::GatherRSCFiles(out);
	}

	void WeaponFileCrawlerImpl_V2::GatherMiscs(std::vector<PathPtr>& out)
	{
		i3RscPartialFile* rscfile = GetPef();
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		const int idxNation = IPefable::GetPefNationIdx();

		// ScopeType : Scope 자동화에 따라 pef에서 스코프 경로 추가
		i3ResourceFileObjectInfo2* registryKeySub = rscfile->FindChildi3TreeElement(m_currRegistryKey, "Extension1");
		if (registryKeySub)
		{
			PushbackRegStringData(idxNation, "ScopeType"
				, rscfile, registryKeySub, out);
		}
		
		// _SpecialDamageHud 		
		PushbackRegStringData(idxNation, "_SpecialDamageHud"
			, rscfile, m_currRegistryKey, out);

		// _SpecialDieHud
		PushbackRegStringData(idxNation, "_SpecialDieHud"
			, rscfile, m_currRegistryKey, out);
	}
}