#pragma once

#include "UIPopupBase.h"

class UIPopupWebBox : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupWebBox, UIPopupBase);
	typedef UIPopupBase base;

public:
	UIPopupWebBox();
	virtual ~UIPopupWebBox();

	virtual void OnCreate( i3GameNode * pParent) override;

	/*
	param[in] pArg1 char*
	param[in] pArg2 bool*
	*/
	virtual bool OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnEntranceEnd() override;

	virtual bool OnExitStart(void) override;

	virtual void OnExitEnd(void) override;

	void	OnQueryJScript();

private: 

	void	_createweb(const char* URL);
	void	_calc_rect( i3::pack::RECT& rc);

private:
	HWND			m_hExplorerhWnd;
	SIZE32			m_WebScreenSize;
	bool			m_bCreatedWebBilling;
	CMsgBoxCallBack m_pCallBackFunc;
	i3UIFrameWnd *	m_pFrame;

	bool			m_bIngameWebButton;
	i3::string		m_sUrl;
};