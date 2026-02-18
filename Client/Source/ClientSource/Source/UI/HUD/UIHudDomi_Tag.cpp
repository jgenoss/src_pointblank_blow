#include "pch.h"


#include "UIHudDomi_Tag.h"
#include "../../BattleHud.h"


// 무기에 표시 아이콘을 붙인다.
// 현재는 제압에서 스킬이나 회복 오브젝트에 붙이는데 쓰인다.
// ( 회복 오브젝트도 무기로 되어있음 )
bool	SetTagToWeapon( i3Quad*& pQuad, WEAPON_TAG_TYPE type )
{
	if( CHud::i() == nullptr ) return false;

	if( pQuad )
	{
		pQuad->RemoveFromParent();
		I3_SAFE_RELEASE( pQuad );
	}

	pQuad	= i3Quad::new_object();
	pQuad->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );
	pQuad->Create( 1, I3SG_QUAD_SPACE_WORLD, true, false, true );
	pQuad->SetColor( 0, 255, 255, 255, 100 );
	pQuad->SetSize( 0, 1.0f, 1.0f );

	// 위치 설정
	switch( type )
	{
	case WEP_TAG_SKILL_SHOP:
		pQuad->SetCenter( 0, 0.0f, 2.1f, 0.0f );
		break;

	case WEP_TAG_DUMMY:
		pQuad->SetCenter( 0, 0.0f, 2.6f, 0.0f );
		break;

	case WEP_TAG_SENTYGUN:
		pQuad->SetCenter( 0, 0.0f, 2.6f, 0.0f );
		break;

	case WEP_TAG_OBJ_HP:
	case WEP_TAG_OBJ_AMMO:
		pQuad->SetCenter( 0, 0.0f, 2.9f, 0.0f );
		break;

	case TAG_OBJ_HP:
		pQuad->SetCenter( 0, 0.f, 2.f, 0.f);
		pQuad->SetSize( 0, 1.0f, 0.05f);
		break;
	}

	
	VEC2D	vU, vV;

	pQuad->SetTexture( CHud::i()->getHud3Texture() );

	// 텍스쳐 좌표 설정
	switch( type )
	{
	case WEP_TAG_SKILL_SHOP :	i3Vector::Set( &vU, 819.0f, 911.0f );	i3Vector::Set( &vV, 387.0f, 491.0f );	break;
	case WEP_TAG_DUMMY :		i3Vector::Set( &vU, 919.0f, 1011.0f );	i3Vector::Set( &vV, 387.0f, 491.0f );	break;
	case WEP_TAG_SENTYGUN :		i3Vector::Set( &vU, 717.0f, 809.0f );	i3Vector::Set( &vV, 387.0f, 491.0f );	break;
	case WEP_TAG_OBJ_HP :		i3Vector::Set( &vU, 819.0f, 911.0f );	i3Vector::Set( &vV, 276.0f, 380.0f );	break;
	case WEP_TAG_OBJ_AMMO :		i3Vector::Set( &vU, 919.0f, 1011.0f );	i3Vector::Set( &vV, 276.0f, 380.0f );	break;

	case TAG_OBJ_HP:		i3Vector::Set( &vU, 74.f, 248.0f );		i3Vector::Set( &vV, 487.0f, 495.0f );	break;
	}

	i3Vector::Scale( &vU, &vU, 1.0f / 1024.0f );
	i3Vector::Scale( &vV, &vV, 1.0f / 512.0f );

	pQuad->SetTextureCoord( 0, vU.x, vV.x, vU.y, vV.y);

	i3LightingEnableAttr *pLightEnable		= i3LightingEnableAttr::new_object_ref();
	pLightEnable->Set( false );
	pQuad->AddAttr( pLightEnable );		

	i3TextureFunctionAttr * pTexFuncAttr	= i3TextureFunctionAttr::new_object_ref();
	pTexFuncAttr->SetFunction( I3G_TEXTURE_FUNC_MODULATE );
	pQuad->AddAttr( pTexFuncAttr );

	i3ZWriteEnableAttr * pZWirteAttr		= i3ZWriteEnableAttr::new_object_ref();
	pZWirteAttr->Set( true );
	pQuad->AddAttr( pZWirteAttr );

	i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
	pAttr->SetSource( I3G_BLEND_SRCALPHA);
	pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
	pQuad->AddAttr( pAttr);

	i3AlphaFuncAttr * pAlphaFunc			= i3AlphaFuncAttr::new_object_ref();
	pAlphaFunc->SetFunction( I3G_COMP_GREATER);
	pAlphaFunc->SetRefValue( 50);
	pQuad->AddAttr( pAlphaFunc);

	pQuad->SetEnable( 0, true);

	return true;
}

