#include "i3CommonType.h"
#include "i3ResourceObject.h"
#include "i3CRC32.h"
#include "i3String.h"
#include "i3MD5.h"

I3_ABSTRACT_CLASS_INSTANCE( i3ResourceObject, i3NamedElement);

i3ResourceObject::i3ResourceObject(void)
{
	m_Checksum = 0;
	m_ResStyle = 0;
}

i3ResourceObject::~i3ResourceObject(void)
{
}

UINT32	i3ResourceObject::_CalcChecksum(void)
{
	if( hasName())
	{
		char conv[MAX_PATH];
		i3String::Copy( conv, GetName(), MAX_PATH );
		i3String::ToUpper( conv);

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

	i3String::Copy( conv, pszPath, MAX_PATH );
	i3String::MakeUnixPath( conv);

	SetName( conv);

	_CalcChecksum();
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

