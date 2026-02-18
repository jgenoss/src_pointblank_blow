#include "stdafx.h"
#include "i3LevelResLight.h"
#include "i3LevelResSceneGraph.h"
#include "i3LevelLight.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelResLight);

i3LevelResLight::i3LevelResLight(void)
{
	m_ResType	= I3LV_TYPE_LIGHT;
}

i3LevelResLight::~i3LevelResLight(void)
{
	I3_SAFE_RELEASE( m_pLight);
	I3_SAFE_RELEASE(m_pScene);		// 당황스럽게도 소거처리가 누락되있었음..(2012.09.11.수빈)
}

i3LevelElement3D *	i3LevelResLight::CreateInstance( i3LevelScene * pScene)
{
	i3LevelResLight	*	pNewRes = i3LevelResLight::new_object_ref();

	pNewRes->OnValidate( pScene, false);

	i3LevelLight	* pLevelLight	= i3LevelLight::new_object_ref();
	pLevelLight->BindRes( pNewRes);

	//GetElementCount
	i3::vector<i3LevelElement3D*> list;

	pScene->FindElementByType( list, i3LevelLight::static_meta());

	char conv[MAX_PATH];

	//Path
	sprintf( conv, "World/%s/Light/%s_Light%d.i3Light", pScene->GetName(), pScene->GetName(), (INT32)list.size());
	pNewRes->setPath( conv);

	//Name
	sprintf( conv, "%s_Light%d", pScene->GetName(), (INT32)list.size());
	pLevelLight->SetName( conv);
	pNewRes->SetName( conv);

	//State
	pNewRes->addResState( I3LV_RES_STATE_MODIFIED);

	//Link
	pScene->AddElement( pLevelLight, "Light");
	g_pResDB->AddRes( pNewRes);

	return pLevelLight;
}

bool i3LevelResLight::OnExport( i3LevelScene * pScene, const char * pszPath)
{
	i3ResourceFile file;

	file.setKeyObject( m_pLight);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		return false;
	}

	return true;
}

bool				i3LevelResLight::OnValidateBrief( bool bForce)
{
 	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;

		if( isLoaded())
			return true;
	}

	if( m_pScene == nullptr)
	{
		m_pScene = i3SceneGraphInfo::new_object();

		i3LevelResSceneGraph * pRes = i3LevelScene::GetCommonCameraResource();	// 반드시 이렇게 한다..(스태틱함수이며, 
			//g_pScene->GetCommonCameraResource();								// g_pScene 유효성과 무관하게 동작함...)

		i3SceneGraphInfo * pInfo = pRes->getSceneGraphInfo();			// pRes가 널이 되지 않도록, 미리 CreateCommonResource()가
																		// 호출되어 있었어야한다...
		pInfo->CopyTo( m_pScene, I3_COPY_INSTANCE);
	}

	if( getPath()[0] == 0)
	{
		return true;
	}

	char szFull[MAX_PATH];

	getFullPath( szFull);

	i3ResourceFile file;
	i3LightObject * pLightObj = nullptr;

	BeginValidate();
	{
		getFullPath( szFull);

		if( file.Load( szFull) == STREAM_ERR)
			return false;

		pLightObj = (i3LightObject*) file.getKeyObject();
	}
	EndValidate();

	I3_REF_CHANGE( m_pLight, pLightObj);
	
	addResState( I3LV_RES_STATE_BRIEF_LOADED | I3LV_RES_STATE_LOADED);

	return true;
}

bool				i3LevelResLight::OnValidate( i3LevelScene * pScene, bool bForce)
{
	return OnValidateBrief( bForce);
}

i3GameRes *			i3LevelResLight::CreateGameRes(void)
{
	i3GameResLight * pRes = i3GameResLight::new_object();

	pRes->setLightObject( m_pLight);

    return pRes;
}

bool				i3LevelResLight::OnSerialize(void)
{
	i3ResourceFile file;
	char szFull[ MAX_PATH];

	if( m_pLight == nullptr)
		return true;

	getFullPath( szFull);

	sprintf( szFull, "%s/%s", g_szResPath, getPath());

	file.setKeyObject( m_pLight);

	if( file.Save( szFull) == STREAM_ERR)
	{
		return false;
	}

	return true;
}
