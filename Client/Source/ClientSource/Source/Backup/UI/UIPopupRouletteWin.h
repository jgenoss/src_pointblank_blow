#if !defined( __UI_POPUP_ROULETTEWIN_H__)
#define __UI_POPUP_ROULETTEWIN_H__

#include "UIPopupBase.h"
#include "ItemImageMgr.h"

class UIRouletteView;

class UIPopupRoulettWin : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRoulettWin, UIPopupBase);

protected:
	void			_ModifyControl( void);

	class UIImgSetCtrl*		m_ImgBox;
	INT32 m_idxEffect;

	i3UI3DViewBox *		m_p3DView;

	REAL32	m_rElapsedTime;
	REAL32	m_rElapsedTime1;

public:
	UIPopupRoulettWin();
	virtual ~UIPopupRoulettWin();

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



	
	/** \brief 3DViewBox를 생성합니다.
		\param[in] pParent Attach될 Parent Node */
	void			Create3DViewBox( i3UIControl * pParent);

	/** \brief 3DViewBox를 제거합니다. */
	void			Delete3DViewBox( void);

	/** \brief Character를 생성합니다.
		\desc 선행조건으로 3DViewBox가 생성되고 호출되야 함*/

	void			CreateViewEffect();

};

#endif
