#if !defined( __I3TDK_PROPERTY_SUBITEM_USERCTRL_H)
#define __I3TDK_PROPERTY_SUBITEM_USERCTRL_H

#include "i3TDKPropertySubItem.h"

class i3TDKPropertySubItemUserCtrl;

struct CallOnDrawRef
{
	i3TDKPropertySubItemUserCtrl *	pUserCtrl = nullptr;
	LPDRAWITEMSTRUCT				lpDrawItemStruct = nullptr;
	RECT *							pRect = nullptr;
	SubItemColorSet *				pColorSet = nullptr;

} ;

struct CallOnHitRef
{
	i3TDKPropertySubItemUserCtrl *	pUserCtrl = nullptr;
	CWnd *							pParentWnd = nullptr;
	RECT *							pRect = nullptr;
	DWORD							dwStyle = 0;

} ;

struct CallOnUpdateRef
{
	i3TDKPropertySubItemUserCtrl *	pUserCtrl = nullptr;
	SubItemUpdateInfo *				pInfo = nullptr;

} ;

struct CallOnReleaseRef
{
	i3TDKPropertySubItemUserCtrl *	pUserCtrl = nullptr;

} ;

//	Callback Function
typedef void (*CALL_DRAW) ( CallOnDrawRef * pRef);
typedef void (*CALL_HIT) ( CallOnHitRef * pRef);
typedef void (*CALL_UPDATE) ( CallOnUpdateRef * pRef);
typedef void (*CALL_RELEASE) ( CallOnReleaseRef * pRef);

struct SubItemUserCtrlInitStruct
{
	INT32			_id = -1;				//	usable
	UINT32			_style = 0;				//	Edit style
	UINT32			_type = 0;				//	usable
	const TCHAR*	_label = nullptr;				//	Label in control
	
	void *			_data = nullptr;				//	user data pointer

	INT32			_row = -1;
	INT32			_column = -1;

	CALL_DRAW		_pCallOnDraw = nullptr;		//	Call OnDraw Func
	CALL_HIT		_pCallOnHit = nullptr;		//	Call OnHit Func
	CALL_UPDATE		_pCallOnUpdate = nullptr;		//	Call OnUpdate Func
	CALL_RELEASE	_pCallOnRelease = nullptr;	//	Call OnRelease Func
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

class I3_EXPORT_TDK i3TDKPropertySubItemUserCtrl : public i3TDKPropertySubItem
{
	I3_EXPORT_CLASS_DEFINE( i3TDKPropertySubItemUserCtrl, i3TDKPropertySubItem);

protected:
	CWnd *				m_pParent;
	INT32				m_nCtrlID;

	//	Callback Function
	CALL_DRAW			m_pCallOnDraw;
	CALL_HIT			m_pCallOnHit;
	CALL_UPDATE			m_pCallOnUpdate;
	CALL_RELEASE		m_pCallOnRelease;

public:
	i3TDKPropertySubItemUserCtrl( void);
	i3TDKPropertySubItemUserCtrl( SubItemUserCtrlInitStruct * pInitStruct);
	virtual ~i3TDKPropertySubItemUserCtrl(void) {}

	BOOL	Create( SubItemUserCtrlInitStruct * pInitStruct);
	void	UpdateMessage( CWnd * pUserCtrl, const TCHAR* pszLabel);

	void	SetCallOnDraw( CALL_DRAW pFunc)				{	m_pCallOnDraw = pFunc;				}
	void	SetCallOnHit( CALL_HIT pFunc)				{	m_pCallOnHit = pFunc;				}
	void	SetCallOnUpdate( CALL_UPDATE pFunc)			{	m_pCallOnUpdate = pFunc;			}
	void	SetCallOnRelease( CALL_RELEASE pFunc)		{	m_pCallOnRelease = pFunc;			}

	const CALL_DRAW		GetCallOnDraw( void)			{	return m_pCallOnDraw;				}
	const CALL_HIT		GetCallOnHit( void)				{	return m_pCallOnHit;				}
	const CALL_UPDATE	GetCallOnUpdate( void)			{	return m_pCallOnUpdate;				}
	const CALL_RELEASE	GetCallOnRelease( void)			{	return m_pCallOnRelease;			}

	virtual void	Reset( void);
	virtual void	CopyTo( i3TDKPropertySubItemUserCtrl * pSubItem);

	virtual	void	OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet);
	virtual void	OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle);
	virtual void	OnUpdate( SubItemUpdateInfo * pInfo);
	virtual void	OnRelease( void);
};


#endif //__I3TDK_PROPERTY_SUBITEM_USERCTRL_H