#include "stdafx.h"
#include "DragdropDataContext.h"

namespace rsc
{
	void DragdropDataContext::SetDatas(const std::vector<ContentsDataPtr>& dataList)
	{
		m_dataList.clear();

		for (auto &data : dataList)
			m_dataList.push_back(data);
	}

	void DragdropDataContext::GetDatas(std::vector<ContentsDataPtr>& outList)
	{
		for (auto &data : m_dataList)
			outList.push_back(data);

		m_dataList.clear();
	}

	void DragdropDataContext::Clear()
	{
		m_dataList.clear();
	}

	bool DragdropDataContext::IsEmpty() const
	{
		return m_dataList.empty();
	}
}