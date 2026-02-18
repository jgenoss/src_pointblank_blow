#pragma once

#include <functional>
#include "ContentsDefines.h"
#include "Settings.h"

namespace rsc
{	
	class IFileCrawler;
	class IContentsStateCallback;
	using ContentsStateCallbackPtr = std::unique_ptr < IContentsStateCallback >;

	class ContentsStateChecker final
	{
	public:
		ContentsStateChecker(ContentsDataPtr contentsData, CountryDataPtr countryData);
		~ContentsStateChecker();

		void operator()() const;

		void SetCallback(ContentsStateCallbackPtr callback);

	private:
		ContentsDataPtr m_currContentsData;
		const CountryDataPtr m_currCountryData;
		ContentsStateCallbackPtr m_callback = nullptr;
	};
}