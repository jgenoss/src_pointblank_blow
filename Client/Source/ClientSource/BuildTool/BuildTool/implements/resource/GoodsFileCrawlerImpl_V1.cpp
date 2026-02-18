#include "stdafx.h"
#include "GoodsFileCrawlerImpl_V1.h"
#include <array>
#include "Settings.h"

namespace
{
	std::array<std::pair<const char*, const char*>, 7> g_arrShapeTexRelPathFormatInfo =
	{
		std::make_pair("Head",		"%s/Gui/Item/item_head_%02d.i3i"),
		std::make_pair("Beret",		"%s/Gui/Item/item_Beret_New%02d.i3i"),
		std::make_pair("Chara",		"%s/Gui/Item/item_cha_%02d.i3i"),
		std::make_pair("Mask",		"%s/Gui/Item/item_Mask_%d.i3i"),
		std::make_pair("Count",		"%s/Gui/Item/item_cash_A%d.i3i"),
		std::make_pair("Perido",	"%s/Gui/Item/item_cash_B%d.i3i"),
		std::make_pair("Set",		"%s/Gui/Item/item_cash_set%d.i3i"),
	};

	const Path GetShapeTexRelPath(const char* type, const std::string& strLocaleRelPath, const int idx)
	{
		for (auto &formatInfo : g_arrShapeTexRelPathFormatInfo)
		{
			if (i3::generic_is_iequal(type, std::get<0>(formatInfo)))
			{
				const char* format = std::get<1>(formatInfo);

				i3::stack_string strTexRelPath;
				i3::sprintf(strTexRelPath, format, strLocaleRelPath.c_str(), idx);

				return Path(strTexRelPath.c_str());
			}
		}

		return Path("");
	}

	enum CASHITEM_USAGE_TYPE	// 클라이언트 코드에 정의되어 있음.
	{
		CASHITEM_USAGE_UNKNOWN = -1,
		CASHITEM_USAGE_ALL = CASHITEM_USAGE_UNKNOWN,
		CASHITEM_USAGE_MAINTENANCE = 0,	//	지속성 아이템
		CASHITEM_USAGE_EXPENDABLES = 1,	//	소모성 아이템
		// 1~70 까지는 Expenable이 차지합니다.

		// 71번부터 확장 Maintenance
		CASHITEM_USAGE_EXTEND_MAINTENANCE = 71,

		CASHITEM_USAGE_COUNT
	};
}

namespace rsc
{
	GoodsFileCrawlerImpl_V1::GoodsFileCrawlerImpl_V1(ContentsDataPtr contentsData,
													 const Path& workdir,
													 const Path& PEFRelPath,
													 const std::string& strCountryName)
		: GoodsFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	GoodsFileCrawlerImpl_V1::~GoodsFileCrawlerImpl_V1()
	{}

	bool GoodsFileCrawlerImpl_V1::LoadPef()
	{
		return GoodsFileCrawlerImpl::LoadPef();
	}

	void GoodsFileCrawlerImpl_V1::GatherPefFiles(std::vector<PathPtr>& out)
	{
		GoodsFileCrawlerImpl::GatherPefFiles(out);
	}

	void GoodsFileCrawlerImpl_V1::GatherUITextures(std::vector<PathPtr>& out)
	{
		const int idxNation = IPefable::GetPefNationIdx();

		i3ResourceFileObjectInfo2* currRegistryKey = nullptr;

		const std::string& strSubCategoryName = m_contentsData->strSubcategoryName;

		const std::string strLocaleFolderRelPath = 
			CountrySettings::i()->FindData(GetCountryName())->GetMediaLocaleFolderRelPath().string();

		if (i3::generic_is_iequal("SetGoods", strSubCategoryName))
		{
			i3::rc_wstring wstrTexType;
			if (!(currRegistryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_UITexType")))
				return;

			if (!GetPef()->Readi3RegStringArrayElement(currRegistryKey, idxNation, wstrTexType))
				return;

			if (wstrTexType.empty())
				return;

			i3::rc_string strTexType;	i3::utf16_to_mb(wstrTexType, strTexType);

			INT32 idx = 0;
			if (!(currRegistryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_UITexIndex")))
				return;

			if (!GetPef()->Readi3RegIntArrayElement(currRegistryKey, idxNation, idx))
				return;

			const Path shapeTexRelPath = GetShapeTexRelPath(strTexType.c_str(), strLocaleFolderRelPath, idx);
			if (!shapeTexRelPath.empty())
				out.push_back(std::make_shared<Path>(shapeTexRelPath));
		}
		else if (i3::generic_is_iequal("ShopItem", strSubCategoryName))
		{
			INT32 type;
			if (!(currRegistryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_Type")))
				return;

			if (!GetPef()->Readi3RegIntArrayElement(currRegistryKey, idxNation, type))
				return;

			INT32 idx = 0;
			if (!(currRegistryKey = GetPef()->Findi3RegData(m_currRegistryKey, "_UITexIndex")))
				return;

			if (!GetPef()->Readi3RegIntArrayElement(currRegistryKey, idxNation, idx))
				return;

			std::string strTexTypeName;
			if (type == CASHITEM_USAGE_MAINTENANCE || CASHITEM_USAGE_EXTEND_MAINTENANCE <= type)
			{
				strTexTypeName = "Perido";
			}
			else if (CASHITEM_USAGE_EXPENDABLES < type && type < CASHITEM_USAGE_EXTEND_MAINTENANCE)
			{
				strTexTypeName = "Count";
			}
			else
			{
				I3PRINTLOG(I3LOG_NOTICE, "Goods.pef, ShopItem, invalid value, see '_Type' of %s \n", m_contentsData->strContentsName.c_str());
				return;
			}

			const Path shapeTexRelPath = GetShapeTexRelPath(strTexTypeName.c_str(), strLocaleFolderRelPath, idx);
			if (!shapeTexRelPath.empty())
				out.push_back(std::make_shared<Path>(shapeTexRelPath));
		}
	}

	void GoodsFileCrawlerImpl_V1::GatherMiscs(std::vector<PathPtr>& out)
	{}
}