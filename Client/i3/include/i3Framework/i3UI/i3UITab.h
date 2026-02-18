#ifndef __I3UI_TAB_H__
#define __I3UI_TAB_H__

#include "i3UICaptionControl.h"	

enum I3UI_TAB_STATE
{
	I3UI_TAB_STATE_NORMAL = 0,
	I3UI_TAB_STATE_PUSHED,
	I3UI_TAB_STATE_ONMOUSE,
	I3UI_TAB_STATE_DISABLED,

	I3UI_TAB_STATE_COUNT,
};

// I3UI_TAB_STATE m_TabState는 의미상 상당이 요긴할수 있는 멤버변수 였으나..
// 실제코드에서 하나도 쓰이지 않고 있으므로 일단 삭제하는게 혼동을 방지할것 같다..(m_TabState 삭제합니다..)
// 2015.01.06.수빈

class I3_EXPORT_FRAMEWORK i3UITab : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UITab, i3UICaptionControl);

protected:

	INT32			m_nGroupID = 0;
	bool			m_bDefault = false;		//같은 Group id를 가진 탭들 가운데 오직 하나만 이 값이 true여야 합니다.(init할 때 이 탭이 선택됨)

	i3::rc_string	m_strBuddyName;
	i3UIFrameWnd*	m_pBuddyFrame = nullptr;

	COLOR			m_aTextColor[I3UI_TAB_STATE_COUNT];

	virtual void	_UpdateShapeState(void) override;
	
	void			setGroupID( INT32 id)		{	m_nGroupID = id;	}

public:
	i3UITab();
	
	void			GetTabsInTheSameGroup( i3::vector<i3UITab*>& Out);

	void			InitBuddy( void);
	void			EnableBuddyFrame( bool bEnable = true);

	INT32			getGroupID( void)					{	return m_nGroupID;	}
	void			SetDefault( bool bDefault = true)	{	m_bDefault = bDefault;	}

	void			OnTabSelected( void);
	void			RemoveTabSelect( void);

	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	UINT32			GetTextColorByStateUINT32( I3UI_TAB_STATE state) const;
	const COLOR&	GetTextColorByState(I3UI_TAB_STATE state) const;
	void			SetTextColorByState(  I3UI_TAB_STATE state, UINT32 clr);
	void			SetTextColorByState(  I3UI_TAB_STATE state, const COLOR& clr);

	void			SetTextColorByStateAll( UINT32 clr);
	void			SetTextColorByStateAll(const COLOR& clr);

	// 오버로딩 가림 처리..
	void			SetTextColor(I3UI_TAB_STATE state, UINT32 clr );
	void			SetTextColor(I3UI_TAB_STATE state, const COLOR& clr);



#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	static	void	RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_TAB_H__