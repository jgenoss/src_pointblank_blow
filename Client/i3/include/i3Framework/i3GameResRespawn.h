#if !defined( __I3_GAMERES_RESPAWN_H_)
#define __I3_GAMERES_RESPAWN_H_

#include "i3GameRes.h"
#include "i3RespawnObj.h"

class I3_EXPORT_FRAMEWORK i3GameResRespawn : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResRespawn, i3GameRes);

protected:
	i3RespawnObj	*	m_pRespawn = nullptr;

public:
	i3GameResRespawn(void);
	virtual ~i3GameResRespawn(void);

	void			setRespawnObj( i3RespawnObj * pObj)	{	I3_REF_CHANGE( m_pRespawn, pObj);}
	i3RespawnObj	*getRespawnObj(void)				{	return m_pRespawn;}

	virtual void	Destroy( void) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif