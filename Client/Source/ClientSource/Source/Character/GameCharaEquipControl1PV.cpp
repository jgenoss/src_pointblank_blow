#include "pch.h"
#include "GameCharaEquipControl1PV.h"
#include "GameCharaEquip.h"

I3_CLASS_INSTANCE( CGameCharaEquipControl1PV);


CGameCharaEquip * CGameCharaEquipControl1PV::OnDropEquip( EQUIP::ePART type, i3Node * pSceneNode, i3GameNode * pGameNode, VEC3D * pVecPos )
{
	CGameCharaEquip * pCharaEquip = CGameCharaEquipControl::OnDropEquip( type, pSceneNode, pGameNode, pVecPos);

	//	Å»Âø À§Ä¡ ÁöÁ¤
	if( pCharaEquip != nullptr && pVecPos != nullptr )
	{
		pCharaEquip->SetPos( pVecPos );

		MATRIX mat;
		i3Skeleton *pSkel = pCharaEquip->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet == nullptr ) continue;

			pShapeSet->getShapeGlobalPose( &mat );
			i3Matrix::SetPos( &mat, pVecPos );
			pShapeSet->setShapeGlobalPose( &mat );
		}
	}

	return pCharaEquip;
}