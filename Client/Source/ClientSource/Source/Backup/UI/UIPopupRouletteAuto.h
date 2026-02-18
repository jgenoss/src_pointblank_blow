#if !defined( __UI_POPUP_ROULETTE_AUTO_H__)
#define __UI_POPUP_ROULETTE_AUTO_H__

#include "UIPopupBase.h"


class UIPopupRoulettAuto : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRoulettAuto, UIPopupBase);

protected:
	void			_ModifyControl( void);

	INT32			m_nIndex;

public:
	UIPopupRoulettAuto();
	virtual ~UIPopupRoulettAuto();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnKey_Enter( void);

	virtual bool	OnKey_Escape( void);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual bool	OnOKButton( void);

	virtual bool	OnCloseButton( void);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	void			ConfirmOK( void);
};

#endif
