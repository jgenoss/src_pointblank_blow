#include "i3FrameworkPCH.h"
 #include "i3ResourceManager.h"
#include "i3Framework.h"

#include "i3SoundH.h"

#include "i3GameResAnimation.h"
#include "i3GameResTexture.h"
#include "i3GameResChara.h"
#include "i3GameResSound.h"
#include "i3GameResSceneGraph.h"
#include "i3GameResWorld.h"
#include "i3GameResObject.h"
#include "i3GameResLight.h"
#include "i3GameResRespawn.h"
#include "i3GameResEvent.h"
#include "i3GameResPath.h"

#include "i3GamePath.h"
#include "i3LightObject.h"
#include "i3CameraObject.h"
#include "i3Gfx/i3TextureResManager.h"

#include "i3Base/itl/vector_set_util.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"

#include "i3Base/string/ext/extract_directoryname_filename.h"

#include "i3Base/string/ext/insert_suffix_to_filename.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/extract_filetitle_fileext.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/compare/generic_compare.h"

#include "i3Math/octree/occ_quad_set.h"

#include "i3GameObjBase.h"

#include "i3ResourceFile_i3Chara.h"
#include "i3CharaSGChangeManager.h"
#include "i3Scene/i3SGTexChangeManager.h"
#include "i3Base/i3ParserHelper.h"
#include "i3Base/i3RSGHelper.h"

#include "i3UI/i3UIFilePathMgr.h"

#include <stdio.h>

#if defined( I3_WINDOWS)
#include <shlobj.h>
#endif

#if defined( I3_PSP) || defined( I3_WIPI)		
#include <stdarg.h>	
#endif

#if defined( I3_WINCE)
#define __FUNCTION__ " "
#endif

//#define		LOADTIME_DUMP

//#if defined( I3_WINDOWS ) && defined( I3_DEBUG )
//	#define CHECK_PATH(_P)	{ if( strchr( _P, ':' ) != nullptr ){ ReportErr( "Invalid File Path. (%s)", _P ); } }
//#else
	#if defined( I3_COMPILER_VC)
		#if defined( I3_COMPILER_EVC )
			#define CHECK_PATH(_P)						(void)0			
		#else			
			#define CHECK_PATH(_P)						__noop
		#endif
	#else
		#define CHECK_PATH(_P)							(void)0
	#endif
//#endif

I3_CLASS_INSTANCE( i3QueueFileInfo);
I3_CLASS_INSTANCE( i3ResourceManager);

/*
static INT32 _ResCompareProc( i3GameRes * pRes1, i3GameRes * pRes2)
{
	if( pRes1->getChecksum() > pRes2->getChecksum())
		return 1;
	else if( pRes1->getChecksum() < pRes2->getChecksum())
		return -1;

	return 0;
}
*/
bool		operator<(const i3GameRes& a1, const i3GameRes& a2)
{
	return a1.getChecksum() < a2.getChecksum();
}

/*
static INT32 _ResFindProc( i3GameRes * pRes1, UINT128 * hash)
{
	if( pRes1->getChecksum() > *hash)
		return 1;
	else if( pRes1->getChecksum() < *hash)
		return -1;

	return 0;
}
*/
struct _ResFindProc
{
	bool operator()( const i3GameRes* pRes1, const UINT128& hash) const { 	return pRes1->getChecksum() < hash; 	}
	bool operator()( const UINT128& hash, const i3GameRes* pRes1) const { 	return hash < pRes1->getChecksum();  	}
};



static void sFindAndLoadSound( i3ResourceManager * pManager, i3GameNode * pNode, bool bInThread)
{
	INT32 i, j;

	I3ASSERT( pNode != nullptr);

	if( i3::same_of<i3AIContext* >(pNode))
	{
		i3AIContext * pAICtx = (i3AIContext*) pNode;

		if( pAICtx->getAI() != nullptr)
		{
			for( i = 0; i < pAICtx->getAI()->getAIStateCount(); i++)
			{
				i3AIState * pState = pAICtx->getAI()->getAIState( i);
				if( pState->getTimeSeq() == nullptr)
					continue;

				for( j = 0; j < pState->getTimeSeq()->GetSequenceCount(); j++)
				{
					i3TimeSequenceInfo * pSeqInfo = pState->getTimeSeq()->GetSequence( j);

					if( !i3::same_of< i3SoundNode* >(pSeqInfo->GetNode()))
						continue;
					
					i3SoundNode * pSoundNode = (i3SoundNode*) pSeqInfo->GetNode();
					if( pSoundNode->getSoundPlayInfo() == nullptr)
						continue;

					if( i3::generic_string_size( pSoundNode->GetNameString()) > 0)
					{
						i3GameResSound * pRes = (i3GameResSound *) pManager->FindResourceByOnlyName( pSoundNode->GetName());

						if( pRes == nullptr)
						{	// 없는 리소스 로드하자
							pRes = (i3GameResSound*)pManager->LoadBackgroundRes( pSoundNode->GetName(), false, bInThread);
							I3ASSERT( pRes != nullptr);
						}

						if( pRes != nullptr)
						{
#if defined( I3_DEBUG) && 0
							REAL32 min = 0.f;
							REAL32 max = 0.f;
							if( pSoundNode->getSoundPlayInfo() != nullptr)
							{
								min = pSoundNode->getSoundPlayInfo()->getMinDistance();
								max = pSoundNode->getSoundPlayInfo()->getMaxDistance();
							}
							I3TRACE( "Change Sound %s(min:%.1f->%.1f, max:%.1f->%.1f\n", pRes->getSound()->getSound()->GetName(), min, pRes->getSound()->getMinDistance(), max, pRes->getSound()->getMaxDistance());
#endif
							pSoundNode->setSoundPlayInfo( pRes->getSound());
						}
					}
					else
					{
						I3TRACE( "not set Sound Name in CHR file!!!");
					}
				}
			}
		}
	}

	i3GameNode * pChild = pNode->getFirstChild();

	while( pChild != nullptr)
	{
		sFindAndLoadSound( pManager, pChild, bInThread);

		pChild = pChild->getNext();
	}
}

namespace
{
	bool IsFile_ResChangeManager(const char* pszPath)
	{
		i3::stack_string strExt;
		i3::extract_fileext(pszPath, strExt);

		if ( i3::generic_is_iequal(strExt, "I3CHR"))
		{
			const i3CharaSGChangeInfo* SGChangeInfo = i3CharaSGChangeManager::i()->FindSGChangeInfo(pszPath);
			return SGChangeInfo != nullptr;
		}
		else
		if ( i3::generic_is_iequal(strExt, "I3S"))
		{
			const i3SGTexChangeInfo* TexChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(pszPath);
			return TexChangeInfo != nullptr;
		}

		return false;
	}

	const char* GetFileName_ResChangeManager(const char* pszPath)
	{
		i3::stack_string strExt;
		i3::extract_fileext(pszPath, strExt);

		if ( i3::generic_is_iequal(strExt, "I3CHR"))
		{
			const i3CharaSGChangeInfo* SGChangeInfo = i3CharaSGChangeManager::i()->FindSGChangeInfo(pszPath);
			if (SGChangeInfo != nullptr)
			{
				return i3CharaSGChangeManager::GetTgti3CharaFileName(SGChangeInfo).c_str();
			}
		}
		else
		if ( i3::generic_is_iequal(strExt, "I3S"))
		{
			const i3SGTexChangeInfo* TexChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(pszPath);
			if (TexChangeInfo != nullptr)
			{
				return i3SGTexChangeManager::GetTgti3sFilename(TexChangeInfo).c_str();
			}
		}
		
		return nullptr;
	}

}

i3ResourceManager::i3ResourceManager(void)
{
	i3FileStream::SetIsFileCallback( &IsFile_ResChangeManager);
		
	m_QueueFileList.reserve( 128);
	m_ResList.reserve( 1024);
	m_BackgroundFileList.reserve( 1024);
}

i3ResourceManager::~i3ResourceManager(void)
{
	i3GameRes * pRes;

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];
		I3_MUST_RELEASE( pRes);
	}

	m_ResList.clear();

	i3NamedResourceManager::Reset();
	
	i3::vector_map<std::string,tagBACKGROUNDRES_INFO*>::iterator it;
	it = m_BackgroundFileList.begin();

	while( it != m_BackgroundFileList.end())
	{
		tagBACKGROUNDRES_INFO * pInfo = (tagBACKGROUNDRES_INFO*) it->second;
		I3_SAFE_RELEASE( pInfo->_pRes);
		I3MEM_SAFE_FREE( pInfo);
		it++;
	}

	m_BackgroundFileList.clear();
}

void i3ResourceManager::ReportErr( const char *szFmt, ... )
{
	char szTemp[512];

	va_list marker;

	va_start( marker, szFmt );

	vsprintf( szTemp, szFmt, marker );

	ReportErrMsg( szTemp );
}

void i3ResourceManager::ReportErrMsg( const char* szMsg )
{
	i3Log( "i3ResourceManager", (char*)szMsg );
}

void i3ResourceManager::SetLanguagePostfix( const char *szPostfix )
{
	m_strLangPostfix = szPostfix; 
}

void i3ResourceManager::Reset(void)
{
}

static void sProgProc( void * pUserData, UINT32 curPos, UINT32 maxPos)
{
}

i3SceneGraphInfo *	i3ResourceManager::_LoadI3S( const char * pszPath, bool bInThread, bool bBGLoadEnable, bool bClone, UINT32 * pSize )
{
	i3SceneGraphInfo * pSceneRoot;

	i3SceneFile file;
	UINT32 rc;

	file.SetInThread( bInThread);
	file.setBackgroundLoadEnable( bBGLoadEnable);

	rc = file.Load( pszPath);
	if( rc == STREAM_ERR)
	{
		ReportErr( "%s : Could not load %s scene file.", __FUNCTION__, pszPath);
		return nullptr;
	}		

	pSceneRoot = file.getSceneGraphInfo();
	I3_SAFE_ADDREF( pSceneRoot);

#if defined( I3_DEBUG)
	if( pSize != nullptr)
		*pSize = rc;
#endif
	
	return pSceneRoot;
}

i3Animation * i3ResourceManager::_LoadI3A( const char * pszPath, UINT32 * pSize)
{
	i3Animation *pAnim = nullptr;

	{
		pAnim = i3Animation::LoadFromFile( pszPath, pSize ); 
		if( pAnim == nullptr )
		{
			ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, pszPath );
			I3ASSERT_0;
		}
	//	pAnim->AddRef();	// 내부에서 i3Animation::LoadFromFile()함수에 AddRef()추가했으므로 여기서는 주석처리.(2012.09.11.수빈)
	}
	
	return pAnim;
}

i3Texture * i3ResourceManager::_LoadImage( const char * pszPath)
{
	i3Texture *pTex = nullptr;

	{
		i3ImageFile file;

		pTex = file.Load( pszPath);
		if( pTex == nullptr )
		{
			if( !file.isSkipResource())
			{
				ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, pszPath );
				I3ASSERT_0;
			}
		}
		else
		{
			I3ASSERT( pTex->GetRefCount() == 1 );
		}
	}

	return pTex;
}

i3World	* i3ResourceManager::LoadWorld( const char * szPath, i3World * pWorld, 
							bool RotateToMax, const char* szPostFix, bool bClone)
{
	CHECK_PATH( szPath );

	char szWorld[MAX_PATH];
	i3World * pCurWorld = nullptr;

	if( szPath == nullptr &&  pWorld == nullptr)
		return nullptr; 

	if( bClone )
	{
		I3ASSERT( pWorld == nullptr );
	}

	if( pWorld == nullptr)
	{	// Load from file

		if(szPostFix)
		{
			i3::insert_suffix_to_filename(szWorld, szPath, szPostFix);
			/*
			char szBuf[MAX_PATH];
			char szExt[16];
			i3String::SplitFileExt( szPath, szExt, 16 );
			i3::safe_string_copy( szBuf, szPath, MAX_PATH );
			i3::remove_ext( szBuf );
			sprintf( (PSTR) szWorld, (PSTR) "%s%s.%s", szBuf, szPostFix, szExt );
			*/
		}
		else
		{
			strncpy( szWorld, szPath, sizeof( szWorld) - 1);
		}

		pCurWorld = _LoadWorld( szWorld);
	
		if( pCurWorld == nullptr)
		{
			ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, szPath );
			I3ASSERT_0;
			return nullptr; 
		}
	}
	else
	{
		i3World * pNewWorld = i3World::new_object();
		pWorld->CopyTo( pNewWorld, I3_COPY_INSTANCE);
		pCurWorld = pNewWorld;
	}

	//I3TRACE( "Load World : %s\n", szPath);

	return pCurWorld;

}

namespace
{
	void MakeWorldOCC(i3World* pWorld, i3Node* pOccRoot)
	{
		if (pOccRoot == nullptr) return;

		i3::occ_mesh_set occMeshSet;
		i3MatrixStack	 matrixStack;			// _GetWrappingBound와 유사하게 처리하기 위해서 필요..
		pOccRoot->CalcOccMeshSet(&occMeshSet, &matrixStack, false);

		pWorld->CreateOccCull(occMeshSet);
	}
}


i3World * i3ResourceManager::_LoadWorld( const char * pszPath)
{
	i3World * pWorld = nullptr;

	i3ResourceFile File;
	if( File.Load( pszPath) == STREAM_ERR)
	{
		ReportErr( "%s : Could not Load Chara %s ", __FUNCTION__, pszPath);
		return nullptr;
	}

	pWorld = (i3World*)File.getKeyObject();
	if( pWorld == nullptr)
	{
		ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, pszPath );
		I3ASSERT_0;
	}

	I3_SAFE_ADDREF( pWorld);

	if (m_bOccMeshLoadEnable)
	{
		// 일단 월드 파일 로딩 이후에 오클루전 파일을 읽을수 있도록 해본다...
		i3::stack_string strOCCPath;
		{
			i3::stack_string strDir, strWorldFileName, strOccFileName;
			i3::extract_directoryname_filename(pszPath, strDir, strWorldFileName);
			i3::extract_filetitle(strWorldFileName, strOccFileName);
			// 파일 타이틀에 접미어 (_occ)를 붙이고, 확장자는 기존 i3WRD에서 i3s로 변경한다...
			strOccFileName += "_occ.i3s";

			strOCCPath = strDir;
			strOCCPath += '/';
			strOCCPath += strOccFileName;
		}

		if (i3FileStream::isFile(strOCCPath.c_str()))		// 파일이 존재하는 경우에만 적용...(없는 것이 허용된다..)
		{
			i3SceneFile scnFile;
			UINT32 rc = scnFile.Load(strOCCPath.c_str());

			if (rc == STREAM_ERR)
			{
				ReportErr("%s : Could not load %s scene file.", __FUNCTION__, strOCCPath.c_str());		// 단순이 에러만 표기해둔다..
			}
			else
			{
				i3Node* pOccRoot = scnFile.GetSceneGraph();
				MakeWorldOCC(pWorld, pOccRoot);
			}
		}
	}

	return pWorld;
}


i3Chara * i3ResourceManager::LoadCharactor( const char * szPath, i3Chara * pChar,
										   bool RotateToMax, const char* szPostFix,
										   bool bClone, bool bCommonRes, bool bInThread, UINT32 * pSize )
{
	CHECK_PATH( szPath );

	char szCharaPath[MAX_PATH];
	i3Chara * pCharactor;

	if( szPath == nullptr &&  pChar == nullptr)
		return nullptr; 

	if( bClone )
	{
		I3ASSERT( pChar == nullptr );
	}

	if( pChar == nullptr)
	{	// Load from file
		if(szPostFix)
		{
			i3::insert_suffix_to_filename(szCharaPath, szPath, szPostFix);
		}
		else
		{
			strncpy( szCharaPath, szPath, sizeof( szCharaPath) -1);
		}

		i3ResourceFile_i3Chara i3CharaFile;

		UINT32 Rc = 0;

		Rc = i3CharaFile.LoadOrClone( szCharaPath, bCommonRes, bInThread);
		if( Rc == STREAM_ERR)
		{
			ReportErr( "%s : Could not Load Chara %s ", __FUNCTION__, szCharaPath);
			return nullptr;
		}

		pCharactor = reinterpret_cast<i3Chara*>(i3CharaFile.getKeyObject());
		if( pCharactor == nullptr)
		{
			ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, szCharaPath );
			return nullptr;
		}

		I3_SAFE_ADDREF( pCharactor);

		//if( bCommonRes == false)
		if (i3CharaFile.IsClonedResult() == false)
			sFindAndLoadSound( this, pCharactor, bInThread);

	#if defined( I3_DEBUG)
		if( pSize != nullptr)
			*pSize = Rc;
	#endif
	
	}
	else
	{	// Copy Clone
		i3Chara * pNewChar  = (i3Chara *) i3GameNode::CreateClone( pChar, I3_COPY_INSTANCE);
		I3ASSERT( pNewChar != nullptr);

		pCharactor = pNewChar;
	}

	//I3TRACE( "Load Chara : %s\n", szPath);

	return pCharactor;
}

i3SceneGraphInfo *i3ResourceManager::LoadModel( const char * szPath, bool bInThread, bool bBGLoadEnable, const char* szPostFix, UINT32 * pSize /*= nullptr*/ )
{
	CHECK_PATH(szPath);

	char szModPath[MAX_PATH];
	i3SceneGraphInfo * pSg = nullptr;
	i3NamedResourceManager * pManager;

	if (szPath == nullptr)
		return nullptr;

	pManager = i3ResourceFile::GetResourceManager(i3SceneGraphInfo::static_meta());
	I3ASSERT(pManager != nullptr);

	// 우선 Manager를 통해, Loading된 Scene Graph가 있는지 확인한다.
	// pSg = (i3SceneGraphInfo *) pManager->FindResource( szPath);

	pSg = static_cast<i3SceneGraphInfo*>(pManager->GetResource(szPath, i3ResourceFile::GetWorkingFolder(), szPath));

	if (pSg == nullptr)
	{
		// 기존에 Loading된 적이 없었기 때문에 직접 Loading
		if (szPostFix)
		{
			i3::insert_suffix_to_filename(szModPath, szPath, szPostFix);

			pSg = static_cast<i3SceneGraphInfo*>(pManager->GetResource(szModPath, i3ResourceFile::GetWorkingFolder(), szModPath));

			/*
			char szBuf[MAX_PATH];
			char szExt[16];
			i3String::SplitFileExt( szPath, szExt, 16 );
			i3::safe_string_copy( szBuf, szPath, MAX_PATH );
			i3::remove_ext( szBuf );
			sprintf( (PSTR) szModPath, (PSTR) "%s%s.%s", szBuf, szPostFix, szExt );
			*/
		}

		/*
		pSg = _LoadI3S( szModPath, bInThread, bBGLoadEnable, false, pSize);

		if( pSg == nullptr)
		{
		//ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, szPath );
		return nullptr;
		}

		pManager->AddResource( pSg, szPath);
		I3_MUST_RELEASE( pSg);
		*/

		//I3TRACE( "Load Scene : %s\n", szPath);
	}
#if defined( I3_DEBUG)
	else
	{
		if (pSize != nullptr)
			*pSize = pSg->m_FileSize;
	}
#endif

	return pSg;
}

/*
i3SceneGraphInfo * i3ResourceManager::LoadScene( const char * szPath, i3NamedResourceManager *pNamedResMgr, bool bClone )
{
	CHECK_PATH( szPath );

	i3SceneGraphInfo * pSceneRoot;

	pSceneRoot = _LoadI3S( szPath, bClone );

	if( pSceneRoot == nullptr)
	{
		ReportErr( "%s : Could not load \"%s\"\n", __FUNCTION__, szPath );
		I3ASSERT_0;
		return  nullptr;
	}

	return pSceneRoot;
}
*/

i3Animation* i3ResourceManager::LoadAnimation( const char *szPath, UINT32 * pSize )
{
	CHECK_PATH( szPath );

	//I3TRACE( "Load Animation : %s\n", szPath);

	return _LoadI3A( szPath, pSize);
}

void i3ResourceManager::LoadAnimationPack( const char * pszPath)
{
	i3AnimationPackFile file;

	//file.LoadFromFile( (char *) pszPath);
	file.LoadAnimationList( pszPath);

	//I3TRACE( "Load AnimationPack : %s\n", pszPath);
}

// 텍스쳐 로딩
i3Texture*	i3ResourceManager::LoadTexture( const char *szPath, bool bRef, UINT32 * pSize )
{
	CHECK_PATH( szPath );

	i3NamedResourceManager * pManager;
	i3Texture * pTex = nullptr;

	if( szPath == nullptr)
		return nullptr; 

	pManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
	I3ASSERT( pManager != nullptr);

	// 우선 Manager를 통해, Loading된 Scene Graph가 있는지 확인한다. (????) 이해가 좀 안되...
	// 이거 UI나 gui 아닌 일반 텍스쳐들도 이 함수를 쓰는데..이렇게 막 조건문 걸고 UI전용 검사 해도 되는거에요? ( 2016.04.18.수빈)
	// 일단 다른 방법 찾기 전에 필요한 것만 고침..

	i3::rc_string strUIVer = i3UIFilePathMgr::i()->GetUIVersionString();

	if (!strUIVer.empty())
	{
		i3::rc_string strLocaleFolder = i3UIFilePathMgr::i()->Retrieve_LocaleUIRootDirectory(true);

		pTex = (i3Texture *)pManager->GetResource(szPath, strLocaleFolder.c_str(), szPath);		// 다른 일반 텍스쳐도 이함수를 마구 통과하므로 문제이다..

		if (pTex == nullptr)
		{
			pTex = (i3Texture *)pManager->GetResource(szPath, i3ResourceFile::GetWorkingFolder(), szPath);	// 없으면 원래 함수 호출..
		}
	}
	else
	{
		pTex = (i3Texture *)pManager->GetResource(szPath, i3ResourceFile::GetWorkingFolder(), szPath); // 수정 전 이전 원본 함수호출..
	}

	// 아래 문제 있는 함수구현부는 주석처리..
	/*
	if( i3UI::getManager()->GetUIVersionName()->length() > 0 )
	{
		char szRoot[MAX_PATH];
		char szTempPath[MAX_PATH];
		i3::extract_filename(i3ResourceFile::GetWorkingFolder(), szRoot);
		if (i3::generic_is_iequal(szRoot, "VirtualTexture"))
		{
			i3::extract_directoryname(i3ResourceFile::GetWorkingFolder(), szRoot);
			i3::snprintf(szTempPath, MAX_PATH, "%s", szRoot);
		}
		else
		{
			i3::snprintf(szTempPath, MAX_PATH, "%s/%s/%s", i3ResourceFile::GetWorkingFolder(),
				i3UI::getManager()->GetUIResourceFolder()->c_str(),
				i3UI::getManager()->GetUIVersionName()->c_str());
		}
		pTex = (i3Texture *) pManager->GetResource( szPath, szTempPath, szPath);

		if (pTex == nullptr)
		{
			pTex = (i3Texture *)pManager->GetResource(szPath, i3ResourceFile::GetWorkingFolder(), szPath);
		}
	}
	else
		pTex = (i3Texture *) pManager->GetResource( szPath, i3ResourceFile::GetWorkingFolder(), szPath);
	*/

	I3_SAFE_ADDREF( pTex);

#if defined( I3_DEBUG)
	if( pSize != nullptr && pTex != nullptr)
	{
		*pSize = sizeof( i3Texture);
		for( UINT32 i = 0; i < pTex->GetLevelCount(); i++)
		{
			*pSize += pTex->GetDataSize( i);
		}
	}
#endif

    return pTex;
}

i3GamePath *	i3ResourceManager::LoadPath( const char * pszPath)
{
	CHECK_PATH( szPath);

	i3GamePath * pPath;
	i3ResourceFile file;

	if( file.Load( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not load %s path resource.", pszPath);
		return nullptr;
	}

	pPath = (i3GamePath *) file.getKeyObject();
	I3ASSERT( pPath != nullptr);

	I3_SAFE_ADDREF( pPath);

	//I3TRACE( "Load Path : %s\n", pszPath);

	return pPath;
}

// 파일이 존재하는 지 여부 판단
bool i3ResourceManager::IsExist( const char *szPath )
{
	{
		bool bRet;
		i3FileStream * pStream = i3FileStream::new_object();
		bRet = pStream->Open( szPath, STREAM_READ | STREAM_SHAREREAD );
		I3_SAFE_RELEASE( pStream );

		return bRet;
	}
}

i3Stream *	i3ResourceManager::OpenStream( const char * pszPath)
{
	//CHECK_PATH( pszPath );

	bool Rv;
	i3FileStream * pStream = nullptr;

	// 혹시 Pack 파일에서 못 찾으면 직접 찾는다.
	if( pStream == nullptr )
	{
		pStream = i3FileStream::new_object();

		Rv = pStream->Open( pszPath, STREAM_READ | STREAM_SHAREREAD );
		if( Rv == false)
		{
			I3TRACE( "ERROR : Could not find file. \"%s\"\n", pszPath );
			I3_SAFE_RELEASE( pStream );
		}
	}

	return pStream;
}

// 사운드 로딩
i3Sound* i3ResourceManager::_LoadSound( const char *szPath, UINT32 * pSize)
{
	CHECK_PATH( szPath );

	i3NamedResourceManager * pMng = i3ResourceFile::GetResourceManager( i3Sound::static_meta());
	I3ASSERT( pMng != nullptr);

	i3Sound * pRes = (i3Sound*) pMng->GetResource(nullptr, i3ResourceFile::GetWorkingFolder(), szPath);
	I3_SAFE_ADDREF( pRes);

#if defined( I3_DEBUG)
	if( pSize != nullptr)
	{
		*pSize = sizeof( i3Sound);
		*pSize += pRes->GetDataLength();
	}
#endif

	return pRes;
}

void i3ResourceManager::GetFileNameByLanguage( char * newPath , const char * szPath, UINT32 nBufSize )
{
	if( m_strLangPostfix.empty() )
	{
		strncpy( newPath, szPath, nBufSize );
	}
	else
	{
		i3::insert_suffix_to_filename(newPath, szPath, m_strLangPostfix);
		/*
		char szBuf[MAX_PATH];
		char szExt[16];
		i3String::SplitFileExt( szPath, szExt, 16 );
		i3::safe_string_copy( szBuf, szPath, MAX_PATH );
		i3::remove_ext( szBuf );

		sprintf( (PSTR) newPath, (PSTR) "%s%s.%s", szBuf, m_strLangPostfix.c_str(), szExt );
		*/
	}
}

// 폰트 로딩
i3Font*	i3ResourceManager::LoadFont( const char *szPath, bool bUseLangPostfix )
{
	CHECK_PATH( szPath );

	bool bRet = false;
	char szTemp[MAX_PATH];

	if( bUseLangPostfix )
	{
		GetFileNameByLanguage( szTemp, szPath, sizeof( szTemp) );
	}
	else
	{
		strncpy( szTemp, szPath, sizeof( szTemp) -1 );
	}

	i3Font *pFont = nullptr;

	char szExt[32];
//	i3String::SplitFileExt( szPath, szExt, sizeof(szExt) );
	i3::extract_fileext(szPath, szExt);

	bool bOldFont = i3::generic_is_iequal( szExt, "i3fnt" ) ;

	if( bOldFont )
	{
		pFont = i3Font::new_object();
		if( ! pFont->Create( szTemp ) )
		{
			goto ExitLabel;
		}
	}
	else
	{
		pFont = i3Font::LoadFromFile( szTemp );
		if( pFont == nullptr )
		{
			goto ExitLabel;
		}
		if( pFont->GetRefCount() == 0 )
		{
			I3_SAFE_ADDREF( pFont);
		}
	}

	bRet = true;
ExitLabel:
	if( !bRet )
	{
		I3_SAFE_RELEASE( pFont );
		ReportErr( "%s : Could not create font (%s)", __FUNCTION__, szTemp );
	}
	return pFont;
}

// 무자열 테이블 로딩
i3StringTable*	i3ResourceManager::LoadStringTable( const char *szPath, bool bUseLangPostfix )
{
	CHECK_PATH( szPath );

	char szTemp[MAX_PATH];

	if( bUseLangPostfix )
	{
		GetFileNameByLanguage( szTemp, szPath, sizeof( szTemp) );
	}
	else
	{
		strncpy( szTemp, szPath, sizeof( szTemp) -1 );
	}

	i3Stream *pStream = nullptr;
	pStream = OpenStream( szTemp );
	if( pStream == nullptr )
	{
		return nullptr;
	}

	i3StringTable *pStringTable;
	pStringTable = i3StringTable::new_object();
	if( ! pStringTable->CreateFromStream( pStream ) )
	{
		ReportErr( "%s : Could not create StringTable (%s)", __FUNCTION__, szTemp );
		I3_SAFE_RELEASE( pStream );
		I3_SAFE_RELEASE( pStringTable );
		return nullptr;
	}

	I3_SAFE_RELEASE( pStream );
	return pStringTable;
}

////////////////////////////////////////////////////////////////////////////
// Resource
void		i3ResourceManager::AddResource( i3GameRes * pRes)
{
	i3Node * pAttachRoot;

	I3ASSERT( pRes != nullptr);

	I3_SAFE_ADDREF( pRes);

	if( m_bCommonRes)
	{
		pRes->addResStyle( I3_RES_STYLE_COMMON);
	}

	m_ResList.insert(pRes);

	pAttachRoot = m_pFramework->OnQueryAttachNode( pRes);
	I3ASSERT( pAttachRoot != nullptr);

	pRes->Create();
}

void	i3ResourceManager::RemoveAllResource( bool bReleaseCommon)
{
	
	i3GameRes * pRes;

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];

		if((bReleaseCommon == false) && (pRes->isCommonRes()))
			continue;

		pRes->Destroy();
		I3_MUST_RELEASE( pRes);

		m_ResList.erase( m_ResList.begin() + i);			// 그냥 클리어하는게 더 나을것 같은데..그냥 놔둠..(2012.05.22.수빈)
		i--;
	}

	i3NamedResourceManager::Reset();
}

void i3ResourceManager::RemoveResource( i3GameRes * pRes)
{
	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		if( m_ResList.get_vector()[i] == pRes)
		{
			pRes->Destroy();
			I3_MUST_RELEASE( pRes);

			m_ResList.erase( m_ResList.begin() + i);
			break;
		}
	}
}


void i3ResourceManager::DumpAllResources(void)
{
// 릴리즈에서 오류가 남==> 대부분이 I3TRACE 출력이므로, 릴리즈때 실행되지 않는 것을 가정하고, 디버그 조건을 추가함..(2012.06.09.수빈)
#if defined( I3_DEBUG)

	i3GameRes * pRes;
	UINT32 t = 0;

	I3TRACE( "----------------------------------------------------------------\n");

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];						

		UINT32 size = sizeof( *pRes) + pRes->m_FileSize;
		t += size;
		I3TRACE( "file : %s (%dbyte) (ref:%d)\n", pRes->GetName(), size, pRes->GetRefCount());

		if( pRes->getInstanceCount() > 0)
		{
			I3TRACE( " ------- instance count %d ------\n", pRes->getInstanceCount() );
			for( INT32 j = 0; j < pRes->getInstanceCount(); j++)
			{
				i3GameObjBase * pObj = pRes->getInstance(j);
				I3TRACE( " - instance %s (%d)\n", pObj->GetName(), pObj->GetRefCount() );
			}
			I3TRACE( " --------------------------------\n");
		}
	}

	I3TRACE( "Total count : %d\n", m_ResList.size());
	I3TRACE( "Total size : %d\n", t);

#endif
}

i3GameRes *	i3ResourceManager::FindResourceByPath( const char * pszName)
{
	UINT128 hash;

	char conv[MAX_PATH];
	i3::safe_string_copy( conv, pszName, MAX_PATH );
	i3::make_unix_path( conv);
	i3::to_upper( conv);

	//hash = CRC32( 0xFFFFFFFF, (UINT8 *) conv, i3::generic_string_size( conv));
	i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &hash);

	return FindResourceByHash( &hash);
}

i3ResourceObject *	i3ResourceManager::FindResourceFromManager( const char * pszName, i3ClassMeta * pMeta)
{
	i3NamedResourceManager * pManager;

	pManager = i3ResourceFile::GetResourceManager( pMeta);
	if( pManager == nullptr)
		return nullptr;

	return pManager->FindResource( pszName);

}

i3GameRes *	i3ResourceManager::FindResourceByHash( UINT128 * hash)
{
	i3::pair<i3::vector_multiset<i3GameRes*>::const_iterator, bool> pr = i3::vsu::binary_find(m_ResList, *hash, _ResFindProc());
	
	if (pr.second == false)
		return nullptr;
	return *pr.first;
}

i3GameRes * i3ResourceManager::FindResourceByOnlyName( const char * pszName)
{
	i3GameRes * pRes;
	char szName[256];
	char szExt[ 128];

	i3::extract_fileext(pszName, szExt);
	
	I3_GAMERES_TYPE resType = __ConvertExt2ResType( szExt);

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];

		if( pRes->getType() != resType)	continue;

		i3::extract_filename(pRes->GetNameString(), szName);
	
		if( i3::generic_is_iequal( szName, pszName) )
			return pRes;
	}

	return nullptr;
}

i3GameRes *		i3ResourceManager::FindResourceByOnlyNameAndClassMeta(const char* pszName, i3::class_meta* pMeta)
{
	i3GameRes * pRes;
	char szName[256];
	char szExt[128];

	i3::extract_fileext(pszName, szExt);
	
	I3_GAMERES_TYPE resType = __ConvertExt2ResType(szExt);

	for (size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];

		if (pRes->getType() != resType)	continue;
		
		i3::extract_filename(pRes->GetNameString(), szName);

		if (i3::generic_is_iequal(szName, pszName))
		{
			if (i3::kind_of<i3GameResSceneGraph*>(pRes))
			{
				i3GameResSceneGraph* pResSceneGraph = static_cast<i3GameResSceneGraph*>(pRes);

				i3Node* pNode = pResSceneGraph->getScene();

				if (pNode)
				{
					if (pNode->kind_of(pMeta) == false) continue;		// 12월 버전 적용시 이 루틴만 추가되는 것만 유의하면 됨..
				}
			}
			
			return pRes;
		}
		
	}

	return nullptr;
}



i3GameRes *	i3ResourceManager::FindResourceByType( INT32 type)
{
	
	i3GameRes * pRes;

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];

		if( pRes->getType() == type)
			return pRes;
	}

	return nullptr;
}

I3_GAMERES_TYPE	i3ResourceManager::__ConvertExt2ResType( const char * szExt)
{
	i3::iterator_range<const char*> strExt = i3::as_literal(szExt);

	if( i3::generic_is_iequal( strExt, "I3S") )
		return I3_GAMERES_SCENE;
	else if( i3::generic_is_iequal( strExt, "I3CHR") )
		return I3_GAMERES_CHARA;
	else if( i3::generic_is_iequal( strExt, "I3WRD") )
		return I3_GAMERES_WORLD;
	else if( i3::generic_is_iequal( strExt, "I3A") )
		return I3_GAMERES_ANIMATION;
	else if( i3::generic_is_iequal( strExt, "I3ANIMPACK") )
		return I3_GAMERES_ANIMATIONPACK;
	else if((i3::generic_is_iequal( strExt, "I3I") ) || (i3::generic_is_iequal( strExt, "TGA") ) || (i3::generic_is_iequal( strExt, "BMP") ))
		return I3_GAMERES_TEXTURE;
	else if( i3::generic_is_iequal( strExt, "WAV") )
		return I3_GAMERES_SOUND;
	else if( i3::generic_is_iequal( strExt, "I3OBJ") )
		return I3_GAMERES_OBJECT;
	else
	{
		I3TRACE( "알수 없는 파일 형식!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}

	return I3_GAMERES_NONE;
}

i3GameRes * i3ResourceManager::__LoadResource( const char * pszPath, i3GameRes * pSrcRes, INT32 layer, bool bCommonRes, bool bInThread, bool bBGLoadEnable)
{
	char szName[ MAX_PATH];
	char szExt[32];
	i3GameRes * pRes = nullptr;

	//I3TRACE( "* RES LOAD : %s\n", pszPath);

	i3::extract_filename(pszPath, szName);
	i3::to_upper( szName);

	i3::extract_fileext(pszPath, szExt);
	i3::to_upper( szExt);

	I3_GAMERES_TYPE resType = __ConvertExt2ResType( szExt);

	switch( resType)
	{
	case I3_GAMERES_SCENE :
		{
			// Scene Graph
			i3SceneGraphInfo * pInfo;

			if( pSrcRes != nullptr)
				pRes = pSrcRes;
			else
				pRes = i3GameResSceneGraph::new_object_ref();

			pInfo = LoadModel( pszPath, bInThread, bBGLoadEnable, nullptr
			#if defined( I3_DEBUG)
				, &pRes->m_FileSize
			#endif
				);

			((i3GameResSceneGraph *)pRes)->setSceneGraphInfo( pInfo);
			((i3GameResSceneGraph *)pRes)->setLayer( 0);
		}
		break;

	case I3_GAMERES_CHARA :
		{
			// Character
			i3GameResChara * pChar;
			if( pSrcRes != nullptr)
				pChar = (i3GameResChara*) pSrcRes;
			else
			{
				pChar = i3GameResChara::new_object_ref();
				pChar->setLayer( 0);
			}
			
			// 메인쓰레드에서 이미 로딩되고, 백그라운드에서 같은 리소스가 아직 완료 안된 경우에
			// pChar에 중복된 데이터가 존재할수 있으므로, 이런 경우엔 로딩을 아예 스킵한다..
			if (pChar->getChara() && pChar->getSceneGraphInfo() )
			{
				I3TRACE("redundant loading occurred. (before LoadCharacter) : %s \n", pszPath);
			}
			else
			{

				i3Chara * pChara = LoadCharactor( pszPath , nullptr, false, nullptr, false, bCommonRes, bInThread
	#if defined( I3_DEBUG)
				, &pChar->m_FileSize
	#endif
				);
				if( pChara == nullptr)
				{
					I3PRINTLOG(I3LOG_FATAL,  "no res %s", pszPath);
					if (pSrcRes == nullptr)
					{
						pChar->destroy_instance();
					}
					return nullptr;
				}
				
				if (pChar->getChara() && pChar->getSceneGraphInfo() )
				{
					I3TRACE("redundant loading occurred. (after LoadCharacter): %s \n", pszPath);
					I3_MUST_RELEASE(pChara);
				}
				else
				{
					pChar->setChara( pChara);
					pChar->setSceneGraphInfo( pChara->getSceneGraphInfo());
					I3_MUST_RELEASE( pChara);
				}
			}

			pRes = pChar;
		}
		break;

	case I3_GAMERES_WORLD :
		{
			i3GameResWorld * pWorldRes;
			
			if( pSrcRes != nullptr)
				pWorldRes = (i3GameResWorld*) pSrcRes;
			else
				pWorldRes = i3GameResWorld::new_object_ref();

			i3World * pWorld = LoadWorld( pszPath, 0, false);
			I3ASSERT( pWorld != nullptr);

			i3SceneGraphInfo * pInfo = i3SceneGraphInfo::new_object_ref();
			pInfo->setInstanceSg( pWorld->GetNode());

			pWorldRes->setWorld( pWorld);
			pWorldRes->setSceneGraphInfo( pInfo);
			pWorldRes->setLayer( 0);

			I3_MUST_RELEASE( pWorld);
			pRes = pWorldRes;
		}
		break;

	case I3_GAMERES_ANIMATION :
		{
			i3GameResAnimation * pAnimRes;
			
			if( pSrcRes != nullptr)
				pAnimRes = (i3GameResAnimation*) pSrcRes;
			else
				pAnimRes = i3GameResAnimation::new_object_ref();

			i3Animation * pAnim = LoadAnimation( pszPath
	#if defined( I3_DEBUG)
				, &pAnimRes->m_FileSize
	#endif
				);
			if( pAnim == nullptr)
			{
				i3Log( "i3Framework::LoadResource()", "Could not load resource : %s\n", pszPath);
			}
			else
			{
				pAnimRes->setAnimation( pAnim);
				I3_MUST_RELEASE( pAnim);
			}

			pRes = pAnimRes;
		}
		break;

	case I3_GAMERES_ANIMATIONPACK :
		{
			LoadAnimationPack( pszPath);
		}
		break;

	case I3_GAMERES_TEXTURE :
		{
			i3GameResTexture * pTexRes;
			
			if( pSrcRes != nullptr)
				pTexRes = (i3GameResTexture*) pSrcRes;
			else
				pTexRes = i3GameResTexture::new_object_ref();

			i3Texture * pTex = LoadTexture( pszPath
	#if defined( I3_DEBUG)
				, false, &pTexRes->m_FileSize
	#endif
				);
			if( pTex == nullptr)
			{
				i3Log( "i3Framework::LoadRSC()", "Could not load resource : %s\n", pszPath);
			}
			else
			{
				pTexRes->setTexture( pTex);
			}

			pRes = pTexRes;
		}
		break;

	case I3_GAMERES_SOUND :
		{
			i3GameResSound * pSoundRes;

			if( pSrcRes != nullptr)
				pSoundRes = (i3GameResSound*) pSrcRes;
			else
			{
				pSoundRes = i3GameResSound::new_object_ref();
				i3SoundPlayInfo * pPlayInfo = i3SoundPlayInfo::new_object_ref();
				pSoundRes->setSound( pPlayInfo);
			}

			i3Sound * pSnd = _LoadSound( pszPath
	#if defined( I3_DEBUG)
				, &pSoundRes->m_FileSize
	#endif
				);

			if( pSnd == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN,  "%s sound file was not loaded.", pszPath);
			}
			else
			{
				pSoundRes->getSound()->setSound( pSnd);
				I3_MUST_RELEASE( pSnd);	// RefCount를 맞춘다.
			}

			pRes = pSoundRes;
		}
		break;
	}

	if( pRes != nullptr)
	{
		pRes->SetResourcePath( pszPath);
		if( bCommonRes)
			pRes->addResStyle( I3_RES_STYLE_COMMON);

		if( bInThread == false)	// Thread에서 load한 경우는 반드시 mainthread에서 등록하도록 해줘야 한다.
			AddResource( pRes);
	}

	return pRes;
}

i3GameRes * i3ResourceManager::LoadResource( const char * pszPath, INT32 layer, bool bCommonRes, bool bInThread, bool bBGLoadEnable)
{
	i3GameRes * pRes = nullptr;

	pRes = FindResourceByPath( pszPath);
	if( pRes != nullptr)
		return pRes;

	return __LoadResource( pszPath, nullptr, layer, bCommonRes?true:false, bInThread, bBGLoadEnable);
}

// RSC에서 사용하는 리소스를 파일로 뽑기위해 만든함수
// i3ResourceManager::LoadRSC() 의 내용이 변할경우 코드 동기화필요합니다.
bool i3ResourceManager::DumpRSCInfo(const char* rscPath, const char* srcMediaPath, 
	const char* savePath, bool genResInfo, bool copyRefRes)
{
	typedef i3::vector<i3::string> ResPathVec;
	ResPathVec resPaths;

	bool bRet = false;
	i3FileStream* pStream = i3FileStream::new_object();
	INT32 nCount = 0;

	INT32 i;
	i3GameRes * pRes;

	char szName[MAX_PATH];
	char szFilePath[MAX_PATH];

	char fullRscPath[MAX_PATH] = {0};
	sprintf_s(fullRscPath, MAX_PATH, "%s/%s", srcMediaPath ? srcMediaPath : ".", rscPath);

	bool Rv = pStream->Open(fullRscPath, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == false)
	{
		I3TRACE( "ERROR : Could not load file. \"%s\"\n", fullRscPath);
		bRet = false;
		goto ExitPart;
	}

	UINT8 nID[4];
	if( pStream->Read( nID, sizeof( UINT8) * 4) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed To Read ID. %s", fullRscPath );
		bRet = false;
		goto ExitPart;
	}

	if( i3::generic_is_niequal( (char*)nID, "RSCF", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Not a RSCFile. %s", fullRscPath );
		bRet = false;
		goto ExitPart;
	}
	
	if( pStream->Read( &nCount, sizeof( INT32)) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed to Get ResCount. %s", fullRscPath );
		bRet = false;
		goto ExitPart;
	}

	for( i = 0; i < nCount; i++)
	{
		pRes = nullptr;
		i3::pack::RSC_HEADER data;

		szName[0]			= 0;
		szFilePath[0]		= 0;

		INT32	nVersion	= 1;

		if( pStream->Read( &data, sizeof( data)) == STREAM_ERR)
		{
			bRet = false;
			goto ExitPart;
		}

//		UINT32 nStyle = (data.m_nType & 0x0FFF0000) >> 16;

		if( i3::generic_is_niequal( (char*)data.m_ID, "RSC1", 4) )
		{
			nVersion = 1;
		}
		else if( i3::generic_is_niequal( (char*)data.m_ID, "RSC2", 4) )
		{
			nVersion = 2;
		}
		else
		{
			bRet = false;
			goto ExitPart;
		}

		if( data.m_nNameBufSize > 0)
		{
			if( pStream->Read( &szName, data.m_nNameBufSize + 1) == STREAM_ERR)
			{
				bRet = false;
				goto ExitPart;
			}
		}

		if( data.m_nFilePathSize > 0)
		{
			if( pStream->Read( &szFilePath, data.m_nFilePathSize + 1) == STREAM_ERR)
			{
				bRet = false;
				goto ExitPart;
			}

			if( data.m_nNameBufSize <= 0)
			{
				strncpy( szName, szFilePath, sizeof( szName) - 1);
			}
		}

		struct fnPathNormalizer 
		{ 
			i3::string::value_type operator()(char in) 
			{ 
				if(in == '\\')
					return '/';

				return in;
			}	
		};

		i3::transform(szFilePath, szFilePath + strlen(szFilePath), szFilePath, fnPathNormalizer());


		//bool bUnique = true;
		//if( FindResourceByPath( szFilePath) != nullptr)
		//{
		//	I3TRACE( "!!!! RESMNG : %s is redundant resource.\n", szName);
		//	bUnique = false;
		//}

		if( nVersion == 1)
		{
			i3QueueFileInfo::TYPE nType = static_cast<i3QueueFileInfo::TYPE>(data.m_nType & 0x000000FF);
			if( nType == i3QueueFileInfo::TYPE_SCENE)				
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResSceneGraph * pSGRes = i3GameResSceneGraph::new_object();
				pSGRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pSGRes);
			}
			else if( nType == i3QueueFileInfo::TYPE_CHARA)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResChara * pChar = i3GameResChara::new_object();
				pChar->LoadFromFile(pStream, nVersion);
				I3_SAFE_RELEASE(pChar);
			}
			else if( nType == i3QueueFileInfo::TYPE_OBJECT)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResObject * pObjRes = i3GameResObject::new_object();
				pObjRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pObjRes);
			}
			else if( nType == i3QueueFileInfo::TYPE_WORLD)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResWorld* pWorld = i3GameResWorld::new_object();
				pWorld->LoadFromFile(pStream, nVersion);
				I3_SAFE_RELEASE(pWorld);
			}
			else if( nType == i3QueueFileInfo::TYPE_ANIMATION)		
			{ 
				resPaths.push_back(i3::string(szFilePath)); 
			}
			else if( nType == i3QueueFileInfo::TYPE_TEXTURE)		
			{ 
				resPaths.push_back(i3::string(szFilePath)); 
			}
			else if( nType == i3QueueFileInfo::TYPE_SOUND)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResSound * pSndRes = i3GameResSound::new_object();
				pSndRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pSndRes);
			}
			else if( nType == i3QueueFileInfo::TYPE_ANIMATIONPACK)	
			{ 
				resPaths.push_back(i3::string(szFilePath)); 
			}
			else 
			{ 
				resPaths.push_back(i3::string(szFilePath)); 
			}
		}
		else if( nVersion == 2)
		{
			I3_GAMERES_TYPE nType = static_cast<I3_GAMERES_TYPE>(data.m_nType & 0x000000FF);
			if( nType == I3_GAMERES_SCENE)					
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResSceneGraph * pSGRes;
				pSGRes = i3GameResSceneGraph::new_object();				
				pSGRes->LoadFromFile(pStream, nVersion);
				I3_SAFE_RELEASE(pSGRes);
			}
			else if( nType == I3_GAMERES_CHARA)				
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResChara * pChar = i3GameResChara::new_object();
				pChar->LoadFromFile(pStream, nVersion);
				I3_SAFE_RELEASE(pChar);
			}
			else if( nType == I3_GAMERES_OBJECT)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResObject * pObjRes = i3GameResObject::new_object();
				pObjRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pObjRes);
			}
			else if( nType == I3_GAMERES_WORLD)				
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResWorld * pWorld= i3GameResWorld::new_object();
				pWorld->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pWorld);
			}
			else if( nType == I3_GAMERES_ANIMATION)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
			}
			else if( nType == I3_GAMERES_TEXTURE)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
			}
			else if( nType == I3_GAMERES_SOUND)				
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResSound * pSndRes = i3GameResSound::new_object();
				pSndRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pSndRes);
			}
			else if( nType == I3_GAMERES_LIGHT)				
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResLight * pLightRes = i3GameResLight::new_object();
				pLightRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pLightRes);
			}
			else if( nType == I3_GAMERES_RESPAWN)			
			{ 
				resPaths.push_back(i3::string(szFilePath));  
				i3GameResRespawn * pRespawnRes = i3GameResRespawn::new_object();
				pRespawnRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pRespawnRes);
			}
			/*
			else if( nType == I3_GAMERES_OBJECT)			
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResObject * pObjectRes = i3GameResObject::new_object();
				pObjectRes->LoadFromFile(pStream, nVersion);
				I3_SAFE_RELEASE(pObjectRes);
			}
			*/
			else if( nType == I3_GAMERES_ANIMATIONPACK)		
			{ 
				resPaths.push_back(i3::string(szFilePath));  
			}
			else if( nType == I3_GAMERES_ACTION_EVENT)		
			{ 
				resPaths.push_back(i3::string(szFilePath));
				i3GameResEvent * pEventRes = i3GameResEvent::new_object();
				pEventRes->LoadFromFile( pStream, nVersion);
				I3_SAFE_RELEASE(pEventRes);
			}
			else if( nType == I3_GAMERES_PATH)				
			{ 
				resPaths.push_back(i3::string(szFilePath));  
			}
			else 
			{ 
				resPaths.push_back(i3::string(szFilePath));  
			}
		}
	}

	if(savePath)
	{
		if(genResInfo || copyRefRes)
		{
			if(!MakePathDirectory(savePath))
				return false;
		}

		char iniPath[MAX_PATH] = {0};		
		char *p = (char*)strrchr((char*)rscPath, '/');
		sprintf_s(iniPath, MAX_PATH, "%s/%s.resinfo", savePath, p ? p+1 : rscPath);

		if(genResInfo)
			WritePrivateProfileSection(fullRscPath, "\0\0", iniPath);

		i3::sort(resPaths.begin(), resPaths.end(), i3::less<i3::string>());

		// copy content
		for(ResPathVec::size_type i=0; i<resPaths.size(); i++)
		{
			const char* val = " ";

			if(copyRefRes)
			{
				char srcPath[MAX_PATH] = {0};
				char dstPath[MAX_PATH] = {0};

				sprintf_s(srcPath, MAX_PATH, "%s/%s", srcMediaPath ? srcMediaPath : ".", resPaths[i].c_str());
				sprintf_s(dstPath, MAX_PATH, "%s/%s", savePath, resPaths[i].c_str());
				
				val = "copy fail";

				char dstPathOnly[MAX_PATH] = {0};
				strcpy_s(dstPathOnly, MAX_PATH, dstPath);
				
				char *p = strrchr(dstPathOnly, '/');
				if(p)
				{
					*(p+1) = 0;
				}

				if(MakePathDirectory(dstPathOnly))
				{
					if(0 != ::CopyFileEx(srcPath, dstPath, NULL, NULL, FALSE, 0))
						val = "copy ok";
				}
			}

			if(genResInfo)
				WritePrivateProfileString(fullRscPath, resPaths[i].c_str(), val, iniPath);
		}
	}

	bRet = true;

ExitPart:
	I3_SAFE_RELEASE(pStream);

	return bRet;
}

bool i3ResourceManager::MakePathDirectory(const char* path)
{
	if(!path)
		return false;

	char operPath[MAX_PATH] = {0};
	strcpy_s(operPath, MAX_PATH, path);

	for(char* c=operPath; *c != 0; c++)
	{
		if(*c == '/') 
			*c = '\\'; // for SHCreateDirectoryEX
	}

	DWORD attr = GetFileAttributes(operPath);

	if(attr == INVALID_FILE_ATTRIBUTES)
	{
		// create with subdirectory, requires winxp sp2 or win2003 or higher
		return ERROR_SUCCESS == SHCreateDirectoryEx(NULL, operPath, NULL);
	}
	else
	{
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
}

bool i3ResourceManager::LoadRSC( const char * pszPath, bool bCommonRes)
{
	bool bRet = false;
	i3Stream *pStream = nullptr;
	INT32 nCount = 0;

	INT32 i;//, val;
	//char szTemp[256];
	i3GameRes * pRes;
	INT32 bRotateAxis = 0;	//	GIGA에서 bool은 unsigned char 이므로 GetValue를 사용하기 위해 INT32로 변경 by KOMET

	char szName[MAX_PATH];
	char szFilePath[MAX_PATH];

	I3PRINTLOG(I3LOG_NOTICE,  "--- RSC : %s", pszPath);

	if (!i3FileStream::isFile(pszPath))
	{
		char szFileName[MAX_PATH];
		char szExt[MAX_PATH];
		i3::extract_filetitle_fileext(pszPath, szFileName, szExt);
		if (i3::generic_compare(szExt, "RSC") == 0)
		{
			i3::generic_copy_range(szFileName, pszPath);
			i3::change_file_ext((char*)szFileName, "RSG");
			return LoadRSG(szFileName, bCommonRes);
		}
	}
	else
		pStream = OpenStream(pszPath);

	if( pStream == nullptr )
	{
		

		I3TRACE( "WARNING : Could not find resource configuration file. %s", pszPath );
		bRet = true;
		goto ExitPart;
	}

	UINT8 nID[4];
	if( pStream->Read( nID, sizeof( UINT8) * 4) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed To Read ID. %s", pszPath );
		bRet = false;
		goto ExitPart;
	}

	if( i3::generic_is_niequal( (char*)nID, "RSCF", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "Not a RSCFile. %s", pszPath );
		bRet = false;
		goto ExitPart;
	}
	
	if( pStream->Read( &nCount, sizeof( INT32)) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed to Get ResCount. %s", pszPath );
		bRet = false;
		goto ExitPart;
	}

	for( i = 0; i < nCount; i++)
	{
		pRes = nullptr;
		i3::pack::RSC_HEADER data;

		szName[0]			= 0;
		szFilePath[0]		= 0;

		INT32	nVersion	= 1;

		if( pStream->Read( &data, sizeof( data)) == STREAM_ERR)
		{
			bRet = false;
			goto ExitPart;
		}

		UINT32 nType = data.m_nType & 0x000000FF;
		I3_RES_STYLE nStyle = static_cast<I3_RES_STYLE>((data.m_nType & 0x0FFF0000) >> 16);

		if( i3::generic_is_niequal( (char*)data.m_ID, "RSC1", 4) )
		{
			nVersion = 1;
		}
		else if( i3::generic_is_niequal( (char*)data.m_ID, "RSC2", 4) )
		{
			nVersion = 2;
		}
		else
		{
			bRet = false;
			goto ExitPart;
		}

		if( data.m_nNameBufSize > 0)
		{
			if( pStream->Read( &szName, data.m_nNameBufSize + 1) == STREAM_ERR)
			{
				bRet = false;
				goto ExitPart;
			}
		}

		if( data.m_nFilePathSize > 0)
		{
			if( pStream->Read( &szFilePath, data.m_nFilePathSize + 1) == STREAM_ERR)
			{
				bRet = false;
				goto ExitPart;
			}

			if( data.m_nNameBufSize <= 0)
			{
				strncpy( szName, szFilePath, sizeof(szName) - 1);
			}
		}

		bool bUnique = true;
		if( FindResourceByPath( szFilePath) != nullptr)
		{
			I3TRACE( "!!!! RESMNG : %s is redundant resource.\n", szName);

			bUnique = false;
		}

		if( nVersion == 1)
		{	
			if( nType == i3QueueFileInfo::TYPE_SCENE)
			{
				i3GameResSceneGraph * pSGRes;
				INT32 cc = 0;

				pRes = pSGRes = i3GameResSceneGraph::new_object_ref();

				pSGRes->LoadFromFile( pStream, nVersion);
				pSGRes->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_SCENE, pSGRes, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pSGRes->setCompletionCount( cc);
			}
			else if( nType == i3QueueFileInfo::TYPE_CHARA)
			{
				i3GameResChara * pChar;
				INT32 cc = 0;

				pRes = pChar = i3GameResChara::new_object_ref();

				pChar->LoadFromFile( pStream, nVersion);
				pChar->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0 )
				{
					if( pRes->isCommonRes() == bCommonRes || isCommonLoadRes(szFilePath, pRes->isCommonRes()))
					{
						__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_CHARA, pChar, 1, bRotateAxis);
						cc++;
					}
					else
					{
						__AddBackgroundFile( szFilePath, i3QueueFileInfo::TYPE_CHARA, pRes, 1, bRotateAxis);
						continue;
					}
				}
				else 
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pChar->setCompletionCount( cc);
			}
			else if( nType == i3QueueFileInfo::TYPE_OBJECT)
			{
				i3GameResObject * pObjRes;

				pRes = pObjRes = i3GameResObject::new_object_ref();

				pObjRes->LoadFromFile( pStream, nVersion);
				pObjRes->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjRes, 1, bRotateAxis);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == i3QueueFileInfo::TYPE_WORLD)
			{
				i3GameResWorld * pWorld;
				INT32 cc = 0;

				pRes = pWorld = i3GameResWorld::new_object_ref();

				pWorld->LoadFromFile( pStream, nVersion);
				pWorld->SetResourcePath( szFilePath);

				// World
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_WORLD, pWorld, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pWorld->setCompletionCount( cc);
			}
			else if( nType == i3QueueFileInfo::TYPE_ANIMATION)
			{
				i3GameResAnimation * pAnimRes = i3GameResAnimation::new_object_ref();;
				I3ASSERT( pAnimRes != nullptr);

				pAnimRes->SetResourcePath( szFilePath);
				pRes = pAnimRes;

				if( bUnique && szFilePath[0] != 0)
				{
					__AddBackgroundFile( szFilePath, i3QueueFileInfo::TYPE_ANIMATION, pAnimRes, 0, 0);
					continue;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == i3QueueFileInfo::TYPE_TEXTURE)
			{
				// Texture
				i3GameResTexture * pTexRes = i3GameResTexture::new_object_ref();
				I3ASSERT( pTexRes != nullptr);
				
				pTexRes->SetResourcePath( szFilePath);
				pRes = pTexRes;

				if( bUnique && szFilePath[0] != 0)
				{
					if( pRes->isCommonRes() == bCommonRes )
						__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
					else
					{
						__AddBackgroundFile( szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
						continue;
					}
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == i3QueueFileInfo::TYPE_SOUND)
			{
				i3GameResSound * pSndRes = i3GameResSound::new_object_ref();

				pSndRes->LoadFromFile( pStream, nVersion);

				pSndRes->SetResourcePath( szFilePath);

				pRes = pSndRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					if( pRes->isCommonRes() == bCommonRes )
					{
						__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_SOUND, pSndRes, 0, 0);
					}
					else
					{
						__AddBackgroundFile( szFilePath, i3QueueFileInfo::TYPE_SOUND, pRes, 0, 0);
						continue;
					}
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == i3QueueFileInfo::TYPE_ANIMATIONPACK)
			{
				__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_ANIMATIONPACK, nullptr, 0, 0);
			}
			else
			{
				__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_BINARY, nullptr, 0, 0);
			}
		}
		else if( nVersion == 2)
		{
			if( nType == I3_GAMERES_SCENE)
			{
				i3GameResSceneGraph * pSGRes;
				INT32 cc = 0;

				pRes = pSGRes = i3GameResSceneGraph::new_object_ref();
				pRes->addResStyle( nStyle);

				pSGRes->LoadFromFile( pStream, nVersion);

				//pSGRes->SetName( szName);
				pSGRes->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_SCENE, pSGRes, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pSGRes->setCompletionCount( cc);
			}
			else if( nType == I3_GAMERES_CHARA)
			{
				i3GameResChara * pChar;
				INT32 cc = 0;

				pRes = pChar = i3GameResChara::new_object_ref();
				pRes->addResStyle( nStyle);
				pChar->LoadFromFile( pStream, nVersion);

				//pChar->SetName( szName);
				pChar->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_CHARA, pChar, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pChar->setCompletionCount( cc);
			}
			else if( nType == I3_GAMERES_OBJECT)
			{
				i3GameResObject * pObjRes;
				INT32 cc = 0;

				pRes = pObjRes = i3GameResObject::new_object_ref();
				pRes->addResStyle( nStyle);
				pObjRes->LoadFromFile( pStream, nVersion);
				pObjRes->SetResourcePath( szFilePath);

				// Scene
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjRes, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_WORLD)
			{
				i3GameResWorld * pWorld;
				INT32 cc = 0;

				pRes = pWorld = i3GameResWorld::new_object_ref();
				pRes->addResStyle( nStyle);
				pWorld->LoadFromFile( pStream, nVersion);

				//pWorld->SetName( szName);
				pWorld->SetResourcePath( szFilePath);

				// World
				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_WORLD, pWorld, 1, bRotateAxis);
					cc++;
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}

				pWorld->setCompletionCount( cc);
			}
			else if( nType == I3_GAMERES_ANIMATION)
			{
				i3GameResAnimation * pAnimRes = i3GameResAnimation::new_object_ref();;
				I3ASSERT( pAnimRes != nullptr);

				//pAnimRes->SetName( szName);
				pAnimRes->SetResourcePath( szFilePath);
				pRes = pAnimRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_ANIMATION, pAnimRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_TEXTURE)
			{
				// Texture
				i3GameResTexture * pTexRes = i3GameResTexture::new_object_ref();
				I3ASSERT( pTexRes != nullptr);

				//pTexRes->SetName( szName);
				pTexRes->SetResourcePath( szFilePath);
				pRes = pTexRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_SOUND)
			{
				i3GameResSound * pSndRes = i3GameResSound::new_object_ref();

				pSndRes->LoadFromFile( pStream, nVersion);

				pSndRes->SetResourcePath( szFilePath);
				pSndRes->SetName( szName);

				pRes = pSndRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_SOUND, pSndRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_LIGHT)
			{
				i3GameResLight * pLightRes = i3GameResLight::new_object_ref();

				pLightRes->LoadFromFile( pStream, nVersion);

				pLightRes->SetResourcePath( szFilePath);
				pLightRes->SetName( szName);

				pRes = pLightRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_LIGHT, pLightRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_RESPAWN)
			{
				i3GameResRespawn * pRespawnRes = i3GameResRespawn::new_object_ref();

				pRespawnRes->LoadFromFile( pStream, nVersion);

				pRespawnRes->SetResourcePath( szFilePath);
				pRespawnRes->SetName( szName);

				pRes = pRespawnRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_RESPAWN, pRespawnRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			/*
			else if( nType == I3_GAMERES_OBJECT)
			{
				i3GameResObject * pObjectRes = i3GameResObject::new_object_ref();

				pObjectRes->LoadFromFile( pStream, nVersion);

				pObjectRes->SetResourcePath( szFilePath);
				pObjectRes->SetName( szName);

				pRes = pObjectRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjectRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			*/
			else if( nType == I3_GAMERES_ANIMATIONPACK)
			{
				__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_ANIMATIONPACK, nullptr, 0, 0);
			}
			else if( nType == I3_GAMERES_ACTION_EVENT)
			{
				i3GameResEvent * pEventRes = i3GameResEvent::new_object_ref();

				pEventRes->LoadFromFile( pStream, nVersion);

				pEventRes->SetResourcePath( szFilePath);
				pEventRes->SetName( szName);

				pRes = pEventRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_EVENT, pEventRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else if( nType == I3_GAMERES_PATH)
			{
				i3GameResPath * pPathRes = i3GameResPath::new_object_ref();

				pPathRes->SetResourcePath( szFilePath);
				pPathRes->SetName( szName);

				pRes = pPathRes;
				pRes->addResStyle( nStyle);

				if( bUnique && szFilePath[0] != 0)
				{
					__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_PATH, pPathRes, 0, 0);
				}
				else
				{
					I3_SAFE_ADDREF( pRes);
					I3_SAFE_RELEASE( pRes);
					continue;
				}
			}
			else
			{
				__AddLoadRequest( szFilePath, i3QueueFileInfo::TYPE_BINARY, nullptr, 0, 0);
			}
		}

		/*if( pRes != nullptr && bCommonRes)
		{
			pRes->addStyle( I3_GAMERES_STYLE_COMMON);
		}*/
	}

	bRet = true;
ExitPart:
	I3_SAFE_RELEASE( pStream );

	return bRet;
}

bool i3ResourceManager::LoadRSG(const char * pszPath, bool bCommonRes)//const char * szRSCFileName)
{
	bool bRet = false;

	i3FileStream File;
	if (!File.Open(pszPath, STREAM_READ))
	{
		return bRet ;
	}

	i3IniParser	Parser;

	if (!Parser.Open(&File))
	{
		return bRet;
	}

	INT32 i;
	INT32 nCount = Parser.GetSectionCount();
	i3GameRes * pRes;
	char szName[MAX_PATH];
	char szFilePath[MAX_PATH];
	INT32 bRotateAxis = 0;

	for (i = 0; i < nCount; ++i)
	{
		if (Parser.ReadSectionByIndex(i))
		{
			i3::rc_string strFileName;
			i3IniSectionInfo * pSection = Parser.GetSection(i);
			
			pRes = nullptr;
			i3::pack::RSC_HEADER data;

			szName[0] = 0;
			szFilePath[0] = 0;

			INT32	nVersion = 1;

			char szString[MAX_PATH];
			Parser.GetString("Type", 0, szString, sizeof(szString));

			i3::iterator_range<const char*> strString = i3::as_literal(szString);

			if (i3::generic_is_iequal(strString, "SCENE"))			data.m_nType = I3RSC_TYPE_SCENE;
			else if (i3::generic_is_iequal(strString, "CHARA"))		data.m_nType = I3RSC_TYPE_CHARA;
			else if (i3::generic_is_iequal(strString, "WORLD"))		data.m_nType = I3RSC_TYPE_WORLD;
			else if (i3::generic_is_iequal(strString, "ANIMATION"))	data.m_nType = I3RSC_TYPE_ANIMATION;
			else if (i3::generic_is_iequal(strString, "TEXTURE"))		data.m_nType = I3RSC_TYPE_TEXTURE;
			else if (i3::generic_is_iequal(strString, "SOUND"))		data.m_nType = I3RSC_TYPE_SOUND;
			else if (i3::generic_is_iequal(strString, "BINARY"))		data.m_nType = I3RSC_TYPE_BINARY;
			else if (i3::generic_is_iequal(strString, "ANIMPACK"))		data.m_nType = I3RSC_TYPE_ANIMPACK;
			else if (i3::generic_is_iequal(strString, "OBJECT"))		data.m_nType = I3RSC_TYPE_OBJECT;
			else if (i3::generic_is_iequal(strString, "UI"))			data.m_nType = I3RSC_TYPE_UI;
			else														data.m_nType = I3RSC_TYPE_BINARY;

			i3::extract_filename(pSection->Name, strFileName);

			data.m_nNameBufSize = i3::generic_string_size(strFileName);
			data.m_nFilePathSize = i3::generic_string_size(pSection->Name);
			data.m_pParent = 0;

			UINT32 nType = data.m_nType & 0x000000FF;
			I3_RES_STYLE nStyle = static_cast<I3_RES_STYLE>((data.m_nType & 0x0FFF0000) >> 16);

			if (i3::generic_is_niequal((char*)data.m_ID, "RSC1", 4))
			{
				nVersion = 1;
			}
			else if (i3::generic_is_niequal((char*)data.m_ID, "RSC2", 4))
			{
				nVersion = 2;
			}
			else
			{
				bRet = false;
				goto ExitPart;
			}

			if (data.m_nNameBufSize > 0)
			{
				i3::generic_string_copy(szName, strFileName);
			}

			if (data.m_nFilePathSize > 0)
			{
				i3::generic_string_copy(szFilePath, pSection->Name);

				if (data.m_nNameBufSize <= 0)
				{
					strncpy(szName, szFilePath, sizeof(szName) - 1);
				}
			}

			bool bUnique = true;
			if (FindResourceByPath(szFilePath) != nullptr)
			{
				I3TRACE("!!!! RESMNG : %s is redundant resource.\n", szName);

				bUnique = false;
			}

 			if (nVersion == 1)
 			{
 				if (nType == i3QueueFileInfo::TYPE_SCENE)
 				{
 					i3GameResSceneGraph * pSGRes;
 					INT32 cc = 0;
 
 					pRes = pSGRes = i3GameResSceneGraph::new_object_ref();

 					pSGRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 					pSGRes->SetResourcePath(szFilePath);
 
 					// Scene
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_SCENE, pSGRes, 1, bRotateAxis);
 						cc++;
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 
 					pSGRes->setCompletionCount(cc);
 				}
  				else if (nType == i3QueueFileInfo::TYPE_CHARA)
  				{
  					i3GameResChara * pChar;
  					INT32 cc = 0;
  
  					pRes = pChar = i3GameResChara::new_object_ref();
  
  					pChar->LoadFromFile(&Parser, nVersion, data.m_nType);
  					pChar->SetResourcePath(szFilePath);
  
  					// Scene
  					if (bUnique && szFilePath[0] != 0)
  					{
  						if (pRes->isCommonRes() == bCommonRes || isCommonLoadRes(szFilePath, pRes->isCommonRes()))
  						{
  							__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_CHARA, pChar, 1, bRotateAxis);
  							cc++;
  						}
  						else
  						{
  							__AddBackgroundFile(szFilePath, i3QueueFileInfo::TYPE_CHARA, pRes, 1, bRotateAxis);
  							continue;
  						}
  					}
  					else
  					{
  						I3_SAFE_ADDREF(pRes);
  						I3_SAFE_RELEASE(pRes);
  						continue;
  					}
  
  					pChar->setCompletionCount(cc);
  				}
  				else if (nType == i3QueueFileInfo::TYPE_OBJECT)
  				{
  					i3GameResObject * pObjRes;
  
  					pRes = pObjRes = i3GameResObject::new_object_ref();
  
  					pObjRes->LoadFromFile(&Parser, nVersion, data.m_nType);
  					pObjRes->SetResourcePath(szFilePath);
  
  					// Scene
  					if (bUnique && szFilePath[0] != 0)
  					{
  						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjRes, 1, bRotateAxis);
  					}
  					else
  					{
  						I3_SAFE_ADDREF(pRes);
  						I3_SAFE_RELEASE(pRes);
  						continue;
  					}
  				}
  				else if (nType == i3QueueFileInfo::TYPE_WORLD)
  				{
  					i3GameResWorld * pWorld;
  					INT32 cc = 0;
  
  					pRes = pWorld = i3GameResWorld::new_object_ref();
  
  					pWorld->LoadFromFile(&Parser, nVersion, data.m_nType);
  					pWorld->SetResourcePath(szFilePath);
  
  					// World
  					if (bUnique && szFilePath[0] != 0)
  					{
  						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_WORLD, pWorld, 1, bRotateAxis);
  						cc++;
  					}
  					else
  					{
  						I3_SAFE_ADDREF(pRes);
  						I3_SAFE_RELEASE(pRes);
  						continue;
  					}
  
  					pWorld->setCompletionCount(cc);
  				}
  				else if (nType == i3QueueFileInfo::TYPE_ANIMATION)
  				{
  					i3GameResAnimation * pAnimRes = i3GameResAnimation::new_object_ref();;
  					I3ASSERT(pAnimRes != nullptr);
  
  					pAnimRes->SetResourcePath(szFilePath);
  					pRes = pAnimRes;
  
  					if (bUnique && szFilePath[0] != 0)
  					{
  						__AddBackgroundFile(szFilePath, i3QueueFileInfo::TYPE_ANIMATION, pAnimRes, 0, 0);
  						continue;
  					}
  					else
  					{
  						I3_SAFE_ADDREF(pRes);
  						I3_SAFE_RELEASE(pRes);
  						continue;
  					}
  				}
 				else if (nType == i3QueueFileInfo::TYPE_TEXTURE)
 				{
 					// Texture
 					i3GameResTexture * pTexRes = i3GameResTexture::new_object_ref();
 					I3ASSERT(pTexRes != nullptr);
 
 					pTexRes->SetResourcePath(szFilePath);
 					pRes = pTexRes;
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						if (pRes->isCommonRes() == bCommonRes)
 							__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
 						else
 						{
 							__AddBackgroundFile(szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
 							continue;
 						}
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == i3QueueFileInfo::TYPE_SOUND)
 				{
 					i3GameResSound * pSndRes = i3GameResSound::new_object_ref();
 
 					pSndRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pSndRes->SetResourcePath(szFilePath);
 
 					pRes = pSndRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						if (pRes->isCommonRes() == bCommonRes)
 						{
 							__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_SOUND, pSndRes, 0, 0);
 						}
 						else
 						{
 							__AddBackgroundFile(szFilePath, i3QueueFileInfo::TYPE_SOUND, pRes, 0, 0);
 							continue;
 						}
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
				else if (nType == i3QueueFileInfo::TYPE_ANIMATIONPACK)
				{
					__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_ANIMATIONPACK, nullptr, 0, 0);
				}
				else
				{
					__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_BINARY, nullptr, 0, 0);
				}
 			}
 			else if (nVersion == 2)
 			{
 				if (nType == I3_GAMERES_SCENE)
 				{
 					i3GameResSceneGraph * pSGRes;
 					INT32 cc = 0;
 
 					pRes = pSGRes = i3GameResSceneGraph::new_object_ref();
 					pRes->addResStyle(nStyle);
 
 					pSGRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					//pSGRes->SetName( szName);
 					pSGRes->SetResourcePath(szFilePath);
 
 					// Scene
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_SCENE, pSGRes, 1, bRotateAxis);
 						cc++;
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 
 					pSGRes->setCompletionCount(cc);
 				}
 				else if (nType == I3_GAMERES_CHARA)
 				{
 					i3GameResChara * pChar;
 					INT32 cc = 0;
 
 					pRes = pChar = i3GameResChara::new_object_ref();
 					pRes->addResStyle(nStyle);
 					pChar->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					//pChar->SetName( szName);
 					pChar->SetResourcePath(szFilePath);
 
 					// Scene
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_CHARA, pChar, 1, bRotateAxis);
 						cc++;
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 
 					pChar->setCompletionCount(cc);
 				}
 				else if (nType == I3_GAMERES_OBJECT)
 				{
 					i3GameResObject * pObjRes;
 					INT32 cc = 0;
 
 					pRes = pObjRes = i3GameResObject::new_object_ref();
 					pRes->addResStyle(nStyle);
 					pObjRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 					pObjRes->SetResourcePath(szFilePath);
 
 					// Scene
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjRes, 1, bRotateAxis);
 						cc++;
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_WORLD)
 				{
 					i3GameResWorld * pWorld;
 					INT32 cc = 0;
 
 					pRes = pWorld = i3GameResWorld::new_object_ref();
 					pRes->addResStyle(nStyle);
 					pWorld->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					//pWorld->SetName( szName);
 					pWorld->SetResourcePath(szFilePath);
 
 					// World
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_WORLD, pWorld, 1, bRotateAxis);
 						cc++;
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 
 					pWorld->setCompletionCount(cc);
 				}
 				else if (nType == I3_GAMERES_ANIMATION)
 				{
 					i3GameResAnimation * pAnimRes = i3GameResAnimation::new_object_ref();;
 					I3ASSERT(pAnimRes != nullptr);
 
 					//pAnimRes->SetName( szName);
 					pAnimRes->SetResourcePath(szFilePath);
 					pRes = pAnimRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_ANIMATION, pAnimRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_TEXTURE)
 				{
 					// Texture
 					i3GameResTexture * pTexRes = i3GameResTexture::new_object_ref();
 					I3ASSERT(pTexRes != nullptr);
 
 					//pTexRes->SetName( szName);
 					pTexRes->SetResourcePath(szFilePath);
 					pRes = pTexRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_TEXTURE, pTexRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_SOUND)
 				{
 					i3GameResSound * pSndRes = i3GameResSound::new_object_ref();
 
 					pSndRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pSndRes->SetResourcePath(szFilePath);
 					pSndRes->SetName(szName);
 
 					pRes = pSndRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_SOUND, pSndRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_LIGHT)
 				{
 					i3GameResLight * pLightRes = i3GameResLight::new_object_ref();
 
 					pLightRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pLightRes->SetResourcePath(szFilePath);
 					pLightRes->SetName(szName);
 
 					pRes = pLightRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_LIGHT, pLightRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_RESPAWN)
 				{
 					i3GameResRespawn * pRespawnRes = i3GameResRespawn::new_object_ref();
 
 					pRespawnRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pRespawnRes->SetResourcePath(szFilePath);
 					pRespawnRes->SetName(szName);
 
 					pRes = pRespawnRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_RESPAWN, pRespawnRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
				/*  위에 이미 같은 조건 처리가 되있어서 이것은 죽은 코드로 지적됨.(2016.08.05.수빈)
 				else if (nType == I3_GAMERES_OBJECT)
 				{
 					i3GameResObject * pObjectRes = i3GameResObject::new_object_ref();
 
 					pObjectRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pObjectRes->SetResourcePath(szFilePath);
 					pObjectRes->SetName(szName);
 
 					pRes = pObjectRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_OBJECT, pObjectRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
				*/
 				else if (nType == I3_GAMERES_ANIMATIONPACK)
 				{
 					__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_ANIMATIONPACK, nullptr, 0, 0);
 				}
 				else if (nType == I3_GAMERES_ACTION_EVENT)
 				{
 					i3GameResEvent * pEventRes = i3GameResEvent::new_object_ref();
 
 					pEventRes->LoadFromFile(&Parser, nVersion, data.m_nType);
 
 					pEventRes->SetResourcePath(szFilePath);
 					pEventRes->SetName(szName);
 
 					pRes = pEventRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_EVENT, pEventRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else if (nType == I3_GAMERES_PATH)
 				{
 					i3GameResPath * pPathRes = i3GameResPath::new_object_ref();
 
 					pPathRes->SetResourcePath(szFilePath);
 					pPathRes->SetName(szName);
 
 					pRes = pPathRes;
 					pRes->addResStyle(nStyle);
 
 					if (bUnique && szFilePath[0] != 0)
 					{
 						__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_PATH, pPathRes, 0, 0);
 					}
 					else
 					{
 						I3_SAFE_ADDREF(pRes);
 						I3_SAFE_RELEASE(pRes);
 						continue;
 					}
 				}
 				else
 				{
 					__AddLoadRequest(szFilePath, i3QueueFileInfo::TYPE_BINARY, nullptr, 0, 0);
 				}
 			}
		}
	}

	Parser.Close();

	bRet = true;
ExitPart:
	return bRet;
}

void i3ResourceManager::__AddLoadRequest( char * pszPath, i3QueueFileInfo::TYPE type, void * pUserData, INT32 param1, INT32 param2)
{
	i3QueueFileInfo * pInfo;

	pInfo = i3QueueFileInfo::new_object();

	pInfo->SetPath( pszPath);
	pInfo->SetType( type);
	pInfo->SetUserPointer( pUserData);
	pInfo->SetUserParam1( param1);
	pInfo->SetUserParam2( param2);

	m_QueueFileList.push_back( pInfo);

	I3TRACE( "LoadRequest %s\n", pszPath);

	if( (INT32)m_QueueFileList.size() > m_MaxQueueFileCount)
	{
		m_MaxQueueFileCount = m_QueueFileList.size();
	}
}

void i3ResourceManager::__AddLoadRequest( char * pszPath, void * pUserData, INT32 param1, INT32 param2)
{
	// char szExt[32];
	i3::stack_string strExt;

	i3QueueFileInfo::TYPE type;
	
//	i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);
	i3::extract_fileext(pszPath, strExt);
	i3::to_upper(strExt);

	if( i3::generic_is_equal( strExt, "I3S") )
	{
		type = i3QueueFileInfo::TYPE_SCENE;
	}
	else if( i3::generic_is_equal( strExt, "I3CHR") )
	{
		type = i3QueueFileInfo::TYPE_CHARA;
	}
	else if( i3::generic_is_equal( strExt, "I3WRD") )
	{
		type = i3QueueFileInfo::TYPE_WORLD;
	}
	else if( i3::generic_is_equal( strExt, "I3A") )
	{
		type = i3QueueFileInfo::TYPE_ANIMATION;
	}
	else if(	(i3::generic_is_equal( strExt, "I3I") ) ||
				(i3::generic_is_equal( strExt, "TGA") ) ||
				(i3::generic_is_equal( strExt, "BMP") ))
	{
		type = i3QueueFileInfo::TYPE_TEXTURE;
	}
	else if( i3::generic_is_equal( strExt, "I3ANIMPACK") )
	{
		type = i3QueueFileInfo::TYPE_ANIMATIONPACK;
	}
	else if( i3::generic_is_equal( strExt, "I3EVT") )
	{
		type = i3QueueFileInfo::TYPE_EVENT;
	}
	else if( i3::generic_is_equal( strExt, "I3PATH") )
	{
		type = i3QueueFileInfo::TYPE_PATH;
	}
	else if( i3::generic_is_equal( strExt, "I3OBJ") )
	{
		type = i3QueueFileInfo::TYPE_OBJECT;
	}
	else if( i3::generic_is_equal( strExt, "I3UI") )
	{
		type = i3QueueFileInfo::TYPE_UI;
	}
	else 
	{
		type = i3QueueFileInfo::TYPE_BINARY;
	}

	__AddLoadRequest( pszPath, type, pUserData, param1, param2);
}

void i3ResourceManager::RemoveAllLoadRequest(void)
{
	i3QueueFileInfo * pInfo;

	for(size_t i = 0; i < m_QueueFileList.size(); i++)
	{
		pInfo = m_QueueFileList[i];
		I3_MUST_RELEASE( pInfo);
	}

	m_QueueFileList.clear();

	m_MaxQueueFileCount = 0;
}

INT32 i3ResourceManager::GetLoadingProgress(void)
{
	if( m_MaxQueueFileCount == 0)
		return 100;

	return 100 * (m_MaxQueueFileCount - (INT32)m_QueueFileList.size()) / m_MaxQueueFileCount;
}


void i3ResourceManager::OnUpdate( REAL32 timeDelta)
{
	REAL32 tm = 0.0f;
	i3QueueFileInfo * pInfo;
#if defined( LOADTIME_DUMP)
	INT64 tmStart, tmEnd;
#endif

	while((m_QueueFileList.size() > 0) && ( tm < m_timeLoadInterval))
	{
		pInfo = m_QueueFileList[0];

		#if defined( LOADTIME_DUMP)
		tmStart = i3Timer::GetPerformanceTime();
		#endif

		switch( pInfo->GetType())
		{
			case i3QueueFileInfo::TYPE_SCENE :
				{
					i3GameResSceneGraph * pSGRes = (i3GameResSceneGraph *) pInfo->GetUserPointer();
					i3SceneGraphInfo * pNode;

					pNode = LoadModel( pInfo->GetPath(), false, false
				#if defined( I3_DEBUG)
						, nullptr, &pSGRes->m_FileSize
				#endif
						);

					if( pNode != nullptr)
					{
						char Path[256] = "";
						i3::to_lower_copy(Path, pInfo->GetPath() );

						if( i3::contain_string( Path, "_world.i3s" ) != -1 )
						{
							i3Node * pInstance = pNode->getInstanceSg();

							const i3::vector<i3Node*>& childList = pInstance->GetChildList();

							i3Transform* pTransform = (i3Transform*)childList[0];

							pTransform->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );
						}

						pSGRes->setSceneGraphInfo( pNode);
						if( pSGRes->decCompletionCount() <= 0)
						{
							AddResource( pSGRes);
						}
					}
					else
					{
						//I3PRINTLOG(I3LOG_NOTICE, "Could not load resource : %s", pInfo->GetPath());
					}
				}
				break;
			case i3QueueFileInfo::TYPE_CHARA :
				{
					i3GameResChara * pChar = (i3GameResChara *) pInfo->GetUserPointer();
					i3Chara * pChara;

					pChara = LoadCharactor( pInfo->GetPath(), nullptr, pInfo->GetUserParam2()?true:false, nullptr, false, true);
					I3ASSERT( pChara != nullptr);

					if (pChara)
					{
						pChar->setChara( pChara);
						pChar->setSceneGraphInfo( pChara->getSceneGraphInfo());

						if( pChar->decCompletionCount() <= 0)
						{
							AddResource( pChar);
						}

						I3_MUST_RELEASE( pChara);
					}
					else
					{
						I3_SAFE_ADDREF( pChar);
						I3_SAFE_RELEASE( pChar);
						I3PRINTLOG(I3LOG_NOTICE, "Could not load resource : %s", pInfo->GetPath());
					}
				}
				break;
			case i3QueueFileInfo::TYPE_WORLD :
				{
					i3GameResWorld * pWorldRes = (i3GameResWorld*)pInfo->GetUserPointer();
					i3World * pWorld;

					pWorld = LoadWorld( pInfo->GetPath(), 0, pInfo->GetUserParam2() != 0);
					I3ASSERT( pWorld != nullptr);

					if (pWorld)
					{
						i3SceneGraphInfo * pInfoSg = i3SceneGraphInfo::new_object_ref();
						pInfoSg->setInstanceSg( pWorld->GetNode());

						pWorldRes->setWorld( pWorld);
						pWorldRes->setSceneGraphInfo( pInfoSg);

						if( pWorldRes->decCompletionCount() <= 0)
						{
							AddResource( pWorldRes);
						}

						I3_MUST_RELEASE( pWorld);
					}
					else
					{
						I3PRINTLOG(I3LOG_NOTICE, "Could not load resource : %s", pInfo->GetPath());
					}
				}
				break;

			case i3QueueFileInfo::TYPE_ANIMATION :
				{
					i3Animation * pAnim;
					i3GameResAnimation * pAnimRes = (i3GameResAnimation *) pInfo->GetUserPointer();

					pAnim = LoadAnimation( pInfo->GetPath()
			#if defined( I3_DEBUG)
						, &pAnimRes->m_FileSize
			#endif
						);

					if( pAnim == nullptr)
					{
						i3Log( "i3Framework::LoadRSC()", "Could not load resource : %s\n", pAnimRes->GetName());
					}
					else
					{
						pAnimRes->setAnimation( pAnim);
						I3_MUST_RELEASE( pAnim);
					}

					AddResource( pAnimRes);
				}
				break;

			case i3QueueFileInfo::TYPE_TEXTURE :
				{
					i3Texture * pTex;
					i3GameResTexture * pTexRes = (i3GameResTexture *) pInfo->GetUserPointer();

					pTex = LoadTexture( pInfo->GetPath()
#if defined( I3_DEBUG)
						, false, &pTexRes->m_FileSize
#endif
						);
					if( pTex == nullptr)
					{
						i3Log( "i3Framework::LoadRSC()", "Could not load resource : %s\n", pTexRes->GetName());
					}
					else
					{
						pTexRes->setTexture( pTex);
						I3_MUST_RELEASE( pTex);
					}

					AddResource( pTexRes);
				}
				break;

			case i3QueueFileInfo::TYPE_PATH :
				{
					i3GamePath * pPath;
					i3GameResPath * pPathRes = (i3GameResPath *) pInfo->GetUserPointer();

					pPath = LoadPath( pInfo->GetPath());
					if( pPath == nullptr)
					{
						I3PRINTLOG(I3LOG_FATAL,  "Could not load path resource : %s", pPathRes->GetName());
					}
					else
					{
						pPathRes->setPath( pPath);
						I3_MUST_RELEASE( pPath);
					}

					AddResource( pPathRes);
				}
				break;

			case i3QueueFileInfo::TYPE_BINARY :
				{
					I3ASSERT_0;
					//LoadResource( pInfo->GetPath(), 0);
				}
				break;
			case i3QueueFileInfo::TYPE_LIGHT :
				{
					i3ResourceFile file;
					i3GameResLight * pResLight = (i3GameResLight *)pInfo->GetUserPointer();
					i3LightObject  * pLightObj = nullptr;

					if( file.Load( pInfo->GetPath()))
					{
						pLightObj = (i3LightObject *)file.getKeyObject();

						pResLight->setLightObject( pLightObj);

						AddResource( pResLight);

						//I3TRACE( "Load Light : %s\n", pInfo->GetPath());
					}
					else
					{
						I3_SAFE_RELEASE( pResLight);
					}
				}
				break;

			case i3QueueFileInfo::TYPE_ANIMATIONPACK:
				{
					LoadAnimationPack( pInfo->GetPath());
				}
				break;

			case i3QueueFileInfo::TYPE_SOUND :
				{
					i3SoundFile			file;
					i3GameResSound *	pSndRes = (i3GameResSound *) pInfo->GetUserPointer();

					i3NamedResourceManager * pMng = i3ResourceFile::GetResourceManager( i3Sound::static_meta());
					I3ASSERT( pMng != nullptr);

					i3Sound * pSound = (i3Sound*)pMng->GetResource(nullptr, i3ResourceFile::GetWorkingFolder(), pInfo->GetPath());
					if( pSound == nullptr)
					{
						I3PRINTLOG(I3LOG_WARN,  "%s sound file was not loaded.", pInfo->GetPath());

						if( pSndRes != nullptr)
						{// 로드하지 못한 리소스 GameRes는 삭제하자.
							I3_MUST_ADDREF(pSndRes);
							I3_MUST_RELEASE( pSndRes);
						}
					}
					else
					{
						i3SoundPlayInfo * pPlayInfo = pSndRes->getSound();

						pPlayInfo->setSound( pSound);

					#if defined( I3_DEBUG)
						pSndRes->m_FileSize = pSound->m_FileSize;
					#endif

						AddResource( pSndRes);

						//I3TRACE( "Load Sound : %s\n", pInfo->GetPath());
					}
				}
				break;
			case i3QueueFileInfo::TYPE_EVENT:
				{
					i3ResourceFile file;
					i3GameResEvent * pEvtRes = (i3GameResEvent*)pInfo->GetUserPointer();

					i3TimeEventGen * pEvt = nullptr;

					if( file.Load( pInfo->GetPath()))
					{
						pEvt = (i3TimeEventGen*)file.getKeyObject();

						pEvtRes->setTimeEvent( pEvt);

						AddResource( pEvtRes);

						//I3TRACE( "Load Event : %s\n", pInfo->GetPath());
					}
				}
				break;
			case i3QueueFileInfo::TYPE_OBJECT:
				{
					i3ResourceFile file;
					i3GameResObject * pResObj = (i3GameResObject *)pInfo->GetUserPointer();
					i3Object		* pObj	  = nullptr;

					if( file.Load( pInfo->GetPath()))
					{
						pObj = (i3Object *)file.getKeyObject();

						pResObj->SetGameObject( pObj);
						AddResource( pResObj);

						//I3TRACE( "Load Object : %s\n", pInfo->GetPath());
					}
					else
					{
						I3_SAFE_RELEASE( pResObj);
					}
				}
				break;
		}

		tm += m_timer.GetDeltaSec();

		#if defined( LOADTIME_DUMP)
		{
			tmEnd = i3Timer::GetPerformanceTime();

			REAL32 elp = (REAL32)((tmEnd - tmStart) * i3Timer::GetProfileResolution());

			if( elp > 50.0f)
			{
				I3TRACE( "[%0d%%] LOAD : %s : %.2f\n", GetLoadingProgress(), pInfo->GetPath(), elp);
			}
		}
		#endif

		I3_MUST_RELEASE( pInfo);
		m_QueueFileList.erase(m_QueueFileList.begin());
	};

	i3TextureResManager * pTexResManager = static_cast<i3TextureResManager*>(i3ResourceFile::GetResourceManager(i3Texture::static_meta()));
	I3ASSERT( pTexResManager != nullptr);
	pTexResManager->UpdateSplitLoadQue();
}

void i3ResourceManager::__AddBackgroundFile( const char * pszPath, i3QueueFileInfo::TYPE type, i3GameRes * pRes, INT32 param1, INT32 param2)
{
	char szName[ MAX_PATH];
	i3::extract_filename(pszPath, szName);
	i3::to_upper( szName);

	if( __FindBackgroundRes( szName) == nullptr)
	{
		tagBACKGROUNDRES_INFO * pInfo = (tagBACKGROUNDRES_INFO*) i3MemAlloc( sizeof( tagBACKGROUNDRES_INFO));

		pInfo->_type = type;
		pInfo->_pRes = pRes;
		I3_SAFE_ADDREF( pRes);
		pInfo->_param1 = param1;
		pInfo->_param2 = param2;

		m_BackgroundFileList.insert( i3::vector_map<std::string, tagBACKGROUNDRES_INFO*>::value_type( szName, pInfo));
	}
	else
	{	// 이미 등록된 리소스	삭제하자
		I3_SAFE_ADDREF( pRes);
		I3_SAFE_RELEASE( pRes);
	}
}

i3ResourceManager::tagBACKGROUNDRES_INFO * i3ResourceManager::__FindBackgroundRes( const char * pszName)
{
	i3::vector_map<std::string, tagBACKGROUNDRES_INFO*>::iterator it;
	it = m_BackgroundFileList.find( pszName);

	if( it != m_BackgroundFileList.end())
	{
		return (tagBACKGROUNDRES_INFO*) it->second;
	}

	return nullptr;
}

const char * i3ResourceManager::FindFullPathByOnlyName( const char * pszName)
{
	char szTemp[ MAX_PATH];

	i3::safe_string_copy( szTemp, pszName, sizeof( szTemp));
	i3::to_upper( szTemp);

	tagBACKGROUNDRES_INFO * pFind = __FindBackgroundRes( szTemp);

	if( pFind != nullptr && pFind->_pRes != nullptr)
	{
		return (const char*) pFind->_pRes->GetName();
	}
	
	// RSC List에 없더라도, ResChangeManager에서 검색되는 것들은 실제로 존재하는 파일이 아니라..치환이 될 것임...
	// 그런 것들은 그냥 full_path 대신 자기 자신을 리턴하도록 만든다..
	const char* pszNameFromChangeManager = GetFileName_ResChangeManager(pszName);
	
	if (pszNameFromChangeManager != nullptr)
		return pszNameFromChangeManager;

	i3GameRes * pRes = FindResourceByOnlyName( pszName);
	if( pRes != nullptr)
		return pRes->GetName();

	return nullptr;
}

i3GameRes * i3ResourceManager::FindBackgroundRes( const char * pszName)
{
	char szTemp[ MAX_PATH];
	
	i3::safe_string_copy( szTemp, pszName, sizeof( szTemp));
	i3::to_upper( szTemp);

	tagBACKGROUNDRES_INFO * pFind = __FindBackgroundRes( szTemp);

	if( pFind != nullptr)
	{
		return pFind->_pRes;
	}

	return nullptr;
}

i3GameRes * i3ResourceManager::LoadBackgroundRes( const char * pszName, bool bCommonLoad, bool bInThread)
{
	char szTemp[ MAX_PATH];
	
	i3::safe_string_copy( szTemp, pszName, sizeof( szTemp));
	i3::to_upper( szTemp);

	tagBACKGROUNDRES_INFO * pFind = __FindBackgroundRes( szTemp);

	if( pFind != nullptr)
	{
		return __LoadResource( (const char*) pFind->_pRes->GetName(), pFind->_pRes, 0, bCommonLoad, bInThread);
	}
	else
	{
		return __LoadResource( pszName, nullptr, 0, bCommonLoad, false, true);
	}
}

bool i3ResourceManager::isCommonLoadRes( char* szFilePath, bool isCommon)
{
	if(cbPreLoad != nullptr)
		return cbPreLoad(szFilePath, isCommon);
	return false;
}
