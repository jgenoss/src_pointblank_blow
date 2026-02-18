#if !defined( __I3LV_RES_PATH_H_)
#define __I3LV_RES_PATH_H_

#include "i3LevelRes.h"

class I3_EXPORT_TDK i3LevelResPath : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResPath, i3LevelRes);

protected:
	// Persistant members
	i3GamePath *		m_pPath = nullptr;

	INT32				m_PointCount = 0;
	INT32				m_LineCount = 0;

public:
	i3LevelResPath(void);
	virtual ~i3LevelResPath( void);

	void				Create(void);

	i3GamePath *		getPathObject(void)			{ return m_pPath; }

public:
	virtual bool		OnValidateBrief( bool bForce) override;
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *	CreateGameRes(void) override;
	virtual bool				OnSerialize(void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual i3LevelElement3D *	CreateInstance(i3LevelScene * pScene) override;
};

#endif // #if !defined ( __I3LV_RES_ANIMATION_H_)