#include "stdafx.h"
#include "ContentsStateCallback.h"

namespace rsc
{
	ContentsStateCallback::ContentsStateCallback(const FuncType& f)
	{
		m_callback = f;
	}

	void ContentsStateCallback::OnUpdateState(const ContentsDataPtr& currContentsData, const std::string& strCurrCountryName, const ContentsStatusFlag& status)
	{
		m_callback(currContentsData, strCurrCountryName, status);
	}
}