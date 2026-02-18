#include "stdafx.h"
#include "AnimOptProject.h"
#include "GlobVar.h"

I3_CLASS_INSTANCE( AnimOptProject, i3ElementBase);

AnimOptProject::AnimOptProject(void)
{
	m_pRoot = NULL;
	m_pSceneObj = NULL;
	m_szScenePath[0] = 0;
}

AnimOptProject::~AnimOptProject(void)
{
	Reset();
}

void AnimOptProject::Reset(void)
{
	SetRoot( NULL);

	RemoveAllAnim();
}

void AnimOptProject::SetRoot( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_pSceneObj);

	m_pRoot = pNode;

	if( m_pRoot != NULL)
	{
		if( m_pRoot->IsTypeOf( i3SceneObject::GetClassMeta()))
		{
			m_pSceneObj = (i3SceneObject *) m_pRoot;
			m_pSceneObj->AddRef();
		}
		else
		{
			m_pSceneObj = i3SceneObject::NewObject();

			m_pSceneObj->AddChild( m_pRoot);
			m_pSceneObj->BuildBoneList();
		}
	}
}

void AnimOptProject::AddAnim( i3Animation * pAnim)
{
	pAnim->AddRef();

	m_AnimList.Add( pAnim);
}

void AnimOptProject::RemoveAnim( i3Animation * pAnim)
{
	INT32 idx;

	idx = FindAnim( pAnim);
	I3ASSERT( idx != -1);

	RemoveAnim( idx);
}

void AnimOptProject::RemoveAnim( INT32 idx)
{
	i3Animation * pAnim;

	pAnim = GetAnim( idx);

	pAnim->Release();

	m_AnimList.Delete( idx);
}

void AnimOptProject::RemoveAllAnim(void)
{
	INT32 i;
	i3Animation * pAnim;

	for( i = 0; i < m_AnimList.GetCount(); i++)
	{
		pAnim = GetAnim( i);

		pAnim->Release();
	}

	m_AnimList.Clear();
}

INT32 AnimOptProject::FindAnimByName( char * pszName)
{
	INT32 i;
	i3Animation * pAnim;

	for( i = 0; i < GetAnimCount(); i++)
	{
		pAnim = GetAnim( i);

		if( i3String::Compare( pAnim->GetName(), pszName) == 0)
		{
			return i;
		}
	}

	return -1;
}

INT32 AnimOptProject::FindAnim( i3Animation * pAnim)
{
	return m_AnimList.IndexOf( pAnim);
}

void AnimOptProject::LoadScene( const char * pszPath)
{
	i3SceneFile file;
	i3Node * pNode;

	file.Load( pszPath);

	strcpy( m_szScenePath, pszPath);

	pNode = file.GetSceneGraph();

	SetRoot( pNode);

	Update( NULL, I3_TDK_UPDATE_SCENE, pNode);
}

void AnimOptProject::LoadAnim( const char * pszPath)
{
	i3Animation * pAnim;

	pAnim = i3Animation::LoadFromFile( pszPath);
	if( pAnim == NULL)
	{
		DebugBox( NULL, "%s 파일을 읽을 수 없습니다.", pszPath);
		return;
	}

	pAnim->SetName( pszPath);

	AddAnim( pAnim);

	Update( NULL, I3_TDK_UPDATE_ADD, pAnim);
}


void AnimOptProject::SaveScene(void)
{
	if( m_pRoot == NULL)
		return;

	if( m_szScenePath[0] == 0)
	{
		SaveAsScene();
		return;
	}

	i3SceneFile file;

	file.SetSceneGraph( m_pRoot);

	file.Save( m_szScenePath);
}

void AnimOptProject::SaveAsScene(void)
{
	if( m_pRoot == NULL)
		return;

	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene Graph File(*.i3s)|*.i3s||", NULL);

	if( Dlg.DoModal() != IDOK)return;

	strcpy( m_szScenePath, LPCTSTR( Dlg.GetPathName()));

	i3SceneFile file;

	file.SetSceneGraph( m_pRoot);

	file.Save( m_szScenePath);
}

void AnimOptProject::SaveAllAnim(void)
{
	INT32 i;
	i3Animation * pAnim;

	for( i = 0; i < GetAnimCount(); i++)
	{
		pAnim = GetAnim( i);

		pAnim->SaveToFile( pAnim->GetName());
	}

	DebugBox( NULL, "Total %d animation are saved\n", GetAnimCount());
}

void AnimOptProject::BuildBoneIndex(void)
{
	i3SceneObject * pSceneObj;
	INT32 i, j, idx;
	i3Animation * pAnim;
	i3Animation2 * pAnim2;

	if( m_pRoot->IsTypeOf( i3SceneObject::GetClassMeta()))
	{
		pSceneObj = (i3SceneObject *) m_pRoot;
		pSceneObj->AddRef();
	}
	else
	{
		pSceneObj = i3SceneObject::NewObjectRef();
	}

	pSceneObj->BuildBoneList();

	for( i = 0; i < GetAnimCount(); i++)
	{
		pAnim = GetAnim( i);

		pAnim2 = i3Animation2::NewObject();
 
		pAnim2->Create( pAnim->GetTrackCount());
		pAnim2->SetName( pAnim->GetName());

		for( j = 0; j < pAnim->GetTrackCount(); j++)
		{
			pAnim2->SetTrack( j, pAnim->GetTrack( j), pAnim->GetTrackBoneName(j));

			idx = pSceneObj->FindBoneIndexByName( pAnim->GetTrackBoneName(j));
			pAnim2->SetBoneIndex( j, idx);

			if( idx == -1)
			{
				DebugBox( NULL, "Could not find %s bone\n", pAnim->GetTrackBoneName(j));
			}
		}

		m_AnimList.Items[i] = pAnim2;
		pAnim->Release();
	}

	SaveAllAnim();
}

void AnimOptProject::PlayAnim( i3Animation * pAnim)
{
	m_pSceneObj->PlayAnim( pAnim);
}
