#include "pch.h"
#include "ComponentBase.h"

//----------------------------------------------
// game object
//----------------------------------------------
GameObject::GameObject() {}
GameObject::~GameObject() 
{
	lookupTable::iterator it = m_components.begin();
	for (; it != m_components.end(); it++)
	{
		I3_SAFE_DELETE( it->second);
	}
	m_components.clear(); 
}

Component* GameObject::GetComponent(const char* familyID)
{
	lookupTable::iterator it = m_components.find(familyID);
	return (it != m_components.end()) ? it->second : nullptr;
}

Component* GameObject::SetComponent(Component* newComponent)
{
	newComponent->SetOwner(this);
	m_components.insert(lookupTable::value_type(newComponent->GetFamilyID(), newComponent));

	return newComponent;
}