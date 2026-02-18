#if !defined ( _I3REGISTRY_H__ )
#define _I3REGISTRY_H__

#include "i3RegData.h"
#include "i3RegKey.h"
#include "i3RegistrySet.h"

extern I3_EXPORT_SCENE i3RegistrySet * g_pRegistry;

namespace i3Registry
{
	I3_EXPORT_SCENE	UINT32		Init(const char * pszFileName);
	I3_EXPORT_SCENE BOOL		Save( const char * pszFileName);
	I3_EXPORT_SCENE void		End(const char * pszFileNames = NULL);
	I3_EXPORT_SCENE void		Destroy();

	I3_EXPORT_SCENE i3RegKey	* FindKey( const char * pszKeyName, i3RegKey * pRoot = NULL);
	I3_EXPORT_SCENE i3RegData	* FindData( i3RegKey * pKey, const char * pszDataName);
};
#endif