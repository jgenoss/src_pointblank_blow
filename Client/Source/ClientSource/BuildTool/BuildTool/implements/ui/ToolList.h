#pragma once

#include "CustomUGCtrl.h"

namespace ui
{
	class TasksBar;

	class ToolList
	{
	public:
		ToolList(TasksBar* owner);
		virtual ~ToolList();

	public:
		void AdjustLayout(const RECT& rt);
		void AddTool(const std::string& strToolName);
		long GetNumOfTools();
	
	private:
		CustomUGCtrl m_ctrl;
		CPen m_penIvoryBlack;

		friend struct ToolListCallback;
	};
}