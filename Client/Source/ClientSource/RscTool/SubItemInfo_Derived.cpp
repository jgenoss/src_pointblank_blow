#include "stdafx.h"
#include "SubItemInfo_Derived.h"

#include "ItemData_Derived.h"


namespace
{
	ItemDataPtr  CreateItemData_Weapon( SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info)
	{
		return i3::make_shared<ItemData_Weapon>(parent, f, child_info);
	}
	
	ItemDataPtr  CreateItemData_Equipment( SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info)
	{
		return i3::make_shared<ItemData_Equipment>(parent, f, child_info);
	}

	ItemDataPtr CreateItemData_Character(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info)
	{
		return i3::make_shared<ItemData_Character>(parent, f, child_info);
	}

	ItemDataPtr CreateItemData_World(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info)
	{
		return i3::make_shared<ItemData_World>(parent, f, child_info);
	}
}


SubItemInfo_Weapon::SubItemInfo_Weapon(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) 
: SubItemInfo(f, objInfo, &CreateItemData_Weapon) 
{
	
}

SubItemInfo_Equipment::SubItemInfo_Equipment(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
: SubItemInfo(f, objInfo, &CreateItemData_Equipment) 
{

}

SubItemInfo_Character::SubItemInfo_Character(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
: SubItemInfo(f, objInfo, &CreateItemData_Character)
{

}

SubItemInfo_World::SubItemInfo_World(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) 
: SubItemInfo(f, objInfo, &CreateItemData_World, i3::identity<SubItemInfo::SubItemInfo_World_Tag>() )
{
	
}
