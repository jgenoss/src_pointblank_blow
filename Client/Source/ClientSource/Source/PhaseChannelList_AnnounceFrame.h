#ifndef		__PHASE_SERVER_SERVER_ANNOUNCE_FRAME_H__
#define		__PHASE_SERVER_SERVER_ANNOUNCE_FRAME_H__


#define MAX_ANNOUNCE_BUTTON				5


class	CPhaseChannelList;
class	CPhaseChannelList_AnnounceFrame
{
public:
	CPhaseChannelList_AnnounceFrame();
	~CPhaseChannelList_AnnounceFrame();


public:
	void	SetEnable(BOOL Enable);
	BOOL	OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	void	OnInitControl(void);
	void	OnUpdate(RT_REAL32 rDeltaSeconds);
	void	OnWheelUpdate(INT32 scrollby);
	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	BOOL	OnSliding(RT_REAL32 rDeltaSeconds, CPhaseChannelList * pOwner);

	void	OnEntranceStart(void);
	void	OnEntranceEnd(CPhaseChannelList * pOwner);
	void	OnExitStart(CPhaseChannelList * pOwner);
	void	OnExitEnd(void);

	void 	OnUpdateAnnounceImage(REAL32 rDeltaSeconds);
	void 	ShowAnnounceShort(i3Texture* shortTexture);
	void 	ShowAnnounceLong(i3Texture* longTexture,INT32 height);
	void	OnAnnounceButton(INT32 announceIdx);

	void	CloseAnnouce();
private:
	i3GuiStatic* m_pAnnounceWin;
	REAL32 m_rAnnounceWinX;
	i3GuiEditBox* m_pAnnouceCaption;
	i3GuiButton* m_pAnnounce[MAX_ANNOUNCE_BUTTON];

	i3GuiStatic *	m_pImageFrame;
	i3GuiStatic *	m_pScrollFrame;
	i3GuiList *		m_pScrollList;

	i3GuiStatic* m_pDummyAnnounceShort;
	i3GuiStatic* m_pDummyAnnounceLong;

	INT32 m_announceImageHeight;
	i3Texture* m_pAnnounceTexture[MAX_ANNOUNCE_BUTTON];
	BOOL m_bAnnounceScroll[MAX_ANNOUNCE_BUTTON];


	// 공지사항용
	INT32	m_nNoticeIndex;
	REAL32	m_fLocalNoticeTime;
	BOOL	m_bEnableNoticeText;
	BOOL	m_bClickedNoticeCaption;
	REAL32	m_fAnnounceFilpElapsedTime;
	REAL32	m_fAnnounceFlipTime;
	INT32	m_nCurrentSelectedIndex;

	// 웹공지 핸들
	HWND	m_hWebAnnounce;

};

#endif	// __PHASE_SERVER_SERVER_ANNOUNCE_FRAME_H__

