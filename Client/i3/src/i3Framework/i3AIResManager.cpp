#include "i3FrameworkPCH.h"
#include "i3AIResManager.h"
#include "i3AI.h"


I3_CLASS_INSTANCE( i3AIResManager);


i3ResourceObject*	i3AIResManager::GetResource( const char * pszResFile, const char * pszBasePath,
												const char *szName, bool bBackgroundLoad )
{
	i3AI * pAI;
	char szFull[ MAX_PATH];

	pAI = (i3AI *) i3NamedResourceManager::GetResource( pszResFile, pszBasePath, szName);
	if( pAI != nullptr)
		return (i3ResourceObject *) pAI;

	if( pszBasePath[0] != 0)
		sprintf( szFull, "%s/%s", pszBasePath, szName);
	else
		strncpy( szFull, szName, sizeof( szFull) - 1);

	
	i3ResourceFile file;

	if( file.Load( szFull) == STREAM_ERR)
		return nullptr;

	pAI = (i3AI *) file.getKeyObject();
	I3ASSERT( i3::same_of< i3AI*>(pAI));

	AddResource( (i3ResourceObject *) pAI, szName);

	return (i3ResourceObject *) pAI;
}

UINT32				i3AIResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	i3AI * pAI = (i3AI *) pObj;
	char szFull[ MAX_PATH];
	i3ResourceFile file;
	UINT32 Rc;

	I3ASSERT( pObj != nullptr);
	I3ASSERT( pObj->hasName());

	if( pszBasePath[0] != 0)
		i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, pObj->GetNameString());
	else
		i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);

	file.setKeyObject( pAI);

	Rc = file.Save( szFull);

	return Rc;
}
