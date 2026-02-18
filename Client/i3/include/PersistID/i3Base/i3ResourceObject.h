#if !defined( __I3_RESOURCE_OBJECT_H)
#define __I3_RESOURCE_OBJECT_H

#include "i3CommonType.h"
#include "i3NamedElement.h"
#include "i3ResourceFile.h"

class I3_EXPORT_BASE i3ResourceObject : public i3NamedElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3ResourceObject);
protected:
	UINT32		m_Checksum[4];

protected:
	UINT32		_CalcChecksum(void);

public:
	i3ResourceObject(void);
	virtual ~i3ResourceObject(void);

	UINT32 		getChecksum( INT32 idx)			{ return m_Checksum[idx]; }
	void		SetResourcePath( const char * pszPath);

	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
