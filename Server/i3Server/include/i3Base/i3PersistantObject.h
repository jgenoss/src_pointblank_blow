#if !defined( __I3_PERSISTANT_ELEMENT_H)
#define __I3_PERSISTANT_ELEMENT_H

#include "i3ElementBase.h"

class i3ResourceFile;

class i3PersistantElement : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PersistantElement);

protected:

public:
	i3PersistantElement(void);
	virtual ~i3PersistantElement(void);

	virtual void		OnQueryStrings( i3StringList * pStrList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile) = 0;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) = 0;
};

#endif