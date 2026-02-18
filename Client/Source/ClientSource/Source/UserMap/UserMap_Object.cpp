#include "pch.h"
#include "UserMap_Object.h"
#include "UserMap_Define.h"

I3_CLASS_INSTANCE( CUserMap_Object, i3Object);

CUserMap_Object::CUserMap_Object(void)
{
	m_Info.init();

	m_pBlock = NULL;
}


CUserMap_Object::~CUserMap_Object(void)
{
}

void CUserMap_Object::Create( UINT16 blockID)
{
	m_pBlock = USMAP::BLOCK::FindInfo( blockID);
	I3ASSERT( m_pBlock != NULL);

	m_Info.m_ID = blockID;
	
	I3ASSERT( m_pBlock->m_pRes != NULL);

	i3Object::Create( g_pFramework, m_pBlock->m_pRes);

	Instantiate();

	getAttachNode()->GetWrappingBound( &m_BoundBox);
}

void CUserMap_Object::SetSize( REAL32 cx, REAL32 cy, REAL32 cz, bool bConfirm)
{
	REAL32 oldCX, oldCY, oldCZ;
	VEC3D sz;
	
	// Scale은 항상 0.5 단위로만 설정이 가능하다.
	// 이것은 USMAP::OBJECT::INFO에서 Scale의 값을 INT8 Type으로 저장하며
	// 0.5의 배수로 사용하기 때문이다.

	oldCX = m_Info.getScale( 0);
	oldCY = m_Info.getScale( 1);
	oldCZ = m_Info.getScale( 2);

	if( m_pBlock->m_EditStyle & USMAP::BLOCK::EDIT_STYLE_DISABLE_SCALE_X)
		cx = 1.0f;

	if( m_pBlock->m_EditStyle & USMAP::BLOCK::EDIT_STYLE_DISABLE_SCALE_Y)
		cy = 1.0f;

	if( m_pBlock->m_EditStyle & USMAP::BLOCK::EDIT_STYLE_DISABLE_SCALE_Z)
		cz = 1.0f;

	m_Info.setScale( cx, cy, cz);

	sz.x = cx = m_Info.getScale( 0);
	sz.y = cy = m_Info.getScale( 1);
	sz.z = cz = m_Info.getScale( 2);


	if( m_pBlock->m_EditStyle & USMAP::BLOCK::EDIT_STYLE_DISABLE_TEX)
	{
		_Rec_ResizeGeo( getAttachNode(), &sz);
	}
	else
	{
		CUserMap_Block * pBlock = m_pBlock->m_pBlock;
		I3ASSERT( pBlock != NULL);

		pBlock->Resize( getAttachNode(), &sz);
	}

	// Convex
	if( bConfirm)
	{
		//_Rec_ResizeConvex( getAttachNode(), &sz);
	}
	
}

void CUserMap_Object::SetSnappedPos( VEC3D * pPos)
{
	INT32 cx, cy, cz;

	cx = (INT32)(pPos->x / 0.5f);
	cy = (INT32)(pPos->y / 0.5f);
	cz = (INT32)(pPos->z / 0.5f);

	SetPos( cx * 0.5f, cy * 0.5f, cz * 0.5f);
}

void CUserMap_Object::_getScaleOption( char * pszName, VEC3D * pOrg, VEC3D * pNew)
{
	pNew->x = 1.0f;
	pNew->y = 1.0f;
	pNew->z = 1.0f;

	while( *pszName != 0)
	{
		switch( *pszName)
		{
			case 'x' :
			case 'X' :		pNew->x = pOrg->x;	break;

			case 'y' :
			case 'Y' :		pNew->y = pOrg->y;	break;

			case 'z' :
			case 'Z' :		pNew->z = pOrg->z;	break;
		}

		pszName++;
	}
}

void CUserMap_Object::_Rec_ResizeGeo( i3Node * pNode, VEC3D * pSz)
{
	INT32 i;
	i3Node * pChild;
	VEC3D newSz;

	if( pNode->hasName())
	{
		if( memcmp( pNode->GetName(), "_ScaleTX", 8) == 0)
		{
			// Geometry Position Scale
			I3ASSERT( pNode->IsTypeOf( i3Transform::GetClassMeta()));

			_getScaleOption( (char *) pNode->GetName() + 8, pSz, &newSz);

			i3Transform * pTrans = (i3Transform *) pNode;
			MATRIX * pMtx = pTrans->GetMatrix();

			i3Matrix::SetScale( pMtx, newSz.x, newSz.y, newSz.z);
		}

		if( memcmp( pNode->GetName(), "_TexTX", 6) == 0)
		{
			// Geometry Position Scale
			I3ASSERT( pNode->IsTypeOf( i3AttrSet::GetClassMeta()));

			i3TextureMatrixAttr * pAttr = (i3TextureMatrixAttr *) ((i3AttrSet *) pNode)->GetAttr( 0);
			I3ASSERT( pAttr != NULL);
			I3ASSERT( pAttr->IsExactTypeOf( i3TextureMatrixAttr::GetClassMeta()));

			_getScaleOption( (char *) pNode->GetName() + 6, pSz, &newSz);

			i3Matrix::SetScale( pAttr->GetMatrix(), newSz.x, newSz.z, newSz.y);
		}
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		_Rec_ResizeGeo( pChild, pSz);
	}
}
