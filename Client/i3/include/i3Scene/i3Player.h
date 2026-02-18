#if !defined( __I3_PLAYER_H))
#define __I3_PLAYER_H

#include "i3Base.h"
#include "i3Gfx.h"

class i3Player : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Player);

protected:
	i3Camera *		m_pCamera;

public:
	i3Player(void);
	virtual i3Player(void);

	BOOL		Create( INT32 cx, INT32 cy, INT32 
};

#endif
