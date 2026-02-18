#pragma once

#include "RibbonBarItem.h"

class cItemTabBase : public i3ElementBase
{
public:
	I3_CLASS_DEFINE(cItemTabBase, i3ElementBase);

	cItemTabBase(void);
	virtual ~cItemTabBase(void);

public:
	void OnCreate();
};
