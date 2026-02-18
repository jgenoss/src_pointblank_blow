#if !defined( __I3_ACTION_DYNAMIC_STATE_H)
#define __I3_ACTION_DYNAMIC_STATE_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionDynamicState : public i3Action
{
	I3_CLASS_DEFINE( i3ActionDynamicState);
protected:
	I3GAME_DYNAMIC_STATE		m_State;
	// Volatile members

public:
	i3ActionDynamicState(void);
	virtual ~i3ActionDynamicState(void);

	I3GAME_DYNAMIC_STATE		getState(void)			{ return m_State; }
	void						setState( I3GAME_DYNAMIC_STATE state)
	{
		m_State = state;
	}

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
