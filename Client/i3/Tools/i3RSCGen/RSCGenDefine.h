#ifndef _RSCGEN_DEFINE_H__
#define _RSCGEN_DEFINE_H__

//
// C++의 응용프로그래밍에서는 비트플래그는 지양해야한다...배우지는 말것..(2014.04.15.수빈)
//

#define	I3RSC_ATTR_FOLDER			0x10000000

////////////////////////////////////////////////////////////////////////////////
//필드의 정보 형태
#define I3RSC_FIELD_TYPE_MASK		0x000000FF
#define	I3RSC_FIELD_TYPE_STRING		0x00000001	//문자열
#define I3RSC_FIELD_TYPE_INDEX		0x00000002	//INDEX
#define	I3RSC_FIELD_TYPE_VALUE		0x00000004	//정수
#define I3RSC_FIELD_TYPE_REALVALUE	0x00000008	//실수

//필드의 EDIT 가능 여부
#define I3RSC_FIELD_ATTR_UNEDITABLE	0x10000000

////////////////////////////////////////////////////////////////////////////////
//Field Pack 이 가지는 필드의 종류들
#define I3RSC_FIELDPACK_MASK				0x0FFF0000
#define I3RSC_FIELDPACK_TYPE				0x00010000
#define I3RSC_FIELDPACK_PATH				0x00020000
#define I3RSC_FIELDPACK_LAYER				0x00040000
#define I3RSC_FIELDPACK_CLASS				0x00080000
#define I3RSC_FIELDPACK_COMMON				0x00100000
#define I3RSC_FIELDPACK_SOUND_STYLE			0x00200000
#define I3RSC_FIELDPACK_SOUND_MAXDIST		0x00400000
#define I3RSC_FIELDPACK_SOUND_MINDIST		0x00800000
#define I3RSC_FIELDPACK_SOUND_LISTVOLUME	0x01000000
#define I3RSC_FIELDPACK_SOUND_DEFVOLUME		0x02000000
#define I3RSC_FIELDPACK_COUNT				10

////////////////////////////////////////////////////////////////////////////////
// Field Pack을 묶음으로하는 rsc의 형태들
#define I3RSC_TYPE_MASK				0x000000FF
#define I3RSC_TYPE_SCENE			0x00000000
#define I3RSC_TYPE_CHARA			0x00000001
#define I3RSC_TYPE_WORLD			0x00000002
#define I3RSC_TYPE_ANIMATION		0x00000003
#define I3RSC_TYPE_TEXTURE			0x00000004
#define I3RSC_TYPE_SOUND			0x00000005
#define I3RSC_TYPE_BINARY			0x00000006
#define I3RSC_TYPE_ANIMPACK			0x00000007
#define I3RSC_TYPE_OBJECT			0x00000008
#define I3RSC_TYPE_UI				0x00000009
#define I3RSC_TYPE_COUNT			0x0000000A			// 0x10을 일단 0x0A로 변경함..(2014.04.15.수빈)					

#define I3RSC_TYPE_TYPEROOT			0x00000F00


//////////////////////////////////////////////////////////////////////////////////////
// DEFINE FIELD PACK
// 이부분은 다른곳으로 옮기는것이 좋을것으로 사료됨
struct I3RSC_FIELD_STYLE_DEF
{
	INT32				m_nFieldStyle;
	i3::rc_string		m_strFieldName;
};


//각 필드값을 선언
static I3RSC_FIELD_STYLE_DEF g_szFieldInfoDef[I3RSC_FIELDPACK_COUNT] = 
{
	{	I3RSC_FIELD_TYPE_STRING | I3RSC_FIELD_ATTR_UNEDITABLE	, "Type"			},
	{	I3RSC_FIELD_TYPE_STRING									, "Path"			},
	{	I3RSC_FIELD_TYPE_INDEX									, "Layer"			},
	{	I3RSC_FIELD_TYPE_STRING									, "Class"			},
	{	I3RSC_FIELD_TYPE_STRING									, "Common"			},
	{	I3RSC_FIELD_TYPE_INDEX									, "Sound_Style"		},
	{	I3RSC_FIELD_TYPE_REALVALUE								, "Sound_MaxDist"	},
	{	I3RSC_FIELD_TYPE_REALVALUE								, "Sound_MinDist"	},
	{	I3RSC_FIELD_TYPE_VALUE									, "DefaultVolume"	},
	{	I3RSC_FIELD_TYPE_VALUE									, "ListenerVolume"	}
};

/////////////
struct I3RSC_STYLE_DEF
{
	INT32			m_nFieldPackStyle;		//fpdef의 style
	i3::rc_string	m_strFieldPackName;	
	char			m_nFieldCount;
};

static I3RSC_STYLE_DEF g_RSCStyleDef[I3RSC_TYPE_COUNT] = 
{
	//Scene
	{	I3RSC_FIELDPACK_TYPE
		|	I3RSC_FIELDPACK_PATH 
		| 	I3RSC_FIELDPACK_LAYER 
		|	I3RSC_FIELDPACK_CLASS 
		|	I3RSC_FIELDPACK_COMMON							, "SCENE"		, 5},

	{
		I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		| I3RSC_FIELDPACK_LAYER | I3RSC_FIELDPACK_CLASS
		| I3RSC_FIELDPACK_COMMON							, "CHARA"		, 5	},

	{
		I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		| I3RSC_FIELDPACK_LAYER | I3RSC_FIELDPACK_CLASS
		| I3RSC_FIELDPACK_COMMON							, "WORLD"		, 5 },

	//Animation
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		|	I3RSC_FIELDPACK_COMMON,							"ANIMATION"	, 2	},

    //Texture
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		|	I3RSC_FIELDPACK_COMMON							, "TEXTURE"		, 3	},

	//Sound
	{	I3RSC_FIELDPACK_TYPE 
		|	I3RSC_FIELDPACK_PATH 
		| 	I3RSC_FIELDPACK_SOUND_MAXDIST 
		|	I3RSC_FIELDPACK_SOUND_MINDIST			
		|	I3RSC_FIELDPACK_SOUND_DEFVOLUME
		|	I3RSC_FIELDPACK_SOUND_LISTVOLUME
		|	I3RSC_FIELDPACK_COMMON
															, "SOUND"		, 7	},

	//Binary
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "BINARY"	,	 2	},
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "ANIMPACK"	, 2	},
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "OBJECT"		, 2	},
	{	I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "UI"			, 2	}
};

#define	I3RSC_LAYER_TYPECOUNT	4
static i3::rc_string g_LayerTypeDef[I3RSC_LAYER_TYPECOUNT] = 
{
	"CUSTOM",
	"OPAQUE",
	"TRANSPARENT",
	"EFFECT",
};
// 요기 까지 
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//하나의 Field를 정의 한다.
/*
class RSC_FIELD_INFO : public i3ElementBase
{
	I3_CLASS_DEFINE( RSC_FIELD_INFO, i3ElementBase);

public:
	UINT32		m_nFieldStyle;		//Field의 스타일
	char		m_szFieldName[32];	//Field의 이름
	void	*	m_pFieldData;

	RSC_FIELD_INFO();
	~RSC_FIELD_INFO();
};
*/

struct RSC_FIELD_INFO
{
	UINT32				m_nFieldStyle = I3RSC_TYPE_COUNT;	//Field의 스타일
	i3::rc_string		m_strFieldName;	//Field의 이름
	i3::any				m_FieldData;
};

typedef i3::shared_ptr<RSC_FIELD_INFO>		RSC_FIELD_INFO_PTR;

////////////////////////////////////////////////////////////////////////////////////////////////////
class CFieldPack : public i3::class_common_pool<CFieldPack>
{
//	I3_CLASS_DEFINE( CFieldPack, i3NamedElement);

public:
	CFieldPack();
	~CFieldPack();

	void		Create( UINT32	nRSCType);
private:
	void		CreateInternal( INT32 nFieldCount);

protected:
	INT32				m_nFieldCount;
	i3::vector<RSC_FIELD_INFO_PTR>	m_FieldList;
	//RSC_FIELD_INFO *	m_pFieldInfo;

public:
	void				InitField( RSC_FIELD_INFO * pInfo, const I3RSC_FIELD_STYLE_DEF& def);

	INT32				GetFieldCount()		{	return m_nFieldCount;}
	RSC_FIELD_INFO *	GetField( INT32 nIndex);	

	//save Load
	void				Save(i3IniParser * pParser);
	void				Load(i3IniParser * pParser, i3IniSectionInfo * pSectionInfo);

protected:
	i3IniParser		*	m_pIniParser;

	void				_SaveField( RSC_FIELD_INFO * pInfo);
	void				_LoadField( RSC_FIELD_INFO * pInfo, INT32 nType);

};

#include "i3Base/smart_ptr/scoped_ptr.h"

typedef i3::scoped_ptr<CFieldPack>		CFieldPackPtr;

////////////////////////////////////////////////////////////////////////////////////////
struct  RSC_DATA 
{

	UINT32		m_Attr = 0;
	i3::rc_string	m_strFilePath;
	CFieldPackPtr	m_spField;
};

INT32		Geti3RscTypeWithFileExt( const char * pszExt);

#endif