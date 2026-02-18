#pragma once


#include "SubItemInfo.h"

// 기본클래스 생성자내 가상함수호출은 되지 않으므로 주의하고 대신에 인수로 생성방법을 제공하도록 한다.

class SubItemInfo_Weapon : public SubItemInfo
{
public:	
	SubItemInfo_Weapon(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
};


class SubItemInfo_Equipment : public SubItemInfo
{
public:
	SubItemInfo_Equipment(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
};

class SubItemInfo_Character : public SubItemInfo
{
public:
	SubItemInfo_Character(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
};

class SubItemInfo_World : public SubItemInfo
{
public:
	SubItemInfo_World(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
};

