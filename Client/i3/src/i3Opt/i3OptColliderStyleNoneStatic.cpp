#include "i3OptPCH.h"
#include "i3OptColliderStyleNoneStatic.h"

I3_CLASS_INSTANCE( i3OptColliderStyleNoneStatic);

i3OptColliderStyleNoneStatic::i3OptColliderStyleNoneStatic(void)
{
	m_Class = CLASS_PHYSICS;
}

void i3OptColliderStyleNoneStatic::OnColliderSet( MATRIX * pMat, i3ColliderSet * pCollSet)
{
	/*
	INT32 i;
	i3Collider * pCollider;
	i3Collidee * pCollidee;

	for( i = 0; i < pCollSet->GetColliderCount(); i++)
	{
		pCollider = pCollSet->GetCollider( i);

		pCollidee = pCollider->GetCollidee();
		pCollidee->Transform( pMat, pCollidee);
	}
	*/
}

bool i3OptColliderStyleNoneStatic::OnPreNode( i3Node * pNode, i3Node * pParent)
{
	INT32 i = 0;

	if( i3::same_of<i3ColliderSet* >(pNode))
	{
		i3ColliderSet * pColSet = (i3ColliderSet *) pNode;
		
		for( i = 0; i < pColSet->GetColliderCount(); i++) 
		{
			i3Collider * pCollider = pColSet->GetCollider( i);
			pCollider->SetStyle( I3_COLLIDER_STYLE_TRANSFORMED);
		}
	}
	
	return true;
}


void i3OptColliderStyleNoneStatic::Trace( i3Node * pRoot)
{
	Rec_Trace( pRoot, nullptr);
}

