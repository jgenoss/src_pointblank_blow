#pragma once

#include <vector>
#include "ContentsDefines.h"

namespace rsc
{
	class DragdropDataContext : public i3::shared_ginst < DragdropDataContext >
	{
	public:
		void SetDatas(const std::vector<ContentsDataPtr>& dataList);
		void GetDatas(std::vector<ContentsDataPtr>& outList);
		void Clear();
		bool IsEmpty() const;
		
	private:
		std::vector<ContentsDataPtr> m_dataList;
	};
}