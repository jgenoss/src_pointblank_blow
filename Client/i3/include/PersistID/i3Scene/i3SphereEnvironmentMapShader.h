#if !defined( I3SG_SPHERE_ENVIRONMENT_MAP_SHADER_H)
#define I3SG_SPHERE_ENVIRONMENT_MAP_SHADER_H

#include "i3Node.h"
#include "i3Gfx.h"
#include "i3TextureBindAttr.h"
#include "i3RenderAttrListAttr.h"
#include "i3TextureFunctionAttr.h"
#include "i3TextureMatrixAttr.h"
#include "i3TextureCoordSourceAttr.h"
#include "i3TextureFilterAttr.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3SphereEnvironmentMapShader : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3SphereEnvironmentMapShader);
protected:
	i3TextureBindAttr *			m_pSphereTexBind;
	i3TextureBindAttr *			m_pGlossTexBind;
	i3RenderAttrListAttr *		m_pNewOutput;
	i3TextureCoordSourceAttr *	m_pTexCoordSource;
	i3TextureFunctionAttr *		m_pTexFunc;
	i3TextureMatrixAttr *		m_pTexMatrix;
	i3TextureFilterAttr *		m_pTexFilter;

	i3Texture * m_pSphereTex;
	i3Texture *	m_pGlossTex;

	REAL32						m_ReflectionAmount;

public:
	i3SphereEnvironmentMapShader(void);
	virtual ~i3SphereEnvironmentMapShader(void);

	void		SetSphereTexture( i3Texture * pAttr);
	i3Texture *	GetSphereTexture(void)						{ return m_pSphereTex; }

	void		SetGlossTexture( i3Texture * pAttr);
	i3Texture *	GetGlossTexture(void)						{ return m_pGlossTex;}

	void		SetReflectionAmount( REAL32 val)			{ m_ReflectionAmount = val; }
	REAL32		GetReflectionAmount(void)					{ return m_ReflectionAmount; }

	///////////////////////////////
	// Internal Use Only
	void		Apply( i3SceneTracer * pTracer);

	i3RenderAttrListAttr *	GetRenderAttrList(void)			{ return m_pNewOutput; }

	// Internal Use Only
	///////////////////////////////


	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
