#if !defined( __I3_NODE_H)
#define __I3_NODE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3SceneDef.h"

typedef UINT32 I3_NODEFLAG;
#define	I3_NODEFLAG_DISABLE						0x00000001		/// Node를 Disable 시킨다. (Tranverse 하지 않는다.)
#define	I3_NODEFLAG_INVISIBLE					0x00000002		/// Node를 Invisible 상태로 만든다. (Traverse 하지만, Rendering 되지 않는다.)
#define I3_NODEFLAG_MUSTBEPROCESS				0x00000004		/// Node가 Culling과는 무관하게 항상 처리되어야 함을 나타낸다.
#define I3_NODEFLAG_TRANSFORM					0x00000008		/// Transform을 가지고 있는 Node라는 의미이다.
#define I3_NODEFLAG_TRANSPARENT					0x00000010
#define I3_NODEFLAG_STATIC						0x00000020		/// Runtime에 변경되지 않는 Node임을 의미한다. (i3GPUCommandList를 생성한다.)
#define I3_NODEFLAG_DELETEORIGINALNODE			0x00000040		/// Static Node인 경우, i3GPUCommandList를 생성하고 나서, 원본 Node들은 제거한다.
#define I3_NODEFLAG_CULLED						0x00000080
#define I3_NODEFLAG_CACHEMATRIX					0x00000100		/// i3Tranform Node의 경우, Cache Matrix에 현재 Matrix Stack의 Top을 복사한다.
#define I3_NODEFLAG_VOLATILE					0x00000200		/// Runtime용 Node임을 나타낸다. 저장하지 않는다.
#define I3_NODEFLAG_CONTROL						0x00000400		// Export 시에 원본 데이터에서부터 만들어진 것이 아니라, Sg.의 제어용으로 이후에 덧붙여진 Node임을 나타낸다.
#define	I3_NODEFLAG_MANUAL_UPDATE				0x00000800
#define	I3_NODEFLAG_DYNAMIC_UPDATE				0x00001000		// Dynamic Simulation에 의해, Matrix Stack이 아닌 방법으로 World-Space Transform이 생기는 경우. (i3SceneObject용)
#define I3_NODEFLAG_DISABLE_SHADOW_CASTER		0x00002000
#define	I3_NODEFLAG_DISABLE_SHADOW_RECEIVER		0x00004000	
#define I3_NODEFLAG_USER						0x00010000
#define I3_NODEFLAG_NO_WORK						0x00020000		// Disable과 유사하나, 해당 Node에 대해서만 Disable 시키는 점이 다르다.
#define I3_NODEFLAG_ATTACH_POINT				0x00040000
#define	I3_NODEFLAG_FORCE_CULL					0x00080000
#define I3_NODEFLAG_ALPHASORT					0x00100000	
#define I3_NODEFLAG_DISABLE_RENDER_OFFSCREEN	0x00200000		// 메인카메라 아닌 카메라에서 렌더링을 하지 않도록 하는 플래그 추가.
																
#define I3_NODEFLAG_DYNAMIC_OCCLUDEE			0x00400000		// UPDATE_AABB와 병행되어야함..(0x00800000 과 0x00400000을 합침)
#define I3_NODEFLAG_OCC_CULL_VISIBLE			0x00800000

class i3SceneTracer;
class i3Node;

enum I3_TRACE_RESULT
{
	I3_TRACE_RESULT_CONTINUE = 0,
	I3_TRACE_RESULT_IGNORE,
	I3_TRACE_RESULT_STOP,
};

typedef I3_TRACE_RESULT (*I3_NODE_PROCESS_PROC)( i3Node * pNode, i3SceneTracer * pTracer);

#define I3SG_NODE_PROC( nodename)	nodename::TraceProc##nodename

#define	I3SG_NODE_CLASS_DEFINE( nodename, basename) \
	I3_EXPORT_CLASS_DEFINE( nodename, basename);\
	public:\
	static I3_TRACE_RESULT			I3SG_NODE_PROC( nodename)( i3Node * pNode, i3SceneTracer * pTracer);\
	static I3_TRACE_RESULT			CallTraceProc( i3Node * pNode, i3SceneTracer * pTracer)\
	{\
		return I3SG_NODE_PROC(nodename)( pNode, pTracer);\
	}

class i3Skeleton;

struct I3_GETBOUND_INFO
{
	i3MatrixStack *		m_pStack = nullptr;	// out
	i3BoundBox *		m_pBound = nullptr;	// out
	i3Skeleton *		m_pSkel = nullptr;	// out

	bool				m_bShadow = false;	// in
};

struct I3_ONINSTANCING_INFO
{
	i3ElementBase *		_pBase = nullptr;
	i3MatrixStack *		_pMatrixStack = nullptr;
} ;

namespace i3 { class aabb; }
namespace i3 { class occ_mesh_set;  }
namespace i3 { class octree_item; }
namespace i3 { class occ_culler;  }

class I3_EXPORT_SCENE i3Node : public i3NamedElement
{
	I3SG_NODE_CLASS_DEFINE( i3Node, i3NamedElement);

protected:
	i3::vector<i3Node*>	m_ChildList;
	i3Node *			m_pParent = nullptr;

	I3_NODEFLAG		m_Flag = 0;

	i3BoundBox *	m_pBound = nullptr;
	i3::aabb*		m_pAABB = nullptr;		// m_pBound는 기존바운딩박스이고, octree,occ 호환성이 떨어진다고 추정되서 별도로 만들어둔다..개별존재해야하므로, 레퍼런스카운팅이 요구되지 않는다..
	i3::octree_item*		m_octree_item = nullptr;

	I3_NODE_PROCESS_PROC	m_pTraverseProc;

	i3Node *		m_pNodeLinkLeft = nullptr;
	i3Node *		m_pNodeLinkRight = nullptr;

protected:
	void			_Link(void);
	void			_Unlink(void);

	void			_PropergateFlag( UINT32 addFlag, UINT32 removeFlag);

	void			_setTraceProc( I3_NODE_PROCESS_PROC pProc)
	{
		m_pTraverseProc = pProc; 
	}

public:
	i3Node(void);
	virtual ~i3Node(void);

	INT32			GetChildCount(void)				{ return INT32(m_ChildList.size()); }
	i3Node *		GetChild( INT32 idx)			{ return m_ChildList[idx]; }
	i3::vector<i3Node*>&		GetChildList(void)	{ return m_ChildList; }
	const i3::vector<i3Node*>&	GetChildList(void) const { return m_ChildList; }

	void			SetChild( INT32 idx, i3Node * pNode)	{ m_ChildList[idx] = pNode; }

	void			AddChild( i3Node * pNode);
	void			InsertChild( INT32 idx, i3Node * pNode);
	void			RemoveChild( i3Node * pNode);
	void			RemoveAllChild(void);
	void			RemoveFromParent(void);
	void			DetachChild( i3Node * pNode);
	bool			IsChild( i3Node * pNode);

	void			ChangeParent( i3Node * pParent);

	i3Node *		GetParent(void)					{ return m_pParent; }
	void			SetParent( i3Node * pParent);

	I3_NODEFLAG		GetFlag(void)					{ return m_Flag; }
	void			SetFlag( I3_NODEFLAG val, bool bRecurs = false);
	void			AddFlag( I3_NODEFLAG flag, bool bRecurs = false);
	void			RemoveFlag( I3_NODEFLAG flag, bool bRecurs = false);
	bool			IsFlag( I3_NODEFLAG flag)			{ return (m_Flag & flag) != 0;}

	i3BoundBox *	GetBound(void)					{ return m_pBound; }
	void			SetBound( i3BoundBox * pBox);

	
	void			GetWrappingBound( i3BoundBox * pBox, bool bShadow = false);
	virtual void	_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	virtual void	CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled);

	virtual bool	OnQueryInFrustum( i3SceneTracer * pTracer);

	virtual void	OnUpdate( i3SceneTracer * pTracer);
	virtual	void	OnChangeTime( REAL32 tm);
	virtual void	OnDeactivate(void);
	virtual void	OnActivate(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual	UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void	OnInstancing( I3_ONINSTANCING_INFO * pInfo);
	virtual void	Instantiate( void);

	virtual void	OnLostDevice( bool bLostDevice);
	virtual void	OnRevive( i3RenderContext * pCtx);

	virtual bool	OnQueryDeactivateNode()	{	return true;}

	I3_TRACE_RESULT		Trace( i3SceneTracer * pTracer)
	{
		return m_pTraverseProc( this, pTracer);
	}
	virtual void		PreTrace(i3SceneTracer * pTracer);

	void			EnableOccludee(i3::occ_culler* pCuller, bool isSceneAttached, bool bEnable);
	void			EnableUpdateAABB(bool bEnable);
	bool			IsEnabledUpdateAABB() const { return m_pAABB != nullptr; }
	const i3::aabb*	GetAABB() const { return m_pAABB; }
	bool			IsEnabledOccludee() const;

	i3::octree_item*		GetOctreeItem() const { return m_octree_item;  }

	I3_NODE_PROCESS_PROC	getTraceProc(void)	{ return m_pTraverseProc;  }

	static		void	ResetSortingTable(void);

	static		void	CallLostDevice( bool bLostDevice);
	static		void	CallRevive( i3RenderContext * pCtx);

	//특정한 역할을 하는 Flag함수
	void			DeactivateNode(bool bRecurse =  false);
	void			ActivateNode(bool bRecurse = false);

	static void		DeleteStaticMember();


#if defined( I3_DEBUG)
private:
	i3ElementBase *	m_pOwner = nullptr;		// LOD 같은 경우는 Node가 아니다.

public:
	void			SetOwner( i3ElementBase * pOwner)		{ m_pOwner = pOwner; }
	i3ElementBase *	GetOwner( void) const					{ return m_pOwner; }
#endif
};

#if defined( I3_DEBUG)
#define I3_SAFE_NODE_RELEASE( pNode)		if( (pNode) != nullptr) { (pNode)->RemoveFromParent(); (pNode)->Release(__FILE__,__LINE__); (pNode) = nullptr; }
#else
#define I3_SAFE_NODE_RELEASE( pNode)		if( (pNode) != nullptr) { (pNode)->RemoveFromParent(); (pNode)->Release(); (pNode) = nullptr; }
#endif

extern I3_EXPORT_SCENE i3Node *		g_pDebugNode;

#endif
