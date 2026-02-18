#pragma once

#include "CustomUGCtrl.h"

namespace ui
{
	class TasksBar;

	class LiveCountryList
	{
	public:
		LiveCountryList(TasksBar* owner);
		virtual ~LiveCountryList();

	public:
		void AdjustLayout(const RECT& rt);
		void AddCountry(const std::string& strCountryName);
		void DelCountry(const std::string& strCountryName);
		void Clear();

	private:
		CustomUGCtrl m_ctrl;
		CPen m_penIvoryBlack;

		friend struct LiveCountryListCallback;
	};
}