#if !defined( _GAME_CHARA_TEXTURE)
#define _GAME_CHARA_TEXTURE

#include "GameCharaTextureDef.h"

namespace CharaTextureUtil
{
	i3Node *	FindNodeByContainName( i3Node * pNode, const char * pszName);
	i3RenderAttr * FindTextureBindInAttrSet( i3LOD * pLOD, const char * pszAttrName, i3ClassMeta * pMeta, bool bSevereCheck = true);
	
	void FindTextureBindSet( i3LOD * pLOD, const char * pszAttrName, CharaTexBindSet * pOut);	
}

#endif //_GAME_CHARA_TEXTURE