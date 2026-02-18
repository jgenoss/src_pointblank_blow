#include "stdafx.h"
#include "i3ExpProcessInfo.h"

i3ExpProcessInfo::i3ExpProcessInfo(void)
{
	m_pParentINode = NULL;
	m_pINode = NULL;
	m_pi3Node = NULL;
	m_State = 0;
	m_Bookmark = 0;
	m_pAttachNode = NULL;
}

i3ExpProcessInfo::~i3ExpProcessInfo(void)
{
	I3_SAFE_RELEASE( m_pi3Node);
	m_pINode = NULL;
	m_pParentINode = NULL;
}

void i3ExpProcessInfo::replaceTop( i3Node * pNode)
{
	if( m_pi3Node != NULL)
	{
		pNode->AddChild( m_pi3Node);
	}

	I3_REF_CHANGE( m_pi3Node, pNode);

	m_NodeStack.PushToTop( pNode);
}

void i3ExpProcessInfo::pushi3Node( i3Node * pNode, bool bToHead)
{
	if( m_pi3Node == NULL)
	{
		I3_REF_CHANGE( m_pi3Node, pNode);
	}
	else
	{
		i3Node * pLast;

		if( m_NodeStack.getCount() > 0)
		{
			pLast = (i3Node *) m_NodeStack.getTop();
		}
		else
		{
			// 새로운 Dummy Node를 하나 붙이고 그것을 새로운 Root로 설정해줄 필요가 있다.
			pLast = i3Node::new_object_ref();

			pLast->AddChild( m_pi3Node);
			I3_REF_CHANGE( m_pi3Node, pLast);

			m_NodeStack.Push( pLast);
		}

		if( bToHead)
			pLast->InsertChild( 0, pNode);
		else
			pLast->AddChild( pNode);
	}

	m_NodeStack.Push( pNode);
}

void i3ExpProcessInfo::popi3Node(void)
{
	i3Node * pTemp;

	m_NodeStack.Pop( (void **) &pTemp);
}

i3Node * i3ExpProcessInfo::getTop(void)
{
	if( m_NodeStack.getTop() == NULL)
		return NULL;

	return (i3Node *) m_NodeStack.getTop();
}

void i3ExpProcessInfo::setBookmark( INT32 bookmark)
{
	INT32 i, cnt;

	cnt = m_NodeStack.getCount() - bookmark;

	I3ASSERT( cnt >= 0);

	for( i = 0; i < cnt; i++)
		m_NodeStack.Pop( NULL);
}

void i3ExpProcessInfo::pushAttr( i3RenderAttr * pAttr)
{
	i3Node * pTop;
	i3AttrSet * pAttrSet = NULL;

	if( m_NodeStack.getTop() != NULL)
	{
		pTop = (i3Node *) m_NodeStack.getTop();

		while( pTop != NULL)
		{
			if( i3::kind_of<i3AttrSet*>(pTop)) //->IsTypeOf( i3AttrSet::static_meta()))
			{
				pAttrSet = (i3AttrSet *) pTop;
				break;
			}

			pTop = pTop->GetParent();
		}
	}

	if( pAttrSet == NULL)
	{
		pAttrSet = i3AttrSet::new_object();

		if( m_pi3Node != NULL)
		{
			pAttrSet->AddChild( m_pi3Node);
			m_pi3Node->Release();
		}
		else
		{
			m_NodeStack.Push( pAttrSet);
		}

		m_pi3Node = pAttrSet;
	}
	
	pAttrSet->AddAttr( pAttr);
}

i3RenderAttr *	i3ExpProcessInfo::findLastAttr( i3ClassMeta * pMeta)
{
	i3::vector<i3Node*> 	list;

	if( m_pi3Node == NULL)
		return NULL;

	i3Scene::FindNodeByType( m_pi3Node, i3AttrSet::static_meta(), list);

	for( size_t i = 0; i < list.size(); i++)
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) list[i];
		i3RenderAttr * pAttr;
		INT32 j;

		for( j = 0; j < pAttrSet->GetAttrCount(); j++)
		{
			pAttr = pAttrSet->GetAttr( j);

			if( pAttr->kind_of( pMeta))
				return pAttr;
		}
	}

	return NULL;
}