#if !defined( __I3_COLLIDEE_H)
#define __I3_COLLIDEE_H

#include "i3Base.h"
#include "i3Math.h"

// 거리에 따라 Collision Test를 On/Off하는 기능을 설정한다.
#define		I3_COLLISION_STYLE_DISTANCE_CHECK		0x00000001

// 결과에 대해 사용자-처리할 것인지, 그렇지 않으면 Collision Engine에 의한 
// 물리 계산을 사용할 것인지 선택한다.
#define		I3_COLLISION_STYLE_PHYSICS				0x00000002

// Result에 대해 사용자-차리하는 경우, 교점(Intersection)을 산출할 것인지를 나타낸다.
#define		I3_COLLISION_STYLE_CALC_INTERSECT		0x00000004

// Result에 대해 사용자-처리하는 경우, 충돌점에 대한 Normal을 산출할 것인지를 나타낸다.
#define		I3_COLLISION_STYLE_CALC_NORMAL			0x00000008

// 계산에 속도(Velocity)를 반영할 것인지를 나타낸다.
// 그렇지 않다면, 단순히 현재의 Collidee Data에 대한 검사만
// 수행한다.
#define		I3_COLLISION_STYLE_VELOCITY				0x00000010

// Collision Test된 결과들 중에 가장 가까운 하나만 구하는 것.
#define		I3_COLLISION_STYLE_NEAREST_ONE			0x00000020

// Collision Test시 노멀 벡터를 무시한 양방향 테스트 여부.
#define		I3_COLLISION_STYLE_BOTH_SIDE			0x00000040

enum I3_TERRAIN_TYPE
{
	I3_TERRAIN_NA,							// Not-Assigned
	I3_TERRAIN_CONCRETE = 1,				// 콘크리트
	I3_TERRAIN_STEEL,						// 철제
	I3_TERRAIN_GROUND,						// 흙
	I3_TERRAIN_WOOD,						// 나무
	I3_TERRAIN_SNOW,						// 눈
	I3_TERRAIN_WATER_DEEP,					// 물 (깊은)
	I3_TERRAIN_WATER_SHALLOW,				// 물 (얕은)
	I3_TERRAIN_WET_CLAY,					// 진흙
	I3_TERRAIN_GRASS,						// 잔디
	I3_TERRAIN_MARBLE,						// 대리석
	I3_TERRAIN_FALLLEAF,					// 나뭇잎
	I3_TERRAIN_CONCRETE_THIN,				// 얇은 콘크리트
	I3_TERRAIN_STEEL_THIN,					//
	I3_TERRAIN_WOOD_THIN,
	I3_TERRAIN_MARBLE_THIN,
	I3_TERRAIN_PAPER,						// 종이
	I3_TERRAIN_GLASS,						// 유리
	I3_TERRAIN_PLASTIC,						// 플라스틱
	I3_TERRAIN_RUBBER,						// 고무
	I3_TERRAIN_CLOTH,						// 천
	I3_TERRAIN_GROUND_THIN,
	I3_TERRAIN_SNOW_THIN,
	I3_TERRAIN_WET_CLAY_THIN,
	I3_TERRAIN_GRASS_THIN,
	I3_TERRAIN_PAPER_THIN,
	I3_TERRAIN_FALLLEAF_THIN,
	I3_TERRAIN_GLASS_THIN,
	I3_TERRAIN_PLASTIC_THIN,
	I3_TERRAIN_RUBBER_THIN,
	I3_TERRAIN_CLOTH_THIN,
	I3_TERRAIN_BARBEDWIRE,

	I3_TERRAIN_BLOOD,						// 피
	I3_TERRAIN_TEMP1,						// 임시 1 ( 이것은 리니지)
	I3_TERRAIN_TEMP2,						// 임시 2

	I3_TERRAIN_TYPE_COUNT
};

typedef struct ALIGN2 _tagCollideeTriangle32
{
	UINT32		m_Index[3];
	UINT32		m_NormalIndex;
	UINT8		m_Reverb;					// 0 - No Reverb, 1, 2, 3 ..
	INT8		m_Elasticity;				// -128 ~ 127
	UINT8		m_Terrain;
	INT8		m_ReflectMapIndex;
	UINT8		m_Color[4];
} I3_COLLIDEE_TRI32 ALIGN2;


class I3_EXPORT_SCENE i3Collidee : public i3PersistantElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3Collidee);

protected:

public:
	i3Collidee(void);

	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest) = 0;

	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

class i3ColliderSet;

typedef struct _tagi3CollisionResult
{
	i3Collidee *		m_pSrc;
	i3ColliderSet *		m_pDestColliderSet;
	i3Collidee *		m_pDest;
	I3_COLLIDEE_TRI32 *	m_pTri;
	VEC3D				m_Normal;
	VEC3D				m_Intersect;
	REAL32				m_T;
} I3_COLLISION_RESULT;

typedef struct _tagi3CollisionResultContext
{
	I3_COLLISION_RESULT *	m_pResult;
	i3ColliderSet *			m_pDestColliderSet;
	INT32					m_Count;
	INT32					m_LimitCount;
	UINT32					m_Style;
	REAL32					m_LastT;
} I3_COLLISION_RESULT_CONTEXT;

#endif

