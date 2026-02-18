#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3PersistantElement.h"
#include "i3Macro.h"
#include "itl/vector.h"
#include "itl/vector_util.h"
#include "string/rc_string.h"

I3_CLASS_INSTANCE( i3PersistantElement);

void i3PersistantElement::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	const i3::fixed_string& str = meta()->persist_class_name();

	if (i3::vu::int_index_of(StrList, str ) == -1 )
	{
		StrList.push_back( str);
	}
}

void i3PersistantElement::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	I3ASSERT( g_pi3DebugPointer != this );
	List.push_back(this);
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
