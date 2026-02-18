#include "stdafx.h"
#include "i3treeProperty.h"
#include "i3treePropertyBase.h"
#include "i3GraphPropertySkeleton.h"
#include "i3GraphPropertyBodySkeleton.h"
#include "i3GraphPropertyShapeSet.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"

///////////////////////////////////////////////////////////////////////////
// 이거 꼬옥 변경해줘야 합니다. 자동으로 해줄까?
// Node 에서 공통으로 사용되는 컬럼 갯수 입니다. 
#define NODE_PROPERTY_START_ROW		6
void PropertyBoard::SetNode(void)
{	
	i3Node * pNode; 
	pNode = (i3Node *)m_pSelObj; 
	BOOL	rv; 

	//For Property	
	I3_P_STRING  * pPropertyString;
	I3_P_BOOL	 * pPropertyBool;
	I3_P_VEC3D	 * pPropertyVector; 
	I3_P_SET_NODEFLAG*	pNodeFlag;

	//For Class Name 	 0번
	pPropertyString = new I3_P_STRING; 
	pPropertyString->SetName( "ClassName" ); 
	pPropertyString->SetValue(pNode->GetMeta()->GetClassName()); 
	pPropertyString->AddStyle(I3_READONLY); 
	m_PropertyList.Add((void *)pPropertyString); 

	//For Name 	 	     1번
	pPropertyString = new I3_P_STRING;
	pPropertyString->SetName( "Name" );
	if( pNode->hasName() == false)
		pPropertyString->SetValue( "-------");
	else
		pPropertyString->SetValue(pNode->GetName());
	m_PropertyList.Add((void *)pPropertyString); 

	//For Bound Box		2번
	i3BoundBox * pBoundBox; 
	pPropertyBool = new I3_P_BOOL; 
	pPropertyBool->SetName("BoundBox"); 	
	pBoundBox = pNode->GetBound(); 
	if(pBoundBox == NULL) rv = FALSE; 
	else				  rv = TRUE; 	
	pPropertyBool->SetValue(rv); 
	pPropertyBool->AddStyle(I3_READONLY); 	
	m_PropertyList.Add((void *)pPropertyBool); 


	VEC3D TVector; 
	i3Vector::Set( &TVector, 0.0f, 0.0f, 0.0f);

	//For Bound Box	Min	3번
	pPropertyVector = new I3_P_VEC3D; 
	pPropertyVector->SetName("BoundBox Min"); 	
	if(pBoundBox == NULL) 
	{
		pPropertyVector->SetValue(TVector); 
	}
	else
	{
		pPropertyVector->SetValue(* (pBoundBox->GetMin()) );
	}
	if( pNode->GetBound() == NULL)
		pPropertyVector->AddStyle(I3_READONLY); 	
	m_PropertyList.Add((void *)pPropertyVector); 

	//For Bound Box	Max	4번
	pPropertyVector = new I3_P_VEC3D; 
	pPropertyVector->SetName("BoundBox Max"); 	
	if(pBoundBox == NULL) 
	{
		pPropertyVector->SetValue(TVector); 
	}
	else
	{
		pPropertyVector->SetValue(* (pBoundBox->GetMax()) );
	}
	if( pNode->GetBound() == NULL)
		pPropertyVector->AddStyle(I3_READONLY); 	
	m_PropertyList.Add((void *)pPropertyVector); 


	pNodeFlag = new I3_P_SET_NODEFLAG;
	pNodeFlag->SetName("Flag");
	pNodeFlag->SetValue(pNode->GetFlag());
	m_PropertyList.Add((void*)pNodeFlag);

	if( pNode->IsExactTypeOf( i3ShapeSetContainer::GetClassMeta() ) )
	{
		I3_P_SHAPESET * pShapeSetProp = new I3_P_SHAPESET;

		pShapeSetProp->m_pShapeSet = ((i3ShapeSetContainer*)pNode)->getShapeSet();
		pShapeSetProp->SetName( "ShapeSet"); 
		m_PropertyList.Add((void *)pShapeSetProp);
	}

}

BOOL PropertyBoard::ChangeNode(void)
{
	i3Node * pNode = (i3Node *)m_pSelObj; 		

	//For Name
	if(m_SelectLine  ==  1)
	{
		CpropertyBase * pPropertyBase; 	
		char temp[64];

		//Get Property Value 
		pPropertyBase = GetProperty(); 		
		pPropertyBase->Get(temp);

		//Set Property Value 
		pNode->SetName(temp); 
		return TRUE; 
	}
	else if( m_SelectLine == 3)
	{
		I3_P_VEC3D * pProp = (I3_P_VEC3D *) GetProperty();

		i3Vector::Copy( pNode->GetBound()->GetMin(), & pProp->GetValue());
	}
	else if( m_SelectLine == 4)
	{
		I3_P_VEC3D * pProp = (I3_P_VEC3D *) GetProperty();

		i3Vector::Copy( pNode->GetBound()->GetMax(), & pProp->GetValue());
	}
	///for node - flag
	else if(m_SelectLine == 5)
	{
		I3_P_SET_NODEFLAG*	pNodeFlag;
		pNodeFlag = (I3_P_SET_NODEFLAG*)GetProperty();
		pNode->SetFlag(pNodeFlag->GetValue());
		return TRUE;
	}
	//For Class Name - Read Only 
	//For Bound Box - Read Only
	return FALSE; 

}

///////////////////////////////////////////////////////////////////////////
//For GeometryList 
void PropertyBoard::SetGeometryList(void)
{
	I3_P_INT * pInt; 
	i3Geometry * pAttrSet; 
	pAttrSet = (i3Geometry *)m_pSelObj; 

	//For Geometry Attr Count		3
	pInt = new I3_P_INT; 
	pInt->SetName("Geometry Attr Count"); 
	pInt->AddStyle(I3_READONLY); 
	pInt->SetValue(pAttrSet->GetGeometryAttrCount()); 
	m_PropertyList.Add((void*)pInt); 

	//For Total VertexCount()
	pInt = new I3_P_INT; 
	pInt->SetName("Vertex Count"); 
	pInt->AddStyle(I3_READONLY); 
	pInt->SetValue(pAttrSet->GetTotalVertexCount()); 
	m_PropertyList.Add((void*)pInt); 
}

void PropertyBoard::ChangeGeometryList(void)
{

}

///////////////////////////////////////////////////////////////////////////
void PropertyBoard::SetOcTreeList(void)
{
}

void PropertyBoard::ChangeOcTreeList(void)
{

}


///////////////////////////////////////////////////////////////////////////
//For AttrSetList 
void PropertyBoard::SetAttrSetList(void)
{
	I3_P_INT * pInt; 
	i3AttrSet * pAttrSet; 
	pAttrSet = (i3AttrSet*)m_pSelObj; 

	//For Attr Count		3
	pInt = new I3_P_INT; 
	pInt->SetName("Attr Count"); 
	pInt->AddStyle(I3_READONLY); 
	pInt->SetValue(pAttrSet->GetAttrCount()); 
	m_PropertyList.Add((void*)pInt); 

	I3_P_INT * pPriority;

	INT32 priority = (INT32) pAttrSet->GetPriority();

	pPriority = new I3_P_INT;
	pPriority->SetName( "Priority");
	pPriority->SetValue( priority);
	m_PropertyList.Add((void*)pPriority);
}

void PropertyBoard::ChangeAttrSetList(void)
{
	I3_P_INT *		pPriority;
	i3AttrSet *		pAttrSet = (i3AttrSet *) m_pSelObj;

	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( i3String::Compare( pProp->GetName(), "Priority") == 0)
	{
		pPriority = (I3_P_INT *) pProp;

		pAttrSet->SetPriority( (UINT32) pPriority->GetValue());
	}

}

///////////////////////////////////////////////////////////////////////////
//CameraList
void PropertyBoard::SetCameraList(void)
{
	I3_P_VEC3D * pVec; 
	i3Camera * pCamera; 
	MATRIX	* pMatrix; 
	pCamera = (i3Camera *)m_pSelObj; 
	pMatrix = pCamera->GetMatrix();

	//For Right			3
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Right"); 	
	pVec->SetValue(*(i3Matrix::GetRight(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For Up			4
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Up"); 	
	pVec->SetValue(*(i3Matrix::GetUp(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For At			5
	pVec = new I3_P_VEC3D; 
	pVec->SetName("At"); 	
	pVec->SetValue(*(i3Matrix::GetAt(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For Position		6
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Position"); 	
	pVec->SetValue(*(i3Matrix::GetPos(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	I3_P_REAL32 * pReal; 

	pReal = new I3_P_REAL32; 
	pReal->SetName("Near"); 
	pReal->SetValue( pCamera->GetNear() ); 
	m_PropertyList.Add((void *)pReal); 

	pReal = new I3_P_REAL32; 
	pReal->SetName("Far"); 
	pReal->SetValue( pCamera->GetFar() ); 
	m_PropertyList.Add((void *)pReal); 
	
	pReal = new I3_P_REAL32; 
	pReal->SetName("FOV"); 
	pReal->SetValue( pCamera->GetFOV() ); 
	m_PropertyList.Add((void *)pReal); 
	

}

void PropertyBoard::ChangeCameraList(void)
{
	I3_P_VEC3D * pVec; 
	I3_P_REAL32 * pReal; 
	i3Camera * pCamera; 
	MATRIX	* pMatrix; 
	VEC3D	Vec; 
	pCamera = (i3Camera *)m_pSelObj; 
	pMatrix = pCamera->GetMatrix();
	switch(m_SelectLine)
	{
	case NODE_PROPERTY_START_ROW :				//For Right
		pVec = (I3_P_VEC3D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 1):				//For Up
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break;	
	case (NODE_PROPERTY_START_ROW + 2):				//For At
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 3):				//For Pos
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix, &Vec); 
		break; 
	case (NODE_PROPERTY_START_ROW + 4):				//Near
		pReal = (I3_P_REAL32 *)GetProperty(); 
		pCamera->SetNear(pReal->GetValue()); 		
		break; 
	case (NODE_PROPERTY_START_ROW + 5):				//Far
		pReal = (I3_P_REAL32 *)GetProperty(); 
		pCamera->SetFar(pReal->GetValue()); 		
		break; 
	case (NODE_PROPERTY_START_ROW + 6):				//FOV
		pReal = (I3_P_REAL32 *)GetProperty(); 
		pCamera->SetFOV(pReal->GetValue());
		break; 
	}
}

///////////////////////////////////////////////////////////////////////////
//For JointList
void PropertyBoard::SetJointList(void)
{
	I3_P_VEC3D * pVec; 
	i3Transform * pTransform; 
	MATRIX	* pMatrix; 
	pTransform = (i3Transform *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();

	//For Right			3
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Right"); 	
	pVec->SetValue(*(i3Matrix::GetRight(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 
	//For Up			4
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Up"); 	
	pVec->SetValue(*(i3Matrix::GetUp(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For At			5
	pVec = new I3_P_VEC3D; 
	pVec->SetName("At"); 	
	pVec->SetValue(*(i3Matrix::GetAt(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For Position		6
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Position"); 	
	pVec->SetValue(*(i3Matrix::GetPos(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 
}

void PropertyBoard::ChangeJointList(void)
{
	I3_P_VEC3D * pVec; 
	i3Transform * pTransform; 
	MATRIX	* pMatrix; 
	VEC3D	Vec; 
	pTransform = (i3Transform *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();
	switch(m_SelectLine)
	{
	case NODE_PROPERTY_START_ROW:				//For Right
		pVec = (I3_P_VEC3D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 1):				//For Up
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break;	
	case (NODE_PROPERTY_START_ROW + 2):				//For At
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 3):				//For Pos
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix, &Vec); 
		break; 
	}
}

///////////////////////////////////////////////////////////////////////////
//For NodeList
void PropertyBoard::SetNodeList(void)
{

}

void PropertyBoard::ChangeNodeList(void)
{

}

void PropertyBoard::SetSceneObjectList(void)
{
	I3_P_VEC3D * pVec; 
	i3SceneObject * pTransform; 
	MATRIX	* pMatrix; 
	pTransform = (i3SceneObject *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();

	//For Right			3
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Right"); 	
	pVec->SetValue(*(i3Matrix::GetRight(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 
	//For Up			4
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Up"); 	
	pVec->SetValue(*(i3Matrix::GetUp(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For At			5
	pVec = new I3_P_VEC3D; 
	pVec->SetName("At"); 	
	pVec->SetValue(*(i3Matrix::GetAt(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 


	//For Position		6
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Position"); 	
	pVec->SetValue(*(i3Matrix::GetPos(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	return; 
}

void PropertyBoard::ChangeSceneObjectList(void)
{
	I3_P_VEC3D * pVec; 
	i3SceneObject * pTransform; 
	MATRIX	* pMatrix; 
	VEC3D	Vec; 
	pTransform = (i3SceneObject *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();
	switch(m_SelectLine)
	{
	case NODE_PROPERTY_START_ROW:				//For Right
		pVec = (I3_P_VEC3D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 1):		//For Up
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break;	
	case (NODE_PROPERTY_START_ROW + 2):		//For At
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 3):		//For Pos
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix, &Vec); 
		break; 
	}
	return; 
}

///////////////////////////////////////////////////////////////////////////
//For TransformList
void PropertyBoard::SetTransformList(void)
{
	I3_P_VEC3D * pVec; 
	I3_P_BOOL	*	pBool;
	I3_P_SHAPESET * pShapeSetProp;
	i3Transform * pTransform;
	MATRIX	* pMatrix; 
	pTransform = (i3Transform *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();

	//For Right			3
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Right"); 	
	pVec->SetValue(*(i3Matrix::GetRight(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 
	//For Up			4
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Up"); 	
	pVec->SetValue(*(i3Matrix::GetUp(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	//For At			5
	pVec = new I3_P_VEC3D; 
	pVec->SetName("At"); 	
	pVec->SetValue(*(i3Matrix::GetAt(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 


	//For Position		6
	pVec = new I3_P_VEC3D; 
	pVec->SetName("Position"); 	
	pVec->SetValue(*(i3Matrix::GetPos(pMatrix))); 
	m_PropertyList.Add((void *)pVec); 

	pBool = new I3_P_BOOL;
	pBool->SetName( "Transform Source");
	pBool->SetValue( pTransform->IsAnimated());
	pBool->SetStyle( I3_READONLY);
	m_PropertyList.Add( pBool);

	// Transform2 Physix set by 정순구
	if( pTransform->IsExactTypeOf( i3Transform2::GetClassMeta() ) )
	{
		pShapeSetProp = new I3_P_SHAPESET;

		pShapeSetProp->m_pShapeSet = ((i3Transform2*)pTransform)->getShapeSet();
		pShapeSetProp->SetName( "ShapeSet"); 
		m_PropertyList.Add((void *)pShapeSetProp);
	}
}

void PropertyBoard::ChangeTransformList(void)
{

	I3_P_VEC3D * pVec; 
	i3Transform * pTransform; 
	MATRIX	* pMatrix; 
	VEC3D	Vec; 
	pTransform = (i3Transform *)m_pSelObj; 
	pMatrix = pTransform->GetMatrix();
	switch(m_SelectLine)
	{
	case NODE_PROPERTY_START_ROW:				//For Right
		pVec = (I3_P_VEC3D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 1):		//For Up
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break;	
	case (NODE_PROPERTY_START_ROW + 2):		//For At
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), 0.0); 
		break; 
	case (NODE_PROPERTY_START_ROW + 3):		//For Pos
		pVec = (I3_P_VEC3D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix, &Vec); 
		break; 
	}
}

///////////////////////////////////////////////////////////////////////////////
// Particle List
void PropertyBoard::SetParticleList(void)
{
	i3Particle		*pParticle;
	i3Texture		*pTexture;
	I3_P_BOOL		*pBool;
	I3_P_STRING		*pString;
	I3_P_IMAGE		*pImage;

	pParticle = (i3Particle *) m_pSelObj; 
	pTexture = pParticle->getTexture();
	if( pTexture != NULL )
	{
		pBool = new I3_P_BOOL;
		pBool->SetName( "Extern Image" );
		pBool->SetValue( pTexture->IsExternRes() );
		pBool->AddStyle( I3_READONLY );
		m_PropertyList.Add( pBool );

		if( pTexture->hasName())
		{
			// External Image
			pString = new I3_P_STRING;
			pString->SetName( "Image Name" );
			pString->SetValue(  pTexture->GetName() ); 
			pString->AddStyle( I3_READONLY ); 
			m_PropertyList.Add( pString ); 
		}

		pImage = new I3_P_IMAGE; 
		pImage->SetTexture( pTexture );
		pImage->SetName("Image"); 
		m_PropertyList.Add((void*)pImage); 
	}

	i3TextureSequence *pTexSeq = pParticle->GetTextureSequence();
	if( pTexSeq != NULL )
	{
		pTexture = pTexSeq->GetTexture();
		if( pTexture != NULL )
		{
			pBool = new I3_P_BOOL;
			pBool->SetName( "Tex. Seq. Extern Image" );
			pBool->SetValue( pTexture->IsExternRes() );
			pBool->AddStyle( I3_READONLY );
			m_PropertyList.Add( pBool );

			if( pTexture->hasName())
			{
				pString = new I3_P_STRING;
				pString->SetName( "Tex. Seq. Image Name" );
				pString->SetValue(  pTexture->GetName() ); 
				pString->AddStyle( I3_READONLY ); 
				m_PropertyList.Add( pString ); 
			}

			pImage = new I3_P_IMAGE; 
			pImage->SetTexture( pTexture );
			pImage->SetName("Tex. Seq."); 
			m_PropertyList.Add((void*)pImage); 
		}
	}

	{
		I3_P_BOOL * pProp = new I3_P_BOOL;
		pProp->SetValue( pParticle->getDecalEnable());
		pProp->SetName( "Decal");
		m_PropertyList.Add( pProp);
	}
}

void PropertyBoard::ChangeParticleList(void)
{
	I3_P_IMAGE	*pImage;
	i3Particle  *pParticle;
	pParticle = (i3Particle *)m_pSelObj; 

	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );
	if( _stricmp( "Image", pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pParticle->getTexture())
		{
			pParticle->setTexture(pImage->GetTexture());
			SetBoard( m_pSelObj );
		}
	}
	else if( _stricmp( "Tex. Seq.", pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pParticle->getTexture())
		{
			i3TextureSequence *pTexSeq;
			pTexSeq = pParticle->GetTextureSequence();
			pTexSeq->SetTexture( pImage->GetTexture() );
			SetBoard( m_pSelObj );
		}
	}
	else if( strcmp( pProp->GetName(), "Decal") == 0)
	{
		I3_P_BOOL * pProp = (I3_P_BOOL *) GetProperty();

		pParticle->setDecalEnable( pProp->GetValue() == TRUE);
	}
}

////////////////////////////////////////////////////////////////
void PropertyBoard::SetTimeSequenceList(void)
{
	I3_P_REAL32 * pReal; 
	i3TimeSequence * pTimeSeq = (i3TimeSequence *) m_pSelObj;
	INT32 i;
	i3TimeSequenceInfo * pInfo;
	char conv[128];

	for( i = 0; i < pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = pTimeSeq->GetSequence( i);

		sprintf( conv, "Sequence %d", i);

		pReal = new I3_P_REAL32; 
		pReal->SetName( conv); 
		pReal->SetValue( pInfo->GetDuration()); 
		m_PropertyList.Add((void *)pReal); 
	}
}

void PropertyBoard::SetColliderSetList(void)
{
	i3ColliderSet		*	pColl;
	I3_P_INT			*	pInt;
	
	pColl = (i3ColliderSet *) m_pSelObj; 

	pInt = new I3_P_INT;				
	pInt->SetValue( pColl->GetGroup() ); 
	pInt->SetName("GroupIndex"); 
	pInt->AddStyle( I3_READONLY ); 
	m_PropertyList.Add((void*)pInt); 
	
	return; 
}

void PropertyBoard::ChangeColliderSetList(void)
{
	i3ColliderSet		*	pColl;

	pColl = (i3ColliderSet *) m_pSelObj; 
	switch(m_SelectLine)
	{
	case NODE_PROPERTY_START_ROW:			
		break; 
	case (NODE_PROPERTY_START_ROW + 1):	
		break; 
	case (NODE_PROPERTY_START_ROW + 2):		
		break; 
	}

	return;
}

void PropertyBoard::SetLightSetList(void)
{
	I3_P_INT	*	pInt;
	i3LightSet	* pLightSet; 
	pLightSet = ( i3LightSet *) m_pSelObj;

	pInt = new I3_P_INT;				
	pInt->SetValue( pLightSet->GetLightAttrCount() ); 
	pInt->SetName("Light Count"); 
	pInt->AddStyle( I3_READONLY ); 
	m_PropertyList.Add((void*)pInt); 
	return;
}

void PropertyBoard::ChangeLightSetList(void)
{
	return; 
}

void PropertyBoard::SetSkinList(void)
{
	CpropertySkeleton * pSkelProp;

	pSkelProp = new CpropertySkeleton;
	pSkelProp->m_pSkin = (i3Skin *) m_pSelObj;
	pSkelProp->SetName( "Skeletons"); 
	m_PropertyList.Add((void *)pSkelProp);
}

void PropertyBoard::ChangeSkinList(void)
{
}

void PropertyBoard::SetBodyList(void)
{
	i3Body * pBody = (i3Body *) m_pSelObj;

	{
		I3_P_REAL32 * pReal32 = new I3_P_REAL32;
		pReal32->SetName( "LOD Slope");
		pReal32->SetValue( pBody->getSlope());
		m_PropertyList.Add( (void*)pReal32);
	}

	{
		I3_P_REAL32 * pReal32 = new I3_P_REAL32;
		pReal32->SetName( "LOD Const");
		pReal32->SetValue( pBody->getConst());
		m_PropertyList.Add( (void*)pReal32);
	}
}

void PropertyBoard::ChangeBodyList(void)
{
	I3_P_REAL32 *		pReal;
	i3Body *			pBody = (i3Body *) m_pSelObj;

	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( i3String::Compare( pProp->GetName(), "LOD Slope") == 0)
	{
		pReal = (I3_P_REAL32 *) pProp;

		pBody->setSlope( pReal->GetValue());
	}
	else if( i3String::Compare( pProp->GetName(), "LOD Const") == 0)
	{
		pReal = (I3_P_REAL32 *) pProp;

		pBody->setConst( pReal->GetValue());
	}
}

void PropertyBoard::SetLODList(void)
{
	CPropertyBodySkeleton * pSkelProp;

	pSkelProp = new CPropertyBodySkeleton;

	pSkelProp->m_pLOD = (i3LOD *) m_pSelObj;
	pSkelProp->SetName( "Skeleton"); 
	m_PropertyList.Add((void *)pSkelProp);
}

void PropertyBoard::ChangeLODList(void)
{
}

void PropertyBoard::SetBoneRefList(void)
{
	I3_P_INT	*	pInt;
	i3BoneRef *		pBoneRef = (i3BoneRef *) m_pSelObj;

	pInt = new I3_P_INT;				
	pInt->SetValue( pBoneRef->getBoneIndex()); 
	pInt->SetName("Reference Bone Index"); 
	m_PropertyList.Add((void*)pInt); 
}

void PropertyBoard::ChangeBoneRefList(void)
{
	I3_P_INT *			pInt;
	i3BoneRef *			pBoneRef = (i3BoneRef *) m_pSelObj;

	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( i3String::Compare( pProp->GetName(), "Reference Bone Index") == 0)
	{
		pInt = (I3_P_INT *) pProp;

		pBoneRef->setBoneIndex( pInt->GetValue());
	}
}
