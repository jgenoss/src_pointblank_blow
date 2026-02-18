#include "pch.h"
#include "GameCharaTexture.h"

i3Node * CharaTextureUtil::FindNodeByContainName( i3Node * pNode, const char * pszName)
{
	if( pNode->GetName() )
	{
		i3::string strUpper = pNode->GetName();
		i3::to_upper( strUpper);

		if( i3::contain_string( strUpper, pszName) >= 0)
			return pNode;
	}

	for( INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pFindNode = FindNodeByContainName( pNode->GetChild(i), pszName);
		if( pFindNode != nullptr)
			return pFindNode;
	}

	return nullptr;
}

i3RenderAttr * CharaTextureUtil::FindTextureBindInAttrSet( i3LOD * pLOD, const char * pszAttrName, i3ClassMeta * pMeta, bool bSevereCheck)
{	
	i3Node * pFindNode = i3Scene::FindNodeByName( pLOD->getShapeNode(), pszAttrName);
	if( pFindNode == nullptr)
	{
		pFindNode = FindNodeByContainName( pLOD->getShapeNode(), pszAttrName);
	}

	i3AttrSet * pAttrSet = nullptr;

	//	AttrSet 검사
	if( pFindNode != nullptr)
	{
		if( i3::same_of<i3AttrSet*>(pFindNode))
		{				
			pAttrSet = (i3AttrSet *) pFindNode;
		}
		else if( bSevereCheck)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Not i3AttrSet Class (name: %s)", pszAttrName);		
			return nullptr;
		}
	}
	else if( bSevereCheck)
	{	// bSevereCheck 인자 true이면 지정된 AttrSet이름을 찾지 못하면 다운된다.
		I3PRINTLOG(I3LOG_FATAL,  "Not find Node (name: %s)", pszAttrName);
		return nullptr;
	}	
	else
	{
		I3TRACE( "[%s] warning! Cannot find %s (%s)\n", __FUNCTION__, pMeta->class_name(), pszAttrName);
		return nullptr;
	}

	i3RenderAttr * pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( pAttrSet, pMeta);

	//	Attribute가 GatherUp 되어 있을 경우를 대비
	if( pRenderAttr == nullptr)
	{
		i3Node * pParent = pAttrSet->GetParent();
		while( pParent != nullptr)
		{
			//	그 부모가 AttrSet이면 nullptr이 될때까지 해당 Attribute를 찾는다.
			if( i3::kind_of<i3AttrSet*>(pParent))
			{
				pRenderAttr = (i3RenderAttr *) i3Scene::FindAttrByExactType( (i3AttrSet *) pParent, pMeta);

				//	찾았다.
				if( pRenderAttr != nullptr)		break;
			}

			pParent = pParent->GetParent();
		}
	}
	//I3ASSERT( pRenderAttr != nullptr);

	return pRenderAttr;
};

void CharaTextureUtil::FindTextureBindSet( i3LOD * pLOD, const char * pszAttrName, CharaTexBindSet * pOut)
{
	I3ASSERT( pOut != nullptr);

	//	AttrSet의 이름으로 검색하여 TextureBind 찾아 저장한다.
	pOut->pDiffBind = (i3TextureBindAttr *)		CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3TextureBindAttr::static_meta());
	pOut->pNormBind = (i3NormalMapBindAttr *)	CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3NormalMapBindAttr::static_meta(), false);
	pOut->pSpecBind = (i3SpecularMapBindAttr *)	CharaTextureUtil::FindTextureBindInAttrSet( pLOD, pszAttrName, i3SpecularMapBindAttr::static_meta(), false);
};