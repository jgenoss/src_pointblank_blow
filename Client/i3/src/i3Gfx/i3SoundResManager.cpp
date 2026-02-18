#include "i3GfxType.h"
#include "i3TextureResManager.h"
#include "i3Texture.h"
#include "i3ImageFile.h"

I3_CLASS_INSTANCE( i3TextureResManager, i3NamedResourceManager);

i3TextureResManager::i3TextureResManager(void)
{
}

i3TextureResManager::~i3TextureResManager(void)
{
}

i3ResourceObject*	i3TextureResManager::GetResource( const char * pszBasePath, const char *szName )
{
	i3Texture * pTex;
	char szFull[ MAX_PATH];

	pTex = (i3Texture *) i3NamedResourceManager::GetResource( pszBasePath, szName);
	if( pTex != NULL)
		return pTex;

	if( pszBasePath[0] != 0)
		sprintf( szFull, "%s/%s", pszBasePath, szName);
	else
		strcpy( szFull, szName);

	
	i3ImageFile file;

	pTex = file.Load( szFull);

	AddResource( pTex, szName);

	return pTex;
}

UINT32				i3TextureResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	i3Texture * pTex = (i3Texture *) pObj;
	char szFull[ MAX_PATH];
	i3ImageFile file;
	UINT32 Rc;

	I3ASSERT( pObj != NULL);
	I3ASSERT( pObj->GetName() != NULL);

	if( pszBasePath[0] != 0)
		sprintf( szFull, "%s/%s", pszBasePath, pObj->GetName());
	else
		strcpy( szFull, pObj->GetName());

	Rc = file.Save( szFull, pTex);

	return Rc;
}
