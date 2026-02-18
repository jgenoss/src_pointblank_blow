#include "stdafx.h"
#include "IContentsCrawler.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"

namespace rsc
{
	IContentsCrawler::IContentsCrawler()
	{
	}

	IContentsCrawler::~IContentsCrawler()
	{
	}

	bool IContentsCrawler::operator()()
	{
		if (!__Load())			return false;
		if (!__PreGather())		return false;
		if (!__Gather())		return false;
		if (!__PostGather())	return false;

		return true;
	}

	void IContentsCrawler::SetCallback(ContentsCrawlerCallbackPtr callback)
	{
		if (m_callback.get() != callback.get())
			m_callback = std::move(callback);
	}

	IContentsCrawlerCallback* IContentsCrawler::GetCallback() const
	{
		return m_callback.get();
	}

	void IContentsCrawler::SetCountryName(const std::string& name)
	{
		m_countryName = name;
	}

	const std::string& IContentsCrawler::GetCountryName() const
	{
		return m_countryName;
	}
}