#if !defined( _I3LV_RES_PRIM_H__)
#define _I3LV_RES_PRIM_H__

#include "i3LevelRes.h"
#include "i3LevelResSceneGraph.h"

enum	I3LV_RES_PRIMTYPE
{
	I3LV_RES_PRIM_NA = -1,
	I3LV_RES_PRIM_BOX = 0,
	I3LV_RES_PRIM_SPHERE,
	I3LV_RES_PRIM_CAPSULE,
	I3LV_RES_PRIMITIVE_RESPAWN,
	I3LV_RES_PRIM_PATH,
};

class I3_EXPORT_TDK i3LevelResPrimitive : public i3LevelRes
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelResPrimitive, i3LevelRes);

protected:
	i3AttrSet			*	m_pScene = nullptr;

	I3LV_RES_PRIMTYPE	m_nPrimType = I3LV_RES_PRIM_NA;

public:
	i3LevelResPrimitive();
	virtual ~i3LevelResPrimitive();


	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual i3Node *	GetElementAttachNode( i3LevelScene * pScene) override;

	virtual bool		OnValidateBrief( bool bForce) override;
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;

	I3LV_RES_PRIMTYPE	getPrimType()	{	return m_nPrimType;}

	i3Node	*			getScene()		{	return m_pScene;}

	virtual void		OnQueryExportPath( i3LevelScene * pScene, i3LevelElement3D * pObj, char * pszPath, UINT16 nBufSize) override;
	virtual bool		OnExport( i3LevelScene * pScene, const char * pszPath) override;

protected:
	virtual i3Node	*	_BuildScene(void) = 0;
	virtual i3ClassMeta *	GetInstanceMeta(void)		{ return nullptr; }
};

#endif