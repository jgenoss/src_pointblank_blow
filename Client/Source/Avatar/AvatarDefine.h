#if !defined( __AVATAR_DEFINE_H)
#define __AVATAR_DEFINE_H

#include "../ClientSource/Source/GameCharaEquipDef.h"

#define		AVT_TEX_WIDTH		2048
#define		AVT_TEX_HEIGHT		2048

#define		AVT_LOD_COUNT		4

typedef UINT32 AVT_PART_STYLE;
#define		AVT_PART_STYLE_SKIN			0x00000001		// Merge할 수 있는 모델링
#define		AVT_PART_STYLE_EXCHANGE		0x00000002		// Merge할 수 없지만 다른 모델링으로 변경이 가능
#define		AVT_PART_STYLE_EQUIP		0x00000004		// 장비는 Merge는 안하지만 MacroTex는 만든다.
#define		AVT_PART_STYLE_FULLSKIN		0x00000008		// 통짜 Body

#define		AVT_PART_STYLE_BODY			(AVT_PART_STYLE_SKIN|AVT_PART_STYLE_EXCHANGE|AVT_PART_STYLE_FULLSKIN)		// 몸으로 인식

struct AVT_PARTINFO		// [array initialize]
{
	AVT_PART_STYLE	m_Style;
	i3::fixed_string m_strName;
	i3::fixed_string m_strAttachBone;
	i3::fixed_string m_strDataBaseName;
};

namespace AVATAR
{
	enum VIEW
	{
		VIEW_NA = -1,
		VIEW_1P = 0,
		VIEW_3P = 1
	};

	struct TEXINFO		// array initializer
	{
		EQUIP::ePART	m_Part;
		INT32			m_X;
		INT32			m_Y;
		INT32			m_W;
		INT32			m_H;
		i3::rc_string m_strPathName;
	};

	enum HEAD_TYPE {
		HEAD_TYPE_NONE = -1,
		HEAD_TYPE_NORMAL = 0,
		HEAD_TYPE_COVER,
		HEAD_TYPE_DEEP,
		HEAD_TYPE_NOTHING,

		HEAD_TYPE_MAX
	};
	
	enum TEX_TYPE {
		TEX_DIFF,
		TEX_NORM,
		TEX_SPEC,
		TEX_REFM,

		TEX_COUNT
	};

	i3Texture *		LoadTex( const char * pszTexName, TEAM_TYPE team);

	AVT_PART_STYLE			getPartStyle( EQUIP::ePART part);
	const i3::fixed_string&	getPartName( EQUIP::ePART part);
	const AVT_PARTINFO *	getPartInfo( EQUIP::ePART part);
	const TEXINFO *			getPartTexInfo( VIEW view, EQUIP::ePART part);
	const i3::fixed_string&	getDataBaseName( EQUIP::ePART part);
	void					SetTexturePathName(const char* szPathName, VIEW view, EQUIP::ePART part);
};


struct AVT_PARTPROP
{
	VEC4D m_vecScale = { 1.0f, 1.0f, 1.0f, 1.0f };// w = all
	VEC3D m_vecTrans;
	INT32 m_nParts = -1;
};

struct tagLOADMACROTEXDATA {
	INT32		_lodStep = 0;
	INT32		_offsetX = 0;
	INT32		_offsetY = 0;
	INT32		_destWidth = 0;
	INT32		_destHeight = 0;

	i3Texture *	_pSrcTex = nullptr;
	i3Texture *	_pDestTex = nullptr;

	const AVATAR::TEXINFO * _pTexInfo = nullptr;

	bool		_bUseMacroTexture = false;

	void *		_pOwner = nullptr;

	void	Clear( void)
	{
		I3_SAFE_RELEASE( _pSrcTex);
		I3_SAFE_RELEASE( _pDestTex);
	}

	void	CopyTo( tagLOADMACROTEXDATA * pDest) {
		pDest->_lodStep = _lodStep;
		pDest->_offsetX = _offsetX;
		pDest->_offsetY = _offsetY;
		pDest->_destWidth = _destWidth;
		pDest->_destHeight = _destHeight;

		pDest->_pSrcTex = _pSrcTex;
		pDest->_pDestTex = _pDestTex;
		pDest->_pTexInfo = _pTexInfo;

		pDest->_bUseMacroTexture = _bUseMacroTexture;

		pDest->_pOwner = _pOwner;
	}
};

#endif

