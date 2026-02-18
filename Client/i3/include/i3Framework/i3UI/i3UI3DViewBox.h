#if !defined( __I3UI_3DVIEWBOX_H__)
#define __I3UI_3DVIEWBOX_H__

#include "i3UIControl.h"

class I3_EXPORT_FRAMEWORK i3UI3DViewBox : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UI3DViewBox, i3UIControl);

private:
	i3AttrSet	*		m_pAttrSet = nullptr;
	i3Camera	*		m_pCamera = nullptr;	// 3D View를 위한 카메라로써 하나만 사용합니다.

	// Light
	i3LightingEnableAttr * m_pLightingEnableAttr = nullptr;

	i3ViewportAttr *	m_pViewport = nullptr;
	i3Sprite2D *		m_pSprite = nullptr;
	
	INT32				m_iWidth = 0;
	INT32				m_iHeight = 0;

	MATRIX			m_mtxReviveCam;
	REAL32		m_ReviveFov = 0.0f;

	bool			m_bUsingAnotherRenderT = false;
	i3Texture *		m_pAnotherRenderT = nullptr;
	i3Texture *		m_pAnotherRenderD = nullptr;
	bool			m_bControlEnable = false;

public:
	i3UI3DViewBox();
	virtual ~i3UI3DViewBox();

	virtual void	prepareSprites();

	void			SetAnotherRenderTarget( i3Texture * pRT, i3Texture * pDT);

	void			ClearView(void);
	void			Create3DView( void);
	void			Set3DView( i3Node* pResource, MATRIX * pCamMtx = nullptr);
	void			UpdateViewport(void);
	void			SetViewMatrix(const MATRIX& mt);
	void			SetLightingEnable( bool bFlag);
	void			RecreateTarget();

	i3AttrSet	*	getAttrSetNode( void)	{ return 	m_pAttrSet; }

	i3Camera	*	get3DCamera(void)							{ return m_pCamera;			}
	
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	/** \brief Visible을 enable시 Child에 대한 설정을 합니다.
		\desc I3UI_STYLE_HIDE를 삭제합니다. 해당 Flag가 있으면 Rendering을 하지 않음.*/
	virtual void	OnVisibleScene(void) override;

	/** \brief Visible을 disable시 Child에 대한 설정을 합니다.
		\desc I3UI_STYLE_HIDE를 추가합니다. 해당 Flag가 있으면 Rendering을 하지 않음.
			Sprite를 Disable 시킵니다. */
	virtual void	OnInvisibleScene(void) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	i3Sprite2D * GetSprite() { return m_pSprite; }

};

#endif
