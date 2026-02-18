#pragma once

#include "../../PropertyEditor/i3registryset_impl.h"


class i3RegistrySet_Impl2 :
	public i3RegistrySet_Impl
{
	I3_CLASS_DEFINE( i3RegistrySet_Impl2, i3RegistrySet_Impl );

public:
	i3RegistrySet_Impl2(void);
	virtual ~i3RegistrySet_Impl2(void);

	void SelectData(i3RegKey* pKey);
};
