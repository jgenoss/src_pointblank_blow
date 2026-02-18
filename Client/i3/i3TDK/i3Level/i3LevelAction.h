#if !defined( __I3_Level_ACTION_BASE_H)
#define __I3_Level_ACTION_BASE_H

#include "i3LevelPropertyContainer.h"

class I3_EXPORT_TDK i3LevelAction : public i3LevelPropertyContainer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelAction, i3LevelPropertyContainer);

public:
	virtual char *	GetTypeName(void) = 0;
	virtual void	GetInformationString( char * pszStr);

	virtual void	Freeze(void) {}
	virtual void	Unfreeze(void) {}
};

#endif
