#include "i3CommonType.h"
#include "i3ResourceObject.h"
#include "i3CRC32.h"
#include "i3String.h"
#include "i3MD5.h"

#include "string/ext/make_unix_path.h"
#include "string/algorithm/to_upper.h"
#include "i3Base/string/ext/safe_string_copy.h"

I3_CLASS_INSTANCE( i3ResourceObject);

UINT32	i3ResourceObject::_CalcChecksum(void)
{
	if( hasName())
	{
		char conv[MAX_PATH];
		i3::safe_string_copy( conv, GetNameString(), MAX_PATH );
		i3::to_upper( conv);

		//m_Checksum = CRC32( 0xFFFFFFFF, (UINT8 *) conv, strlen( conv));
		i3MD5::Gen( conv, strlen( conv), (char *) &m_Checksum);
	}
	else
	{
		m_Checksum = 0;
	}

	return 0;
}

void i3ResourceObject::SetResourcePath( const char * pszPath)
{
	char conv[MAX_PATH];

	i3::safe_string_copy( conv, pszPath, MAX_PATH );
	i3::make_unix_path( conv);

	SetName( conv);

	_CalcChecksum();
}

void i3ResourceObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3ResourceObject * pDestRes = (i3ResourceObject *) pDest;

	pDestRes->m_ResStyle = m_ResStyle;
	pDestRes->m_Checksum = m_Checksum;
}

UINT32 i3ResourceObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = i3NamedElement::OnLoad( pResFile);
	
	_CalcChecksum();

	return Rc;
}


bool i3ResourceObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv;

	bRv = i3NamedElement::OnLoadXML( pFile, pXML);

	_CalcChecksum();

	return bRv;
}

