#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3PersistantElement.h"
#include "i3Macro.h"
#include "i3List.h"
#include "itl/vector.h"

I3_ABSTRACT_CLASS_INSTANCE( i3PersistantElement, i3ElementBase);

i3PersistantElement::i3PersistantElement(void)
{
	m_PersistState = 0;
}

i3PersistantElement::~i3PersistantElement(void)
{
}

void i3PersistantElement::OnQueryStrings( i3StringList * pStrList)
{
	if( pStrList->IndexOf( GetMeta()->GetPersistClassName()) == -1)
	{
		char * pszClassName = GetMeta()->GetPersistClassName();
		pStrList->Add( pszClassName);
	}
}

void i3PersistantElement::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	I3ASSERT( g_pi3DebugPointer != this );
	List.push_back( this);
}

void i3PersistantElement::OnInitAfterLoad(void)
{
}

bool i3PersistantElement::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	// OnSaveXML 함수가 구현되지 않은 상태에서 Save되거나 Load되는 경우를
	// 발견하기 위해 Assertion함.
	I3ASSERT_0;

	return true;
}

bool i3PersistantElement::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	// OnSaveXML 함수가 구현되지 않은 상태에서 Save되거나 Load되는 경우를
	// 발견하기 위해 Assertion함.
	I3ASSERT_0;

	return true;
}
