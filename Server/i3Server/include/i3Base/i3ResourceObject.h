#if !defined( __I3_RESOURCE_OBJECT_H)
#define __I3_RESOURCE_OBJECT_H

#include "i3CommonType.h"
#include "i3NamedElement.h"
#include "i3ResourceFile.h"

#define		I3_RES_STYLE_COMMON			0x00000001

class I3_EXPORT_BASE i3ResourceObject : public i3NamedElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3ResourceObject);
protected:
	UINT32			m_ResStyle;
	UINT128			m_Checksum;

protected:
	UINT32		_CalcChecksum(void);

public:
	i3ResourceObject(void);
	virtual ~i3ResourceObject(void);

	UINT128		getChecksum(void)				{ return m_Checksum; }
	UINT64 		getChecksum( INT32 idx)			{ return m_Checksum[idx]; }

	bool		isCommonRes(void)				{ return (m_ResStyle & I3_RES_STYLE_COMMON) != 0; }
	void		setCommonRes( bool bState)
	{
		if( bState)
			m_ResStyle |= I3_RES_STYLE_COMMON;
		else
			m_ResStyle &= ~I3_RES_STYLE_COMMON;
	}

	void		SetResourcePath( const char * pszPath);

	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
