#if !defined( __I3_GFX_LIGHT_H)
#define __I3_GFX_LIGHT_H

#include "i3GfxResource.h"
#include "i3Texture.h"

#define		I3G_LIGHT_STYLE_SHADOWMAP		0x00000001
#define		I3G_LIGHT_STYLE_CALC_BOUND		0x00000002

#define		I3G_LIGHT_STATE_APPLIED			0x00000001			// Light가 사용되었느냐를 판단하는 상태. 이 Flag이 있어야만 Shadow 처리가 된다.
#define		I3G_LIGHT_STATE_DISABLE			0x00000002

class I3_EXPORT_GFX i3Light : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3Light, i3GfxResource);
protected:
	I3G_LIGHT_TYPE		m_Type = I3G_LIGHT_DIRECTIONAL;
	UINT32				m_Style = 0;
	UINT32				m_State = 0;
	UINT32				m_Group = 0;
	COLORREAL			m_DiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	COLORREAL			m_SpecularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	REAL32				m_Range = 50.0f;
	REAL32				m_InnerAngle = I3_DEG2RAD(10.0f);
	REAL32				m_OuterAngle = I3_DEG2RAD(20.0f);

	MATRIX				m_mWorldTx;

	i3Texture *			m_pPatternMap = nullptr;

	i3Texture *			m_pShadowMap[2] = { nullptr, nullptr };
	i3Texture *			m_pDepthRT = nullptr;
	INT32				m_ShadowMapSize = 512;
	MATRIX				m_ShadowMapMatrix;
	REAL32				m_LS_ZRange = 0.0f;

	// Volatile members
	REAL32				m_SqDist = 0.0f;

public:
	virtual ~i3Light(void);

	void				SetLightType( I3G_LIGHT_TYPE type);
	I3G_LIGHT_TYPE		GetLightType(void)								{ return m_Type; }

	UINT32				getStyle(void)									{ return m_Style; }
	void				setStyle( UINT32 mask)							{ m_Style = mask; }
	void				addStyle( UINT32 mask)							{ m_Style |= mask; }
	void				removeStyle( UINT32 mask)						{ m_Style &= ~mask; }
	bool				isStyle( UINT32 mask)							{ return (m_Style & mask) != 0; }

	UINT32				getState(void)									{ return m_State; }
	void				setState( UINT32 state)							{ m_State = state; }
	void				addState( UINT32 mask)							{ m_State |= mask; }
	void				removeState( UINT32 mask)						{ m_State &= ~mask; }
	bool				isState( UINT32 mask)							{ return (m_State & mask) != 0; }

	UINT32				getGroup(void)									{ return m_Group; }
	void				setGroup( UINT32 grp)							{ m_Group = grp; }

	void				SetDiffuse( COLORREAL * pColor);
	COLORREAL *			GetDiffuse(void)								{ return &m_DiffuseColor; }

	void				SetSpecular( COLORREAL * pColor);
	COLORREAL *			GetSpecular(void)								{ return &m_SpecularColor; }

	void				setInnerAngle( REAL32 val)						{ m_InnerAngle = val; }
	REAL32				getInnerAngle(void)								{ return m_InnerAngle; }

	REAL32				getOuterAngle(void)								{ return m_OuterAngle; }
	void				setOuterAngle( REAL32 val)						{ m_OuterAngle = val; }

	void				setRange( REAL32 val)							{ m_Range = val; }
	REAL32				getRange(void)									{ return m_Range; }

	MATRIX *			GetWorldSpaceTransform(void)					{ return &m_mWorldTx; }
	void				SetWorldSpaceTransform( MATRIX * pMatrix);

	VEC3D	*			GetWorldSpaceDirection(void)							
	{
		return i3Matrix::GetAt( &m_mWorldTx);
	}

	VEC3D	*			GetWorldSpacePosition(void)
	{
		return i3Matrix::GetPos( &m_mWorldTx);
	}

	i3Texture *			getPatternMap(void)								{ return m_pPatternMap; }
	void				setPatternMap( i3Texture * pTex)				{ I3_REF_CHANGE( m_pPatternMap, pTex); }

	INT32				getShadowMapSize(void)							{ return m_ShadowMapSize; }
	void				setShadowMapSize( INT32 sz)						{ m_ShadowMapSize = sz; }

	i3Texture *			getShadowMap( INT32 idx = 0)					{ return m_pShadowMap[idx]; }
	i3Texture *			getDepthBuffer(void)							{ return m_pDepthRT; }

	i3Texture *			getActualShadowMap(void);

	REAL32				getLightSpaceZRange(void)						{ return m_LS_ZRange; }
	void				setLightSpaceZRange( REAL32 range)				{ m_LS_ZRange = range; }

	MATRIX *			getShadowMapMatrix(void)						{ return &m_ShadowMapMatrix; }
	void				setShadowMapMatrix( MATRIX * pMtx)				{ i3Matrix::Copy( &m_ShadowMapMatrix, pMtx); }

	//
	REAL32				getSqDistance(void)	const						{ return m_SqDist; }
	void				setSqDistance( REAL32 sqdist)					{ m_SqDist = sqdist; }

	bool				isAffect( VEC3D * pWSCenter, REAL32 r);

	void				CreateShadowMap(void);

	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
