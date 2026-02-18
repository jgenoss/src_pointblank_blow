#include "i3SceneDef.h"
#include "i3SceneGraphInfoResManager.h"
#include "i3SceneFile.h"

#include "i3SceneUtil.h"
#include "i3TextureBindAttr.h"
#include "i3SGTexChangeManager.h"

I3_CLASS_INSTANCE( i3SceneGraphInfoResManager);

namespace
{

void	SetTextureIntoBindAttr(i3NamedResourceManager* pTexManager, const i3::vector<i3RenderAttr*>& bindAttrList, const i3SGTexChangeInfo* pTexChangeInfo, INT32 lodIdx)
{
	
	for (size_t i = 0 ; i < bindAttrList.size() ; ++i )
	{
		i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(bindAttrList[i]);

		i3::rc_string ref_tex_path;

		i3Texture* prev_tex = tex_attr->GetTexture();

		if (prev_tex)
		{
			if (prev_tex->getResourceLoadPath())
				ref_tex_path = prev_tex->getResourceLoadPath();
			else
				ref_tex_path = prev_tex->GetNameString();
		}
		else
		{
			ref_tex_path = tex_attr->GetSkippedTextureFilename();
		}

		if ( !ref_tex_path.empty() )
		{
			i3::rc_string tgt_tex_filepath;
			if ( lodIdx == -1)
				tgt_tex_filepath =	i3SGTexChangeManager::FindTargetTexturePath(pTexChangeInfo, ref_tex_path.c_str() );
			else
				tgt_tex_filepath = i3SGTexChangeManager::FindLODTargetTexturePath(pTexChangeInfo, lodIdx, ref_tex_path.c_str());

			bool  tgt_tex_load_sucess = false;

			if (!tgt_tex_filepath.empty())
			{
				if (tgt_tex_filepath != "(null)" )
				{
					i3Texture* new_tex = (i3Texture *) pTexManager->GetResource(tgt_tex_filepath.c_str() , i3ResourceFile::GetWorkingFolder(), tgt_tex_filepath.c_str() );
					if (new_tex)
					{
						tex_attr->SetTexture(new_tex);
						tex_attr->SetApplyEnable(true);
						tex_attr->SetTextureChange(true);
						tgt_tex_load_sucess = true;
					}
				}
				else		// '(null)'인 경우엔 해당 attr을 불활성화시킨다..
				{
					tex_attr->SetApplyEnable(false);
					tex_attr->SetTextureChange(true);
					tgt_tex_load_sucess = true;
				}
			}

			if (tgt_tex_load_sucess == false)							
			{
				if (prev_tex == nullptr)
				{
					i3Texture* new_tex = (i3Texture *) pTexManager->GetResource(ref_tex_path.c_str() , 
						i3ResourceFile::GetWorkingFolder(), ref_tex_path.c_str() );
					tex_attr->SetTexture(new_tex);
					tex_attr->SetTextureChange(false);
					tex_attr->ClearSkippedTextureFilename();
				}
			}
		}
	}

}


i3ResourceObject*	GetResourceImpl(i3SceneGraphInfoResManager* sgir_mgr, const char * pszResFile, const char * pszBasePath, const char *pszName, bool bExternTextureSkip)
{
	UINT32 Rc;
	i3SceneGraphInfo * pInfo;
	i3ResourceObject * pObj;

	pInfo = (i3SceneGraphInfo *) sgir_mgr->i3NamedResourceManager::GetResource( pszResFile, pszBasePath, pszName);	// 이건 무조건 되도록 함..

	// 기존에 Loading 되어진 것이 있다면...
	if( pInfo != nullptr)
	{
		if ( pInfo->IsExternTextureSkipped() )		// 함수 인수의 bExternTextureSkip와 같다면, 그냥 리턴한다...
		{
			if ( bExternTextureSkip == false )
			{
				// 이 경우엔, 텍스쳐셋팅이 필요하다..
				i3::vector<i3RenderAttr*> list;
				i3Scene::FindAttrByType(pInfo->getInstanceSg(), i3TextureBindAttr::static_meta(), list);
				i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
				
				for (size_t i = 0 ; i < list.size() ; ++i )
				{
					i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(list[i]);
					i3Texture* tex = tex_attr->GetTexture();
					if (tex == nullptr)
					{
						i3::rc_string str_texname = tex_attr->GetSkippedTextureFilename();
						if ( !str_texname.empty() )
						{
							i3Texture* tex = (i3Texture *) pTexManager->GetResource(str_texname.c_str() , i3ResourceFile::GetWorkingFolder(), str_texname.c_str() );
							if (tex)
							{
								tex_attr->SetTexture(tex);
								tex_attr->SetTextureChange(false);
							}
							tex_attr->ClearSkippedTextureFilename();	// 성공/실패와 관계없이 리셋한다.
						}
					}
				}
				pInfo->SetExternTextureSkipped(false);
			}
		}

		return pInfo;
	}

	// 새로 Loading 한다.
	char szFull[ MAX_PATH];

	if( (pszBasePath != nullptr) && (pszBasePath[0] != 0) )
	{
 		sprintf( szFull, "%s/%s", pszBasePath, pszName);
	}
	else
	{
		strncpy( szFull, pszName, sizeof(szFull)-1);
	}
	

	const i3SGTexChangeInfo* pTexChangeInfo = i3SGTexChangeManager::i()->FindTexChangeInfo(szFull);

	if (pTexChangeInfo != nullptr)
	{
		i3::rc_string ref_filename =	i3SGTexChangeManager::GetRefi3sFilename(pTexChangeInfo);

		i3SceneGraphInfo* ref_info = (i3SceneGraphInfo*) GetResourceImpl(sgir_mgr, pszResFile, pszBasePath, ref_filename.c_str(), true);	// 재귀호출임..
		
		if (ref_info == nullptr)			// 이 경우는 원본메시 i3s파일이 존재하지 않는 경우이다.
			return nullptr;

		i3SceneGraphInfo* tgt_info = i3SceneGraphInfo::new_object_ref();
		ref_info->CopyTo(tgt_info, I3_COPY_INSTANCE);

		//  텍스쳐 변경 루틴이 들어가야한다..

		//
		i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());

		INT32 numLOD = i3SGTexChangeManager::GetNumLOD(pTexChangeInfo);

		if (numLOD == 0 )
		{
			i3::vector<i3RenderAttr*> list;
			i3Scene::FindAttrByType(tgt_info->getInstanceSg(), i3TextureBindAttr::static_meta(), list);
			SetTextureIntoBindAttr(pTexManager, list, pTexChangeInfo, -1);
		}
		else
		{
			i3::vector<i3RenderAttr*> list;
			i3Scene::FindAttrByType_NonLOD(tgt_info->getInstanceSg(), i3TextureBindAttr::static_meta(), list);
			SetTextureIntoBindAttr(pTexManager, list, pTexChangeInfo, -1);

			for (INT32 i = 0 ; i < numLOD ; ++i )
			{
				list.clear();
				i3Scene::FindAttrByType_LOD(tgt_info->getInstanceSg(), i, i3TextureBindAttr::static_meta(), list);
				SetTextureIntoBindAttr(pTexManager, list, pTexChangeInfo, i);
			}
		}
		
		//
		tgt_info->SetExternTextureSkipped(false);
		//

		pObj = tgt_info;
		sgir_mgr->AddResource( pObj, pszName);
		//
	}
	else
	{
		i3SceneFile file;

		file.SetExternTextureSkip(bExternTextureSkip);			// 일반적으로 false이고 레퍼런스메시의 경우만..

		Rc = file.Load( szFull);

		if( Rc == STREAM_ERR)
			return nullptr;

		pObj = file.getSceneGraphInfo();
		if( pObj == nullptr)
		{
			return nullptr;
		}

	#if defined( I3_DEBUG)
		pObj->m_FileSize = Rc;
	#endif
		sgir_mgr->AddResource( pObj, pszName);
	}

	return pObj;
}

}



i3ResourceObject * i3SceneGraphInfoResManager::GetResource( const char * pszResFile, const char * pszBasePath,
														   const char * pszName, bool bBackgroundLoad)
{
	return GetResourceImpl(this, pszResFile, pszBasePath, pszName, false);	//
}

UINT32 i3SceneGraphInfoResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	i3SceneFile file;
	char szFull[ MAX_PATH];
	UINT32 Rc;

	I3ASSERT( pObj->hasName());

	if( pszBasePath[0] != 0)
	{
		i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, pObj->GetNameString());
	}
	else
	{
		i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);
	}

	pObj->SetExternRes(false);
	file.setSceneGraphInfo( (i3SceneGraphInfo *) pObj);
	file.setKeyObject( pObj);

	Rc = file.Save( szFull);

	pObj->SetExternRes(true);

	return Rc;
}
