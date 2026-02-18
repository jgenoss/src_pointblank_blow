#if !defined( __UI_POPUP_ROULETTEDEFAULT_H__)
#define __UI_POPUP_ROULETTEDEFAULT_H__

#include "UIPopupBase.h"
#include "ItemImageMgr.h"

class UIRouletteView;

class UIPopupRoulettDefault : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRoulettDefault, UIPopupBase);

protected:
	void			_ModifyControl( void);

	class UIImgSetCtrl*		m_ImgBox;
	INT32 m_idxEffect;

	i3UI3DViewBox *		m_p3DView;

	REAL32	m_rElapsedTime;
	REAL32	m_rElapsedTime1;

public:
	UIPopupRoulettDefault();
	virtual ~UIPopupRoulettDefault();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnKey_Enter( void);

	virtual bool	OnKey_Escape( void);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnEntranceEnd( void);

	virtual bool	OnOKButton( void);

	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	void			ConfirmOK( void);

	void			OpenEventWin(INT32 ItemId);

	void			SetName();

	virtual void	OnUpdate( REAL32 rDeltaSeconds);


};

#endif
