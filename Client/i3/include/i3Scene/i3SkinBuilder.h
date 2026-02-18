#if !defined( __I3_SKIN_BUILDER_H)
#define __I3_SKIN_BUILDER_H

#include "i3Base/smart_ptr/shared_ptr.h"

class i3GeometryAttr;

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct MESH_TRI
{
	INT32	m_Index[3] = { 0, 0, 0 };
	INT32	m_Bone[12] = { 0 };
	INT32	m_BoneCount = 0;

	void	init(void)
	{
		m_BoneCount = 0;
	}

	void	Sort(void);
	void	AddBone( INT32 idx);

	static int	Compare( const void * p1, const void * p2);
};

#define		LIMIT_BONE_COUNT		20
#define		MAX_BONE				256

struct ALIGN4 I3_DRAW_GROUP
{
	I3_DRAW_GROUP()
	{
		for(int i = 0; i < LIMIT_BONE_COUNT; i++)
			m_Bone[ i ] = 0xFF;			// 본 매트릭스 인덱스
	}
	~I3_DRAW_GROUP(){}

	INT32	m_idxStart = 0;				// 인덱스의 시작 위치
	INT32	m_PrimCount = 0;				// 인덱스 카운터
	
	INT32	m_BoneCount = 0;			// 사용된 매트릭스 수
	UINT8	m_Bone[ LIMIT_BONE_COUNT ];			// 본 매트릭스 인덱스

	void	init(void)
	{
		m_idxStart = 0;
		m_PrimCount = 0;
		m_BoneCount = 0;
	}

	INT32	AddBone( INT32 idxBone)
	{
		I3ASSERT( m_BoneCount < LIMIT_BONE_COUNT);
		I3ASSERT( idxBone < 256);

		m_Bone[ m_BoneCount] = (INT8) idxBone;
		m_BoneCount++;

		return m_BoneCount - 1;
	}
};

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

typedef i3::shared_ptr<i3::vector<I3_DRAW_GROUP*> >		I3_DRAW_GROUP_LIST_PTR;

class I3_EXPORT_SCENE i3SkinBuilder : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SkinBuilder, i3ElementBase);
protected:
	INT32		m_LimitBoneCount = LIMIT_BONE_COUNT;

	i3VertexArray *	m_pVA = nullptr;
	I3_DRAW_GROUP_LIST_PTR		m_pGroupList;

	INT32		m_BoneMap[MAX_BONE] = { 0 };
	INT32		m_UsedBoneCount = 0;

	MESH_TRI *	m_pTri = nullptr;

	INT32		m_TriCount = 0;
	INT32		m_cntBone = 0;
	INT32		m_idxStart = 0;

protected:
	void		Prepare( i3GeometryAttr * pGeoAttr);
	void		BuildMesh( i3GeometryAttr * pGeoAttr, bool bOnMem);
	void		AdjustBlendIndex( I3_DRAW_GROUP * pGroup);

	void		ClearBoneMap(void);
	bool		TryAdd( I3_DRAW_GROUP * pGroup, INT32 idxTri);
	void		AddGroup( I3_DRAW_GROUP * pGroup);

	void		DumpTri(void);

public:
	virtual ~i3SkinBuilder(void);

	void			Build( i3GeometryAttr * pGeoAttr, bool bOnMem = false);

	I3_DRAW_GROUP_LIST_PTR&		getGroupList(void)		{ return m_pGroupList; }
	i3VertexArray *	getVertexArray(void)				{ return m_pVA; }
};

#endif