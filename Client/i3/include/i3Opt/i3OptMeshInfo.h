#if !defined( I3_OPT_MESH_INFO_H)
#define I3_OPT_MESH_INFO_H

// This class is used in Octree Builder for storing intermediate mesh informations.

enum PLACE_TAG
{
	PLACE_NEGATIVE = -1,
	PLACE_NA = 0,
	PLACE_POSITIVE = 1,
};

class MESH_INFO : public i3::class_common_pool<MESH_INFO>			// MATRIX멤버변수에 의한 16바이트 정렬 강제로 인한 VS2015경고 문제..(여기서는 풀로 돌려서 일단 막아봄)
{
public:
	i3Node *		m_pNode = nullptr;
	i3Geometry *	m_pGeo = nullptr;
	i3BoundBox		m_Bound;
	MATRIX			m_Transform;
	INT32			m_TriCount = 0;
	PLACE_TAG		m_idxPlace[3] = { PLACE_NA, PLACE_NA, PLACE_NA };

	MESH_INFO(void) {}
	~MESH_INFO(void)
	{
		I3_SAFE_RELEASE( m_pNode);
	}

	void			refresh(void);
	void			setNode( i3Node * pNode);

	MESH_INFO *		Cut( VEC4D * pPlane);

	INT32			getOrdinalIndex(void);
	void			setOrdinalIndex( INT32 idx);

#if defined( I3_DEBUG)
	void			VLog( bool bSideColor = true);
#endif

};

#endif
