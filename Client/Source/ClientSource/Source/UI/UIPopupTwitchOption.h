#if !defined( __UI_POPUP_TWITCHOPTION_H__)
#define __UI_POPUP_TWITCHOPTION_H__

#include "UIPopupBase.h"
#include "GameThread.h"
#include "UILoadingIcon.h"

#include "ProgressEdit.h"

enum { THREAD_PROCESS_START, THREAD_PROCESS_FINISH };

class TwitchLoadingThread : public CGameThread
{
	I3_CLASS_DEFINE( TwitchLoadingThread, CGameThread);
private :
	
public:
	TwitchLoadingThread();
	virtual ~TwitchLoadingThread();
	
	virtual void Run( void *pData);
	
	bool m_bStart;
	bool m_bStop;
	bool m_bStartResult;
	bool m_bStopResult;

	INT32 m_iState;
};

class UIPopupTwitchOption : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupTwitchOption, UIPopupBase);
	
	enum{ MIKE_VOL, SPEAKER_VOL, MAX_VOL };

	struct Volume
	{
		bool enable;
		IntProgressEdit edit;
		int vol;
	};

public:
	void clicked(INT32 idx);
	void clickedlist(INT32 cbidx, INT32 idx);
	void clickedvolume(INT32 volidx, INT32 idx);
public:

	UIPopupTwitchOption();
	virtual ~UIPopupTwitchOption();

	//가상함수
	//========================================================

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void	OnEntranceEnd(void) override;
	virtual void	OnLoadAllScenes() override;

	virtual void	OnUnloadAllScenes() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;

	virtual bool	OnKey_Enter( void) override;

	//========================================================

private:
	void Apply_Game();
	void Apply_Lua();

	void cb_edit_change1( INT32 value );
	void cb_edit_change2( INT32 value );
	IntProgress_Callback< UIPopupTwitchOption, &UIPopupTwitchOption::cb_edit_change1 > m_Callback1;
	IntProgress_Callback< UIPopupTwitchOption, &UIPopupTwitchOption::cb_edit_change2 > m_Callback2;
	
	void TwitchTVBroadCasteProcess();
private:
	bool m_bInitTitleBox;

	INT32 m_nFrameIndex;
	INT32 m_nVideoIndex;

	Volume mVolume[MAX_VOL];

	i3UIEditBox *	m_pTitleBox;
	i3UIComboBox *  m_pFrameComboBox;
	i3UIComboBox *  m_pVideoComboBox;

	TwitchLoadingThread * m_pThread;

	UILoadingIcon *		m_pLoadIcon;
	i3UIFrameWnd *		m_pFrameWnd;
};

#endif
