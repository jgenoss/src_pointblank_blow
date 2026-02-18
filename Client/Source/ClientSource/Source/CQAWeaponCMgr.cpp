#include "pch.h"
#include "CQAWeaponCMgr.h"

#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "i3Base/string/ext/format_string.h"


#define WEAPONCOLLISION_MAX_COUNT 2000

I3_CLASS_INSTANCE( CObjectEvtReciver);//, i3ElementBase);

CQAWeaponCMgr::CQAWeaponCMgr() : onWeaponCollision(false), count_(0), res_count(0),
								non_contact_outbox(0), aim_(0.0), fire_count_(1), 
								box_size_(0.1f), box_max_count_(WEAPONCOLLISION_MAX_COUNT)
{
	shape_list.reserve(box_max_count_);
	out_shape_list.reserve(box_max_count_);
}

CQAWeaponCMgr::~CQAWeaponCMgr()
{
	ReleasePhysXBox();
	shape_list.clear();
	out_shape_list.clear();
}

void CQAWeaponCMgr::CreatePhysXBox(i3World* world)
{
	for(INT32 i = 0; i < box_max_count_; ++i)
	{
		COLORREAL color;
		color.r = 1.f;
		color.g = 0.f;
		color.b	= 0.f;
		color.a = 1.f;

		i3Transform2* pTransform = _CreateShape((REAL32) ((i * (box_size_*10) + 20) - 2560), 5000.f, 0.f, color);
		i3Scene::SetActorPairFlag( world->getWorldCollision()->getInstanceSg(), 
									pTransform->getShapeSet()->getNxActor(), NX_NOTIFY_ON_START_TOUCH );

		g_pFramework->Get3DRoot()->AddChild( pTransform);
		shape_list.push_back(pTransform);
	}
}

void CQAWeaponCMgr::ReleasePhysXBox()
{
	if(!shape_list.empty())
	{
		for(INT32 i = 0; i < box_max_count_; ++i)
		{
			shape_list.at(i)->RemoveFromParent();
			I3_SAFE_RELEASE(shape_list.at(i));
		}
		
		shape_list.clear();
	}

	if(!out_shape_list.empty())
	{
		INT32 size_ = out_shape_list.size();
		for(INT32 i = 0; i < size_; ++i)
		{
			out_shape_list.at(i)->RemoveFromParent();
			I3_SAFE_RELEASE(out_shape_list.at(i));
		}

		out_shape_list.clear();
	}

	onWeaponCollision = false;
	count_ = 0; res_count = 0;
	non_contact_outbox = 0;
	aim_ = 0.0; fire_count_ = 1; box_size_ = 0.1f;
	box_max_count_ = 100;
}

void CQAWeaponCMgr::ThrowBox(CGameCharaBase* pOwner)
{
	VEC3D vLin, vDropPos, vStartPos;
	MATRIX mat;

	//위치
	VEC3D* vRight = i3Matrix::GetRight( pOwner->GetMatrix() );
	i3Vector::Normalize(vRight, vRight);

	i3Vector::Scale(&vStartPos, vRight, -aim_/2);
	pOwner->getCameraContext()->getVirtualCamPosition( &vDropPos);
	i3Vector::Add(&vDropPos, &vDropPos, &vStartPos);

	REAL32 pos = (aim_ / fire_count_);
	VEC3D vPos;
	i3Vector::Scale(&vPos, vRight, pos);

	// 방향
	VEC3D * pAt = i3Matrix::GetAt( pOwner->GetMatrix() );
	i3Matrix::SetRotateAxis( &mat, i3Matrix::GetRight( pOwner->GetMatrix() ), -pOwner->getBoneContext()->getRhoValue() );
	i3Vector::TransformCoord( &vLin, pAt, &mat );
	i3Vector::Scale( &vLin, &vLin, 12.f );

	for(INT32 i = 0; i < fire_count_; i++)
	{
		if(count_ > box_max_count_-1)
		{
			count_ = 0;
			res_count++;
		}

		//위치
		i3Vector::Add(&vDropPos, &vDropPos, &vPos);

		i3Matrix::Copy( &mat, pOwner->GetMatrix());
		i3Matrix::SetPos( &mat, &vDropPos);
		
		i3Transform2* trans = shape_list.at(count_++);
		i3PhysixShapeSet* pShape = trans->getShapeSet();
		pShape->setShapeGlobalPose(&mat);

		pShape->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_GRAVITY);

		pShape->setLinearVelocity( &vLin);
	}
}

i3Node* CQAWeaponCMgr::_CreateBox( bool bTri, bool bLine, COLORREAL * pTriColor, COLORREAL * pLineColor)
{
	i3Node * pRoot;
	i3AttrSet * pTriRoot;
	i3AttrSet * pLineRoot;
	i3VertexArray * pVA;

	pRoot = i3Node::new_object();

	{
		VEC3D vmin, vmax;

		i3Vector::Set( &vmax, box_size_, box_size_/2, box_size_);
		i3Vector::Scale( &vmin, &vmax, -1.0f);

		pVA = i3VertexArray::CreateBox( &vmin, &vmax, false, true, nullptr);
	}

	if( bTri)
	{
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pTriRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pTriRoot);

		if( pTriColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				pAttr->Set( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}

			// Material Attribute
			{
				i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

				pAttr->SetDiffuse( pTriColor);
				pTriRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pTriRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pVA->GetCount() / 3);		
	}

	if( bLine)
	{
		i3IndexArray * pIA;
		i3Geometry * pGeo;
		i3GeometryAttr * pGeoAttr;

		pLineRoot = i3AttrSet::new_object_ref();
		pRoot->AddChild( pLineRoot);

		// Lighting Disable
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set( false);
			pLineRoot->AddAttr( pAttr);
		}

		if( pLineColor != nullptr)
		{
			// Color Attribute
			{
				i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

				pAttr->Set( pLineColor);
				pLineRoot->AddAttr( pAttr);
			}
		}

		pGeo = i3Geometry::new_object_ref();
		pLineRoot->AddChild( pGeo);

		pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		// Index Array
		pIA = i3IndexArray::new_object_ref();

		pIA->Create( 24, 0, I3G_IDX_16BITS);
		pIA->setRestoreType( I3G_RESTORE_MEM);

		pIA->Lock();

		// 위쪽 사각형
		pIA->SetIndex(	0,	17);		// x1, y1, z1 -> x2, y1, z1
		pIA->SetIndex(	1,	11);
		pIA->SetIndex(	2,	11);		// x2, y1, z1 -> x2, y1, z2
		pIA->SetIndex(	3,	10);
		pIA->SetIndex(	4,	10);		// x2, y1, z2 -> x1, y1, z2
		pIA->SetIndex(	5,	4);
		pIA->SetIndex(	6,	4);			// x1, y1, z2 -> x1, y1, z1
		pIA->SetIndex(	7,	17);

		// 아래족 사각형
		pIA->SetIndex(	8,	14);		// x1, y2, z1 -> x2, y2, z1
		pIA->SetIndex(	9,	8);
		pIA->SetIndex(	10,	8);			// x2, y2, z1 -> x2, y2, z2
		pIA->SetIndex(	11,	6);
		pIA->SetIndex(	12,	6);			// x2, y2, z2 -> x1, y2, z2
		pIA->SetIndex(	13,	0);
		pIA->SetIndex(	14,	0);			// x1, y2, z2 -> x1, y2, z1
		pIA->SetIndex(	15,	14);

		// 세로 선 (4개)
		pIA->SetIndex(	16,	17);		// x1, y1, z1 -> x1, y2, z1
		pIA->SetIndex(	17,	14);
		pIA->SetIndex(	18,	11);		// x2, y1, z1 -> x2, y2, z1
		pIA->SetIndex(	19,	8);
		pIA->SetIndex(	20,	7);			// x2, y1, z2 -> x2, y2, z2
		pIA->SetIndex(	21,	6);
		pIA->SetIndex(	22,	1);			// x1, y1, z2 -> x1, y2, z2
		pIA->SetIndex(	23,	0);

		pIA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 12, 0);
		pGeoAttr->SetIndexArray( pIA);
	}

	return pRoot;
}

void CQAWeaponCMgr::Update()
{
	INT32 check_count = 0;
	if(res_count != 0)
		check_count = box_max_count_;
	else
		check_count = count_;

	for(INT32 i = 0; i < check_count; i++)
	{
		MATRIX* mat = shape_list.at(i)->GetCacheMatrix();

		VEC3D* vPos = i3Matrix::GetPos(mat);

		if(vPos->y < -100)
		{
			CObjectEvtReciver* evt = (CObjectEvtReciver*)shape_list.at(i)->getShapeSet()->getEventReceiver();
			
			if(evt->getContact())
			{
				evt->setContact(false);
				VEC3D* evt_pos = i3Matrix::GetPos(evt->getMatrix());

				COLORREAL color;
				color.r = 0.f;
				color.g = 0.f;
				color.b	= 1.f;
				color.a = 1.f;

				i3Transform2* pTransform = _CreateShape(evt_pos->x, evt_pos->y, evt_pos->z, color);

				g_pFramework->Get3DRoot()->AddChild( pTransform);
				out_shape_list.push_back(pTransform);
			}
			else
			{
				non_contact_outbox++;
			}

			MATRIX mat_;
			
			i3Matrix::SetPos( &mat_, (REAL32) ((i * 20) - 2560), 5000.f, 0.f);

			i3PhysixShapeSet* pShape = shape_list.at(i)->getShapeSet();
			pShape->setShapeGlobalPose(&mat_);
			pShape->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_GRAVITY, 0);
		}
	}
}

i3Transform2* CQAWeaponCMgr::_CreateShape( REAL32 x, REAL32 y, REAL32 z, COLORREAL color )
{
	VEC3D vZero, vSize;

	i3Vector::Set( &vZero, 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &vSize, box_size_+0.02f, box_size_+0.02f, box_size_+0.02f);

	i3Transform2* pTransform;
	i3PhysixShapeSet* pShapeSet;
	CObjectEvtReciver* pEvent;
	MATRIX mat;

	pTransform = i3Transform2::new_object();
	pShapeSet = i3PhysixShapeSet::new_object_ref();
	pEvent = CObjectEvtReciver::new_object();

	i3Matrix::SetTranslate( &mat, x, y, z );

	pShapeSet->CreateBoxShape(&vZero, &vSize, CGRP_ITEM, true, &mat);

	pShapeSet->PrepareContactReport();

	pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE |
		I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RAYCAST, 0 );

	pShapeSet->setEventReceiver( pEvent);

	pTransform->SetMatrix(&mat);
	pTransform->setShapeSet(pShapeSet);

	{
		i3Node* geo_node = _CreateBox(true, true, &color, &color);
		pTransform->AddChild(geo_node);
	}	

	return pTransform;
}

i3::wstring CQAWeaponCMgr::getOutCollisionBox() const
{
	return i3::format_string(L"World Out Box : %d\nNon Contact Out Box : %d", out_shape_list.size(), non_contact_outbox);
}

void CQAWeaponCMgr::setFireCount( INT32 count )
{
	fire_count_ = count;
	if(fire_count_ < 1)
		fire_count_ = 1;

	if(fire_count_ > 100)
		fire_count_ = 100;
}

void CQAWeaponCMgr::setValue( REAL32 aim, REAL32 scale, INT32 fire_count, INT32 max_count)
{
	setAim(aim);
	setBoxScale(scale);
	setFireCount(fire_count);
	setBoxMaxCount(max_count);
}

void CQAWeaponCMgr::setBoxScale( REAL32 scale )
{
	if(scale <= 0)
	{
		box_size_ = 0.1f;
	}
	else
	{
		box_size_ = scale / 10;
	}
}

void CQAWeaponCMgr::setAim( REAL32 aim )
{
	if(aim <= 0)
	{
		aim_ = 0.0f;
	}
	else
	{
		aim_ = aim;
	}
}

void CQAWeaponCMgr::setBoxMaxCount( INT32 count )
{
	if(count <= 0)
	{
		box_max_count_ = 100;
	}
	else if(count > 2000)
	{
		box_max_count_ = WEAPONCOLLISION_MAX_COUNT;
	}
	else
	{
		box_max_count_ = count;
	}
}

bool CObjectEvtReciver::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_CONTACT :
			if( i3::same_of<i3PhysixShapeSet*>(pObj))
			{
				contact_ = true;
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
				
				MATRIX _mat;
				pShapeSet->getShapeGlobalPose(&_mat);
				i3Matrix::Copy(&mat_, &_mat);
				
				return true;
			}
			break;
		}
	}

	return true;
}