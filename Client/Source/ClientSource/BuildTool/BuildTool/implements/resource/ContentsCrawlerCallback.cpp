#include "stdafx.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	IContentsCrawlerCallback::~IContentsCrawlerCallback() {}

	ContentsCrawlerCallback::ContentsCrawlerCallback(const PreGatherFuncType& f1, const GatherFuncType& f2, const PostGatherFuncType& f3)
	{
		m_cbPreGather = f1;
		m_cbGather = f2;
		m_cbPostGather = f3;
	}

	ContentsCrawlerCallback::~ContentsCrawlerCallback() 
	{}

	void ContentsCrawlerCallback::OnPreGather(const std::string& countryName, const std::string& category)
	{
		m_cbPreGather(countryName, category);
	}

	void ContentsCrawlerCallback::OnGather(const std::string& countryName,
		const std::string& name, const std::string& category, const std::string& subCategory)
	{
		m_cbGather(countryName, name, category, subCategory);
	}

	void ContentsCrawlerCallback::OnPostGather(const std::string& countryName, const std::string& category)
	{
		m_cbPostGather(countryName, category);
	}
}