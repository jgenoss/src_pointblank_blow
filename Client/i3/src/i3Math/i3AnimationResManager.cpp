#include "i3MathType.h"
#include "i3AnimationResManager.h"
#include "i3AnimationPackFile.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"


I3_CLASS_INSTANCE( i3AnimationResManager);


bool i3AnimationResManager::_FindFromPack( char * pszOut, const char * pszName)
{
	UINT128 hash;

	char conv[MAX_PATH];

	i3::safe_string_copy( conv, pszName, MAX_PATH );
	i3::make_unix_path( conv);
	i3::to_upper( conv);

	i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &hash);

	i3::vector_map<UINT128,UINT32>::iterator it;
	it = m_LinkedPackList.find( hash);

	if( it != m_LinkedPackList.end())
	{
		UINT32 tableIdx = (UINT32)it->second;
		I3ASSERT( tableIdx != 0xFFFFFFFF);

		i3::safe_string_copy( pszOut, m_PackPathList.at( tableIdx).c_str(), MAX_PATH);
		//i3::safe_string_copy( pszOut, ((std::string)it->second).c_str(), MAX_PATH);
		return true;
	}

	return false;
}

i3ResourceObject* i3AnimationResManager::GetResource( const char * pszResFile, const char * pszBasePath, const char *szName, bool bBackgroundLoad )
{
	i3Animation * pAnim;
	char szFull[ MAX_PATH];

	pAnim = (i3Animation *) i3NamedResourceManager::GetResource( pszResFile, pszBasePath, szName);
	
	if( pAnim != nullptr)
		return (i3ResourceObject *) pAnim;

	// PackList에서 찾는다.
	if( _FindFromPack( szFull, szName) == true)
	{
		i3AnimationPackFile file;

		file.LoadFromFile( (char *) szFull);

		pAnim = (i3Animation *) i3NamedResourceManager::GetResource( pszResFile, pszBasePath, szName);
		if( pAnim != nullptr)
			return (i3ResourceObject *) pAnim;

		// Pack에도 없다...
	}

	if( pszBasePath != 0 && pszBasePath[0] != 0)
	{
		sprintf( szFull, "%s/%s", pszBasePath, szName);
	}
	else
	{
		i3::safe_string_copy( szFull, szName, MAX_PATH);
	}

	i3ResourceFile::setProfileEnable( true);
	pAnim = i3Animation::LoadFromFile( szFull);

	i3ResourceFile::setProfileEnable( false);
	if( pAnim == nullptr)
	{
		I3TRACE( "[ANIM_RES_MNGER] Could not load %s animation file.\n", szFull);
		return nullptr;
	}

	AddResource( pAnim, szName);
	I3_MUST_RELEASE( pAnim);
	return pAnim;
}

UINT32 i3AnimationResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	char szFull[ MAX_PATH];
	i3Animation * pAnim = (i3Animation *) pObj;
	UINT32 Rc;

	I3ASSERT( i3::kind_of<i3Animation*>(pObj));
	I3ASSERT( pObj->hasName());

	if( pszBasePath[0] != 0)
	{
		i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, pObj->GetNameString());
	}
	else
	{
		i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);
	}

	Rc = pAnim->SaveToFile( szFull);

	return Rc;
}

UINT32 i3AnimationResManager::_FindPackPathString( const char * pszPackPath)
{
	UINT32 strTableIdx = 0;
	for( strTableIdx = 0; strTableIdx < m_PackPathList.size(); strTableIdx++)
	{
		std::string str = m_PackPathList.at( strTableIdx);
		if( i3::generic_is_iequal( str.c_str(), pszPackPath) )
		{
			return strTableIdx;
		}
	}

	return 0xFFFFFFFF;
}

void i3AnimationResManager::AddResourcePack( const char * pszAnimPath, const char * pszPackPath)
{
	UINT128 hash;

	char conv[MAX_PATH];

	i3::safe_string_copy( conv, pszAnimPath, MAX_PATH );
	i3::make_unix_path( conv);
	i3::to_upper( conv);

	i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &hash);

	UINT32 findIdx = _FindPackPathString( pszPackPath);
	if( findIdx == 0xFFFFFFFF)
	{
		std::string str( pszPackPath);

		findIdx = m_PackPathList.size();
		m_PackPathList.push_back( str);
	}

	m_LinkedPackList.insert(i3::vector_map<UINT128,UINT32>::value_type( hash, findIdx));
}
