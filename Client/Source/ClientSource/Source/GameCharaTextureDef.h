#if !defined( _GAME_CHARA_TEXTURE_DEFINE)
#define _GAME_CHARA_TEXTURE_DEFINE

//--------------------------------------------------------------------------------------------------
//	캐릭터 텍스처 컨테이너 구조
//
//	  (CharaTexture)		(CharaColorSet)		(CharaTexSet)		   (i3Texture)
//
//	m_TexChara[ TypeA] --> _ColorSetList(0) --> _TexSet[ ARMS] --> _pTexDiff (i3Texture)
// 			:		    |					 |					|
//					    -> _ColorSetList(1)  -> _TexSet[ FACE]  -> _pTexNorm (i3Texture)
//					    :					 |					|
//											 -> _TexSet[ BODY]  -> _pTexSpec (i3Texture)
//
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	장비 텍스처 컨테이너 구조
//
//	  (CharaTexture)			(CharaColorSet)		(CharaTexSet)		   (i3Texture)
//
//	m_TexEquip[ type][ num] --> _ColorSetList(0) --> _TexSet[ 0] --> _pTexDiff (i3Texture)
// 			:		         |					  |			      |
//					         -> _ColorSetList(1)  -> _TexSet[ 0]  -> _pTexNorm (i3Texture)
//					         :					  |		   	      |
//											      -> _TexSet[ 0]  -> _pTexSpec (i3Texture)
//
//--------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Chara Texture Resource Container (CharaManager가 관리)
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum CHARATEX_BODY
{
	CHARATEX_BODY_ARM = 0,		//	1인칭 팔	
	CHARATEX_BODY_BODY,			//	3인칭 몸통
	//CHARATEX_BODY_FACE,			//	3인칭 얼굴

	CHARATEX_BODY_COUNT
};

enum CHARATEX_TYPE
{
	CHARATEX_TYPE_DIFF = 0,		
	CHARATEX_TYPE_NORM,			
	CHARATEX_TYPE_SPEC,			

	CHARATEX_TYPE_COUNT
};

struct CharaTexSet
{
	i3Texture *		_pTexDiff = nullptr;	//	diffuse 텍스처
	i3Texture *		_pTexNorm = nullptr;	//	normal map 텍스처 (null일 경우 기존 텍스처를 사용합니다.)
	i3Texture *		_pTexSpec = nullptr;	//	specular 텍스처 (null일 경우 기존 텍스처를 사용합니다.)

} ;

struct CharaColorSet
{
	CharaTexSet		_TexSet[ CHARATEX_BODY_COUNT];
	
} ;

struct ClanTexture
{
	i3::vector<i3Texture*>	_ClanList;								//	클랜 마크 텍스처 리스트 (i3Texture)

};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Chara Texture Bind Container (CharaBase가 관리)
//////////////////////////////////////////////////////////////////////////////////////////////////////

struct CharaTexBindSet
{
	i3TextureBindAttr *		pDiffBind = nullptr;	//	diffuse 텍스처 바인드
	i3NormalMapBindAttr *	pNormBind = nullptr;	//	normal map 텍스처 바인드
	i3SpecularMapBindAttr * pSpecBind = nullptr;	//	specular 텍스처 바인드

} ;

//	3인칭 LOD 수
#define	CHARA_3PV_LOD_COUNT		3


//--------------------------------------
//	CHARA_ATTRSET
//--------------------------------------
enum CHARA_ATTRSET
{
	CHARA_ATTRSET_ARM = 0,
	CHARA_ATTRSET_LOW_BODY,
	CHARA_ATTRSET_UPPER_BODY,
	CHARA_ATTRSET_CLAN,
	CHARA_ATTRSET_FACE,

	CHARA_ATTRSET_COUNT
};

//	캐릭터 모델링의 Geometry를 포함한 AttrSet 이름과 반드시 일치해야 하며 겹치는 이름이 없어야 한다.

//	현재 캐릭터를 CharaTool 에서 Export하면 하나의 LOD당 "Model_UpperBody_Material" AttrSet이 2개가 나옵니다.
//	여기서 클랜 텍스처를 사용하고 있는 "Model_UpperBody_Material" 이름을 "Model_ClanMark_Material"로 GraphTool에서 바꿔줘야 한다 
//	이름을 변경하지 않으면 다른 클랜 마크 텍스처로 교체가 불가능하다. 주의 - komet
const char g_szCharaAttrSetName[ CHARA_ATTRSET_COUNT][256] = 
{
	"Model_Arms_Material",			// CHARA_ATTRSET_ARM
	"Model_LowBody_Material",		// CHARA_ATTRSET_LOW_BODY
	"Model_UpperBody_Material",		// CHARA_ATTRSET_UPPER_BODY
	"Model_Clan_Material",		// CHARA_ATTRSET_CLAN
	"Model_Face_Material",			// CHARA_ATTRSET_FACE
};




#endif //_GAME_CHARA_TEXTURE_DEFINE