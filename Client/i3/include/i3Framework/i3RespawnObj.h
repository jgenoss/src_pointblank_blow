#if !defined( __I3_RESPAWNOBJ_H_)
#define __I3_RESPAWNOBJ_H_

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3RespawnObj : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3RespawnObj, i3GameObj);

protected:
	UINT32				m_nRespawnStyle = 0;

public:
	REAL32	GetTheta(void);

	UINT32	getRespawnStyle(void)			{	return m_nRespawnStyle;}
	void	setRespawnStyle(UINT32 nStyle)	{	m_nRespawnStyle = nStyle;}

public:
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif