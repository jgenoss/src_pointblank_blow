#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

void i3LevelScene::addLayer( i3LevelLayer * pLayer)
{
	I3_MUST_ADDREF(pLayer);

	if( m_pCurLayer == nullptr)
	{
		m_pCurLayer = pLayer;
	}

	m_LayerList.push_back( pLayer);
}

void i3LevelScene::removeLayer( i3LevelLayer * pLayer)
{
	I3_MUST_RELEASE(pLayer);

	i3::vu::remove(m_LayerList, pLayer);
}

void i3LevelScene::removeAllLayer(void)
{
	INT32 i;
	i3LevelLayer * pLayer;

	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);

		I3_MUST_RELEASE(pLayer);
	}

	m_LayerList.clear();

	m_pCurLayer = nullptr;
}

INT32 i3LevelScene::FindLayerByName( const char * pszName)
{
	INT32 i;
	i3LevelLayer * pLayer;

	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);

		if( i3::generic_is_iequal( pLayer->GetName(), pszName) )
			return i;
	}

	return -1;
}

void i3LevelScene::GetUniqueLayerName( const char * pszPrefix, char * pszName)
{
	char conv[256];
	INT32 i = 0;

	if( pszPrefix != nullptr)
		strncpy( conv, pszPrefix, sizeof(conv) - 1);
	else
		strncpy( conv, "Layer", sizeof(conv) - 1);

	do
	{
		if( i == 0)
			sprintf( pszName, "%s", conv);
		else
			sprintf( pszName, "%s%d", conv, i);

		i++;

	} while( FindLayerByName( pszName) != -1);
}
