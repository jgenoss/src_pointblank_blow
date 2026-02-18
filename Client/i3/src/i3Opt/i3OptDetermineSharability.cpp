#include "i3OptPCH.h"
#include "i3OptDetermineSharability.h"

I3_CLASS_INSTANCE( i3OptDetermineSharability);

i3OptDetermineSharability::i3OptDetermineSharability(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

void i3OptDetermineSharability::Trace( i3Node * pNode)
{
	i3::vector<i3Node*> List;

	m_bSharable = true;

	// i3Transform Node가 있으며, Animation이 있다면
	// 공유가 불가능하다.
	{
		i3Scene::FindNodeByType( pNode, i3Transform::static_meta(), List);

		for(size_t i = 0; i < List.size(); i++)
		{
			i3Transform * pTrans = (i3Transform *) List[i];

			if( pTrans->IsAnimated())
			{
				m_bSharable = false;
				return;
			}
		}
	}

	// i3TimeSequence Node가 있다면 공유가 불가능하다.
	// 일반적으로 i3TimeSequence 노드는 개별적인 Local Time을 가져야 하기
	// 때문이다.
	{
		List.clear();

		i3Scene::FindNodeByType( pNode, i3TimeSequence::static_meta(), List);

		if( List.size() > 0)
		{
			m_bSharable = false;
			return;
		}
	}

	// i3Collider에서 이하의 Style이 있다면 공유가 가능하다.
	//
	//	- I3_COLLIDER_STYLE_STATIC
	//
	// 그렇지 않은 모든 경우에 대해 공유가 불가능한 것으로 판단한다.
	/*
	{
		INT32 j;

		List.Clear();

		i3Scene::FindNodeByType( pNode, i3ColliderSet::GetClassMeta(), &List);

		for( i = 0; i < List.GetCount(); i++)
		{
			i3ColliderSet * pCollSet = (i3ColliderSet *) List.Items[i];

			for( j = 0; j < pCollSet->GetColliderCount(); j++)
			{
				i3Collider * pColl = pCollSet->GetCollider( j);

				if((pColl->GetStyle() & I3_COLLIDER_STYLE_STATIC) == 0)
				{
					m_bSharable = false;
					return;
				}
			}
		}
	}
	*/
}
