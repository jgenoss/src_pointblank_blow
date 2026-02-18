#include "stdafx.h"
#include "WeaponFileCrawler.h"
#include "WeaponFileCrawlerImpl.h"
#include "WeaponFileCrawlerImpl_V1.h"
#include "WeaponFileCrawlerImpl_V2.h"
#include "WeaponFileCrawlerImpl_V3.h"

namespace rsc
{
	// 검출되는 리소스 파일 중 i3a 파일의 경우 i3Animpack 으로 대체되므로 제거.
	// 기존 코드를 건드리지 않기 위해 데코레이팅한다.
	class Eliminate_i3a : public FileCrawlerWrapper
	{
	public:
		Eliminate_i3a(IFileCrawler* p) : FileCrawlerWrapper(p) {}
		virtual ~Eliminate_i3a() {}

		virtual bool operator()(std::vector<PathPtr>& out)
		{
			if (!FileCrawlerWrapper::operator()(out))
				return false;

			out.erase(std::remove_if(out.begin(), out.end(),
				[](const PathPtr& src) -> bool
				{
					return i3::generic_is_iequal(src->extension().string(), ".i3a");
				}), 
				out.end());

			return true;
		}
	};

	WeaponFileCrawler::WeaponFileCrawler(WeaponFileCrawlerImpl* impl)
		: m_impl(impl) 
	{}
	
	WeaponFileCrawler::~WeaponFileCrawler() {}
	
	bool WeaponFileCrawler::_Load()
	{
		return m_impl->LoadPef();
	}

	bool WeaponFileCrawler::_Gather(std::vector<PathPtr>& out)
	{
		m_impl->GatherPefFiles(out);
		m_impl->GatherMeshFiles(out);
		m_impl->GatherEffectFiles(out);
		m_impl->GatherSoundFiles(out);
		m_impl->GatherUIFiles(out);
		m_impl->GatherCharaAssocFiles(out);
		m_impl->GatherRSCFiles(out);
		m_impl->GatherMiscs(out);

		return true;
	}

	IFileCrawler* WeaponFileCrawler::Creator_V1(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v1 = 
			new WeaponFileCrawlerImpl_V1(contentsData, workdir, "config/Weapon.pef", strCountryName);
		
		return new Eliminate_i3a(new WeaponFileCrawler(impl_v1));
	}

	IFileCrawler* WeaponFileCrawler::Creator_V2(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v2 =
			new WeaponFileCrawlerImpl_V2(contentsData, workdir, "config/Weapon.pef", strCountryName);

		return new Eliminate_i3a(new WeaponFileCrawler(impl_v2));
	}

	IFileCrawler* WeaponFileCrawler::Creator_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName)
	{
		auto impl_v3 =
			new WeaponFileCrawlerImpl_V3(contentsData, workdir, "config/Weapon.pef", strCountryName);

		return new Eliminate_i3a(new WeaponFileCrawler(impl_v3));
	}
}