#ifndef _COMPONENT_BASE_H_
#define _COMPONENT_BASE_H_

#include <unordered_map>

#define DECLARE_FAMILY_ID(n) virtual const char* GetFamilyID() const { return (n); }
#define DECLARE_COMPONENT_ID(n) virtual const char* GetComponentID() const { return (n); }

//----------------------------------------------
// game object
//----------------------------------------------
class Component;
class GameObject
{
public:
	typedef i3::unordered_map<std::string, Component*> lookupTable;

	GameObject();
	~GameObject();

	Component* GetComponent(const char* familyID);
	Component* SetComponent(Component* newComponent);

private:
	lookupTable m_components;
};

//----------------------------------------------
// component
//----------------------------------------------
class Component
{
public:
	Component() : m_owner(0) {}
	virtual ~Component() {}

	virtual const char* GetComponentID() const = 0;
	virtual const char* GetFamilyID() const = 0;

	virtual void Update(REAL32 tm) {}

	void SetOwner(GameObject* o) { m_owner = o; }
	GameObject* GetOwner() const { return m_owner; }

private:
	GameObject* m_owner;
};

#endif