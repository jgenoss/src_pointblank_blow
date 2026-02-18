#ifndef _PhaseServer_h
#define _PhaseServer_h

#include "PhaseBase.h"
#include "StageDef.h"
#include "PhaseChannelList_AnnounceFrame.h"




#define MAX_CHANNEL_LABEL				(5)
#define MAX_CHANNEL_CONTROL_COUNT		15
#define MAX_SERVER_SELECT_NUMBER_COUNT	20


struct ChannelSlot
{
	ChannelSlot();

	INT32 Index;
	i3GuiStatic* _pColumn;
	i3GuiStatic* _pSlot;
	i3GuiEditBox* _pNumber;
	i3GuiEditBox* _pChannel;
	i3GuiStatic* _pGage;
};

class CMessageBox;



class CPhaseChannelList: public CPhaseBase
{
	I3_CLASS_DEFINE(CPhaseChannelList);
public:
	CPhaseChannelList();
	virtual ~CPhaseChannelList();

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual void OnPreGuiNotify(void);
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnEntranceStart(void);
	virtual BOOL OnEntrancing(RT_REAL32 rDeltaSeconds);
	virtual void OnEntranceEnd(void);
	virtual void OnExitStart(void);
	virtual BOOL OnExiting(RT_REAL32 rDeltaSeconds);
	virtual void OnExitEnd(void);
	virtual void OnWheelUpdate(INT32 scrollby);

	virtual void OnChangeVideo(i3VideoInfo* pInfo);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	virtual	void OnSubExitKey(void);
	static void CbExitNormal(void* pThis,INT32 nParam);

	void OnSelect(INT32 idxSlot);
	void OnFocus(INT32 idxSlot);
	void OnConnect(INT32 idxSlot);
	void OnConnect(void);
	void OnRefresh(void);
	virtual	BOOL OnInviteMove(void);


private:
	void _LimitRefresh(REAL32 rDeltaSeconds);
	void _LimitConnect(REAL32 rDeltaSeconds);
	void _WaitConnect(REAL32 rDeltaSeconds);

	void _Refresh(void);
	void _CloseWaitMessageBox(void);
	BOOL _CheckChannelLimit(INT32 channelIdx);

	void _UpdateChannelLabel(void);
	void _SetServerEnv(INT32 nServer, INT32 nChannel, INT32 nChannelState);

	void _EhFailConnect(void);
	void _EhStageError(INT32 Event,INT32 Arg);

	void _LoadServerTextFromFile(void);
	void	PrintMessageForChannelEnterError(ChannelType channelType);

	const char *	GetCategoryText(ChannelType Type);

public:
	static void CbRefleshServerList(void* pThis,INT32 nParam);
	void BackToServerList();

	void	InputDisable(BOOL Val);

private:
	i3GuiStatic* m_pChannelGui;
	i3GuiStatic* m_pChannelWin;
	i3GuiEditBox* m_pChannelCaption;

	// 서버 페이지 들어가면서 의미 없는 컨트롤이 될것입니다.
	i3GuiButton* m_pPrev;
	i3GuiEditBox* m_pPrevCaption;

	i3GuiButton* m_pNext;
	i3GuiEditBox* m_pNextCaption;

	i3GuiButton* m_pEnter;
	i3GuiEditBox* m_pEnterCaption;

	i3GuiButton* m_pRefresh;
	i3GuiEditBox* m_pRefreshCaption;

	i3GuiButton * m_pGotoServerList;		// ssGoToServerFrame


	ChannelSlot m_ChannelSlot[MAX_CHANNEL_CONTROL_COUNT];
	i3GuiStatic* m_pSelect;
	i3GuiStatic* m_pFocus;

	// 채널리스트의 제목같은것.
	// 고수 채널 / 초보 채널 등 이런 설명을 담는 컨트롤
	// 프로그래밍적으로 위치가 변경된다.
	i3GuiStatic* m_pChannelLabel[MAX_CHANNEL_LABEL];
	i3GuiEditBox* m_pChannelLabelKey[MAX_CHANNEL_LABEL];

	i3GuiEditBox* m_pServerSelectNumber[MAX_SERVER_SELECT_NUMBER_COUNT];

	i3Texture* m_pVerifiTexture;
	i3Sprite2D* m_pVerifiSprite;

	INT32 m_nLastedSelectSlot;

	REAL32 m_fRefreshClickElapseTime;
	REAL32 m_fConnectClickElapseTime;
	REAL32 m_fWaitDelayTime;
	BOOL m_bWaitConnect;
	CMessageBox* m_pMBConnectWait;


	REAL32 m_rChannelWinX;

	// 채널 관련
	INT32	m_nServerChannelIdx;
	char	m_szServerText[SERVERSELECT_MAINTYPE_COUNT][SERVERSELECT_SUBTYPE_COUNT][256];



	CPhaseChannelList_AnnounceFrame		m_AnnounceFrame;		// 공지창

	INT32			m_LogInConnectFailedCount;

	HWND	m_hwnd;
};

#endif //_PhaseServer_h
