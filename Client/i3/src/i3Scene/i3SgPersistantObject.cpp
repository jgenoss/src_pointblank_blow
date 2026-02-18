#include "i3SceneDef.h"
#include "i3SgPersistantObject.h"

i3SgPersistantObject::i3SgPersistantObject(void)
{
}

i3SgPersistantObject::~i3SgPersistantObject(void)
{
}

void i3SgPersistantObject::OnBuildObjectList( i3List * pList)
{
	pList->Add( this);
}
