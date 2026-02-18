#if !defined( __UI_POPUP_EVENT_H__)
#define __UI_POPUP_EVENT_H__

#include "UIPopupBase.h"
#include "ItemImageMgr.h"

class UIPopupEvent : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupEvent, UIPopupBase);

protected:
	void			_ModifyControl( void);

	class UIImgSetCtrl*		m_ImgBox;
	


public:
	UIPopupEvent();
	virtual ~UIPopupEvent();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnKey_Enter( void) override;

	virtual bool	OnKey_Escape( void) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnOKButton( void) override;

	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	void			ConfirmOK( void);

	void			OpenEventWin( T_ItemID ItemId);
};

#endif
