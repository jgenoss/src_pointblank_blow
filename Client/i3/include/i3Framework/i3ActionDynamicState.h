#if !defined( __I3_ACTION_DYNAMIC_STATE_H)
#define __I3_ACTION_DYNAMIC_STATE_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionDynamicState : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionDynamicState, i3Action);
private:
	I3GAME_DYNAMIC_STATE		m_State = I3GAME_DYNAMIC_STATE_DYNAMIC;

public:
	I3GAME_DYNAMIC_STATE		getState(void)			{ return m_State; }
	void						setState( I3GAME_DYNAMIC_STATE state) { m_State = state; }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
