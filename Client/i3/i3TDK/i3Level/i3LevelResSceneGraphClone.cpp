#include "stdafx.h"
#include "i3LevelResSceneGraphClone.h"
#include "i3LevelElement3D.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/extract_fileext.h"

I3_CLASS_INSTANCE( i3LevelResSceneGraphClone);

i3LevelResSceneGraphClone::~i3LevelResSceneGraphClone(void)
{
	I3_SAFE_RELEASE( m_pSourceRes);
}

void i3LevelResSceneGraphClone::setSourceRes( i3LevelResSceneGraph * pRes)
{
	I3_REF_CHANGE( m_pSourceRes, pRes);

	// Dependancy를 알리기 위해...
	if( m_RefResList.size() > 0)
	{
		m_RefResList[0] = pRes;
	}
	else
	{
		m_RefResList.push_back( pRes);
	}
}

bool		i3LevelResSceneGraphClone::OnValidateBrief( bool bForce)
{
	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool		i3LevelResSceneGraphClone::OnValidate( i3LevelScene * pScene, bool bForce)
{
	// Clone Resource는 원본 Resource가 이미 Validate된 상황에서만 생성된다.
	addResState( I3LV_RES_STATE_LOADED);

	return true;
}

bool		i3LevelResSceneGraphClone::OnExport( i3LevelScene * pScene, const char * pszPath)
{
	if( m_pSourceRes == nullptr)
		return true;

	I3ASSERT( m_pElement != nullptr);

	// 원본을 복사하여, 독립적인 Resource로 Export해야 한다.
	{
		char szTemp[ MAX_PATH], szDest[ MAX_PATH];

		// 원본명 + Element명
		char szExt[32];

//		i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);
		i3::extract_fileext(pszPath, szExt);

		i3::safe_string_copy( szTemp, pszPath, MAX_PATH);
		i3::remove_ext( szTemp);

		sprintf( szDest, "%s.%s", szTemp, szExt);

		// 새로운 i3SceneGraphInfo를 생성하고 저장한다.
		{
			i3SceneGraphInfo * pSgInfo = i3SceneGraphInfo::new_object();
			i3SceneGraphInfo * pSrcSgInfo = m_pSourceRes->getSceneGraphInfo();

			pSgInfo->setCommonSg( pSrcSgInfo->getCommonSg());
			pSgInfo->setInstanceSg( m_pElement->GetSymbol());

			i3SceneFile file;

			file.setKeyObject( pSgInfo);

			file.Save( szDest);

			I3_MUST_RELEASE(pSgInfo);
		}
	}

	addResState( I3LV_RES_STATE_MODIFIED);

	return true;
}

i3GameRes *	i3LevelResSceneGraphClone::CreateGameRes(void)
{
	return m_pSourceRes->CreateGameRes();
}

i3Node *	i3LevelResSceneGraphClone::GetElementAttachNode( i3LevelScene * pScene)
{
	return m_pSourceRes->GetElementAttachNode( pScene);
}
