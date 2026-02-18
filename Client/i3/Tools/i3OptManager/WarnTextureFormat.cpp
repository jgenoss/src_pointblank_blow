#include "stdafx.h"
#include "WarnTextureFormat.h"

I3_CLASS_INSTANCE( WarnTextureFormat, i3ElementBase);

WarnTextureFormat::WarnTextureFormat(void)
{
}

WarnTextureFormat::~WarnTextureFormat(void)
{
}

const char * WarnTextureFormat::GetTitle(void)
{
	return "Texture Format";
}

bool WarnTextureFormat::IsInterested( char * pszName, char * pszExt)
{
	if( i3String::Compare( pszExt, "I3I") == 0)
		return true;
	else if( i3String::Compare( pszExt, "I3S") == 0)
		return true;

	return false;
}

bool WarnTextureFormat::_CheckTexture( i3Texture * pTex, i3StringList * pMsgList)
{
	// DXT 등과 같은 압축 포맷이거나, Indexed Format인지를 확인한다.
	if( (pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED) ||
		(pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED))
	{
		return true;
	}

	char conv[256];

	if( pTex->GetName() != NULL)
		sprintf( conv, "%s Texture는 %s Format입니다.", pTex->GetName(), i3Gfx::GetImageFormatName( pTex->GetFormat()));
	else
		sprintf( conv, "이름이 없는 특정 Texture는 %s Format입니다.", i3Gfx::GetImageFormatName( pTex->GetFormat()));

	pMsgList->Add( conv);

	return false;
}

bool WarnTextureFormat::_OnCheckNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData)
{
	if( pNode->IsTypeOf( i3AttrSet::GetClassMeta()))
	{
		i3AttrSet * pAttrSet  = (i3AttrSet *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->IsTypeOf( i3TextureBindAttr::GetClassMeta()))
			{
				i3TextureBindAttr * pTexBind = (i3TextureBindAttr *) pAttr;
				i3Texture * pTex = pTexBind->GetTexture();

				if( pTex != NULL)
				{
					_CheckTexture( pTex, pMsgList);
				}
			}
		}
	}

	return true;
}

bool WarnTextureFormat::Check( char * pszName, char * pszExt, i3ElementBase * pObj, i3StringList * pMsgList)
{
	if( pObj->IsTypeOf( i3Texture::GetClassMeta()))
	{
		return _CheckTexture( (i3Texture *) pObj, pMsgList);
	}
	else if( pObj->IsTypeOf( i3Node::GetClassMeta()))
	{
		_CheckTrace( (i3Node *) pObj, pMsgList);

		return m_bResult;
	}

	return true;
}


i3ElementBase * WarnTextureFormat::Solve( i3ElementBase * pObj, i3StringList * pMsgList)
{

	return NULL;
}
