#ifndef __AVATAR_LOAD_MACRO_TEX_THREAD_H__
#define __AVATAR_LOAD_MACRO_TEX_THREAD_H__

#include "GameThread.h"
#include "../../Avatar/AvatarDefine.h"

class AvatarLoadMacroTexThread : public CGameThread
{
	I3_CLASS_DEFINE( AvatarLoadMacroTexThread, CGameThread);

private:
	tagLOADMACROTEXDATA  	m_LoadInfo;
	UINT32					m_EmptyFlag = 0;
	UINT32					m_Loaded = 0;

public:
	virtual ~AvatarLoadMacroTexThread();

	virtual void	Run( void *pData) override;

	void	SetLoad( tagLOADMACROTEXDATA * pData);
	bool	IsEmpty( void)			{ return (m_EmptyFlag == 0); }
	bool	IsLoaded( void)			{ return (m_Loaded == 1); }
	void	Clear( void);

};

#endif
