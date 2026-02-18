#pragma once

#include "ContentsDefines.h"

namespace rsc
{
	class ResourceController;

	class IContentsStateCallback
	{
	public:
		virtual ~IContentsStateCallback() {}

		virtual void OnUpdateState(const ContentsDataPtr& currContentsData, const std::string& strCurrCountryName, const ContentsStatusFlag& contentsStatus) = 0;
	};

	class ContentsStateCallback_Null : public IContentsStateCallback
	{
	public:
		virtual ~ContentsStateCallback_Null() {}

		virtual void OnUpdateState(const ContentsDataPtr& currContentsData, const std::string& strCurrCountryName, const ContentsStatusFlag& contentsStatus) {}
	};

	class ContentsStateCallback : public IContentsStateCallback
	{
	public:
		using FuncType = std::function< void(const ContentsDataPtr&, const std::string&, const ContentsStatusFlag&) >;

		ContentsStateCallback(const FuncType& f);
		virtual ~ContentsStateCallback() {}

		virtual void OnUpdateState(const ContentsDataPtr& currContentsData, const std::string& strCurrCountryName, const ContentsStatusFlag& contentsStatus);

	private:
		FuncType m_callback;
	};
}