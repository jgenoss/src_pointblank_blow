#include "stdafx.h"
#include "ItemInfo_Derived.h"

#include "SubItemInfo_Derived.h"


bool			ItemInfo_Weapon::OnProcess_LoadPef(i3RscPartialFile* f)
{
	// reg root 이후의 접근...

	i3ResourceFileObjectInfo2* info = f->FindChildi3TreeElement("WeaponList");

	if (!info) return false;

	info = f->FindChildi3TreeElement("Weapon");

	if (!info) return false;
	
	return true;
}

SubItemInfoPtr	ItemInfo_Weapon::CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const
{
	return i3::make_shared<SubItemInfo_Weapon>(f, child_info) ;
}

bool			ItemInfo_Equipment::OnProcess_LoadPef(i3RscPartialFile* f)
{
	i3ResourceFileObjectInfo2* info = f->FindChildi3TreeElement("Equipment");
	if (!info) return false;
	return true;
}

SubItemInfoPtr	ItemInfo_Equipment::CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const
{
	return i3::make_shared<SubItemInfo_Equipment>(f, child_info);
}

bool	ItemInfo_Character::OnProcess_LoadPef(i3RscPartialFile* f)
{
	return true;		// Character.pef의 경우 루트 이하의 노드를 모두 쓴다..
}

SubItemInfoPtr	ItemInfo_Character::CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const
{
	return i3::make_shared<SubItemInfo_Character>(f, child_info);
}

bool	ItemInfo_World::OnProcess_LoadPef(i3RscPartialFile* f)
{
	return true;			// pef를 쓰지 않고, i3Game을 쓰며 i3StageInfo를 콜렉션처리하므로 하는 일은 없다.
}

SubItemInfoPtr	ItemInfo_World::CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const
{
	return i3::make_shared<SubItemInfo_World>(f, child_info);
}


