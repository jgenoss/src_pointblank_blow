#if !defined( __I3_XML_ELEMENT_H)
#define __I3_XML_ELEMENT_H

#include "i3XML.h"

class I3_EXPORT_BASE i3XMLElement : public TiXmlElement
{
protected:

public:
	i3XMLElement( const char * pszName);
	virtual ~i3XMLElement(void);

	void		addAttr( const char * pszName, const char * pszStr)			{	SetAttribute( pszName, pszStr); }
	void		addAttr( const char * pszName, INT32 val)					{	SetAttribute( pszName, val); }
	void		addAttr( const char * pszName, UINT32 val)					{	SetAttribute( pszName, (INT32) val); }
	void		addAttr( const char * pszName, REAL32 val)					{	SetAttribute( pszName, val); }
	void		addAttr( const char * pszName, INT64 val)					{	SetLongLongAttribute( pszName, val); }
	void		addAttr( const char * pszName, UINT64 val)					{	SetLongLongAttribute( pszName, (INT64)val); }

	const char *	getAttr( const char * pszName)							{ return Attribute( pszName); }
	const char *	getAttr( const char * pszName, INT32 * pVal)			{ return Attribute( pszName, pVal); }
	const char *	getAttr( const char * pszName, UINT32 * pVal)			{ return Attribute( pszName, (INT32 *) pVal); }
	const char *	getAttr( const char * pszName, INT64 * pVal)			{ return AttributeLong( pszName, pVal); }
	const char *	getAttr( const char * pszName, UINT64 * pVal)			{ return AttributeLong( pszName, (INT64 *) pVal); }
	const char *	getAttr( const char * pszName, REAL32 * pVal)			
	{ 
		const char * pRv;
		double val;

		pRv = Attribute( pszName, &val);

		*pVal = (REAL32) val;

		return pRv; 
	}

	const char *	getAttr( const char * pszName, UINT16 * pVal)
	{
		INT32 val;
		const char * pRv;

		pRv = Attribute( pszName, &val);

		*pVal = (UINT16) val;

		return pRv;
	}
};

#endif
