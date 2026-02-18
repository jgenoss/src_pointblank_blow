#include "i3SceneDef.h"
#include "i3Node.h"
#include "i3SceneTracer.h"
#include "i3Base/itl/vector_util.h"
#include "i3Math/structure/aabb.h"
#include "i3Math/octree/occ_item.h"
#include "i3Math/octree/occ_culler.h"
#include "i3Math/octree/octree.h"
#include "i3Math/octree/octree_item.h"

I3_CLASS_INSTANCE( i3Node);

struct	I3_NODE_SORT_INFO
{
	i3Node *		m_pNode = nullptr;
	REAL32			m_Depth = 0.0f;
};

//#define			MAX_NODE_DEPTH			2048
//#define			MAX_NODE_DEPTH			4096	// 제압모드에서 3500개를 사용하는 경우가 있다..? swoongoo
#define			MAX_NODE_DEPTH			8192	//월드팀 요청으로 Node사이즈 두배로 늘림

static I3_NODE_SORT_INFO	s_SortInfo[ MAX_NODE_DEPTH];
static INT32				s_SortCount;

static i3Node *				s_pNodeLinkHead = nullptr;
static i3Mutex *			s_pNodeLinkMutex = nullptr;

I3_EXPORT_SCENE
i3Node *		g_pDebugNode = nullptr;


void	i3Node::ResetSortingTable(void)
{
	s_SortCount = 0;
}

I3_TRACE_RESULT I3SG_NODE_PROC( i3Node)( i3Node * pNode, i3SceneTracer * pTracer)
{
	INT32 _i, idx;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	bool bProcess, bTransparent = false;
	INT32 idxStart, idxEnd, boundCount= 0;
	i3Node * pChild;

	// Child들의 Sorting
	{
		//VEC3D diffPos;

		idxStart = s_SortCount;		s_SortCount += pNode->GetChildCount();
		idxEnd = s_SortCount - 1;

		I3ASSERT( s_SortCount < MAX_NODE_DEPTH);

		for( _i = 0, idx = idxStart; _i < pNode->GetChildCount(); _i++, idx++)
		{
			pChild = pNode->GetChild( _i);

			s_SortInfo[ idx].m_pNode = pChild;
			
			{
				if( pChild->GetBound() == nullptr)
				{
					if( pChild->IsFlag( I3_NODEFLAG_TRANSPARENT))
					{
						s_SortInfo[ idx].m_Depth = 200000000.0f;

						bTransparent = true;
					}
					else
						s_SortInfo[ idx].m_Depth = 100000000.0f;
				}
				else
				{
					VEC3D mid;

					i3Vector::Copy( &mid, pChild->GetBound()->getCenter());
					i3Vector::TransformCoord( &mid, &mid, pTracer->GetModelViewMatrixStack()->GetTop());
					i3Vector::TransformCoord( &mid, &mid, pTracer->GetViewMatrix());

					s_SortInfo[ idx].m_Depth = i3Vector::GetZ( &mid);

					if( pChild->IsFlag( I3_NODEFLAG_TRANSPARENT))
						s_SortInfo[ idx].m_Depth = 200000000.0f - s_SortInfo[ idx].m_Depth;

					boundCount++;
				}
			}
		}

		// Sorting
		if( (boundCount > 1) || (bTransparent))
		{
			INT32 i, j;
			I3_NODE_SORT_INFO temp;

			for( i = 0; i < pNode->GetChildCount() - 1; i++)
			{
				for( j = i + 1; j < pNode->GetChildCount(); j++)
				{
					if( s_SortInfo[i + idxStart].m_Depth > s_SortInfo[j + idxStart].m_Depth)
					//if( s_SortInfo[i + idxStart].m_Depth < s_SortInfo[j + idxStart].m_Depth)
					{
						temp.m_pNode = s_SortInfo[i + idxStart].m_pNode;
						temp.m_Depth = s_SortInfo[i + idxStart].m_Depth;

						s_SortInfo[i + idxStart].m_pNode = s_SortInfo[j + idxStart].m_pNode;
						s_SortInfo[i + idxStart].m_Depth = s_SortInfo[j + idxStart].m_Depth;

						s_SortInfo[j + idxStart].m_pNode = temp.m_pNode;
						s_SortInfo[j + idxStart].m_Depth = temp.m_Depth;
					}
				}
			}
		}
	}

	for( _i = 0; _i < (pNode)->GetChildCount(); _i++)
	{
		//pChild = (pNode)->GetChild( _i);
		pChild = s_SortInfo[ _i + idxStart].m_pNode;

		bProcess = false;

		if((pChild->GetFlag() & pTracer->getDisableNodeMask()) == 0)
		{
			bProcess = true;
			bool bAttrStackEnabled = pTracer->isAttrStackManagerEnabled();
			bool bCulled = pTracer->getCulledState();

			if( pChild->IsFlag( I3_NODEFLAG_INVISIBLE | I3_NODEFLAG_FORCE_CULL))
			{
				// Invisible 상태는 상위 Node에서 한번이라도 발견되면, 하위 Node 모두에게 적용된다.
				pTracer->setCulledState( true);
				pTracer->setAttrStackManagerEnable( false);
			}
			else
			{
				if( !pChild->IsFlag( I3_NODEFLAG_MUSTBEPROCESS) )
				{
					// Occ-Culling..
					if (pChild->IsEnabledOccludee() && pTracer->getSgContext()->GetOccCuller() &&
						pTracer->isState(I3SG_TRACER_STATE_GEN_SHADOW) == false)
					{
						if (pChild->IsFlag(I3_NODEFLAG_OCC_CULL_VISIBLE) == false)
						{
							pTracer->setCulledState(true);
							pTracer->setAttrStackManagerEnable(false);

							if (pChild->IsFlag(I3_NODEFLAG_STATIC | I3_NODEFLAG_ALPHASORT))
								bProcess = false;
						}
						else
						{
							pTracer->setAttrStackManagerEnable(true && !pTracer->getCulledState());
						}
					}
					else 
					if ( pTracer->GetFrustumCullEnable() && (pChild->GetBound() != nullptr))
					{

						// View-Frustum Culling... (이제 이쪽은 구버전이고, 현재 일부 구현이 남은 상태임)
						// Bound Box에 대한 Culling 여부는 하위 Node를 완전히 포함하지 않는
						// 경우가 발생할 수 있으므로, 개별 Node에 대해서만 Cull 처리한다.
						if (pChild->OnQueryInFrustum(pTracer) == false)
						{
							pTracer->setAttrStackManagerEnable(false);

							if (pChild->IsFlag(I3_NODEFLAG_STATIC | I3_NODEFLAG_ALPHASORT))
								bProcess = false;
						}
						else
						{
							pTracer->setAttrStackManagerEnable(true && !pTracer->getCulledState());
						}
					}
				}
			}

			if( bProcess)
			{
				#if !defined( I3_NO_PROFILE)
					pTracer->addNodeCount();
				#endif

				if ( pChild->IsFlag( I3_NODEFLAG_ALPHASORT ) )
				{
					if ( pTracer->isState( I3SG_TRACER_STATE_GEN_SHADOW) == false )
					{
						pTracer->ActivateAlphaSortNode(pChild);
						Result = pChild->Trace( pTracer);
						pTracer->DeactivateAlphaSortNode(pChild);
					}
					else
					{
						Result = pChild->Trace( pTracer);
					}
				}
				else
				{
					Result = pChild->Trace( pTracer);
				}

				if( Result == I3_TRACE_RESULT_STOP)
				{
					if( pChild->IsFlag( I3_NODEFLAG_INVISIBLE | I3_NODEFLAG_FORCE_CULL))
						pTracer->setCulledState( bCulled);

					pTracer->setAttrStackManagerEnable( bAttrStackEnabled);

					return I3_TRACE_RESULT_STOP;
				}
			}

			if (pChild->IsFlag(I3_NODEFLAG_INVISIBLE | I3_NODEFLAG_FORCE_CULL))
				pTracer->setCulledState(bCulled);
			else
				if (pChild->IsEnabledOccludee())
					pTracer->setCulledState(bCulled);

			pTracer->setAttrStackManagerEnable( bAttrStackEnabled);
		}
	}

	s_SortCount -= pNode->GetChildCount();

	return Result;
}

i3Node::i3Node(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Node));
	
	m_ChildList.reserve( 16);

	_Link();
}

i3Node::~i3Node(void)
{
	delete m_octree_item;
	i3::delete_by_common_pool(m_pAABB);

	RemoveAllChild();

	SetParent( nullptr);

	SetBound( nullptr);

	_Unlink();

#if defined( I3_DEBUG)
	m_pOwner = nullptr;
#endif
}

void i3Node::DeleteStaticMember()
{
	I3_SAFE_RELEASE(s_pNodeLinkMutex);
}

static bool _NodeTerminateProc( void * pData)
{
	I3_SAFE_RELEASE( s_pNodeLinkMutex);

	return true;
}

void i3Node::_Link(void)
{
	if( s_pNodeLinkMutex == nullptr)
	{
		s_pNodeLinkMutex = i3Mutex::new_object();

		i3System::AddTerminateProc( 5, _NodeTerminateProc, nullptr);
	}

	///
	s_pNodeLinkMutex->Enter();

		m_pNodeLinkRight = s_pNodeLinkHead;

		if( s_pNodeLinkHead == nullptr)
		{
			s_pNodeLinkHead = this;
		}
		else
		{
			s_pNodeLinkHead->m_pNodeLinkLeft = this;
		}

		s_pNodeLinkHead = this;

	s_pNodeLinkMutex->Leave();
}

void i3Node::_Unlink(void)
{
//	I3ASSERT( s_pNodeLinkMutex != nullptr);	// 또 다른 스태틱전역변수가 i3Node인 경우가 있는데..
	if (s_pNodeLinkMutex)					// 안전한 소거를 위해 ASSERT대신...s_pNodeLinkMutex에 널조건을 겁니다.(2012.09.11.수빈)
		s_pNodeLinkMutex->Enter();

	{
		i3Node * pLeft = m_pNodeLinkLeft;
		i3Node * pRight = m_pNodeLinkRight;

		if( pLeft != nullptr)
			pLeft->m_pNodeLinkRight = pRight;

		if( pRight != nullptr)
			pRight->m_pNodeLinkLeft = pLeft;

		if( this == s_pNodeLinkHead)
			s_pNodeLinkHead = pRight;

		m_pNodeLinkLeft = nullptr;
		m_pNodeLinkRight = nullptr;
	}

	if (s_pNodeLinkMutex)
		s_pNodeLinkMutex->Leave();
}


void i3Node::AddChild( i3Node * pChild)
{
	m_ChildList.push_back( pChild);

	I3_MUST_ADDREF( pChild);

	pChild->SetParent( this);

	if( pChild->IsFlag( I3_NODEFLAG_MUSTBEPROCESS))
	{
		_PropergateFlag( I3_NODEFLAG_MUSTBEPROCESS, 0);
	}
}

void	i3Node::SetFlag( UINT32 val, bool bRecurs)
{
	INT32 i;
	i3Node * pChild;

	m_Flag = val;

	if( bRecurs)
	{
		for( i = 0; i < GetChildCount(); i++)
		{
			pChild = GetChild( i);

			pChild->SetFlag( val, true);
		}
	}
}

void	i3Node::AddFlag( UINT32 flag, bool bRecurs)
{
	INT32 i;
	i3Node * pChild;

	m_Flag |= flag;

	if( bRecurs)
	{
		for( i = 0; i < GetChildCount(); i++)
		{
			pChild = GetChild( i);

			pChild->AddFlag( flag, true);
		}
	}
}

void	i3Node::RemoveFlag( UINT32 flag, bool bRecurs)
{
	INT32 i;
	i3Node * pChild;

	m_Flag &= ~flag;

	if( bRecurs)
	{
		for( i = 0; i < GetChildCount(); i++)
		{
			pChild = GetChild( i);

			pChild->RemoveFlag( flag, true);
		}
	}
}

void i3Node::DeactivateNode(bool bRecurse)
{
	INT32 i;
	i3Node * pChild;

	if( !IsFlag( I3_NODEFLAG_DISABLE))
	{
		m_Flag |= I3_NODEFLAG_DISABLE;

		OnDeactivate();
	}

	if( bRecurse)
	{
		for( i = 0; i < GetChildCount(); i++)
		{
			pChild = GetChild( i);

			pChild->DeactivateNode( true);
		}
	}
}

void i3Node::ActivateNode(bool bRecurse)
{
	INT32 i;
	i3Node * pChild;

	if( IsFlag( I3_NODEFLAG_DISABLE))
	{
		m_Flag &= ~I3_NODEFLAG_DISABLE;

		OnActivate();
	}

	if( bRecurse)
	{
		for( i = 0; i < GetChildCount(); i++)
		{
			pChild = GetChild( i);

			pChild->ActivateNode( true);
		}
	}
}

void i3Node::_PropergateFlag( UINT32 addFlag, UINT32 removeFlag)
{
	i3Node * pTemp = this;

	do
	{
		pTemp->AddFlag( addFlag);
		pTemp->RemoveFlag( removeFlag);

		pTemp = pTemp->GetParent();
	} while( pTemp != nullptr);
}

void i3Node::InsertChild( INT32 idx, i3Node * pNode)
{
	m_ChildList.insert(m_ChildList.begin() + idx, pNode);
	I3_MUST_ADDREF( pNode);
	pNode->SetParent( this);
}

void i3Node::RemoveChild( i3Node * pChild)
{
//	if( m_ChildList.Remove( pChild) != -1)
	if (i3::vu::remove(m_ChildList, pChild) )
	{
		pChild->SetParent( nullptr);
		I3_MUST_RELEASE( pChild);
	}
}

void i3Node::RemoveAllChild(void)
{
	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3Node * pNode = m_ChildList[i];

		pNode->SetParent( nullptr);
		I3_MUST_RELEASE( pNode);				// 이 구조에서는 널값이 허용되지 않음이 유추됨..
	}

	m_ChildList.clear();
}

void i3Node::RemoveFromParent(void)
{
	if( m_pParent != nullptr)
	{
		m_pParent->RemoveChild( this);
	}
}

void i3Node::DetachChild( i3Node * pChild)
{
//	m_ChildList.Remove( pChild);
	i3::vu::remove(m_ChildList, pChild);
}

bool i3Node::IsChild( i3Node * pChild)
{
	return i3::vu::index_of(m_ChildList, pChild) != m_ChildList.size();
//	return m_ChildList.IndexOf( pChild) != -1;
}

void i3Node::ChangeParent( i3Node * pParent)
{
	I3ASSERT( pParent != nullptr);
	I3_MUST_ADDREF(this);

	RemoveFromParent();
	pParent->AddChild(this);

	I3_MUST_RELEASE(this);
}

void i3Node::SetParent( i3Node * pParent)
{
	m_pParent = pParent;
}

void i3Node::SetBound( i3BoundBox * pBox)
{
	I3_REF_CHANGE( m_pBound, pBox);
}

void i3Node::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3Node * pNode;

	List.push_back( this);

	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pNode = GetChild( i);

		if( pNode->IsFlag( I3_NODEFLAG_VOLATILE) == false)
		{
			pNode->OnBuildObjectList( List);
		}
	}
}

void i3Node::OnChangeTime( REAL32 tm)
{
	INT32 i;
	i3Node * pChild;

	for( i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);

		pChild->OnChangeTime( tm);
	}
}

void i3Node::OnDeactivate(void)
{
	INT32 i;
	i3Node * pChild;

	for( i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);

		pChild->OnDeactivate();
	}
}

void i3Node::OnActivate(void)
{
	INT32 i;
	i3Node * pChild;

	for( i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);

		pChild->OnActivate();
	}
}

bool i3Node::OnQueryInFrustum( i3SceneTracer * pTracer)
{
	return pTracer->IsInsideFrustum( GetBound());
}

void i3Node::OnUpdate( i3SceneTracer * pTracer)
{
	for(INT32 i = 0; i < GetChildCount(); i++)
	{
		i3Node* pChild = GetChild( i);

		if (pChild)
		{
			if( (pChild->GetFlag() & pTracer->getDisableNodeMask()) == 0)
				pChild->OnUpdate( pTracer);
		}
		else
		{
			i3Log("i3Node::OnUpdate", "%d Child of %s Parent = Null\n", i, this->GetName());
		}
	}
}

namespace
{

	class occludee_item_dynamic_i3Node : public i3::occludee_item_dynamic
	{
	public:
		occludee_item_dynamic_i3Node(i3Node* pOwner, i3::occ_culler* culler)
			: m_pOwner(pOwner), i3::occludee_item_dynamic(culler) {}

		virtual void		on_hidden_frustum_aabb()
		{
			m_pOwner->RemoveFlag(I3_NODEFLAG_OCC_CULL_VISIBLE);
		}

		virtual void		on_hidden_occluder_aabb()
		{
			m_pOwner->RemoveFlag(I3_NODEFLAG_OCC_CULL_VISIBLE);
		}

		virtual void		on_visible_occluder_aabb()
		{
			m_pOwner->AddFlag(I3_NODEFLAG_OCC_CULL_VISIBLE);
		}
		
	private:
		i3Node*			m_pOwner;
	};

}

void	i3Node::PreTrace(i3SceneTracer * pTracer)
{
	const INT32 numChild = GetChildCount();

	for (INT32 i = 0; i < numChild; ++i)
	{
		i3Node* pChild = GetChild(i);
		if (!pChild) 
			continue;

		I3_NODEFLAG ChildFlag = pChild->GetFlag();

		// VOLATILE의 경우에도 PreTrace 계산은 하지 않음..(자식쪽 재귀도 하지 않도록 한다..)
		if (pChild->IsFlag(I3_NODEFLAG_VOLATILE | I3_NODEFLAG_INVISIBLE | I3_NODEFLAG_FORCE_CULL)) // I3_NODEFLAG_NO_WORK
			continue;

		if ( ChildFlag & pTracer->getDisableNodeMask() )
			continue;
		
		if (pChild->m_pAABB != nullptr)
		{
			pChild->m_pAABB->reset();
			pTracer->PushAABBIntoStack(pChild->m_pAABB);
			pChild->PreTrace(pTracer);
			pTracer->PopAABBFromStack();

			if (ChildFlag & I3_NODEFLAG_DYNAMIC_OCCLUDEE)
			{
				i3::occ_culler* pCuller = pTracer->getSgContext()->GetOccCuller();
				
				if (pCuller != nullptr && pChild->m_octree_item->is_in_octree())
				{
					pCuller->update_occludee_item(pChild->m_octree_item);
				}
			}
		}
		else
		{
			pChild->PreTrace(pTracer);
		}
	}

}

void	i3Node::EnableUpdateAABB( bool bEnable)
{
	if (bEnable)
	{
		if (m_pAABB == nullptr)
			m_pAABB = i3::create_by_common_pool(i3::identity<i3::aabb>());
	
	}
	else
	{
		// 오클루전 컬링 적용시는 AABB가 항상 존재해야한다..
		if ( m_octree_item == nullptr && m_pAABB != nullptr )
		{
			i3::delete_by_common_pool(m_pAABB);	m_pAABB = nullptr;
		}
	}
}

void	i3Node::EnableOccludee(i3::occ_culler* pCuller, bool isSceneAttached, bool bEnable)
{
	if (bEnable)
	{
		this->AddFlag(I3_NODEFLAG_DYNAMIC_OCCLUDEE);

		if (this->m_pAABB == nullptr)
		{
			m_pAABB = i3::create_by_common_pool(i3::identity<i3::aabb>());
		}

		if (m_octree_item == nullptr)
		{
			m_octree_item = new occludee_item_dynamic_i3Node(this, pCuller);
			m_octree_item->bind_aabb(m_pAABB);
		}
		
		if (isSceneAttached)
		{
			pCuller->add_occludee_item(m_octree_item);
		}
	}
	else
	{
		this->RemoveFlag(I3_NODEFLAG_DYNAMIC_OCCLUDEE);

		if ( m_octree_item != nullptr )
		{
			if ( m_octree_item->is_in_octree() )
				pCuller->remove_occludee_item(m_octree_item);

		// m_octree_item의 삭제처리는 하지 않는다...
		}
	}
}

bool	i3Node::IsEnabledOccludee() const
{
	return const_cast<i3Node*>(this)->IsFlag(I3_NODEFLAG_DYNAMIC_OCCLUDEE);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED HEADER1
	{
		i3Node *		m_pParent = nullptr;
		UINT32			m_Flag = 0;
		UINT32			m_ID = 0;
		i3BoundBox *	m_pBound = nullptr;
	};

	struct PACKED HEADER2
	{
		INT8			m_ID[4] = { 'I', 'N', 'F', '2' };
		UINT32			m_Flag = 0;
		UINT32			m_ChildCount = 0;
		i3BoundBox *	m_pBound = nullptr;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Node::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;

	// Named Element
	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Header
	pack::HEADER2 header;
	header.m_pBound = GetBound();				// GetObjectPersistID() 함수를 사용하지 않는다. 의도적.
	header.m_Flag = GetFlag();
	header.m_ChildCount = 0;

	for( i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_VOLATILE) == false)
			header.m_ChildCount++;
	}

	Rc = pStream->Write( &header, sizeof(header));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Node::OnSave()", "Could not write Node Information Header.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_VOLATILE))
			continue;

		OBJREF ref = (OBJREF) pResFile->GetObjectPersistID( pChild);

		Rc = pStream->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Node::OnSave()", "Could not write child node reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Bound Box
	if( GetBound() != nullptr)
	{
		Rc = m_pBound->OnSave( pResFile);
		if( Rc == STREAM_ERR)
		{
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3Node::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32	i3Node::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	pack::HEADER2 header;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i, idx;

	// Named Element
	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Header
	Rc = pStream->Read( &header, sizeof(header));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Node::OnLoad()", "Could not read Node information head.");
		return STREAM_ERR;
	}
	Result += Rc;

	SetFlag( header.m_Flag);

	if( memcmp( header.m_ID, "INF2", 4) == 0)
	{
		m_ChildList.resize( header.m_ChildCount);

		// New version
		for( i = 0, idx = 0; i < header.m_ChildCount; i++, idx++)
		{
			OBJREF ref;
			i3Node * pChild;

			Rc = pStream->Read( &ref, sizeof(ref));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Node::OnLoad()", "Could not child node reference.");
				return STREAM_ERR;
			}
			Result += Rc;

			pChild = (i3Node *) pResFile->FindObjectByID( ref);
			if( pChild != nullptr)
			{
				// pChild가 nullptr인 경우를 ASSERT 처리하지 않고, 정상적인 Error 처리한다.
				// 실제로 Child를 저장하지 않는 Node가 있을 수 있기 때문이다.
				//AddChild( pChild);
				m_ChildList[idx] = pChild;
				I3_MUST_ADDREF( pChild);
				pChild->SetParent( this);
			}
			else
			{
				idx--;
				if (!m_ChildList.empty())
					m_ChildList.resize( m_ChildList.size() - 1);
			}
		}
	}
	else
	{
		pack::HEADER1	oldHeader;

		memcpy( &oldHeader, &header, sizeof(oldHeader));

		// Parent
		if( oldHeader.m_pParent != 0)
		{
			m_pParent = (i3Node *) pResFile->FindObjectByID( (UINT32) oldHeader.m_pParent);
			if( m_pParent != nullptr)
				m_pParent->AddChild( this);
			else
				m_pParent = nullptr;
		}
		else
			m_pParent = nullptr;
	}

	if( header.m_pBound != nullptr)
	{
		m_pBound = i3BoundBox::new_object();

		Rc = m_pBound->OnLoad( pResFile);
		if( Rc == STREAM_ERR)
		{
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

bool i3Node::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	int nChildCount = 0;

	for (int i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild(i);

		if (pChild->IsFlag(I3_NODEFLAG_VOLATILE) == false)
			nChildCount++;
	}

	//pXML->addAttr("ID", "INF2");
	pXML->addAttr("Flag", GetFlag());
	pXML->addAttr("ChildCount", nChildCount);

	for (int i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild(i);

		if (pChild->IsFlag(I3_NODEFLAG_VOLATILE))
			continue;

		OBJREF ref = (OBJREF)pFile->GetObjectPersistID(pChild);

		pXML->addAttr("Ref", ref);
	}

	// Bound Box
	if (GetBound() != nullptr)
	{
		m_pBound->OnSaveXML(pFile, pXML);
	}

	return true;
}

bool i3Node::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	// Header
	UINT32 flag;
	pXML->getAttr("Flag", &flag);
	SetFlag(flag);

	int nChildCount;
	pXML->getAttr("ChildCount", &nChildCount);

	//if (memcmp(header.m_ID, "INF2", 4) == 0)
	{
		m_ChildList.resize(nChildCount);

		// New version
		for (int i = 0, idx = 0; i < nChildCount; i++, idx++)
		{
			OBJREF ref;
			i3Node * pChild;

			pXML->getAttr("Ref", &ref);

			pChild = (i3Node *)pFile->FindObjectByID(ref);
			if (pChild != nullptr)
			{
				// pChild가 nullptr인 경우를 ASSERT 처리하지 않고, 정상적인 Error 처리한다.
				// 실제로 Child를 저장하지 않는 Node가 있을 수 있기 때문이다.
				//AddChild( pChild);
				m_ChildList[idx] = pChild;
				I3_MUST_ADDREF(pChild);
				pChild->SetParent(this);
			}
			else
			{
				idx--;
				if (!m_ChildList.empty())
					m_ChildList.resize(m_ChildList.size() - 1);
			}
		}
	}

	return true;
}

void i3Node::GetWrappingBound( i3BoundBox * pBox, bool bShadow)
{
	i3MatrixStack Stack;
	I3_GETBOUND_INFO info;

	info.m_pStack = &Stack;
	info.m_pSkel = nullptr;
	info.m_pBound = pBox;
	info.m_bShadow = bShadow;

	_GetWrappingBound( &info);
}

void i3Node::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	INT32 i;

	if( pInfo->m_bShadow)
	{// 그림자용 wrappingbound를 구하는 경우 그림자를 받지 않는 구역은 계산하지 않는다. soongoo.jung 2013.10.23
		if( IsFlag( I3_NODEFLAG_DISABLE_SHADOW_RECEIVER))
			return;
	}

	for( i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild( i);

		pChild->_GetWrappingBound( pInfo);
	}
}

void	i3Node::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	const INT32 numChild = GetChildCount();
	for (INT32 i = 0; i < numChild; ++i)
	{
		i3Node* pChild = GetChild(i);
		pChild->CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);
	}
}



void i3Node::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	{
		i3Node * pNode = (i3Node *) pDest;

		if( hasName())
			pNode->SetName( GetNameString());

		pNode->SetFlag( GetFlag());
		pNode->SetBound( GetBound());
	}
}

void i3Node::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	INT32 i;

	for( i = 0; i < GetChildCount(); i++)
	{
		i3Node * pChild = GetChild( i);

		pChild->OnInstancing( pInfo);
	}
}

void i3Node::Instantiate( void)
{	
	i3MatrixStack mtxStack;
	I3_ONINSTANCING_INFO info;
	info._pBase			= this;
	info._pMatrixStack	= &mtxStack;

	OnInstancing( &info);
}

void i3Node::OnLostDevice( bool bLostDevice)
{
}

void i3Node::OnRevive( i3RenderContext * pCtx)
{
}

void i3Node::CallLostDevice( bool bLostDevice)
{
	if( s_pNodeLinkMutex != nullptr)
		s_pNodeLinkMutex->Enter();

	i3Node * pTemp = s_pNodeLinkHead;

	INT32	cnt = 0;

	while( pTemp != nullptr)
	{
		pTemp->OnLostDevice( bLostDevice);

		cnt++;

		pTemp = pTemp->m_pNodeLinkRight;
	}

	if( s_pNodeLinkMutex != nullptr)
		s_pNodeLinkMutex->Leave();

	I3TRACE( "i3Node::OnLostDevice() : total %d nodes\n", cnt);
}

void i3Node::CallRevive( i3RenderContext * pCtx)
{
	if( s_pNodeLinkMutex != nullptr)
		s_pNodeLinkMutex->Enter();

	i3Node * pTemp = s_pNodeLinkHead;
	INT32 cnt = 0;

	while( pTemp != nullptr)
	{
		pTemp->OnRevive( pCtx);

		cnt++;

		pTemp = pTemp->m_pNodeLinkRight;
	}

	if( s_pNodeLinkMutex != nullptr)
		s_pNodeLinkMutex->Leave();

	I3TRACE( "i3Node::OnRevive() : total %d nodes\n", cnt);
}
