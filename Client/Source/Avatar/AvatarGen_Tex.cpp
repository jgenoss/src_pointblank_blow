#include "pch.h"
#include "AvatarGen.h"
#include "AvatarDefine.h"

#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/extract_filename.h"

#if defined( CLIENT_SIDE)
	#include "Avatar/AvatarManager.h"
#else
#undef MEMDUMP_NEW
#define MEMDUMP_NEW __noop
#endif

void AvatarGen::SearchTexture( void)
{
	for( INT32 part = 0; part < EQUIP::ePART_COUNT; part++)
	{
		for( INT32 view = AVATAR::VIEW_1P; view <= AVATAR::VIEW_3P; view++)
		{
			const AVATAR::TEXINFO * pTexInfo = AVATAR::getPartTexInfo( (AVATAR::VIEW) view, (EQUIP::ePART) part);

			if( pTexInfo->m_W <= 0)
				continue;

			i3SceneGraphInfo * pSgInfo = getSgInfo( (EQUIP::ePART) part, (AVATAR::VIEW) view);
			if( pSgInfo == nullptr)
			{
				if( part == EQUIP::ePART_CHARA)
					pSgInfo = m_pHeadSgInfo[0][1];	// 0번 타입의 1번 LOD를 대표로 사용

				if( pSgInfo == nullptr)
					continue;
			}

			I3_MUST_ADDREF(pSgInfo);

			// Scene Graph에 있는 Texture를 검색한다.
			// (w x h) 크기에 부합하는지 여부 역시 확인한다.
			
			i3::vector<i3Node*> list;
			list.reserve( 16);
			i3Scene::FindNodeByType( pSgInfo->getInstanceSg(), i3AttrSet::static_meta(), list);

			UINT32 cnt = list.size();
			for( UINT32 i = 0; i < cnt; i++)
			{
				i3AttrSet * pAttrSet = (i3AttrSet*) list.at( i);
				
				if( _CanMergeTexture( pAttrSet, m_bInLobby, hasSkinParts()) == false )
					continue;

				bool bDummyResource = pSgInfo->isCommonRes() && ((pSgInfo->getResStyle() & I3_RES_STYLE_PREPARED) == I3_RES_STYLE_PREPARED);

				i3TextureBindAttr * pAttr = (i3TextureBindAttr*) i3Scene::FindAttrByExactType( pAttrSet, i3TextureBindAttr::static_meta());
				I3ASSERT( pAttr != nullptr);

				if( pAttr->GetTexture() != nullptr && (pAttr->GetTexture()->GetPersistFormat() != I3G_IMAGE_FORMAT_NONE) )
				{
					// team pair texture 확인
					i3Texture * pSrc = pAttr->GetTexture();

					char szFullPath[ MAX_PATH];
					__MakeFullPath( szFullPath, pAttr->GetTexture()->GetName(), sizeof( szFullPath));
					i3Texture * pTeamTex = AVATAR::LoadTex( szFullPath, m_Team);
					if( pTeamTex != nullptr)
						pSrc = pTeamTex;

					if( FindSrcTextureFromList( m_DiffTexList, pSrc, pTexInfo) == -1)
					{
						tagSrcTextureInfo * pInfo = new tagSrcTextureInfo( pSrc, pTexInfo, bDummyResource );
						MEMDUMP_NEW( pInfo, sizeof(tagSrcTextureInfo));
						m_DiffTexList.push_back( pInfo);
					}
				}

				if( m_bEnableTexture[ AVATAR::TEX_NORM] )
				{
					pAttr = (i3TextureBindAttr*) i3Scene::FindAttrByExactType( pAttrSet, i3NormalMapBindAttr::static_meta());
					if( pAttr != nullptr && pAttr->GetTexture() != nullptr
						&& (pAttr->GetTexture()->GetPersistFormat() != I3G_IMAGE_FORMAT_NONE) )
					{
						// team pair texture 확인
						i3Texture * pSrc = pAttr->GetTexture();
						char szFullPath[ MAX_PATH];
						__MakeFullPath( szFullPath, pAttr->GetTexture()->GetName(), sizeof( szFullPath));
						i3Texture * pTeamTex = AVATAR::LoadTex( szFullPath, m_Team);
						if( pTeamTex != nullptr)
							pSrc = pTeamTex;

						if( FindSrcTextureFromList( m_NormTexList, pSrc, pTexInfo) == -1)
						{
							tagSrcTextureInfo * pInfo = new tagSrcTextureInfo( pSrc, pTexInfo, bDummyResource);
							MEMDUMP_NEW( pInfo, sizeof(tagSrcTextureInfo));
							m_NormTexList.push_back( pInfo);
						}
					}
				}

				if( m_bEnableTexture[ AVATAR::TEX_SPEC] )
				{
					pAttr = (i3TextureBindAttr*) i3Scene::FindAttrByExactType( pAttrSet, i3SpecularMapBindAttr::static_meta());
					if( pAttr != nullptr && pAttr->GetTexture() != nullptr
						&& (pAttr->GetTexture()->GetPersistFormat() != I3G_IMAGE_FORMAT_NONE) )
					{
						// team pair texture 확인
						i3Texture * pSrc = pAttr->GetTexture();
						char szFullPath[ MAX_PATH];
						__MakeFullPath( szFullPath, pAttr->GetTexture()->GetName(), sizeof( szFullPath));
						i3Texture * pTeamTex = AVATAR::LoadTex( szFullPath, m_Team);
						if( pTeamTex != nullptr)
							pSrc = pTeamTex;

						if( FindSrcTextureFromList( m_SpecTexList, pSrc, pTexInfo) == -1)
						{
							tagSrcTextureInfo * pInfo = new tagSrcTextureInfo( pSrc, pTexInfo, bDummyResource);
							MEMDUMP_NEW( pInfo, sizeof(tagSrcTextureInfo));
							m_SpecTexList.push_back( pInfo);
						}
					}
				}

				pAttr = (i3TextureBindAttr*) i3Scene::FindAttrByExactType( pAttrSet, i3ReflectMaskMapBindAttr::static_meta());
				if( pAttr != nullptr && pAttr->GetTexture() != nullptr
					&& (pAttr->GetTexture()->GetPersistFormat() != I3G_IMAGE_FORMAT_NONE) )
				{
					// team pair texture 확인
					i3Texture * pSrc = pAttr->GetTexture();
					char szFullPath[ MAX_PATH];
					__MakeFullPath( szFullPath, pAttr->GetTexture()->GetName(), sizeof( szFullPath));
					i3Texture * pTeamTex = AVATAR::LoadTex( szFullPath, m_Team);
					if( pTeamTex != nullptr)
						pSrc = pTeamTex;

					if( FindSrcTextureFromList( m_RefMTexList, pSrc, pTexInfo) == -1)
					{
						tagSrcTextureInfo * pInfo = new tagSrcTextureInfo( pSrc, pTexInfo, bDummyResource);
						MEMDUMP_NEW( pInfo, sizeof(tagSrcTextureInfo));
						m_RefMTexList.push_back( pInfo);
					}
				}
			}

			I3_MUST_RELEASE(pSgInfo);
		}
	}
}

INT32 AvatarGen::FindSrcTextureFromList( i3::vector<tagSrcTextureInfo*> & list, const i3Texture * pSrcTex, INT32 x, INT32 y, INT32 w, INT32 h)
{
	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		if( (list.at( i)->_pTex == pSrcTex) && (list.at(i)->_pInfo->m_X == x) && (list.at(i)->_pInfo->m_Y == y)
			&& (list.at(i)->_pInfo->m_W == w) && (list.at(i)->_pInfo->m_H == h))
		{
			return i;
		}
	}

	return -1;
}

INT32 AvatarGen::FindSrcTextureFromList( i3::vector<tagSrcTextureInfo*> & list, const i3Texture * pSrcTex, const AVATAR::TEXINFO * pInfo)
{
	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		if( (list.at( i)->_pTex == pSrcTex)
			&& (list.at(i)->_pInfo == pInfo) )
		{
			return i;
		}
	}

	return -1;
}

bool AvatarGen::FinishLoadMacroTexture( const i3Texture * pDestTex, const i3Texture * pSrcTex, const AVATAR::TEXINFO * pTexInfo)
{
	{
		i3Monitor2 mon( &m_Mutex);

		if( m_pTex_Diff[ TEX_LOAD_HIGH] == pDestTex)
		{
			INT32 find = FindSrcTextureFromList( m_DiffTexList, pSrcTex, pTexInfo);
			if( find != -1)
			{
				tagSrcTextureInfo * pInfo = m_DiffTexList.at( find);
				I3_SAFE_DELETE( pInfo);
				m_DiffTexList.erase( m_DiffTexList.begin() + find);
			}
		}
		else if( m_pTex_Norm[ TEX_LOAD_HIGH] == pDestTex)
		{
			INT32 find = FindSrcTextureFromList( m_NormTexList, pSrcTex, pTexInfo);
			if( find != -1)
			{
				tagSrcTextureInfo * pInfo = m_NormTexList.at( find);
				I3_SAFE_DELETE( pInfo);
				m_NormTexList.erase( m_NormTexList.begin() + find);
			}
		}
		else if( m_pTex_Spec[ TEX_LOAD_HIGH] == pDestTex)
		{
			INT32 find = FindSrcTextureFromList( m_SpecTexList, pSrcTex, pTexInfo);
			if( find != -1)
			{
				tagSrcTextureInfo * pInfo = m_SpecTexList.at( find);
				I3_SAFE_DELETE( pInfo);
				m_SpecTexList.erase( m_SpecTexList.begin() + find);
			}
		}
		else if( m_pTex_RefMask[ TEX_LOAD_HIGH] == pDestTex)
		{
			INT32 find = FindSrcTextureFromList( m_RefMTexList, pSrcTex, pTexInfo);
			if( find != -1)
			{
				tagSrcTextureInfo * pInfo = m_RefMTexList.at( find);
				I3_SAFE_DELETE( pInfo);
				m_RefMTexList.erase( m_RefMTexList.begin() + find);
			}
		}
	}

	if( (m_DiffTexList.size() == 0) && (m_NormTexList.size() == 0) && (m_SpecTexList.size() == 0) && (m_RefMTexList.size() == 0) )
	{
		while( !IsGenComplate() )
		{
			i3Timer::Sleep( 10);
		}

		// All Loaded
		// BoneRef에 있는 것을 잠시 띄어내야 한다.
		i3::vector<i3Node*> list;
		i3::vector<i3Node*> parentlist;
		i3Scene::FindNodeByExactType( m_pBody, i3BoneRef::static_meta(), list);

		parentlist.resize( list.size());

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3Node * pNode = list.at(i);
			I3_MUST_ADDREF( pNode);
			parentlist[i] = pNode->GetParent();
			if( parentlist[i] != nullptr)
				parentlist[i]->RemoveChild( pNode);
		}

		BindMacroTexture( m_pBody, TEX_LOAD_HIGH, true);

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3Node * pNode = list.at(i);
			if( parentlist[i] != nullptr)
				parentlist[i]->AddChild( pNode);
			I3_MUST_RELEASE( pNode);
		}
#if 0
		i3ImageFile file;
		i3::string str;
		static int sss = 0;
		i3::sprintf( str, "E:/diff%d.i3i", sss);
		file.Save( str.c_str(), m_pTex_Diff[ TEX_LOAD_HIGH]);

		if( m_pTex_Norm[ TEX_LOAD_HIGH] != nullptr)
		{
			i3::sprintf( str, "E:/norm%d.i3i", sss);
			file.Save( str.c_str(), m_pTex_Norm[ TEX_LOAD_HIGH]);
		}

		if( m_pTex_Spec[ TEX_LOAD_HIGH] != nullptr)
		{
			i3::sprintf( str, "E:/spec%d.i3i", sss);
			file.Save( str.c_str(), m_pTex_Spec[ TEX_LOAD_HIGH]);
		}

		if( m_pTex_RefMask[ TEX_LOAD_HIGH] != nullptr)
		{
			i3::sprintf( str, "E:/refm%d.i3i", sss);
			file.Save( str.c_str(), m_pTex_RefMask[ TEX_LOAD_HIGH]);
		}

		sss++;
#endif

		I3TRACE( "Load Complage MacroTexture %s\n", pDestTex->GetName());

		return true;
	}

	return false;
}

void AvatarGen::BindMacroTexture( i3Node * pRoot, TEX_LOAD_LEVEL lv, bool bReleasePrev)
{
	i3::vector<i3Node*> list;
	i3Scene::FindNodeByExactType( pRoot, i3AttrSet::static_meta(), list);

	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		i3AttrSet * pAttrSet = static_cast<i3AttrSet*>( list.at(i));
		if( _CanMergeTexture( pAttrSet, m_bInLobby, hasSkinParts()) == false)
			continue;

		i3TextureBindAttr * pAttr = static_cast<i3TextureBindAttr*>( i3Scene::FindAttrByExactType(pAttrSet, i3TextureBindAttr::static_meta()) );
		if( pAttr != nullptr)
		{
			if( bReleasePrev && pAttr->GetTexture() != nullptr)
			{
				pAttr->GetTexture()->setCommonRes(false);
				I3_SAFE_RELEASE( m_pTex_Diff[ TEX_LOAD_LOW]);

				I3ASSERT( lv != TEX_LOAD_LOW);
			}
			pAttr->SetTexture( m_pTex_Diff[ lv]);
			m_pTex_Diff[ lv]->setThreadMode( false);
		}

		pAttr = static_cast<i3TextureBindAttr*>( i3Scene::FindAttrByExactType(pAttrSet, i3NormalMapBindAttr::static_meta()) );
		if( pAttr != nullptr)
		{
			if( bReleasePrev && pAttr->GetTexture() != nullptr)
			{
				pAttr->GetTexture()->setCommonRes(false);
				I3_SAFE_RELEASE( m_pTex_Norm[ TEX_LOAD_LOW]);
			}
			pAttr->SetTexture( m_pTex_Norm[ lv]);
			if( m_pTex_Norm[ lv] != nullptr)
				m_pTex_Norm[ lv]->setThreadMode( false);
		}

		pAttr = static_cast<i3TextureBindAttr*>( i3Scene::FindAttrByExactType(pAttrSet, i3SpecularMapBindAttr::static_meta()) );
		if( pAttr != nullptr)
		{
			if( bReleasePrev && pAttr->GetTexture() != nullptr)
			{
				pAttr->GetTexture()->setCommonRes(false);
				I3_SAFE_RELEASE( m_pTex_Spec[ TEX_LOAD_LOW]);
			}
			pAttr->SetTexture( m_pTex_Spec[ lv]);
			if( m_pTex_Spec[ lv] != nullptr)
				m_pTex_Spec[ lv]->setThreadMode( false);
		}

		pAttr = static_cast<i3TextureBindAttr*>( i3Scene::FindAttrByExactType(pAttrSet, i3ReflectMaskMapBindAttr::static_meta()) );
		if( pAttr != nullptr)
		{
			if( bReleasePrev && pAttr->GetTexture() != nullptr)
			{
				pAttr->GetTexture()->setCommonRes(false);
				I3_SAFE_RELEASE( m_pTex_RefMask[ TEX_LOAD_LOW]);
			}
			pAttr->SetTexture( m_pTex_RefMask[ lv]);
			if( m_pTex_RefMask[ lv] != nullptr)
				m_pTex_RefMask[ lv]->setThreadMode( false);
		}
	}
}


void AvatarGen::_BuildMacroTex( bool bRebuild)
{
	// 7~8초
	if( bRebuild)
	{	// 다시 생성되야 한다.
		for( UINT32 i = 0; i < TEX_LOAD_LEVEL_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_pTex_Diff[i]);
			I3_SAFE_RELEASE( m_pTex_Norm[i]);
			I3_SAFE_RELEASE( m_pTex_Spec[i]);
			I3_SAFE_RELEASE( m_pTex_RefMask[i]);
		}
	}

	INT32 lv = 4;
	I3G_IMAGE_FORMAT imageFormat = I3G_IMAGE_FORMAT_DXT5;
	INT32 texQual = m_TextureQuality;
	
	switch( texQual)
	{
	case 3 :	imageFormat = I3G_IMAGE_FORMAT_DXT1;	lv = 1;		break;
	case 2 :	imageFormat = I3G_IMAGE_FORMAT_DXT1;	lv = 2;		break;
	case 1 :	imageFormat = I3G_IMAGE_FORMAT_DXT5;	lv = 3;		break;
	default :
		texQual = 0;
		break;
	}

	INT32 qual = (1<<texQual);			// qual은 0인게 불가능..기타 어떤 조건이라도 0,1,2,3의 쉬프트 연산을 못벗어나므로 관련 바운드체크 제거.

	INT32 scale = 1 << (lv - 1);

	// 공통 Texture로 사용될 것들을 생성
	if( m_pTex_Diff[ TEX_LOAD_HIGH] == nullptr )
	{
		// Resource에서 Src용 texture들을 골라낸다.
		SearchTexture();

#if defined( CLIENT_SIDE)
		m_pTex_Diff[TEX_LOAD_LOW] = i3Texture::new_object();
		I3ASSERT( m_pTex_Diff[TEX_LOAD_LOW] != nullptr);
		((i3TextureDX*)m_pTex_Diff[TEX_LOAD_LOW])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Diff[TEX_LOAD_LOW]->SetName( "MacroTex_Diff_Low");
	#endif
		m_pTex_Diff[TEX_LOAD_LOW]->Create( AVT_TEX_WIDTH / qual / scale, AVT_TEX_HEIGHT / qual / scale, 1, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Diff[TEX_LOAD_LOW]->setRestoreType( I3G_RESTORE_MEM);
#endif

		m_pTex_Diff[TEX_LOAD_HIGH] = i3Texture::new_object();
		I3ASSERT( m_pTex_Diff[TEX_LOAD_HIGH] != nullptr);
		((i3TextureDX*)m_pTex_Diff[TEX_LOAD_HIGH])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Diff[TEX_LOAD_HIGH]->SetName( "MacroTex_Diff_High");
	#endif
		m_pTex_Diff[TEX_LOAD_HIGH]->Create( AVT_TEX_WIDTH / qual, AVT_TEX_HEIGHT / qual, lv, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Diff[TEX_LOAD_HIGH]->setRestoreType( I3G_RESTORE_MEM);

		if( TransTex( m_pTex_Diff[TEX_LOAD_LOW], m_pTex_Diff[TEX_LOAD_HIGH], m_DiffTexList, qual) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "do not craete avatar diffuse texture.");
		}

		if( scale == 1)
		{
			I3_REF_CHANGE( m_pTex_Diff[TEX_LOAD_HIGH], m_pTex_Diff[TEX_LOAD_LOW]);
		}
	}

	if( (m_pTex_Norm[ TEX_LOAD_HIGH] == nullptr) && (m_NormTexList.size() > 0) )
	{
#if defined( CLIENT_SIDE)
		m_pTex_Norm[ TEX_LOAD_LOW] = i3Texture::new_object();
		I3ASSERT( m_pTex_Norm[ TEX_LOAD_LOW] != nullptr);
		((i3TextureDX*)m_pTex_Norm[ TEX_LOAD_LOW])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Norm[TEX_LOAD_LOW]->SetName( "MacroTex_Norm_Low");
	#endif
		m_pTex_Norm[ TEX_LOAD_LOW]->Create( AVT_TEX_WIDTH / qual / scale, AVT_TEX_HEIGHT / qual / scale, 1, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Norm[ TEX_LOAD_LOW]->setRestoreType( I3G_RESTORE_MEM);
#endif

		m_pTex_Norm[ TEX_LOAD_HIGH] = i3Texture::new_object();
		I3ASSERT( m_pTex_Norm[ TEX_LOAD_HIGH] != nullptr);
		((i3TextureDX*)m_pTex_Norm[ TEX_LOAD_HIGH])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Norm[TEX_LOAD_HIGH]->SetName( "MacroTex_Norm_High");
	#endif
		m_pTex_Norm[ TEX_LOAD_HIGH]->Create( AVT_TEX_WIDTH / qual, AVT_TEX_HEIGHT / qual, lv, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Norm[ TEX_LOAD_HIGH]->setRestoreType( I3G_RESTORE_MEM);

		if( TransTex( m_pTex_Norm[ TEX_LOAD_LOW], m_pTex_Norm[ TEX_LOAD_HIGH], m_NormTexList, qual) == false)
		{	// 사용하지 않음.
			I3_SAFE_RELEASE( m_pTex_Norm[ TEX_LOAD_LOW]);
			I3_SAFE_RELEASE( m_pTex_Norm[ TEX_LOAD_HIGH]);
		}

		if( scale == 1)
		{
			I3_REF_CHANGE( m_pTex_Norm[TEX_LOAD_HIGH], m_pTex_Norm[TEX_LOAD_LOW]);
		}
	}

	if( (m_pTex_Spec[ TEX_LOAD_HIGH] == nullptr) && (m_SpecTexList.size() > 0) )
	{
#if defined( CLIENT_SIDE)
		m_pTex_Spec[ TEX_LOAD_LOW] = i3Texture::new_object();
		I3ASSERT( m_pTex_Spec[ TEX_LOAD_LOW] != nullptr);
		((i3TextureDX*)m_pTex_Spec[ TEX_LOAD_LOW])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Spec[ TEX_LOAD_LOW]->SetName( "MacroTex_Spec_Low");
	#endif
		m_pTex_Spec[ TEX_LOAD_LOW]->Create( AVT_TEX_WIDTH / qual / (scale * 2), AVT_TEX_HEIGHT / qual / (scale * 2), 1, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Spec[ TEX_LOAD_LOW]->setRestoreType( I3G_RESTORE_MEM);
#endif

		m_pTex_Spec[ TEX_LOAD_HIGH] = i3Texture::new_object();
		I3ASSERT( m_pTex_Spec[ TEX_LOAD_HIGH] != nullptr);
		((i3TextureDX*)m_pTex_Spec[ TEX_LOAD_HIGH])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_Spec[TEX_LOAD_HIGH]->SetName( "MacroTex_Spec_High");
	#endif
		m_pTex_Spec[ TEX_LOAD_HIGH]->Create( AVT_TEX_WIDTH / qual / 2, AVT_TEX_HEIGHT / qual / 2, lv, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_Spec[ TEX_LOAD_HIGH]->setRestoreType( I3G_RESTORE_MEM);

		if( TransTex( m_pTex_Spec[ TEX_LOAD_LOW], m_pTex_Spec[ TEX_LOAD_HIGH], m_SpecTexList, qual * 2) == false)
		{	// 사용하지 않음
			I3_SAFE_RELEASE( m_pTex_Spec[ TEX_LOAD_LOW]);
			I3_SAFE_RELEASE( m_pTex_Spec[ TEX_LOAD_HIGH]);
		}

		if( scale == 1)
		{
			I3_REF_CHANGE( m_pTex_Spec[TEX_LOAD_HIGH], m_pTex_Spec[TEX_LOAD_LOW]);
		}
	}

	if( (m_pTex_RefMask[ TEX_LOAD_HIGH] == nullptr) && (m_RefMTexList.size() > 0) )
	{
#if defined( CLIENT_SIDE)
		m_pTex_RefMask[ TEX_LOAD_LOW] = i3Texture::new_object();
		I3ASSERT( m_pTex_RefMask[ TEX_LOAD_LOW] != nullptr);
		((i3TextureDX*)m_pTex_RefMask[ TEX_LOAD_LOW])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_RefMask[ TEX_LOAD_LOW]->SetName( "MacroTex_RefM_Low");
	#endif
		m_pTex_RefMask[ TEX_LOAD_LOW]->Create( AVT_TEX_WIDTH / qual / (scale * 2), AVT_TEX_HEIGHT / qual / (scale * 2), 1, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_RefMask[ TEX_LOAD_LOW]->setRestoreType( I3G_RESTORE_MEM);
#endif

		m_pTex_RefMask[ TEX_LOAD_HIGH] = i3Texture::new_object();
		I3ASSERT( m_pTex_RefMask[ TEX_LOAD_HIGH] != nullptr);
		((i3TextureDX*)m_pTex_RefMask[ TEX_LOAD_HIGH])->setThreadMode( m_bInThread);
	#if defined( I3_DEBUG)
		m_pTex_RefMask[ TEX_LOAD_HIGH]->SetName( "MacroTex_RefM_High");
	#endif
		m_pTex_RefMask[ TEX_LOAD_HIGH]->Create( AVT_TEX_WIDTH / qual / 2, AVT_TEX_HEIGHT / qual / 2, lv, imageFormat, I3G_USAGE_RESTORE_MEM);
		m_pTex_RefMask[ TEX_LOAD_HIGH]->setRestoreType( I3G_RESTORE_MEM);

		if( TransTex( m_pTex_RefMask[ TEX_LOAD_LOW], m_pTex_RefMask[ TEX_LOAD_HIGH], m_RefMTexList, qual * 2) == false)
		{
			I3_SAFE_RELEASE( m_pTex_RefMask[ TEX_LOAD_LOW]);
			I3_SAFE_RELEASE( m_pTex_RefMask[ TEX_LOAD_HIGH]);
		}

		if( scale == 1)
		{
			I3_REF_CHANGE( m_pTex_RefMask[TEX_LOAD_HIGH], m_pTex_RefMask[TEX_LOAD_LOW]);
		}
	}

#if !defined( CLIENT_SIDE)
	// flush src texture list
	for( UINT32 i = 0; i < m_DiffTexList.size(); i++)
	{
		I3_SAFE_DELETE( m_DiffTexList.at(i));
	}
	m_DiffTexList.clear();

	for( UINT32 i = 0; i < m_NormTexList.size(); i++)
	{
		I3_SAFE_DELETE( m_NormTexList.at(i));
	}
	m_NormTexList.clear();

	for( UINT32 i = 0; i < m_SpecTexList.size(); i++)
	{
		I3_SAFE_DELETE( m_SpecTexList.at(i));
	}
	m_SpecTexList.clear();

	for( UINT32 i = 0; i < m_RefMTexList.size(); i++)
	{
		I3_SAFE_DELETE( m_RefMTexList.at(i));
	}
	m_RefMTexList.clear();
#endif
}

i3Texture * AvatarGen::LoadTex( const char * pszTexName)
{
	return AVATAR::LoadTex( pszTexName, m_Team);
}

bool AvatarGen::TransTex( i3Texture * pDestTex, i3Texture * pDestTex2, i3::vector<tagSrcTextureInfo*> &list, INT32 scaleFactor)
{
	// FinishLoadMacroTexture 를 호출시 이 List에서 삭제될 수 있다.
	// list를 다 push하도록 mutex를 건다.
	i3Monitor2 mon( &m_Mutex);

	UINT32 cnt = list.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		const AVATAR::TEXINFO * pTexInfo = list.at(i)->_pInfo;

		INT32 x = pTexInfo->m_X / scaleFactor;
		INT32 y = pTexInfo->m_Y / scaleFactor;
		INT32 w = pTexInfo->m_W / scaleFactor;
		INT32 h = pTexInfo->m_H / scaleFactor;

		i3Texture * pSrcTex = list.at(i)->_pTex;

		INT32 lv = MIN( pDestTex2->GetLevelCount(), pSrcTex->GetLevelCount()) - 1;

		INT32 x1 = x;
		INT32 y1 = y;
		INT32 w1 = w;
		INT32 h1 = h;
		INT32 depth = pDestTex2->GetLevelCount() - pSrcTex->GetLevelCount();
		while( depth > 0)
		{
			x1 = x1 >> 1;
			y1 = y1 >> 1;
			w1 = w1 >> 1;
			h1 = h1 >> 1;
			depth--;
		}

#if defined( CLIENT_SIDE)
		i3Texture * pSrcTex1 = pSrcTex;
		if( pSrcTex->GetFullLoadedTexture() != nullptr)
			pSrcTex1 = pSrcTex->GetFullLoadedTexture();
		#if defined( I3_DEBUG)
			if( i3::generic_string_size( pDestTex->GetName()) == 0)
			{
				i3::string str;
				char szPath[ MAX_PATH];
				char szFileName[ MAX_PATH];
				i3::extract_directoryname_filename(	pSrcTex->GetName(), szPath, szFileName);
				i3::extract_filetitle(szFileName, szFileName);
				str = szFileName;
				str += "_CreateLow";
				pDestTex->SetName( str);
			}
		#endif

		if( CopyTex( pSrcTex1, pDestTex, x1, y1, w1, h1, lv, list.at(i)->_bUseMacroTexture) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "what the~~~");
			return false;
		}

		if( pDestTex->GetWidth() == pDestTex2->GetWidth() &&
			pDestTex->GetHeight() == pDestTex2->GetHeight())
		{
		}
		else
		{
			// 파일 로더와 연결시켜서 처리해야 한다.
			// 파일이 완전히 로딩될때까지 que되어 있는 명령은 대기상태가 되야한다.
			if( pDestTex2->GetLevelCount() > 1)
			{
			#if defined( I3_DEBUG)
				if( i3::generic_string_size( pDestTex2->GetName()) == 0)
				{
					i3::string str;
					char szPath[ MAX_PATH];
					char szFileName[ MAX_PATH];
					i3::extract_directoryname_filename(	pSrcTex->GetName(), szPath, szFileName);
					i3::extract_filetitle(szFileName, szFileName);
					str = szFileName;
					str += "_CreateHigh";
					pDestTex2->SetName( str);
				}
			#endif
				g_pCharaManager->GetAvatarManager()->LoadMacroTexQue( this, pSrcTex, pDestTex2, x, y, w, h, m_bInThread, list.at(i)->_bUseMacroTexture, pTexInfo);
			}
		}
#else

		if( CopyTex( list.at(i)->_pTex, pDestTex2, x, y, w, h, 0, list.at(i)->_bUseMacroTexture) == false)
		{
			return false;
		}
#endif
	}

	return true;
}


bool AvatarGen::CopyTex( i3TextureBindAttr * pAttr, i3Texture * pTargetTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 lv, bool bUseMacroTexture)
{
	if (!CopyTex( pAttr->GetTexture(), pTargetTex, x, y, w, h, lv, bUseMacroTexture))
		return false;
	
	return true;
}

INT32 AvatarGen::GetMinMapCount( INT32 w, INT32 h)
{
	INT32 mipcount = 0;

	while(( w >= 32) && ( h >= 32))
	{
		mipcount++;
		w = w >> 1;
		h = h >> 1;
	}

	return min( MAX_MIPMAP_LEVEL, mipcount);
}

bool AvatarGen::CopyTex( i3Texture * pSrcTex, i3Texture * pTargetTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 lv, bool bUseMacroTexture)
{	
	if( pSrcTex == nullptr)
		return false;

	i3Texture * pAlterTex;

	pAlterTex = i3Texture::new_object();
#if defined( I3_DEBUG)
	pAlterTex->SetName( "AvatarAlter");
#endif
	pAlterTex->setRestoreType( I3G_RESTORE_NONE);

	INT32 scale = 1 << lv;

#if defined( CLIENT_SIDE)
	pAlterTex->setThreadMode( m_bInThread);
	if( bUseMacroTexture)
	{	// Copy From Macrotexture
		pAlterTex->Create( w / scale, h / scale, 1, pSrcTex->GetPersistFormat(), pSrcTex->GetUsageFlag() );
	}
	else
	{
		pAlterTex->Create( pSrcTex->GetWidth() / scale, pSrcTex->GetHeight() / scale, 1, pSrcTex->GetPersistFormat(), pSrcTex->GetUsageFlag() );
	}
	pAlterTex->setThreadMode( false);

	pAlterTex->Lock( 0, 0);
	pSrcTex->SafeLock( lv, I3G_LOCK_READONLY);

	if( bUseMacroTexture)
	{
		cAvatarManager::CopyTexture( pAlterTex, pSrcTex, 0, lv, x, y, w, h);
	}
	else
	{
		I3ASSERT( pAlterTex->GetDataSize(0) == pSrcTex->GetDataSize( lv));
		i3mem::Copy( pAlterTex->GetLockedBuffer(0), pSrcTex->GetLockedBuffer( lv), pSrcTex->GetDataSize( lv));
	}

	pSrcTex->SafeUnlock( lv);
	pAlterTex->Unlock(0);

	CopySurface( pTargetTex, pAlterTex, x, y, w, h, scale);
#else
	{
		pAlterTex->Create( pSrcTex->GetWidth(), pSrcTex->GetHeight(), pSrcTex->GetLevelCount(), pSrcTex->GetPersistFormat(), pSrcTex->GetUsageFlag() );
		for( UINT32 l = 0; l < pSrcTex->GetLevelCount(); l++)
		{
			pAlterTex->Lock( l, 0);
			pSrcTex->Lock( l, I3G_LOCK_READONLY);

			I3ASSERT( pAlterTex->GetDataSize(l) == pSrcTex->GetDataSize( l));
			i3mem::Copy( pAlterTex->GetLockedBuffer( l), pSrcTex->GetLockedBuffer( l), pSrcTex->GetDataSize( l));

			pSrcTex->Unlock( l);
			pAlterTex->Unlock(l);
		}
	}

	CopySurface( pTargetTex, pAlterTex, x, y, w, h, 1);

	// Mipmap
	IDirect3DTexture9 * pDXTex = ((i3TextureDX *)pTargetTex)->GetDXImage();
	if( pDXTex != nullptr)
		pDXTex->AddDirtyRect( nullptr);
#endif

	I3_SAFE_RELEASE( pAlterTex);
	
	return true;
}

void AvatarGen::CopySurface( i3Texture * pTargetTex , i3Texture * pSrcTex, INT32 x, INT32 y, INT32 w, INT32 h, INT32 scale)
{
	IDirect3DSurface9 * pDestSurf, * pSrcSurf;
	RECT destRT;
	
	INT32 j = 0;
	const INT32 cntSurface = pTargetTex->getSurfaceCount();
	for( INT32 i = 0; i < cntSurface; i++)
	{
		if( j >= pSrcTex->getSurfaceCount())
			break;
	
		INT32 width = (w / (1<<i)) / scale;
		INT32 height = (h / (1<<i)) / scale;

		destRT.left = x / (1<<i) / scale;
		destRT.top = y / (1<<i) / scale;
		destRT.right = destRT.left + width;
		destRT.bottom = destRT.top + height;

		I3ASSERT( destRT.right <= (INT32) pTargetTex->GetWidth());
		I3ASSERT( destRT.bottom <= (INT32) pTargetTex->GetHeight());

		pDestSurf = pTargetTex->getSurface( i)->getSurface();
		pSrcSurf = pSrcTex->getSurface( j)->getSurface();

		INT32 cnt = 0;
		while( pDestSurf == nullptr)
		{	// ResetDevice시 Surface가 해제되었을 수 있다. 기둘~
			i3Timer::Sleep( 1);	// 잠시 기둘
			pDestSurf = pTargetTex->getSurface( i)->getSurface();
			cnt++;
			if( cnt>1000)
			{
				I3PRINTLOG(I3LOG_FATAL,  "get Dest Surface fail!!!!!");
			}
		}

		cnt = 0;
		while( pSrcSurf == nullptr)
		{
			i3Timer::Sleep( 1);	// 잠시 기둘
			pSrcSurf = pSrcTex->getSurface( j)->getSurface();
			cnt++;
			if( cnt>1000)
			{
				I3PRINTLOG(I3LOG_FATAL,  "get Src surface fail!!!!!");
			}
		}

		// 32ms 많이 느림...
		HRESULT Rv = ::D3DXLoadSurfaceFromSurface( pDestSurf, NULL, &destRT,
												pSrcSurf, NULL, NULL,
												D3DX_FILTER_LINEAR,
												0);

		if( FAILED( Rv))
		{
			WCHAR wstrDesc[MAX_STRING_COUNT];
			DXGetErrorDescription(Rv, wstrDesc, MAX_STRING_COUNT);
			I3PRINTLOG(I3LOG_FATAL,  "Texture를 Macro Texture에 포함할 수 없습니다. %s", wstrDesc);
		}

		j++;
	}
}

void AvatarGen::SaveMacroTex( const char * pszPath)
{
	char szFullPath[ MAX_PATH];
	char szFileName[ MAX_PATH];
	char szPath[ MAX_PATH]; 

	i3ImageFile file;

	i3::extract_directoryname_filename(	pszPath, szPath, szFileName);
	i3::extract_filetitle(szFileName, szFileName);

//
	if( m_pTex_Diff[ TEX_LOAD_HIGH] != nullptr)
	{
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Diff.i3i", szPath, szFileName);
		file.SaveI3I( szFullPath, m_pTex_Diff[ TEX_LOAD_HIGH] );
	}

	if( m_pTex_Norm[ TEX_LOAD_HIGH] != nullptr)
	{
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Norm.i3i", szPath, szFileName);
		file.SaveI3I( szFullPath, m_pTex_Norm[ TEX_LOAD_HIGH]);
	}

	if( m_pTex_Spec[ TEX_LOAD_HIGH] != nullptr)
	{
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Spec.i3i", szPath, szFileName);
		file.SaveI3I( szFullPath, m_pTex_Spec[ TEX_LOAD_HIGH]);
	}

	if( m_pTex_RefMask[ TEX_LOAD_HIGH] != nullptr)
	{
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_RefM.i3i", szPath, szFileName);
		file.SaveI3I( szFullPath, m_pTex_RefMask[ TEX_LOAD_HIGH]);
	}
}

void AvatarGen::_PreloadMacroTex( void)
{
	char szPath[ MAX_PATH];
	char szFullPath[ MAX_PATH];
	char szFileName[ MAX_PATH];
	char szTeam[ 32];

	i3::generic_string_ncopy(szFileName, m_strName[EQUIP::ePART_CHARA], sizeof( szFileName) );
	szFileName[2] = 'F';

	i3::snprintf(szPath, sizeof(szPath), "Avatar/Mask/%s", m_strName[EQUIP::ePART_CHARA]);
	I3_SAFE_RELEASE( m_pTex_Diff[ TEX_LOAD_HIGH]);

	if( m_Team == TEAM_RED)
		i3::generic_string_ncopy( szTeam, "_Red", sizeof( szTeam));
	else
		i3::generic_string_ncopy( szTeam, "_Blue", sizeof( szTeam));

	i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Diff%s.i3i", szPath, szFileName, szTeam);

	m_pTex_Diff[ TEX_LOAD_HIGH] = g_pFramework->GetResourceManager()->LoadTexture( szFullPath);
	m_pTex_Diff[ TEX_LOAD_HIGH]->setRestoreType(I3G_RESTORE_FILE);
#if defined( I3_DEBUG)
	m_pTex_Diff[ TEX_LOAD_HIGH]->SetName( szFullPath);
#endif

#if defined( CLIENT_SIDE)
	if( m_bEnableTexture[ NORMAL] )
#endif
	{
		I3_SAFE_RELEASE( m_pTex_Norm[ TEX_LOAD_HIGH]);
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Norm%s.i3i", szPath, szFileName, szTeam);
		if( i3FileStream::isFile( szFullPath) == true)
		{
			m_pTex_Norm[ TEX_LOAD_HIGH] = g_pFramework->GetResourceManager()->LoadTexture( szFullPath);
			m_pTex_Norm[ TEX_LOAD_HIGH]->setRestoreType(I3G_RESTORE_FILE);
		#if defined( I3_DEBUG)
			m_pTex_Norm[ TEX_LOAD_HIGH]->SetName( szFullPath);
		#endif
		}
	}

#if defined( CLIENT_SIDE)
	if( m_bEnableTexture[ SPECULAR] )
#endif
	{
		I3_SAFE_RELEASE( m_pTex_Spec[ TEX_LOAD_HIGH]);
		i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_Spec%s.i3i", szPath, szFileName, szTeam);
		if( i3FileStream::isFile( szFullPath) == true)
		{
			m_pTex_Spec[ TEX_LOAD_HIGH] = g_pFramework->GetResourceManager()->LoadTexture( szFullPath);
			m_pTex_Spec[ TEX_LOAD_HIGH]->setRestoreType(I3G_RESTORE_FILE);
		#if defined( I3_DEBUG)
			m_pTex_Spec[ TEX_LOAD_HIGH]->SetName( szFullPath);
		#endif
		}
	}

	I3_SAFE_RELEASE( m_pTex_RefMask[ TEX_LOAD_HIGH]);
	i3::snprintf( szFullPath, sizeof( szFullPath), "%s/%s_RefM%s.i3i", szPath, szFileName, szTeam);
	if( i3FileStream::isFile( szFullPath) == true)
	{
		m_pTex_RefMask[ TEX_LOAD_HIGH] = g_pFramework->GetResourceManager()->LoadTexture( szFullPath);
		m_pTex_RefMask[ TEX_LOAD_HIGH]->setRestoreType(I3G_RESTORE_FILE);
	#if defined( I3_DEBUG)
		m_pTex_RefMask[ TEX_LOAD_HIGH]->SetName( szFullPath);
	#endif
	}

	m_CurrentTexLevel = TEX_LOAD_HIGH;
}



