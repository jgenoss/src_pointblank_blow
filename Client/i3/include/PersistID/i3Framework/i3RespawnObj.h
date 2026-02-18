#if !defined( __I3_RESPAWNOBJ_H_)
#define __I3_RESPAWNOBJ_H_

#include "i3GameObj.h"
#include "i3GamePath.h"

class I3_EXPORT_FRAMEWORK i3RespawnObj : public i3GameObj
{
	I3_CLASS_DEFINE( i3RespawnObj);

protected:
	UINT32				m_nRespawnStyle;

public:
	i3RespawnObj();
	virtual ~i3RespawnObj();

	REAL32	GetTheta(void);

	UINT32	getRespawnStyle(void)			{	return m_nRespawnStyle;}
	void	setRespawnStyle(UINT32 nStyle)	{	m_nRespawnStyle = nStyle;}

public:
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif