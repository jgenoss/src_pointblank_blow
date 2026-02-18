#if !defined( __UI_POPUP_GACHA_RESULT_H__)
#define __UI_POPUP_GACHA_RESULT_H__

#include "UIPopupBase.h"
#include "UIItemInfoUtil.h"

class UIImgSetCtrl;

class UIPopupGachaResult : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGachaResult, UIPopupBase);

protected:
	i3UI3DViewBox *			m_p3DView;
	i3UI3DViewBox *			m_p3DView2;

	INT32					m_nIndex;
	i3Texture*				m_pClanMarkTexture;
	REAL32					m_rPopupElapsedTime;
	UIImgSetCtrl*			m_goods_img_ctrl[4];
	i3GameResSceneGraph *	m_pSgRes[14];
	i3Node *				m_pNewNode;
	i3AttrSet *				m_pFlameNode;
	bool					m_bDelay;
	REAL32					m_rDelayTime;

public:
	UIPopupGachaResult();
	virtual ~UIPopupGachaResult();
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnClick( i3UIControl * pControl) override;

	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	virtual bool	OnKey_Enter( void) override;
		
	/** \brief 3DViewBox를 생성합니다.
		\param[in] pParent Attach될 Parent Node */
	void UIPopupGachaResult::Create3DViewBox( i3UIControl * pParent);
	/** \brief 3DViewBox를 제거합니다. */
	void			Delete3DViewBox( void);

	/** \brief Character를 생성합니다.
		\desc 선행조건으로 3DViewBox가 생성되고 호출되야 함*/

	void			CreateViewEffect();
	void			SetUiText();
	void			OnClieck_OK();

	void			_UpdatePopup(REAL32 rDeltaSeconds);



};

#endif
