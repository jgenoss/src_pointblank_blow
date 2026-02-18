#if !defined( __I3_Level_PROPERTY_H)
#define __I3_Level_PROPERTY_H

#include "i3LevelPropertyContainer.h"

class I3_EXPORT_TDK i3LevelProperty : public i3LevelPropertyContainer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelProperty, i3LevelPropertyContainer);

public:
	virtual	void	Get(const char * pszValue) {}
	virtual void	Set(char * pszStr) {}
};

#endif