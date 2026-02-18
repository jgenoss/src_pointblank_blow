#pragma once

#include <functional>

namespace rsc
{
	class IContentsCrawlerCallback
	{
	public:
		virtual ~IContentsCrawlerCallback() = 0;

		virtual void OnPreGather(const std::string&, const std::string&) = 0;
		virtual void OnGather(const std::string&, const std::string&, const std::string&, const std::string&) = 0;
		virtual void OnPostGather(const std::string&, const std::string&) = 0;
	};

	class ContentsCrawlerCallback_Null : public IContentsCrawlerCallback
	{
	public:
		virtual ~ContentsCrawlerCallback_Null() {}

		virtual void OnPreGather(const std::string&, const std::string&) override {}
		virtual void OnGather(const std::string&, const std::string&, const std::string&, const std::string&) override {}
		virtual void OnPostGather(const std::string&, const std::string&) override {}
	};

	class ContentsCrawlerCallback : public IContentsCrawlerCallback
	{
	public:
		using PreGatherFuncType = std::function < void(const std::string&, const std::string&) >;
		using GatherFuncType = std::function < void(const std::string&, const std::string&, const std::string&, const std::string&) >;
		using PostGatherFuncType = std::function < void(const std::string&, const std::string&) >;

		ContentsCrawlerCallback(const PreGatherFuncType& f1, const GatherFuncType& f2, const PostGatherFuncType& f3);
		virtual ~ContentsCrawlerCallback();

		virtual void OnPreGather(const std::string&, const std::string&) override;
		virtual void OnGather(const std::string&, const std::string&, const std::string&, const std::string&) override;
		virtual void OnPostGather(const std::string&, const std::string&) override;

	private:
		PreGatherFuncType	m_cbPreGather;
		GatherFuncType		m_cbGather;
		PostGatherFuncType	m_cbPostGather;
	};
}