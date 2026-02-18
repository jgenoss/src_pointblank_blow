#include "stdafx.h"
#include "i3Sound.h"
#include "i3SoundFile.h"
#include "i3SoundResManager.h"

I3_CLASS_INSTANCE( i3SoundResManager);

i3ResourceObject*	i3SoundResManager::GetResource( const char * pszResFile, const char * pszBasePath,
												   const char *szName, bool bBackgroundLoad )
{
	i3Sound * pSnd;
	char szFull[ MAX_PATH];

	pSnd = (i3Sound *) i3NamedResourceManager::GetResource( pszResFile, pszBasePath, szName);
	if( pSnd != nullptr)
		return pSnd;

	if( pszBasePath[0] != 0)
		sprintf( szFull, "%s/%s", pszBasePath, szName);
	else
		strncpy( szFull, szName, sizeof(szFull) - 1);

	i3SoundFile file;

	pSnd = file.Load( szFull);

#if defined( I3_DEBUG)
	if( pSnd != nullptr)
	{
		pSnd->m_FileSize = sizeof( *pSnd) + pSnd->GetDataLength();
	}
#endif

	AddResource( pSnd, szName);

	/////////////////////////////////////////////////////////////////////////////
	// Texture 와 Sound의 Extern Res인경우
	// OnLoad가 종료된 시점에서 Intern Res와 RefCount가 서로 다르게됨
	// ( Extern : 2( File Load : 1, AddResource : 1 ),  Intern : 0 )
	// AddedResource완료 시점에서 RefCount가 1이되도록함
	{
		I3ASSERT( pSnd && pSnd->GetRefCount() == 2);
		I3_MUST_RELEASE( pSnd);
	}

	return pSnd;
}

UINT32				i3SoundResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	i3Sound * pSnd = (i3Sound *) pObj;
	char szFull[ MAX_PATH];
	i3SoundFile file;
	UINT32 Rc;

	I3ASSERT( pObj != nullptr);

	if( pszBasePath[0] != 0)
		i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, pObj->GetNameString());
	else
		i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);

	Rc = file.Save( szFull, pSnd);

	return Rc;
}

void i3SoundResManager::FlushBuffers(void)
{
	i3Sound * pSnd = nullptr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pSnd = (i3Sound *)m_List.get_vector()[i];

		pSnd->ClearBuffers();
	}
}