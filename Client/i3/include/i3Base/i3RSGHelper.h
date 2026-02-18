#ifndef __I3_RSG_HELPER_H__
#define __I3_RSG_HELPER_H__

#include "i3Base/itl/any.h"

#define I3RSC_FIELD_TYPE_MASK		0x000000FF
#define	I3RSC_FIELD_TYPE_STRING		0x00000001	//문자열
#define I3RSC_FIELD_TYPE_INDEX		0x00000002	//INDEX
#define	I3RSC_FIELD_TYPE_VALUE		0x00000004	//정수
#define I3RSC_FIELD_TYPE_REALVALUE	0x00000008	//실수
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

struct I3RSC_STYLE_DEF
{
	INT32			m_nFieldPackStyle;		//fpdef의 style
	i3::rc_string	m_strFieldPackName;
	char			m_nFieldCount;
};
#define I3RSC_TYPE_COUNT			0x0000000A
static I3RSC_STYLE_DEF g_RSCStyleDef[I3RSC_TYPE_COUNT] =
{
	//Scene
	{ I3RSC_FIELDPACK_TYPE
	| I3RSC_FIELDPACK_PATH
	| I3RSC_FIELDPACK_LAYER
	| I3RSC_FIELDPACK_CLASS
	| I3RSC_FIELDPACK_COMMON							, "SCENE"		, 5 },

	{
		I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		| I3RSC_FIELDPACK_LAYER | I3RSC_FIELDPACK_CLASS
	| I3RSC_FIELDPACK_COMMON							, "CHARA"		, 5 },

	{
		I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
		| I3RSC_FIELDPACK_LAYER | I3RSC_FIELDPACK_CLASS
	| I3RSC_FIELDPACK_COMMON							, "WORLD"		, 5 },

	//Animation
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
	| I3RSC_FIELDPACK_COMMON,							"ANIMATION"	, 2 },

	//Texture
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH
	| I3RSC_FIELDPACK_COMMON							, "TEXTURE"		, 3 },

	//Sound
	{ I3RSC_FIELDPACK_TYPE
	| I3RSC_FIELDPACK_PATH
	| I3RSC_FIELDPACK_SOUND_MAXDIST
	| I3RSC_FIELDPACK_SOUND_MINDIST
	| I3RSC_FIELDPACK_SOUND_DEFVOLUME
	| I3RSC_FIELDPACK_SOUND_LISTVOLUME
	| I3RSC_FIELDPACK_COMMON
	, "SOUND"		, 7 },

	//Binary
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "BINARY"	,	 2 },
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "ANIMPACK"	, 2 },
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "OBJECT"		, 2 },
	{ I3RSC_FIELDPACK_TYPE | I3RSC_FIELDPACK_PATH			, "UI"			, 2 }
};
struct I3RSC_FIELD_STYLE_DEF
{
	INT32				m_nFieldStyle;
	i3::rc_string		m_strFieldName;
};
#define I3RSC_FIELD_ATTR_UNEDITABLE	0x10000000
static I3RSC_FIELD_STYLE_DEF g_szFieldInfoDef[I3RSC_FIELDPACK_COUNT] =
{
	{ I3RSC_FIELD_TYPE_STRING | I3RSC_FIELD_ATTR_UNEDITABLE	, "Type" },
	{ I3RSC_FIELD_TYPE_STRING									, "Path" },
	{ I3RSC_FIELD_TYPE_INDEX									, "Layer" },
	{ I3RSC_FIELD_TYPE_STRING									, "Class" },
	{ I3RSC_FIELD_TYPE_STRING									, "Common" },
	{ I3RSC_FIELD_TYPE_INDEX									, "Sound_Style" },
	{ I3RSC_FIELD_TYPE_REALVALUE								, "Sound_MaxDist" },
	{ I3RSC_FIELD_TYPE_REALVALUE								, "Sound_MinDist" },
	{ I3RSC_FIELD_TYPE_VALUE									, "DefaultVolume" },
	{ I3RSC_FIELD_TYPE_VALUE									, "ListenerVolume" }
};

static void RSGParser(i3IniParser * pParser, INT32 nType, i3::vector<i3::any> &FieldList)
{
	INT32 nSt = g_RSCStyleDef[nType].m_nFieldPackStyle & I3RSC_FIELDPACK_MASK;
	nSt = nSt >> 16;

	for (INT32 i = 2; i < I3RSC_FIELDPACK_COUNT; ++i)
	{
		if (nSt & (1 << i))
		{
			char conv[32];
			char szFieldName[32];

			INT32 nStyle = g_szFieldInfoDef[i].m_nFieldStyle;
			i3::generic_string_copy(szFieldName, g_szFieldInfoDef[i].m_strFieldName);

			i3::any data;
			if (nStyle & I3RSC_FIELD_TYPE_INDEX)
			{
				INT32 val;
				pParser->GetValue(szFieldName, 0, &val);
				data = val;
			}
			else if (nStyle & I3RSC_FIELD_TYPE_STRING)
			{
				pParser->GetString(szFieldName, "", conv, sizeof(conv));
				data = i3::rc_string(conv);
			}
			else if (nStyle & I3RSC_FIELD_TYPE_VALUE)
			{

				INT32 val;
				pParser->GetValue(szFieldName, 0, &val);
				data = val;

			}
			else if (nStyle & I3RSC_FIELD_TYPE_REALVALUE)
			{

				REAL32 val;
				pParser->GetValue(szFieldName, 0.0f, &val);
				data = (float)val;
			}

			FieldList.push_back(data);
		}
	}
}
#endif //__I3_PARSER_HELPER_H__
