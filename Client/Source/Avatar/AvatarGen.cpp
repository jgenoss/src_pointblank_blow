#include "pch.h"
#include "AvatarGen.h"
#include "AvatarDefine.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/itl/range/generic_empty.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#if defined( CLIENT_SIDE)
	#include "Avatar/AvatarManager.h"
#endif


void AvatarGen::setTextureQuality( INT32 qual)
{
	m_TextureQuality = qual;
}

i3Texture * AVATAR::LoadTex( const char * pszTexName, TEAM_TYPE team)
{
	if( pszTexName != nullptr )
	{
		if( i3::generic_string_size( pszTexName) > 0)
		{
			i3::string strCampName;
			if( team == TEAM_BLUE)
				strCampName = "_BLUE.i3i";
			else
				strCampName = "_RED.i3i";

			INT32 nStartPos = 0;

			i3::string szPathName = pszTexName;
			INT32 nEndPos = szPathName.rfind('_');
			i3::string szName = szPathName.substr(nStartPos, nEndPos);

			i3::string szCurrName = szName + strCampName;

			i3Texture * pChangeTex = g_pFramework->GetResourceManager()->LoadTexture( szCurrName.c_str());
			if( pChangeTex != nullptr)
			{
				I3_MUST_RELEASE(pChangeTex);	// RefCount를 맞춰야된다.
				return pChangeTex;
			}
		}
	}

	return nullptr;
}



I3_CLASS_INSTANCE( AvatarGen);

AvatarGen::AvatarGen(void)
{
	i3mem::Fill(m_IO, -1, sizeof(m_IO));
	i3mem::Fill(m_HeadIO, -1, sizeof(m_HeadIO));

#if !defined( CLIENT_SIDE)
	m_bInThread = false;
#endif
}

AvatarGen::~AvatarGen(void)
{
	Destroy();
}

void AvatarGen::Destroy( void)
{
	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE( m_pSgInfo[i][j]);
		}
	}

	for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
	{
		for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE( m_pHeadSgInfo[i][j]);
			I3_SAFE_RELEASE( m_pHeadLOD[i][j]);
		}
	}

	for( INT32 i = 0; i < TEX_LOAD_LEVEL_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pTex_Diff[i]);
		I3_SAFE_RELEASE( m_pTex_Norm[i]);
		I3_SAFE_RELEASE( m_pTex_Spec[i]);
		I3_SAFE_RELEASE( m_pTex_RefMask[i]);
	}

	I3_SAFE_RELEASE( m_pNewLOD);

	I3_SAFE_RELEASE( m_pBody);

	// local로 생성하는 경우 포인터를 담아둬야 해제가 가능
	I3_SAFE_RELEASE( m_pNewGeoAttr);

	for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		m_ItemID[ i] = 0;
		m_strName[i].clear();
	}

	UINT32 cnt = m_DiffTexList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		I3_SAFE_DELETE( m_DiffTexList.at(i));
	}
	m_DiffTexList.clear();

	cnt = m_NormTexList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		I3_SAFE_DELETE( m_NormTexList.at(i));
	}
	m_NormTexList.clear();

	cnt = m_SpecTexList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		I3_SAFE_DELETE( m_SpecTexList.at(i));
	}
	m_SpecTexList.clear();

	cnt = m_RefMTexList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		I3_SAFE_DELETE( m_RefMTexList.at(i));
	}
	m_RefMTexList.clear();
}

i3::rc_string AvatarGen::getPartName( EQUIP::ePART part) const
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	return m_strName[ part];
}

#if defined( CLIENT_SIDE)
void AvatarGen::SetPart( T_ItemID itemID, bool bInLobby, bool bForceRebuild)
{
	m_bInLobby = bInLobby;

	EQUIP::ePART part = EQUIP::ItemID2PartsType( itemID);

	if( part != EQUIP::ePART_UNKNOWN)
	{
		const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( itemID);

		if( (pInfo != nullptr) && ( m_ItemID[ part] != itemID))
		{
			m_ItemID[ part] = itemID;

			EQUIP::eBERET res_subtype = EQUIP::eBERET_NONE;
			if( part == EQUIP::ePART_BERET && m_ItemID[ EQUIP::ePART_HEADGEAR] != 0)
			{
				const CEquipInfo * pHeadInfo = g_pCharaInfoDataBase->GetInfo(  m_ItemID[ EQUIP::ePART_HEADGEAR]);
				if( pHeadInfo )
				{
					res_subtype = static_cast<EQUIP::eBERET>( pHeadInfo->GetCombinationType());
				}
			}

			i3::rc_wstring	wstrname;
			pInfo->GetResPostName(wstrname, m_Team, res_subtype, isFemale() );

			i3::string strname;		
			i3::utf16_to_mb(wstrname, strname);

			SetPartName(part, strname, bForceRebuild);

			if( part == EQUIP::ePART_SKIN)
			{
				// 파츠를 사용하지 않는다.
				m_ItemID[ EQUIP::ePART_VEST] = 0;
				m_strName[ EQUIP::ePART_VEST].clear();
				m_ItemID[ EQUIP::ePART_PANT] = 0;
				m_strName[ EQUIP::ePART_PANT].clear();
				m_ItemID[ EQUIP::ePART_GLOVE] = 0;
				m_strName[ EQUIP::ePART_GLOVE].clear();

				for( INT32 i = 0; i < AVT_LOD_COUNT; i++)
				{
					m_IO[ EQUIP::ePART_VEST][ i] = -1;
					m_bReported[ EQUIP::ePART_VEST][i] = false;
					I3_SAFE_RELEASE( m_pSgInfo[ EQUIP::ePART_VEST][i]);

					m_IO[ EQUIP::ePART_PANT][ i] = -1;
					m_bReported[ EQUIP::ePART_PANT][i] = false;
					I3_SAFE_RELEASE( m_pSgInfo[ EQUIP::ePART_PANT][i]);

					m_IO[ EQUIP::ePART_GLOVE][ i] = -1;
					m_bReported[ EQUIP::ePART_GLOVE][i] = false;
					I3_SAFE_RELEASE( m_pSgInfo[ EQUIP::ePART_GLOVE][i]);
				}

				// 장비를 Skin용으로 교체
				// vv3 - check
				const CSkinInfo * pSkinInfo = static_cast<const CSkinInfo*>(pInfo);
				T_ItemID tempID = pSkinInfo->GetHeadgearItemID();
				if( tempID != 0)
					SetPart( tempID, bInLobby, bForceRebuild);
				else
					m_ItemID[ EQUIP::ePART_HEADGEAR] = 0;

				tempID = pSkinInfo->GetFacegearItemID();
				if( tempID != 0)
					SetPart( tempID, bInLobby, bForceRebuild);
				else
					m_ItemID[ EQUIP::ePART_FACEGEAR] = 0;

				tempID = pSkinInfo->GetBeltItemID();
				if( tempID != 0)
					SetPart( tempID, bInLobby, bForceRebuild);
				else
					m_ItemID[ EQUIP::ePART_BELT] = 0;

				tempID = pSkinInfo->GetHolsterItemID();
				if( tempID != 0)
					SetPart( tempID, bInLobby, bForceRebuild);
				else
					m_ItemID[ EQUIP::ePART_HOLSTER] = 0;
			}
		}
	}
}
#endif

void AvatarGen::SetPartName( EQUIP::ePART part, const i3::string& strPath, bool bForceRebuild)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);

	if( bForceRebuild == false)
	{
		if( i3::generic_is_equal(m_strName[ part], strPath) )
			return;
	}
	
	m_strName[part] = strPath; 

#if defined( CLIENT_SIDE)
	if( part != EQUIP::ePART_CHARA)
	{
		char szFullPath[ MAX_PATH] = {0,};
		GetResPath( part, 1, szFullPath, _countof( szFullPath));
		if( i3FileStream::isFile( szFullPath) == false)
		{
			// 없으면 기본 파츠로 설정
			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( m_ItemID[EQUIP::ePART_CHARA]);
			T_ItemID itemID = pCharaDBInfo->GetPartsItemID( part);
			m_ItemID[ part] = itemID;

			i3::rc_wstring wstr;
			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( itemID);
			pInfo->GetResPostName( wstr, m_Team, EQUIP::eBERET_NONE, isFemale() );

			i3::utf16_to_mb( wstr, m_strName[part]);
		}
	}
#endif

	for( INT32 i = 0; i < AVT_LOD_COUNT; i++)
	{
		m_IO[ part][ i] = -1;
		m_bReported[ part][i] = false;
		I3_SAFE_RELEASE( m_pSgInfo[ part][i]);
	}

#if defined( USE_EXCHANGE_HEAD)
	if( part == EQUIP::ePART_CHARA)
	{
		for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
		{
			for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
			{
				m_HeadIO[i][j] = -1;
				I3_SAFE_RELEASE( m_pHeadSgInfo[i][j]);
			}
		}

		m_LoadHeadState = STATE_LOADSTART;
	}
#endif

	if( AVATAR::getPartStyle( part) & AVT_PART_STYLE_SKIN)
	{	// 스킨을 다시 구성해야 한다면 이전 Body는 삭제
		I3_SAFE_RELEASE( m_pBody);
	}

	m_LoadState = STATE_LOADSTART;
	m_bLoaded = false;
}

INT32 AvatarGen::getIOHandle( EQUIP::ePART part, INT32 lod)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);
	I3_BOUNDCHK( lod, AVT_LOD_COUNT);

	return m_IO[ part][lod];
}


LOADSTATE AvatarGen::getLoadState( EQUIP::ePART part, INT32 lod)
{
	I3_BOUNDCHK( part, EQUIP::ePART_COUNT);
	I3_BOUNDCHK( lod, AVT_LOD_COUNT);

	I3ASSERT( m_IO[ part][lod] != -1);

	i3SceneIOManager * pManager = i3SceneIOManager::getInstance();

	return pManager->getLoadState( m_IO[ part][lod]);
}

bool AvatarGen::LoadingParts(void)
{
	for( INT32 i = 0; i < EQUIP::ePART_HEADGEAR; i++)
	{
		// Loading이 설정된 Path가 없다면 논외!
		if( m_strName[i].empty() )
			continue;

#if defined(USE_EXCHANGE_HEAD)
		if( EQUIP::ePART_CHARA == i)
			continue;
#endif

		if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
		{
			for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
			{
				if( i != EQUIP::ePART_VEST && i != EQUIP::ePART_GLOVE)
				{// 상체만 보이는 LOD의 경우 나머지는 등록하지 않는다.
					if( lod == 0)
						continue;
				}

				if( __LoadingParts( (EQUIP::ePART) i, lod) == false)
					return false;
			}
		}
		else
		{
			if( m_LoadStyle & AVATAR_LOAD_STYLE_1PV)
			{
				if( __LoadingParts( (EQUIP::ePART) i, 0) == false)
					return false;
			}

			if( __LoadingParts( (EQUIP::ePART) i, 1) == false)
				return false;
		}
	}

	return true;
}

bool AvatarGen::isBodyLoaded( void)
{
	if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
	{
		for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
		{
			if( __LoadingParts( EQUIP::ePART_VEST, lod) == false)
				return false;
		}
	}
	else
	{
		if( m_LoadStyle & AVATAR_LOAD_STYLE_1PV)
		{
			if( __LoadingParts( EQUIP::ePART_VEST, 0) == false)
				return false;
		}

		if( __LoadingParts( EQUIP::ePART_VEST, 1) == false)
			return false;
	}

	return true;
}

bool AvatarGen::__LoadingPartsHead( AVATAR::HEAD_TYPE type, INT32 lod)
{
	i3SceneIOManager * pManager = i3SceneIOManager::getInstance();

	if( m_HeadIO[ type][ lod] != -1)
	{
		switch( pManager->getLoadState( m_HeadIO[ type][lod]) )
		{
		case LOADSTATE_LOADING :	return false;	// 아직 Loading중
		case LOADSTATE_ERROR :		I3TRACE( "Load fail head %d %d\n", type, lod); break;
		case LOADSTATE_LOADED :
			{
				m_pHeadSgInfo[ type][ lod] = pManager->PopLoadedScene( m_HeadIO[ type][ lod], g_pFramework->getUpdateID());
				I3ASSERT( m_pHeadSgInfo[ type][ lod] != nullptr);
  				m_HeadIO[ type][ lod] = -1;
			}
			break;
		}
	}

	return true;
}

bool AvatarGen::LoadingPartsHead( void)
{
	for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
	{
		if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
		{
			for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
			{
				if( __LoadingPartsHead( (AVATAR::HEAD_TYPE) i, lod) == false)
					return false;
			}
		}
		else
		{
			if( __LoadingPartsHead( (AVATAR::HEAD_TYPE) i, 1) == false)
				return false;
		}
	}

	return true;
}

void AvatarGen::GetResPath( EQUIP::ePART part, INT32 lod, char * pszDest, INT32 len)
{
	const i3::fixed_string& strPart = AVATAR::getPartName( part);
	AVT_PART_STYLE style = AVATAR::getPartStyle( part);

	if( (style & AVT_PART_STYLE_BODY) > 0)
	{
		char szName[ MAX_PATH];
		i3::generic_string_ncopy(szName, m_strName[ part], sizeof( szName) );
	
		// Mask에서 X,Y,Z는 제거
		if( part == EQUIP::ePART_CHARA)
		{
			INT32 pos = i3::contain_string( szName,"_X");
			if( pos != -1)
				szName[pos] = 0;
			pos = i3::contain_string( szName,"_Y");
			if( pos != -1)
				szName[pos] = 0;
			pos = i3::contain_string( szName,"_Z");
			if( pos != -1)
				szName[pos] = 0;
		}

		INT32 nPos = i3::contain_char( szName, '/');

		if( nPos != -1)
		{// 경로 포함
			i3::snprintf( pszDest, len, "Avatar/%s/%s_LOD%d.i3s", strPart, szName, lod);
		}
		else
			i3::snprintf( pszDest, len, "Avatar/%s/%s/%s_LOD%d.i3s", strPart, szName, szName, lod);
	}
	else
	{	// 장비품
		i3::snprintf( pszDest, len, "Avatar/%s/%s/%s.i3s", strPart, m_strName[part], m_strName[part]);
	}
}

void AvatarGen::__MakeFullPath( char * pszOut, const char * pszPath, UINT32 size)
{
	if( i3System::isAbsolutePath( pszPath))
	{
		i3::safe_string_copy( pszOut, pszPath, size);
	}
	else
	{
		if (i3::generic_empty(i3ResourceFile::GetWorkingFolder()) )
			i3::snprintf( pszOut, size, "%s", pszPath);
		else
			i3::snprintf( pszOut, size, "%s/%s", i3ResourceFile::GetWorkingFolder(), pszPath);
	}
}

void AvatarGen::_LoadParts( const char * pszPath, EQUIP::ePART part, INT32 lod, bool bDummyAvatar)
{
	char szFull[ MAX_PATH];

	__MakeFullPath( szFull, pszPath, sizeof( szFull));

	i3SceneGraphInfo * pSgInfo = i3SceneIOManager::getInstance()->GetLoadedScene( szFull);
 
    if( pSgInfo != nullptr)
    {
    	// 기존에 이미 Loading되어 있던 것을 재활용.
		m_pSgInfo[part][lod] = pSgInfo;
    }
    else
 	{
		// 장비품은 DX형태로 로딩한다.
		// 나머지 파츠는 메모리에만 로드
//		bool bDX = ((part>=EQUIP::ePART_HEADGEAR)?true:false);
		
		m_IO[part][lod] = i3SceneIOManager::getInstance()->Request( szFull, false, bDummyAvatar);	// Request Loading
		//I3TRACE( "2 Req load Resource File %s\n", szFull);
	}
}



void AvatarGen::_LoadHeads( const char * pszPath, AVATAR::HEAD_TYPE type, INT32 lod)
{
#if defined( CLIENT_SIDE)
	i3SceneGraphInfo * pSgInfo = g_pCharaManager->FindFaceResource( pszPath);
	I3ASSERT( pSgInfo != nullptr);
	I3_REF_CHANGE( m_pHeadSgInfo[type][lod], pSgInfo);
#else
	char szFull[ MAX_PATH];

	__MakeFullPath( szFull, pszPath, sizeof( szFull));
 
 	i3SceneGraphInfo * pSgInfo = i3SceneIOManager::getInstance()->GetLoadedScene( szFull);
 
    if( pSgInfo != nullptr)
    {
    	// 기존에 이미 Loading되어 있던 것을 재활용.
		m_pHeadSgInfo[type][lod] = pSgInfo;
    }
    else
 	{
		if( i3FileStream::isFile( szFull) == false)
		{
			// 파일이 없습니다.
			I3PRINTLOG(I3LOG_FATAL,  "No Resource File %s", szFull);
		}

		m_HeadIO[type][lod] = i3SceneIOManager::getInstance()->Request( szFull);			// Request Loading
		//I3TRACE( "2 Req load Head Resource File %s\n", szFull);
	}
#endif
}

void AvatarGen::LoadStart( bool bDummyAvatar)
{
	if( hasSkinParts() == false)
	{
		for( INT32 i = 0; i < EQUIP::ePART_HEADGEAR; i++)
 		{
 			if( m_strName[ i].empty() )
 				continue;

	#if defined( USE_EXCHANGE_HEAD)
			if( i == EQUIP::ePART_CHARA)
			{
				if( m_pHeadSgInfo[i][1] != nullptr)
				{	// 이전의 머리를 사용
					m_LoadHeadState = STATE_LOADING;
				}
				continue;
			}
	#endif

			if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
			{
				for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
 				{
					if( (j == 0) &&
						(i != EQUIP::ePART_VEST && i != EQUIP::ePART_GLOVE) )
					{// 상체만 보이는 LOD의 경우 나머지는 등록하지 않는다.
						continue;
					}

					__LoadPart( (EQUIP::ePART) i, j, bDummyAvatar);
 				}
			}
			else
			{	// Lobby용
				if( m_LoadStyle & AVATAR_LOAD_STYLE_1PV)
				{
					__LoadPart( (EQUIP::ePART) i, 0, bDummyAvatar);
				}

				__LoadPart( (EQUIP::ePART) i, 1, bDummyAvatar);
			}
 		}

		m_LoadState = STATE_LOADING;
	}
	else
	{
		for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			__LoadPart( EQUIP::ePART_SKIN, j, bDummyAvatar);
#if !defined( USE_EXCHANGE_HEAD)
			if( (j > 0) && (m_strName[ EQUIP::ePART_CHARA].empty() == false) )
				__LoadPart( EQUIP::ePART_CHARA, j, bDummyAvatar);
#endif
		}

		m_LoadState = STATE_LOADING_SKIN;
	}
}

void AvatarGen::CreateBody( void)
{
	m_pBody = i3Body::new_object();
#if defined( I3_DEBUG)
	m_pBody->SetName( "AvatarGen Body");
#endif

	UINT32 maxBone = 0;
	i3LOD * pBaseLOD = _FindBaseLOD( &maxBone);
	I3ASSERT(pBaseLOD != nullptr);
	I3ASSERT( m_FrameLODNum < 2);
	m_pPreFrameSkeleton = pBaseLOD->getSkeleton();
}

void AvatarGen::BuildSceneForSkin( void)
{
	m_pBody = i3Body::new_object();
#if defined( I3_DEBUG)
	m_pBody->SetName( "AvatarSkin Body");
#endif

	INT32 maxBone = -1;

	for( INT32 lod = 1; lod < AVT_LOD_COUNT; lod++)
	{
		i3SceneGraphInfo * pSgInfo = getSgInfo( EQUIP::ePART_SKIN, lod);

		if( pSgInfo == nullptr)
			continue;

		if( pSgInfo->getInstanceSg() == nullptr)
			continue;
		
		// New LOD
		i3Body * pSrc = static_cast<i3Body*>(i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta()));
		
		if( pSrc != nullptr)
		{
			if( maxBone < pSrc->getLOD(0)->getSkeleton()->getBoneCount())
			{
				maxBone = pSrc->getLOD(0)->getSkeleton()->getBoneCount();
				m_pPreFrameSkeleton = pSrc->getLOD(0)->getSkeleton();
				m_FrameLODNum = lod;
			}
		}
	}

	for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
	{
		i3SceneGraphInfo * pSgInfo = getSgInfo( EQUIP::ePART_SKIN, lod);

		if( pSgInfo == nullptr)
			continue;

		if( pSgInfo->getInstanceSg() == nullptr)
			continue;
		
		// New LOD
		i3Body * pSrc = static_cast<i3Body*>(i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta()));
		
		if( pSrc != nullptr)
		{
			i3LOD * pNewLOD = i3LOD::new_object_ref();
			m_pBody->AddLOD( pNewLOD);

			pNewLOD->setSkeleton( i3Skeleton::new_object_ref());
			pSrc->getLOD(0)->getSkeleton()->CopyTo( pNewLOD->getSkeleton(), I3_COPY_INSTANCE);

			pNewLOD->setShapeNode( i3AttrSet::new_object_ref());
		#if defined( I3_DEBUG)
			pNewLOD->getShapeNode()->SetName( "SkinNode");
		#endif
			if( lod == 0 && m_bInLobby == false)
				pNewLOD->getShapeNode()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);	// LOD0은 1인칭으로 그림자는 안나오도록

			if( pSrc->getLOD(0)->getSkeleton() != m_pPreFrameSkeleton)
				_BuildBoneMap( pNewLOD, m_pPreFrameSkeleton);

			i3MatrixStack	stack;
			AVT_BUILD_CTX	ctx;

			ctx.m_pAvatar	= this;
			ctx.m_pPart		= AVATAR::getPartInfo( EQUIP::ePART_SKIN);
			ctx.m_pTexInfo	= nullptr;
			ctx.m_pDestLOD	= pNewLOD;
			
			RecMergeGeo( pSrc, _Rec_CopyLODProc, &ctx, &stack);

		#if !defined( USE_EXCHANGE_HEAD)
			pSgInfo = getSgInfo( EQUIP::ePART_CHARA, lod);
			if( (pSgInfo != nullptr) && (pSgInfo->getInstanceSg() != nullptr) )
			{
				pSrc = static_cast<i3Body*>(i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta()));
				ctx.m_pPart		= AVATAR::getPartInfo( EQUIP::ePART_CHARA);
				RecMergeGeo( pSrc, _Rec_CopyLODProc, &ctx, &stack);
			}
		#endif
		}
	}

	UINT32 newFrameLOD = 0;
	maxBone = -1;
	for( UINT32 lod = 1; lod < m_pBody->getLODCount(); lod++)
	{
		if( maxBone < m_pBody->getLOD(lod)->getSkeleton()->getBoneCount())
		{
			maxBone = m_pBody->getLOD(lod)->getSkeleton()->getBoneCount();
			newFrameLOD = lod;
		}
	}

	m_pBody->setFrameLOD( m_pBody->getLOD( newFrameLOD));

	// Blend Attribute 와 ClanMark 부분을 뒤에 렌더링하도록 order를 바꾼다.
	__ReplaceRenderOrder();

	//더미는 총3개..
	{
		m_pEyeDummyLeft = SetDummyBody("SkullExeFXDummyLeft");
		m_pEyeDummyRight = SetDummyBody("SkullExeFXDummyRight");
		SetDummyBody("WeaponPointDummyLeft");
  		SetDummyBody("WeaponPointDummyBack");
  		SetDummyBody("WeaponPointDummyRight");
	}

#if !defined( CLIENT_SIDE)
	m_pBody->Prepare( false);
#endif
}

void AvatarGen::UpdateInThread( bool bDummyAvatar /*= false*/)
{
	switch( m_LoadState)
	{
	case STATE_NONE :
	case STATE_READY :									break;
	case STATE_LOADSTART :	LoadStart( bDummyAvatar);	break;	// Load할 파일 목록을 설정한다.
	case STATE_LOADING :
		if( m_pBody == nullptr)
		{
			if( isBodyLoaded() )		// Upper Body를 구성할 수 있으면 Body를 생성
			{
				CreateBody();
			}
		}
		else if( LoadingParts())
		{
			// 모든 Resource를 Load했다면 LOD를 생성하여 Body를 구성한다.
			// 새롭게 Scene 구성!
			BuildScene( bDummyAvatar);

			m_LoadState = STATE_READY;
		}
		break;
	case STATE_LOADING_SKIN :
		if( hasSkinParts())
		{
			bool bLoaded = true;
			if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
			{
				for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
				{
					if( __LoadingParts( EQUIP::ePART_SKIN, lod) == false)
						bLoaded = false;
			#if !defined( USE_EXCHANGE_HEAD)
					if( (lod > 0) && m_strName[ EQUIP::ePART_CHARA].empty() == false &&
						(__LoadingParts( EQUIP::ePART_CHARA, lod) == false) )
						bLoaded = false;
			#endif
				}
			}
			else
			{	// lobby
				if( __LoadingParts( EQUIP::ePART_SKIN, 1) == false)
					bLoaded = false;
			}

			if( bLoaded)
			{
				BuildSceneForSkin();
				m_LoadState = STATE_READY;
			}
		}
		break;
	}

#if defined( USE_EXCHANGE_HEAD)
	switch( m_LoadHeadState)
	{
	case STATE_NONE :
	case STATE_READY :								break;
	case STATE_LOADSTART :	LoadStartHead();		break;
	case STATE_LOADING :
		if( LoadingPartsHead() && m_pBody != nullptr)
		{
			BuildHeadScene();

			m_LoadHeadState = STATE_READY;
		}
		break;
	}
#endif
}

i3Skeleton * AvatarGen::FindFrameSkeleton(void)
{
	i3SceneGraphInfo * pSgInfo;

	// Upper가 기본 Frame Bone이 된다.

	pSgInfo = getSgInfo( EQUIP::ePART_VEST, 1);
	if( pSgInfo == nullptr) 
	{
		I3PRINTLOG(I3LOG_WARN,  "Upper LOD 1 Part가 반드시 필요합니다. Loading되지 않았습니다.");
		return nullptr;
	}

	if( pSgInfo->getInstanceSg() == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "올바른 Upper Scene Graph가 아닙니다.");
		return nullptr;
	}

	i3Body * pBody = (i3Body *) i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta());
	if( pBody == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "올바른 Upper Scene Graph가 아닙니다.");
		return nullptr;
	}

	i3LOD * pLOD = pBody->getFrameLOD();

	return pLOD->getSkeleton();
}

void AvatarGen::__HeadIOCheck( void)
{
	i3SceneIOManager * pManager = i3SceneIOManager::getInstance();
 
 	for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
 	{
 		for( INT32 j = 0; j < AVT_LOD_COUNT; j++)
 		{
			if( m_HeadIO[i][j] == -1)
  				continue;
  
			if( pManager->getLoadState( m_HeadIO[ i][j]) == LOADSTATE_LOADED)
			{
  				m_pHeadSgInfo[i][j] = pManager->PopLoadedScene( m_HeadIO[i][j], g_pFramework->getUpdateID());
				I3ASSERT( m_pHeadSgInfo[i][j] != nullptr);
  				m_HeadIO[i][j] = -1;
			}
 		}
 	}
}

void AvatarGen::__LoadPart( EQUIP::ePART part, INT32 lod, bool bDummyAvatar)
{
	char szPath[ MAX_PATH];

	if( (m_IO[ part][ lod] == -1) && ( m_pSgInfo[ part][ lod] == nullptr))
 	{
 		GetResPath( part, lod, szPath, _countof( szPath));

	#if !defined( USE_EXCHANGE_HEAD)
		if( EQUIP::ePART_CHARA == part)
		{
			INT32 pos = i3::contain_string( szPath, "LOD");
			INT32 in = i3::contain_string( m_strName[ part].c_str(), "_X");
			if( in == -1)
			{
				in = i3::contain_string( m_strName[ part].c_str(), "_Y");
				if( in == -1)
				{
					in = i3::contain_string( m_strName[ part].c_str(), "_Z");
					if( in == -1)
					{
						i3::insert_string( szPath, pos, "X_", 2);	// 가장 기본적인 X를 붙인다.
					}
					else
						i3::insert_string( szPath, pos, "Z_", 2);
				}
				else
				{
					i3::insert_string( szPath, pos, "Y_", 2);
				}
			}
			else
			{
				i3::insert_string( szPath, pos, "X_", 2);	// 가장 기본적인 X를 붙인다.
			}
		}
	#endif

		_LoadParts( szPath, part, lod, bDummyAvatar);
 	}
}

bool AvatarGen::__LoadingParts( EQUIP::ePART part, INT32 lod)
{
	if( m_IO[part][lod] == -1)
	{
		// IO Handle이 -1이라는 것은 실질적으로 IO가 처리되지 않는 경우인데..
		if( m_pSgInfo[part][lod] == nullptr)		// m_pSgInfo != nullptr이라는 것은 Loading이 완료된 경우.
			return false;
	}
	else
	{
		switch( getLoadState( part, lod))
		{
		case LOADSTATE_LOADING :	return false;	// 아직 Loading중
		case LOADSTATE_ERROR :		I3TRACE( "Load error %d-%d\n", part, lod);	break;
		case LOADSTATE_LOADED :
			{
				m_pSgInfo[part][lod] = i3SceneIOManager::getInstance()->PopLoadedScene( m_IO[part][lod], g_pFramework->getUpdateID());
				I3ASSERT( m_pSgInfo[part][lod] != nullptr);
  				m_IO[part][lod] = -1;
			}
			break;
		}
	}
	return true;
}

void AvatarGen::__ReplaceRenderOrder( void)
{
#if defined( CLIENT_SIDE)
	for( UINT32 i = 0; i < m_pBody->getLODCount(); i++)
	{
		i3Node * pClanNode = CharaTextureUtil::FindNodeByContainName( m_pBody->getLOD(i)->getShapeNode(), "MODEL_CLAN");
		if( pClanNode != nullptr)
		{
			i3Node * pParent = pClanNode->GetParent();
			pClanNode->ChangeParent( pParent);
		}
	}
#endif
}

void AvatarGen::PrepareEquipmentSG( EQUIP::ePART part)
{
	i3SceneGraphInfo * pSGInfo = getSgInfo( part , 1);
	if( pSGInfo == nullptr) return;

	i3Node * pSg = pSGInfo->getInstanceSg();

	VEC2D offset, scale;
	const AVATAR::TEXINFO * pTexInfo = AVATAR::getPartTexInfo( AVATAR::VIEW_3P, part);

	offset.x = (REAL32) pTexInfo->m_X / AVT_TEX_WIDTH;
	offset.y = (REAL32) pTexInfo->m_Y / AVT_TEX_HEIGHT;

	scale.x = (REAL32) pTexInfo->m_W / AVT_TEX_WIDTH;
	scale.y = (REAL32) pTexInfo->m_H / AVT_TEX_HEIGHT;

	// Texture Coords.
	i3::vector<i3RenderAttr*> list;
	i3Scene::FindAttrByExactType( pSg, i3GeometryAttr::static_meta(), list);

	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3GeometryAttr * pGeoAttr = static_cast<i3GeometryAttr*>(list.at(i));

		if( !pGeoAttr->IsModified())
		{
			i3VertexArray * pVA = pGeoAttr->GetVertexArray();
			I3ASSERT( pVA->Lock() );

			UINT32 vaCnt = pVA->GetCount();
			for( UINT32 j = 0; j < vaCnt; j++)
			{
				VEC2D vec2;

				pVA->GetTextureCoord( 0, j, &vec2);

				// Normalize
				if( vec2.x < 0.0f)
					vec2.x = 1.0f + vec2.x;

				if( vec2.y < 0.0f)
					vec2.y = 1.0f + vec2.y;

				i3Vector::Mul( &vec2, &vec2, &scale);
				i3Vector::Add( &vec2, &vec2, &offset);
				pVA->SetTextureCoord( 0, j, &vec2);
			}

			pVA->Unlock();

			pGeoAttr->SetModified( true);
		}
	}
}

void AvatarGen::BuildScene( bool bPreloadMacroTex)
{
	m_bAlphaTestableMesh = false;
	m_AlphaTestFuncValue = 128;

	if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
	{
		// Merge할 Geometry를 추려내서 VertexArray와 IndexArray를 미리 만들어 둔다.
		// VA, IA를 Create하는 것은 MainThread에서 해야 되기때문에 Lock 상태가 된다
		for( INT32 lod = 0; lod < AVT_LOD_COUNT; lod++)
		{
			PrepareGeo( lod);
		}

#if defined( CLIENT_SIDE)
		// 모든 Texture들을 하나로 통합한다.
		// 각 Texture들은 m_pTex_Diff, m_pTex_Norm 등에 복사된다.
		//(현재 텍스쳐 통합한느데 부하가 많이 걸린다...)
		if( bPreloadMacroTex)
			_PreloadMacroTex();

		_BuildMacroTex();
#else
		m_CurrentTexLevel = TEX_LOAD_HIGH;
		_BuildMacroTex(true);
#endif
		// LOD 구성 (0~3)
		BuildShape();

#if defined( I3_DEBUG) && 0
		I3TRACE( "BuildScene - %d\n", m_ItemID[EQUIP::ePART_CHARA]);
		I3TRACE( "			 - %d\n", m_ItemID[EQUIP::ePART_VEST]);
		I3TRACE( "			 - %d\n", m_ItemID[EQUIP::ePART_PANT]);
		I3TRACE( "			 - %d\n", m_ItemID[EQUIP::ePART_GLOVE]);
#endif
	}
	else
	{
		// 따로 분리되어 있는 Geometry를 그냥 Rendering합니다.
		_BuildShapeNoMerge();
	}

	// Blend Attribute 와 ClanMark 부분을 뒤에 렌더링하도록 order를 바꾼다.
	__ReplaceRenderOrder();

	if( m_LoadStyle & AVATAR_LOAD_STYLE_1PV)
	{
		if (m_pBody)
		{
			m_pBody->removeStyle(I3BS_LOD_AUTO);
			m_pBody->setConst(0.f);
			m_pBody->setSlope(3.5f);
			m_pBody->setCurrentLevel(0);
		}
	}
	else
	{
		// 나머지 장비들의 UV좌표를 설정해줘야 합니다.
		PrepareEquipmentSG( EQUIP::ePART_HEADGEAR);
		PrepareEquipmentSG( EQUIP::ePART_FACEGEAR);
		PrepareEquipmentSG( EQUIP::ePART_HOLSTER);
		PrepareEquipmentSG( EQUIP::ePART_BELT);
	}

	//더미는 총3개..
	if( m_pBody != nullptr)
	{
		m_pEyeDummyLeft = SetDummyBody("SkullExeFXDummyLeft");
		m_pEyeDummyRight = SetDummyBody("SkullExeFXDummyRight");
		SetDummyBody("WeaponPointDummyLeft");
  		SetDummyBody("WeaponPointDummyBack");
  		SetDummyBody("WeaponPointDummyRight");
	}
}

void AvatarGen::__LoadHead( INT32 lod)
{
	char szPath[ MAX_PATH];
	char szFull[ MAX_PATH];
	char * pszType = nullptr;

	GetResPath( EQUIP::ePART_CHARA, lod, szPath, _countof( szPath));

	INT32 pos = i3::contain_string( szPath, "LOD");

	if(pos < MAX_PATH)
	{
		for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
		{
			if( (m_HeadIO[i][ lod] == -1) && ( m_pHeadSgInfo[i][ lod] == nullptr))
			{
				// 교환용 Mesh
				switch( i)
				{
				case AVATAR::HEAD_TYPE_NORMAL :	pszType = "X_";		break;
				case AVATAR::HEAD_TYPE_COVER :	pszType = "Y_";		break;
				case AVATAR::HEAD_TYPE_DEEP :	pszType = "Z_";		break;
				default :											continue;
				}

				i3::safe_string_copy( szFull, szPath, sizeof( szFull));
				i3::insert_string( szFull, pos, pszType, 2);

				_LoadHeads( szFull, (AVATAR::HEAD_TYPE) i, lod);
			}
		}
	}
}

void AvatarGen::LoadStartHead( void)
{
#if defined( USE_EXCHANGE_HEAD)

	I3ASSERT( AVATAR::getPartStyle( EQUIP::ePART_CHARA) & AVT_PART_STYLE_EXCHANGE);

	if( m_LoadStyle & AVATAR_LOAD_STYLE_MERGE)
	{
		// LOD 1번부터 머리가 보인다. 0번은 1인칭
		for( INT32 j = 1; j < AVT_LOD_COUNT; j++)
		{
			__LoadHead( j);
		}
	}
	else
	{	// lobby
		__LoadHead( 1);
	}

	m_LoadHeadState = STATE_LOADING;

#endif
}

bool AvatarGen::isReadyScene(void)
{
	return (m_pBody != nullptr) && (m_LoadState == STATE_READY) 
#if defined(USE_EXCHANGE_HEAD)
		&& (m_LoadHeadState == STATE_READY)
#endif
		;
}

namespace {
	void	FindAndDeleteTexture( i3SceneGraphInfo * pInfo)
	{
		if( pInfo == nullptr) return;

		i3NamedResourceManager * pManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());

		I3ASSERT( pInfo->getInstanceSg() != nullptr);
		i3::vector<i3RenderAttr*> list;
		
		i3Scene::FindAttrByType( pInfo->getInstanceSg(), i3TextureBindAttr::static_meta(), list);

		UINT32 cnt = list.size();
		for( UINT32 i = 0; i < cnt; i++)
		{
			i3TextureBindAttr * pAttr = static_cast<i3TextureBindAttr*>(list.at(i));
			if( pAttr->GetTexture() != nullptr)
			{
				//I3TRACE( "Release OriginalTex %s:%d\n", pAttr->GetTexture()->GetName(), pAttr->GetTexture()->GetRefCount());
				if( !pAttr->GetTexture()->isCommonRes() && pAttr->GetTexture()->GetRefCount() == 3)
				{
					UINT32 resCnt = pManager->GetResourceCount();
					for( UINT32 j = 0; j < resCnt; j++)
					{
						if( pAttr->GetTexture() == pManager->GetResource( j) )
						{
							I3_MUST_RELEASE(pAttr->GetTexture());
							pManager->DeleteResource( j);
							pAttr->SetTexture(nullptr);
							break;
						}
					}
				}
			}
		}
	}
};

i3Body *	AvatarGen::GetGeneratedBody( bool bUseMacrotexture)
{
	I3ASSERT( m_pBody != nullptr);
	i3Body * pBody = m_pBody;

	// 머리를 Body에 붙인다.
	if( m_bInLobby)
	{	// Lobby
		if( (m_LoadStyle & AVATAR_LOAD_STYLE_1PV) == 0)
		{
			i3Node * pShapeNode = m_pBody->getLOD( 0)->getShapeNode();

			for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
			{
				// 로비에서는 1번 LOD만 사용한다.
				if( m_pHeadLOD[i][1] == nullptr)
					continue;
				
				pShapeNode->AddChild( m_pHeadLOD[i][1]->getShapeNode());
			}
		}
	}
	else
	{
		i3::vector<i3RenderAttr*> diff;
		i3::vector<i3RenderAttr*> normal;
		i3::vector<i3RenderAttr*> specular;
		i3::vector<i3RenderAttr*> reflect;

		for( INT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
		{
			for( INT32 j = 1; j < AVT_LOD_COUNT; j++)
			{
				if( m_pHeadLOD[i][j] == nullptr)
					continue;

				// MacroTexture로 변경
				if( hasSkinParts() == false )
				{
					i3Scene::FindAttrByExactType( m_pHeadLOD[i][j]->getShapeNode(), i3TextureBindAttr::static_meta(), diff);
					i3Scene::FindAttrByExactType( m_pHeadLOD[i][j]->getShapeNode(), i3NormalMapBindAttr::static_meta(), normal);
					i3Scene::FindAttrByExactType( m_pHeadLOD[i][j]->getShapeNode(), i3SpecularMapBindAttr::static_meta(), specular);
					i3Scene::FindAttrByExactType( m_pHeadLOD[i][j]->getShapeNode(), i3ReflectMaskMapBindAttr::static_meta(), reflect);
				}

				m_pBody->getLOD( j)->getShapeNode()->AddChild( m_pHeadLOD[i][j]->getShapeNode());
			}
		}

		if( m_pTex_Diff[ m_CurrentTexLevel] != nullptr)
		{
			UINT32 cnt = diff.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3TextureBindAttr * pBinder = static_cast<i3TextureBindAttr*>(diff.at( i));
				pBinder->SetTexture( m_pTex_Diff[ m_CurrentTexLevel]);
			}
		}

		if( m_pTex_Norm[ m_CurrentTexLevel] != nullptr)
		{
			UINT32 cnt = normal.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3TextureBindAttr * pBinder = static_cast<i3TextureBindAttr*>(normal.at( i));
				pBinder->SetTexture( m_pTex_Norm[ m_CurrentTexLevel]);
			}
		}

		if( m_pTex_Spec[ m_CurrentTexLevel] != nullptr)
		{
			UINT32 cnt = specular.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3TextureBindAttr * pBinder = static_cast<i3TextureBindAttr*>(specular.at( i));
				pBinder->SetTexture( m_pTex_Spec[ m_CurrentTexLevel]);
			}
		}

		if( m_pTex_RefMask[ m_CurrentTexLevel] != nullptr)
		{
			UINT32 cnt = reflect.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3TextureBindAttr * pBinder = static_cast<i3TextureBindAttr*>(reflect.at( i));
				pBinder->SetTexture( m_pTex_RefMask[ m_CurrentTexLevel]);
			}
		}
	}

	m_LoadState = STATE_NONE;
	m_LoadHeadState = STATE_NONE;

	m_bLoaded = true;

#if defined( CLIENT_SIDE)
	for( UINT32 i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		for( UINT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE( m_pSgInfo[ i][ j]);
		}
	}

	for( UINT32 i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
	{
		for( UINT32 j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE(  m_pHeadSgInfo[ i][ j]);
		}
	}
#endif

	return pBody;
}

i3BoneRef* AvatarGen::SetDummyBody(char* szDummyName)
{
 	i3SceneGraphInfo * pSgInfo = getSgInfo(EQUIP::ePART_VEST, m_FrameLODNum);
	if( pSgInfo == nullptr)
	{
		pSgInfo = getSgInfo( EQUIP::ePART_SKIN, m_FrameLODNum);
		I3ASSERT( pSgInfo != nullptr);
	}
 
 	i3BoneRef * pSrcBoneRef = (i3BoneRef*)i3Scene::FindNodeByName( pSgInfo->getInstanceSg(), szDummyName);
	
	//기존 더미 복사 한다.. (떼엇다가 붙일수가 없다. 로딩된 리소스를 재사용 하기 때문에..)
 	if( pSrcBoneRef != nullptr)
 	{
		i3BoneRef * pDestBoneRef = (i3BoneRef*) i3Scene::CreateClone( pSrcBoneRef, I3_COPY_INSTANCE);

		I3ASSERT( m_pBody != nullptr);
		m_pBody->AddChild(pDestBoneRef);
		
 		//TRACE("Add Dummy To Body : %s\n",szDummyName);
		return pDestBoneRef;
 	}
	return nullptr;
}

void AvatarGen::SetTeam( TEAM_TYPE team)
{
	I3ASSERT( team != TEAM_NONE);

	if( m_Team != team)
	{
		m_Team = team;
	}
}

i3LOD *	 AvatarGen::_FindBaseLOD( UINT32 * pMaxBoneCount)
{
	INT32 i;
	i3LOD * pBaseLOD = nullptr;

	// Upper를 바탕으로 이하를 모두 LOD Shape로 설정
	// Upper + Lower + Mask +  Glove
	for( i = 0; i < AVT_LOD_COUNT; i++)
	{
		i3SceneGraphInfo * pSgInfo = getSgInfo( EQUIP::ePART_VEST, i);
		if( pSgInfo == nullptr)
			continue;

		i3Body * pTempBody = (i3Body *) i3Scene::FindNodeByExactType( pSgInfo->getInstanceSg(), i3Body::static_meta());
		if( pTempBody == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "올바른 Upper Scene Graph가 아닙니다.(%s)", m_strName[ EQUIP::ePART_VEST]);
			return nullptr;
		}

		i3LOD* pLOD = pTempBody->getLOD(0);
		UINT32 boneCount = pLOD->getBoneCount();

		i3Skeleton * pSkel = pLOD->getSkeleton();
		if( pSkel != nullptr)
		{
			// Bone의 개수가 가장 많은 LOD,
			// 만약 Bone의 개수가 같다면, ShapeSet을 가지고 있는 Bone쪽을
			// Frame Bone Hierarchy라고 간주한다.
			// 
			if( boneCount > *pMaxBoneCount)
			{
				pBaseLOD = pLOD;
				*pMaxBoneCount = boneCount;
				m_FrameLODNum = i;
			}
			else if( boneCount == *pMaxBoneCount)
			{
				INT32 shapeSetCnt = pLOD->hasShapeSet();			

				if( (shapeSetCnt > 0) || ( i == 1) )	// PB에서는 LOD1을 기본 Base로 합니다.
				{
					pBaseLOD = pLOD;
					*pMaxBoneCount = boneCount;
					m_FrameLODNum = i;
				}
			}
		}
	}

	return pBaseLOD;
}

void AvatarGen::LostDevice( void)
{
	INT32 i, j;

	for( i = 0; i < EQUIP::ePART_COUNT; i++)
	{
		for( j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE( m_pSgInfo[i][j]);
		}
	}

	for( i = 0; i < AVATAR::HEAD_TYPE_MAX; i++)
	{
		for( j = 0; j < AVT_LOD_COUNT; j++)
		{
			I3_SAFE_RELEASE( m_pHeadSgInfo[i][j]);
			I3_SAFE_RELEASE( m_pHeadLOD[i][j]);
		}
	}

	for( i = 0; i < TEX_LOAD_LEVEL_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pTex_Diff[i]);
		I3_SAFE_RELEASE( m_pTex_Norm[i]);
		I3_SAFE_RELEASE( m_pTex_Spec[i]);
		I3_SAFE_RELEASE( m_pTex_RefMask[i]);
	}

	for( i = 0; i < AVT_LOD_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pTempVA[i]);
		I3_SAFE_RELEASE( m_pTempIA[i]);
	}

	I3_SAFE_RELEASE( m_pNewLOD);
	I3_SAFE_RELEASE( m_pBody);

	// local로 생성하는 경우 포인터를 담아둬야 해제가 가능
	I3_SAFE_RELEASE( m_pNewGeoAttr);
}

void AvatarGen::KeepDummyCharaResource( i3SceneGraphInfo * pSgInfo)
{
	if( pSgInfo != nullptr)
	{
		pSgInfo->addResStyle( I3_RES_STYLE_COMMON | I3_RES_STYLE_PREPARED);

		// texture를 macro texture로 변경
		BindMacroTexture( pSgInfo->getInstanceSg(), m_CurrentTexLevel, true);
	}
}
