#if !defined( __I3_GAMERES_RESPAWN_H_)
#define __I3_GAMERES_RESPAWN_H_

#include "i3GameRes.h"
#include "i3RespawnObj.h"

class I3_EXPORT_FRAMEWORK i3GameResRespawn : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResRespawn);

protected:
	i3RespawnObj	*	m_pRespawn;

public:
	i3GameResRespawn(void);
	virtual ~i3GameResRespawn(void);

	void			setRespawnObj( i3RespawnObj * pObj)	{	I3_REF_CHANGE( m_pRespawn, pObj);}
	i3RespawnObj	*getRespawnObj(void)				{	return m_pRespawn;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif