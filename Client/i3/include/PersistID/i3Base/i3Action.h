#if !defined( __I3_ACTION_H)
#define __I3_ACTION_H

#include "i3NamedElement.h"
#include "i3EventReceiver.h"

#define		I3_ACTION_STYLE_EXTERNAL		0x00000001
#define		I3_ACTION_STYLE_OWNPROCESS		0x00000002
#define		I3_ACTION_STYLE_GLOBAL			0x00000004

#define		I3_ACTION_STYLE_NEEDCOPYINST	0x00000008	//copy할때 instance copy가 필요한 액션

#define		I3_ACTION_STATE_COMPILED		0x00000001
#define		I3_ACITON_STATE_ERROR			0x80000000

// 이름 자체를 LUA Script Filename으로 사용한다.
class I3_EXPORT_BASE i3Action : public i3NamedElement
{
	I3_CLASS_DEFINE( i3Action);
protected:
	UINT32			m_Style;
	UINT32			m_State;

public:
	i3Action(void);
	virtual ~i3Action(void);

	UINT32			getStyle(void)				{ return m_Style; }
	void			setStyle( UINT32 style)		{ m_Style = style; }
	void			addStyle( UINT32 style)		{ m_Style |= style; }
	void			removeStyle( UINT32 style)	{ m_Style &= ~style; }

	UINT32			getState(void)				{ return m_State; }
	void			setState( UINT32 state)		{ m_State = state; }
	void			addState( UINT32 state)		{ m_State |= state; }
	void			removeState( UINT32 state)	{ m_State &= ~state; }

	virtual void	OnUpdate( REAL32 tm);
	virtual void	OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	OnChangeTime( REAL32 tm);

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len = -1);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
