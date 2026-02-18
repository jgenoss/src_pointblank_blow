#pragma once

namespace rsc
{
	class IContentsCrawlerCallback;

	using ContentsCrawlerCallbackPtr = std::unique_ptr < IContentsCrawlerCallback > ;
	
	class IContentsCrawler
	{
	public:
		IContentsCrawler();
		virtual ~IContentsCrawler();

		bool operator()();

		void SetCallback(ContentsCrawlerCallbackPtr callback);
		IContentsCrawlerCallback* GetCallback() const;

		void SetCountryName(const std::string& name);
		const std::string& GetCountryName() const;

	private:
		virtual bool __Load() = 0;
		virtual bool __PreGather() = 0;
		virtual bool __Gather() = 0;
		virtual bool __PostGather() = 0;

		ContentsCrawlerCallbackPtr m_callback = nullptr;
		std::string m_countryName;
	};
}