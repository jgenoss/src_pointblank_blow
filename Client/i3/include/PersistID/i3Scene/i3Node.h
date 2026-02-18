#if !defined( __I3_NODE_H)
#define __I3_NODE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3SceneDef.h"

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

#define	I3SG_NODE_CLASS_DEFINE( nodename) \
	I3_CLASS_DEFINE( nodename);\
	public:\
	static I3_TRACE_RESULT			I3SG_NODE_PROC( nodename)( i3Node * pNode, i3SceneTracer * pTracer);\
	static I3_TRACE_RESULT			CallTraceProc( i3Node * pNode, i3SceneTracer * pTracer)\
	{\
		return I3SG_NODE_PROC(nodename)( pNode, pTracer);\
	}

class i3Skeleton;

typedef struct _tagWrapBoundContext
{
	i3MatrixStack *		m_pStack;
	i3BoundBox *		m_pBound;
	i3Skeleton *		m_pSkel;
} I3_GETBOUND_INFO;

class I3_EXPORT_SCENE i3Node : public i3NamedElement
{
	I3SG_NODE_CLASS_DEFINE( i3Node);

protected:
	i3List			m_ChildList;
	i3Node *		m_pParent;

	UINT32			m_Flag;

	i3BoundBox *	m_pBound;

	I3_NODE_PROCESS_PROC	m_pTraverseProc;

protected:
	void			_PropergateFlag( UINT32 addFlag, UINT32 removeFlag);

	void			_setTraceProc( I3_NODE_PROCESS_PROC pProc)
	{
		m_pTraverseProc = pProc; 
	}

public:
	i3Node(void);
	virtual ~i3Node(void);

	INT32			GetChildCount(void)				{ return m_ChildList.GetCount(); }
	i3Node *		GetChild( INT32 idx)			{ return (i3Node *) m_ChildList.Items[idx]; }
	i3List *		GetChildList(void)				{ return & m_ChildList; }
	void			SetChild( INT32 idx, i3Node * pNode)	{ m_ChildList.Items[idx] = pNode; }
	void			SetOptimizeEnableWhenClear( BOOL bFlag)		{ m_ChildList.SetOptmizeEnableWhenClear( bFlag); }

	void			AddChild( i3Node * pNode);
	void			InsertChild( INT32 idx, i3Node * pNode);
	void			RemoveChild( i3Node * pNode);
	void			RemoveAllChild(void);
	void			RemoveFromParent(void);
	void			DetachChild( i3Node * pNode);
	BOOL			IsChild( i3Node * pNode);

	i3Node *		GetParent(void)					{ return m_pParent; }
	void			SetParent( i3Node * pParent);

	UINT32			GetFlag(void)					{ return m_Flag; }
	void			SetFlag( UINT32 val, bool bRecurs = false);
	void			AddFlag( UINT32 flag, bool bRecurs = false);
	void			RemoveFlag( UINT32 flag, bool bRecurs = false);
	BOOL			IsFlag( UINT32 flag)			{ return (m_Flag & flag) != 0;}

	i3BoundBox *	GetBound(void)					{ return m_pBound; }
	void			SetBound( i3BoundBox * pBox);

	void			GetWrappingBound( i3BoundBox * pBox);
	virtual void	_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	virtual void	OnUpdate( i3SceneTracer * pTracer);
	virtual	void	OnChangeTime( REAL32 tm);
	virtual void	OnDeactivate(void);
	virtual void	OnActivate(void);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	OnInstancing( void * pUserDefault = NULL);

	virtual bool	OnQueryDeactivateNode()	{	return true;}

	I3_TRACE_RESULT		Trace( i3SceneTracer * pTracer)
	{
		return m_pTraverseProc( this, pTracer);
	}

	I3_NODE_PROCESS_PROC	getTraceProc(void)	{ return m_pTraverseProc;  }

	static		void	ResetSortingTable(void);

	//특정한 역할을 하는 Flag함수
	void			DeactivateNode(bool bRecurse =  false);
	void			ActivateNode(bool bRecurse = false);
};

#define I3_SAFE_NODE_RELEASE( pNode)		if( (pNode) != NULL) { (pNode)->RemoveFromParent(); (pNode)->Release(); (pNode) = NULL; }

extern I3_EXPORT_SCENE i3Node *		g_pDebugNode;

#endif
