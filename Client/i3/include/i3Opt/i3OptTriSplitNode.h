#if !defined( __I3_OPT_TRISPLIT_NODE_H)
#define __I3_OPT_TRISPLIT_NODE_H

struct I3OPT_TRISPLIT_INFO
{
	i3VertexArray *	m_pVA = nullptr;
	i3IndexArray *	m_pIA = nullptr;
	INT32			m_idx[3] = { 0 };
	VEC3D			m_Pos[3];
} ;

// Tree 구조를 이용해 주어진 Geometry들의 Triangle을 공간적으로 분류하는 Class

// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3OptTriSplitNode : public i3TreeNode
{
	I3_CLASS_DEFINE( i3OptTriSplitNode, i3TreeNode);
protected:
	i3::vector<I3OPT_TRISPLIT_INFO*> m_TriList;
	
public:
	virtual ~i3OptTriSplitNode(void);

	INT32					getTriCount(void)					{ return INT32(m_TriList.size()); }
	I3OPT_TRISPLIT_INFO *	getTri( INT32 idx)					{ return m_TriList[idx]; }

	void				AddTri( i3VertexArray * pVA, i3IndexArray * pIA, INT32 idx0, INT32 idx1, INT32 idx2);

	i3Geometry *		CreateGatheredGeometry(void);
};

#endif