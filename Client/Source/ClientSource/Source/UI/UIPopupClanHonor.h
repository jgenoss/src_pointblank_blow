#if !defined( __UI_POPUP_CLAN_HONOR_H__)
#define __UI_POPUP_CLAN_HONOR_H__

#include "UIPopupBase.h"


class UIPopupClanHonor : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupClanHonor, UIPopupBase);

protected:
	i3UI3DViewBox *		m_p3DView;

	INT32				m_nIndex;
	i3Texture *			m_pClanMarkTexture[TEAM_COUNT];
	REAL32				m_rPopupElapsedTime;

	i3Node *			m_pEffectParent;

public:
	UIPopupClanHonor();
	virtual ~UIPopupClanHonor();
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;


	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	/** \brief 3DViewBox를 생성합니다.
		\param[in] pParent Attach될 Parent Node */
	void			Create3DViewBox( i3UIControl * pParent );

	/** \brief 3DViewBox를 제거합니다. */
	void			Delete3DViewBox( void);

	/** \brief Character를 생성합니다.
		\desc 선행조건으로 3DViewBox가 생성되고 호출되야 함*/

	void			CreateViewEffect();
	void			SetUiText();

	i3RenderAttr * FindTextureBindInAttrSet( i3Node * pNode, const char * pszAttrName, i3ClassMeta * pMeta, bool bSevereCheck);

	void			_UpdatePopup(REAL32 rDeltaSeconds);

};

#endif
