#pragma once

#include "LoadingProgressMsgBox.h"
//#include "SplashScreenEx.h"

namespace ui
{
	class ILoadingProgressMsgCallback
	{
	public:
		virtual ~ILoadingProgressMsgCallback() {}
		virtual void OnTextMessage(const std::wstring& wstrText) {};
		virtual void OnGaugeProgress(const int progress) {}
	};

	class LoadingProgressMsgCallback final : public ILoadingProgressMsgCallback
	{
	public:
		LoadingProgressMsgCallback();
		virtual ~LoadingProgressMsgCallback();

		void OnTextMessage(const std::wstring& wstrText) override;
		void OnGaugeProgress(const int progress);
		void MoveWindow(const RECT& rt);

	private:
		LoadingProgressMsgBox m_dlg;
		//CSplashScreenEx m_splash;
	};
}