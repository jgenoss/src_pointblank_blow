#if !defined(__UI_FLOW_EDIT_BOX_H__)
#define __UI_FLOW_EDIT_BOX_H__

enum TEXT_FLOW_DIRECTION
{
	TFD_NONE		= 0,
	TFD_HORIZONTAL,
	TFD_VERTICAL,

	TFD_MAX
};

class UIFlowEditBox
{
private:
	i3UIEditBox *			m_pEditBox;
	
	REAL32					m_rElapsedTime;
	REAL32					m_rWaitTime;

	bool					m_bStartFlag;
	bool					m_bContinue;

protected:

	void			_SetFlowDirection(const I3UI_EDITBOX_STYLE style);
	void			_ResetFlowDirection(void);

public:
	UIFlowEditBox();
	virtual ~UIFlowEditBox();
	bool			IsStartFlag(){ return m_bStartFlag; }

	void			SetFlowEditBox(i3UIEditBox * pEdit, REAL32 rWaitTime = 0.0f, REAL32 rFlowSpeed = 1.0f);
	void			SetFlowTextData(const i3::wstring& wstr);
	void			SetFlowTextData(const i3::rc_wstring& wstr);

	/*** Scroll Text 설정 시, Text 시작 위치가 EditBox 영역 밖으로 나가는것을 막기위한 함수. ***/ 
	void			SetFlowTextData(const i3::wstring& wstr, const I3UI_EDITBOX_STYLE style);
	void			SetFlowTextData(const i3::rc_wstring& wstr, const I3UI_EDITBOX_STYLE style);

	void			SetWidth(REAL32 length) { m_pEditBox->setSize(length, m_pEditBox->getHeight()); }
	void			SetPos(REAL32 pos) { m_pEditBox->setPos(pos, m_pEditBox->getPos()->y, m_pEditBox->getPos()->z); }
	REAL32			GetWidth() { return m_pEditBox->getWidth(); };

	void			Reset(void);

	void			PlayEditTextFlow(TEXT_FLOW_DIRECTION eFlowDir, bool bContinue = false, bool bForceScroll = false);
	void			StopEditTextFlow(void);

	void			OnUpdate(REAL32 rDeltaSeconds);

	void			SetEnable(bool bFlag)	{ m_pEditBox->EnableCtrl(bFlag); }
	bool			isReadyToFlow(void) const;
	REAL32			GetElpasedTime(void) const	{ return m_rElapsedTime; }
	bool			isSameText(const i3::wliteral_range& wstrRng);
};

#endif // __UI_FLOW_EDIT_BOX_H__