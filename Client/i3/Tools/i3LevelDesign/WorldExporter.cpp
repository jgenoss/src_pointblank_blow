#include "pch.h"
/*
#include "WorldExporter.h"

CWorldExporter::CWorldExporter()
{

}

CWorldExporter::~CWorldExporter()
{

}

void CWorldExporter::Export(const char * pszFileName)
{
	i3LevelScene * pScene = i3Level::GetScene();

	i3World * pNewWorld = i3World::new_object();

	pNewWorld->Create();

	//Fog
	{
		pNewWorld->SetFogEnable( pScene->GetFogEnable());
		pNewWorld->SetFogColor( pScene->GetFogColor());
		pNewWorld->SetFogDensity( pScene->GetFogDensity());
		pNewWorld->SetFogMode( pScene->GetFogMode());
		pNewWorld->SetFogNear( pScene->GetFogNear());
		pNewWorld->SetFogFar( pScene->GetFogFar());
	}

	//Light------ _+_+_+_+_+_+_+_+_+
	{
		List lstLight;

		pScene->FindElementByType( &lstLight, i3LevelLight::static_meta());
		INT32 nCount = lstLight.GetCount();
		for( INT32 i = 0; i< nCount; ++i)
		{
			i3LevelLight * pLight = ( i3LevelLight*)lstLight.GetItem( i);
			if( pLight != NULL)
			{
				pNewWorld->setLightAttr( pLight->getLightAttr());
				pNewWorld->setLightDistance( pLight->getLightDist());
				pNewWorld->setLightRotation( pLight->getLightRotation());
				pNewWorld->setLightTargetPos( pLight->getLightTargetPos());
			}
		}
	}

	//World
	{
		List lstWorld;

		pScene->FindElementByType( &lstWorld, i3LevelWorld::static_meta());
		INT32 nCount = lstWorld.GetCount();
		for( INT32 i = 0; i < nCount ; ++i)
		{
			i3LevelWorld * pWorld = (i3LevelWorld*)lstWorld.GetItem( i);
			if( pWorld != NULL)
			{
				i3SceneGraphInfo * pInfo = pWorld->GetSymbol()->getSceneGraphInfo();

				pNewWorld->SetWorld( pWorld->getWorldScene());
				pNewWorld->SetWorldCollision( pWorld->getWorldCol());
				pNewWorld->SetHeroCollision( pWorld->getHeroCol());
				pNewWorld->SetHitCollision( pWorld->getHitCol());
				pNewWorld->SetLightVolumeCollision( pWorld->getLightVolumeCol());

				pNewWorld->SetFogEnable( pWorld->GetFogEnable( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);
				pNewWorld->SetFogColor( pWorld->GetFogColor( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);
				pNewWorld->SetFogDensity( pWorld->GetFogDensity( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);
				pNewWorld->SetFogMode( pWorld->GetFogMode( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);
				pNewWorld->SetFogNear( pWorld->GetFogNear( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);
				pNewWorld->SetFogFar( pWorld->GetFogFar( I3FRM_FOG_WORLD), I3FRM_FOG_WORLD);

				pInfo = (i3SceneGraphInfo*)pWorld->getSkyScene();
				if( pInfo != NULL)
				{
					pNewWorld->SetSky(pInfo);

					pNewWorld->SetFogEnable( pWorld->GetFogEnable( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
					pNewWorld->SetFogColor( pWorld->GetFogColor( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
					pNewWorld->SetFogDensity( pWorld->GetFogDensity( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
					pNewWorld->SetFogMode( pWorld->GetFogMode( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
					pNewWorld->SetFogNear( pWorld->GetFogNear( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
					pNewWorld->SetFogFar( pWorld->GetFogFar( I3FRM_FOG_SKY), I3FRM_FOG_SKY);
				}
			}
		}
	}

	i3ResourceFile File;

	File.setKeyObject( pNewWorld);

	if( File.Save( pszFileName) == STREAM_ERR)
	{
		I3TRACE("FAILED TO SAVE WORLD\n");
	}
}*/