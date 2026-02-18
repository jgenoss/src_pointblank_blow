#pragma once

class cUI3DView : public i3GameNode
{
	I3_CLASS_DEFINE(cUI3DView, i3GameNode);

public:
	cUI3DView(void);
	virtual ~cUI3DView(void);

	virtual void OnUpdate( RT_REAL32 rDeltaSeconds );

	void	Destroy(void);

	bool	Set3DView(i3Node* pSceneNode, i3SceneGraphInfo* pInfo);

	void	OnVisibleScene(void);
	void	OnInvisibleScene(void);
	void	SetSize(INT32 nX, INT32 nY, INT32 nWidth, INT32 nHeight);
	

	void	OnRotateCamera(const float& fDelta);

	bool	IsRotate() const { return m_bIsRotate; }
	void	SetRotate(bool val) { m_bIsRotate = val; }

protected:
	bool	_Bind3DView(i3Node* pResource, MATRIX * pCamMtx = NULL);
	bool	_PrepareSprites(void);
	bool	_RecreateTarget(void);
	void	_UpdateViewport(void);


private:

	INT32					m_X, m_Y;
	INT32					m_Width, m_Height;

	i3Camera*				m_pCamera;
	i3Sprite2D*				m_pSprite;

	i3AttrSet*				m_pAttrSet;
	i3Transform*			m_pTrans;
	i3LightingEnableAttr*	m_pLightingEnableAttr;
	i3ViewportAttr*			m_pViewport;

	bool					m_bIsRotate;

	float m_nX, m_nY, m_nZ;
	
};
