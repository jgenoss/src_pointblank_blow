#if !defined( __I3_GFX_LIGHT_H)
#define __I3_GFX_LIGHT_H

#include "i3GfxResource.h"
#include "i3Texture.h"

#define		I3G_LIGHT_STYLE_SHADOWMAP		0x00000001

#define		I3G_LIGHT_STATE_APPLIED			0x00000001			// Light가 사용되었느냐를 판단하는 상태. 이 Flag이 있어야만 Shadow 처리가 된다.
#define		I3G_LIGHT_STATE_DISABLE			0x00000002

class I3_EXPORT_GFX i3Light : public i3GfxResource
{
	I3_CLASS_DEFINE( i3Light);
protected:
	I3G_LIGHT_TYPE		m_Type;
	UINT32				m_Style;
	UINT32				m_State;
	UINT32				m_Group;
	COLORREAL			m_DiffuseColor;
	COLORREAL			m_SpecularColor;
	REAL32				m_Range;
	REAL32				m_InnerAngle;
	REAL32				m_OuterAngle;

	MATRIX				m_mWorldTx;

	i3Texture *			m_pPatternMap;

	i3Texture *			m_pShadowMap;
	i3Texture *			m_pDepthRT;
	INT32				m_ShadowMapSize;
	MATRIX				m_ShadowMapMatrix;

	i3Texture *			m_pSATMap;

	// Volatile members
	REAL32				m_SqDist;

public:
	i3Light(void);
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

	i3Texture *			getShadowMap(void)								{ return m_pShadowMap; }
	i3Texture *			getDepthBuffer(void)							{ return m_pDepthRT; }

	i3Texture *			getSATMap(void)									{ return m_pSATMap; }

	MATRIX *			getShadowMapMatrix(void)						{ return &m_ShadowMapMatrix; }
	void				setShadowMapMatrix( MATRIX * pMtx)				{ i3Matrix::Copy( &m_ShadowMapMatrix, pMtx); }

	//
	REAL32				getSqDistance(void)								{ return m_SqDist; }
	void				setSqDistance( REAL32 sqdist)					{ m_SqDist = sqdist; }

	bool				isAffect( VEC3D * pWSCenter, REAL32 r);

	void				CreateShadowMap(void);

	virtual void		OnBuildObjectList( i3List * pList);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
