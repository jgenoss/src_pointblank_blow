#include "stdafx.h"
#include "ContentsTableCallback.h"

namespace rsc
{
	IContentsTableCallback::~IContentsTableCallback() {}

	ContentsTableCallback::ContentsTableCallback(const InsertFuncType& f1, const DeleteFuncType& f2, const UpdateFuncType& f3)
	{
		m_insertCallback = f1;
		m_deleteCallback = f2;
		m_updateCallback = f3;
	}

	void ContentsTableCallback::OnInsert(const ContentsDataPtr& contentsData, const std::string& strTableName)
	{
		m_insertCallback(contentsData, strTableName);
	}

	void ContentsTableCallback::OnDelete(const ContentsDataPtr& contentsData, const std::string& strTableName)
	{
		m_deleteCallback(contentsData, strTableName);
	}

	void ContentsTableCallback::OnUpdate(const ContentsDataPtr& contentsData, const std::string& strTableName)
	{
		m_updateCallback(contentsData, strTableName);
	}
}