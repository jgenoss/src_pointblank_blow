#if !defined( __UI_SERVERANDCHANNELBANNER_H__)
#define __UI_SERVERANDCHANNELBANNER_H__

#include "UIFloatingBase.h"

#define MAX_ANNOUNCE					6

class UIServerAndChannelBanner : public UIFloatingBase
{
	I3_CLASS_DEFINE(UIServerAndChannelBanner, UIFloatingBase);

private:
	i3UIImageBoxEx *	m_pImageBox;
	i3UIImageBox*		m_pImgWebError; 

	bool		m_bNoImage;

	INT32		m_ButtonCount;

	INT32		m_announceImageHeight;
	i3Texture *	m_pAnnounceTexture[MAX_ANNOUNCE];
	bool		m_bAnnounceScroll[MAX_ANNOUNCE];
	char *		m_pAnnounceURL[MAX_ANNOUNCE];

	// 공지사항용
	INT32		m_nNoticeIndex;
	REAL32		m_fLocalNoticeTime;
	bool		m_bEnableNoticeText;
	bool		m_bClickedNoticeCaption;
	REAL32		m_fAnnounceFilpElapsedTime;
	REAL32		m_fAnnounceFlipTime;
	INT32		m_nCurrentSelectedIndex;

	// 웹공지 핸들
	HWND			 m_hWebAnnounce;
	i3UIStaticText * m_pVersionText;
	bool			 m_bWebError; 

	i3::vector<i3UITab *>	vecTabList;

	//EsportsBanner
	i3::string		m_strEsportsBannerUrl;

	void		__Init(void);

protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	void		_ShowAnnounceLong(i3Texture* longTexture, INT32 height);
	void		_ShowAnnounceShort(i3Texture * shortTexture);

	void		_SetTabText();

public:
	UIServerAndChannelBanner();
	virtual ~UIServerAndChannelBanner();

	virtual void	OnCreate(i3GameNode * pParent) override;

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void	SetEnable(bool bTrue = true, REAL32 tm = 0.f) override;

	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnEntrancing(REAL32 rDeltaSeconds) override;
	virtual void	OnEntranceEnd(void) override;

	virtual bool	OnExitStart(void) override;
	virtual void	OnExitEnd(void) override;
	virtual void	OnLoadAllScenes() override;

	virtual bool	OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code) override;

	// 이벤트 처리가 없다.
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	// Lua Event
	void			AnnounceButton(INT32 idx);

	// 공지사항을 교체한다.
	void			ChangeAnnounce(INT32 idx);

	void			SetCurrentSelectedIndex(INT32 idx) { m_nCurrentSelectedIndex = idx; }
	char *			GetCurrentBannerURL() { return m_pAnnounceURL[m_nCurrentSelectedIndex]; }
	const char*		GetEsportsBannerURL() { return m_strEsportsBannerUrl.c_str(); }

	// 채널리스트의 슬라이드 아웃이 끝나면 작동 함.
	void			ChannelListAfterProcess();
	void			ShowHideWebAunnounce(bool onoff);

private:

	void			_calc_web_announce_rect(i3::pack::RECT& rc);
	void			_on_off_image_announce(bool onoff);

	void			_OffURLBanner();

	//EsportsBanner용
	void			_SetEsportsBanner();

public:

};

#endif
