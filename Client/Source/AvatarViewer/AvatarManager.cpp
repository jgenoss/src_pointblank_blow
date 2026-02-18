#include "pch.h"
#include "AvatarManager.h"
#include "Avatar.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/substr.h"

I3_CLASS_INSTANCE( cThreadAvatar); //, i3ElementBase);
I3_CLASS_INSTANCE( cAvatarManager); //, i3Thread);

void cThreadAvatar::Create(vPartsList list, TEAM_TYPE team)
{
	m_pAvatar = Avatar::new_object();

	m_pAvatar->SetTeam( team);

	for (size_t i = 0; i < list.size(); ++i)
	{
		m_pAvatar->setPartName((EQUIP::ePART)i,list[i].c_str());
	}

	i3ImageFile::SetBGLoadEnable( false);
}


cAvatarManager::cAvatarManager(void)
:	m_nReadIdx(0)
,	m_nWriteIdx(0)
{
}

cAvatarManager::~cAvatarManager(void)
{
	ReleaseMacroTexture();
}

Avatar * cAvatarManager::PushAvatar(INT32 idx, vPartsList list, TEAM_TYPE team)
{
	cThreadAvatar* pThread  = cThreadAvatar::new_object();
	pThread->Create(list, team);
	pThread->GetAvatar()->SetNetIdx(idx);
	m_qAvatarCon.push(pThread);

	if (m_nReadIdx == m_nWriteIdx)
		Reset();

	return pThread->GetAvatar();
}

void cAvatarManager::PushParts(INT32 idx, EQUIP::ePART part, const char * pszPath)
{
	mAvatarCon::iterator iter = m_mAvatarList.begin();
	for (;iter != m_mAvatarList.end();++iter)
	{
		if (iter->first == m_nReadIdx)
		{
			Avatar* pAvatar = m_mAvatarList[m_nReadIdx--];
			pAvatar->setPartName(part, pszPath);

			cThreadAvatar* pThread  = cThreadAvatar::new_object();
			pThread->SetAvatar(pAvatar);
			pThread->GetAvatar()->SetNetIdx(idx);
			pThread->SetState(E_AVATAR_STATE_NA);
			m_qAvatarCon.push(pThread);
		}
	}
}

UINT32 cAvatarManager::OnRunning( void * pData )
{
	while(1)
	{
		while(!m_qAvatarCon.empty())
		{
			cThreadAvatar* pThread = m_qAvatarCon.front();
			if (pThread->GetState() == E_AVATAR_STATE_NA)
			{
				pThread->SetState(E_AVATAR_STATE_LOADING);
				continue;
			}
			else if (pThread->GetState() == E_AVATAR_STATE_LOADING)
			{
				pThread->GetAvatar()->OnUpdate(0.0f);

				if (pThread->GetAvatar()->GetLoaded() == true)
					pThread->SetState(E_AVATAR_STATE_LOADED);
			}
			else if (pThread->GetState() == E_AVATAR_STATE_LOADED)
			{
				m_pCurrAvatar = pThread->GetAvatar();
				
				m_pCurrAvatar->AddRef();

				I3_SAFE_RELEASE(pThread);
				m_qAvatarCon.pop();
				++m_nReadIdx;
				continue;
			}
		}
	}
	
	return 0;
}

void cAvatarManager::GetAvatar(Avatar*& dest)
{
	Avatar* pAvatar = m_mAvatarList[m_nReadIdx];

	I3_REF_CHANGE( dest, pAvatar);
	pAvatar->Release();
	
// 	mAvatarCon::iterator iter = m_mAvatarList.begin();
// 	for (;iter != m_mAvatarList.end();)
// 	{
// 		if (iter->first == m_nReadIdx)
// 		{
// 			m_mAvatarList.erase(iter++);
// 		}
// 		else
// 			++iter;
// 	}
}


cAvatarManager::tagMacroTexInfo * cAvatarManager::_FindMacroTexInfo( T_ItemID upper, T_ItemID lower, T_ItemID glove)
{
	tagMacroTexInfo * pInfo;

	for( size_t i = 0; i < m_mapLoadedMacroTex.size(); i++)
	{
		pInfo = m_mapLoadedMacroTex.at( i);
		if( pInfo->_PartsItemID[ EQUIP::ePART_VEST] != upper)
			continue;

		if( pInfo->_PartsItemID[ EQUIP::ePART_PANT] != lower)
			continue;

// 		if( pInfo->_PartsItemID[ EQUIP::ePART_CHARA] != mask)
// 			continue;

		if( pInfo->_PartsItemID[ EQUIP::ePART_GLOVE] != glove)
			continue;

		// 동일한 파츠이다.
		return pInfo;
	}

	return NULL;
}

i3Texture *	cAvatarManager::FindMacroTexture( AVATAR::TEX_TYPE texType, TEAM_TYPE team, T_ItemID upper, T_ItemID lower, T_ItemID glove)
{
	tagMacroTexInfo * pInfo = _FindMacroTexInfo( upper, lower, glove);
	if( pInfo != NULL)
	{	
		switch( texType)
		{
		case AVATAR::TEX_DIFF :	return pInfo->_pMacroTexDiff[ team];
		case AVATAR::TEX_NORM :	return pInfo->_pMacroTexNorm;
		case AVATAR::TEX_SPEC :	return pInfo->_pMacroTexSpec;
		case AVATAR::TEX_REFM :	return pInfo->_pMacroTexRefMask;
		}
	}

	return NULL;
}

void cAvatarManager::RegisterMacroTexture( i3Texture * pTex, AVATAR::TEX_TYPE texType, TEAM_TYPE team, T_ItemID upper, T_ItemID lower, T_ItemID glove)
{
	tagMacroTexInfo * pInfo = _FindMacroTexInfo( upper, lower, glove);

	if( pInfo == NULL)
	{
		pInfo = (tagMacroTexInfo *) i3MemAlloc( sizeof( tagMacroTexInfo));
		I3ASSERT( pInfo != NULL);

		pInfo->_PartsItemID[ EQUIP::ePART_VEST] = upper;
		pInfo->_PartsItemID[ EQUIP::ePART_PANT] = lower;
		//pInfo->_PartsItemID[ EQUIP::ePART_CHARA] = mask;
		pInfo->_PartsItemID[ EQUIP::ePART_GLOVE] = glove;
		pInfo->_pMacroTexDiff[ 0] = NULL;
		pInfo->_pMacroTexDiff[ 1] = NULL;
		pInfo->_pMacroTexNorm = NULL;
		pInfo->_pMacroTexSpec = NULL;
		pInfo->_pMacroTexRefMask = NULL;

		m_mapLoadedMacroTex.push_back( pInfo);
	}

	switch( texType)
	{
	case AVATAR::TEX_DIFF :	I3_REF_CHANGE( pInfo->_pMacroTexDiff[ team], pTex);	break;
	case AVATAR::TEX_NORM : I3_REF_CHANGE( pInfo->_pMacroTexNorm, pTex);		break;
	case AVATAR::TEX_SPEC :	I3_REF_CHANGE( pInfo->_pMacroTexSpec, pTex);		break;
	case AVATAR::TEX_REFM :	I3_REF_CHANGE( pInfo->_pMacroTexRefMask, pTex);		break;
	}
}

void cAvatarManager::ReleaseMacroTexture( void)
{
	for( size_t i = 0; i < m_mapLoadedMacroTex.size(); i++)
	{
		tagMacroTexInfo * pInfo = m_mapLoadedMacroTex.at( i);
		I3_SAFE_RELEASE( pInfo->_pMacroTexDiff[0]);
		I3_SAFE_RELEASE( pInfo->_pMacroTexDiff[1]);
		I3_SAFE_RELEASE( pInfo->_pMacroTexNorm);
		I3_SAFE_RELEASE( pInfo->_pMacroTexSpec);
		I3_SAFE_RELEASE( pInfo->_pMacroTexRefMask);

		I3MEM_SAFE_FREE( pInfo);
	}

	m_mapLoadedMacroTex.clear();
}

void cAvatarManager::FindPartsFromFolder( const char * pszPartName, mapParts & list)
{
	::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
	if (FALSE == bDir)
	{
		i3ResourceFile::SetWorkingFolder("");
		return;
	}

	char szPath[ MAX_PATH];
	char szExt[ 16];

	i3::snprintf( szPath, sizeof(szPath), "%s/Avatar/%s/*.*", i3ResourceFile::GetWorkingFolder(), pszPartName);

	HANDLE				hFind		= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		ffd;

	hFind = ::FindFirstFile( szPath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	BOOL bFind = TRUE;
	while( bFind)
	{
		bFind = ::FindNextFile( hFind, &ffd);

		if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if( i3::contain_string( ffd.cFileName, ".svn") >= 0 ||
				i3::contain_string( ffd.cFileName, ".") >= 0 ||
				i3::contain_string( ffd.cFileName, "..") >= 0 ||
				i3::contain_string( ffd.cFileName, "Texture") >= 0 ||
				i3::contain_string( ffd.cFileName, "Res") >= 0 )
			{
				continue;
			}

			char szSub[ MAX_PATH];
			i3::snprintf( szSub, sizeof(szSub), "%s/%s", pszPartName, ffd.cFileName);
			FindPartsFromFolder( szSub, list);

		}
		else
		{
			i3::extract_fileext( ffd.cFileName, szExt);
			i3::to_upper( szExt);

			if( i3::generic_is_iequal( szExt, "I3S") )
			{
				// 마지막의 _LOD# Postfix 제거
				INT32 pos = i3::contain_string( ffd.cFileName,"_LOD");
				
				char defaultName[ MAX_PATH];

				if (pos != -1)
					i3::substr( defaultName, ffd.cFileName, 0 ,pos);
				else
					i3::safe_string_copy( defaultName, ffd.cFileName, sizeof( defaultName));

				mapParts::iterator it = list.find( defaultName);

				if( it == list.end())
				{
					tagPARTSINFO * pInfo = (tagPARTSINFO*) i3MemAlloc( sizeof(tagPARTSINFO));
					pInfo->_hItem = NULL;
					pInfo->_flag = false;
					pos = i3::contain_string( szPath,"*.*");
					I3ASSERT( pos != -1);
					i3::substr( pInfo->_szFullPath, szPath, 0, pos);
					i3::generic_string_cat( pInfo->_szFullPath, defaultName);

					list.insert( mapParts::value_type( defaultName, pInfo) );
				}
			}
		}
	}

	::FindClose(hFind);
}