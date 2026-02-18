#pragma once

#include "ItemInfo.h"


class ItemInfo_Weapon : public ItemInfo
{

private:
	virtual bool				OnProcess_LoadPef(i3RscPartialFile* f);
	virtual SubItemInfoPtr		CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const;

};

class ItemInfo_Equipment : public ItemInfo 
{
private:
	virtual bool				OnProcess_LoadPef(i3RscPartialFile* f);
	virtual SubItemInfoPtr		CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const;
};

class ItemInfo_Character : public ItemInfo
{
private:
	virtual bool				OnProcess_LoadPef(i3RscPartialFile* f);
	virtual SubItemInfoPtr		CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const;	
};

class ItemInfo_World : public ItemInfo
{
private:
	virtual bool				OnProcess_LoadPef(i3RscPartialFile* f);
	virtual SubItemInfoPtr		CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const;
};

