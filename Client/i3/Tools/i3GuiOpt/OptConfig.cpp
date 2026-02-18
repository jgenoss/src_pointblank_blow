#include "stdafx.h"
#include "OptConfig.h"

I3_CLASS_INSTANCE( COptConfig); //, i3ResourceObject);

COptConfig::COptConfig(void)
{
}

COptConfig::~COptConfig(void)
{
	removeAllOpt();
}

void COptConfig::addOpt( i3SceneOptimizer * pOpt)
{
	I3ASSERT( pOpt);

	if( i3::same_of<i3OptBuildIndexedGeometry*>(pOpt)) //->IsExactTypeOf( i3OptBuildIndexedGeometry::static_meta() ) )
	{
		((i3OptBuildIndexedGeometry*)pOpt)->SetBuildIndex( TRUE);
	}

	pOpt->AddRef();

	m_OptList.push_back( pOpt);

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pOpt);
}

void COptConfig::removeOpt( i3SceneOptimizer * pOpt)
{
	INT32 idx = i3::vu::int_index_of(m_OptList, pOpt);

	if( idx != -1)
	{
		i3::vu::erase_index(m_OptList, idx);

		i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pOpt);

		pOpt->Release();
	}
}

void COptConfig::removeAllOpt(void)
{
	INT32 i;
	i3SceneOptimizer * pOpt;

	for( i = 0; i < getCount(); i++)
	{
		pOpt = getOpt( i);

		pOpt->Release();
	}

	m_OptList.clear();

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3SceneOptimizer::static_meta());
}

bool COptConfig::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pParentXML)
{
	INT32 i;
	i3SceneOptimizer * pOpt;

	for( i = 0; i < getCount(); i++)
	{
		pOpt = getOpt( i);

		i3XMLElement * pXML = new i3XMLElement( pOpt->meta()->class_name().c_str());

		pParentXML->LinkEndChild( pXML);

		pOpt->SaveToXML( pXML);
	}

	return true;
}

bool COptConfig::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pParentXML)
{
	i3XMLElement * pXML;

	pXML = (i3XMLElement *) pParentXML->FirstChildElement();

	while( pXML != NULL)
	{
		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(pXML->Value());
						//i3ClassMeta::FindClassMetaByName( pXML->Value());

		if( i3::kind_of<i3SceneOptimizer*>(pMeta)) //->IsTypeOf( i3SceneOptimizer::static_meta()))
		{
			i3SceneOptimizer * pOpt = (i3SceneOptimizer *) pMeta->create_instance(); //CREATEINSTANCE( pMeta);

			addOpt( pOpt);

			pOpt->LoadFromXML( pXML);
		}

		pXML = (i3XMLElement *) pXML->NextSiblingElement();
	}

	i3TDK::Update( NULL, I3_TDK_UPDATE_SCENE, this);
	
	return true;
}
