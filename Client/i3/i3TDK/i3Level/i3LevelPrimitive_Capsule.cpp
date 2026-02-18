#include "stdafx.h"
#include "i3LevelPrimitive_Capsule.h"

I3_CLASS_INSTANCE(i3LevelPrimitive_Capsule);

i3LevelPrimitive_Capsule::i3LevelPrimitive_Capsule()
{	
}

void i3LevelPrimitive_Capsule::CreateSymbol()
{
	i3Node * pNode = BuildSg();

	I3ASSERT(pNode != nullptr);

	SetSymbol(pNode);
}

void i3LevelPrimitive_Capsule::SaveMatrix()
{
	i3LevelElement3D::SaveMatrix();

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);
	
	i3Matrix::Copy(&m_SavedUpper, pUpperTrans->GetMatrix());
	i3Matrix::Copy(&m_SavedCylinder, pCylinderTrans->GetMatrix());
	i3Matrix::Copy(&m_SavedLower, pLowerTrans->GetMatrix());
}

void i3LevelPrimitive_Capsule::RestoreMatrix()
{
	i3LevelElement3D::RestoreMatrix();

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	i3Matrix::Copy(pUpperTrans->GetMatrix(), &m_SavedUpper);
	i3Matrix::Copy(pCylinderTrans->GetMatrix(), &m_SavedCylinder);
	i3Matrix::Copy(pLowerTrans->GetMatrix(), &m_SavedLower);
}

void i3LevelPrimitive_Capsule::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	i3LevelPrimitive::OnSetInstanceInfo(pInfo);

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);

	{
		VEC3D scale;

		i3Matrix::GetScale(pUpperTrans->GetMatrix(), &scale);

		pInfo->vScale.x = scale.x;
		pInfo->vScale.z = scale.z;

		i3Matrix::GetScale(pCylinderTrans->GetMatrix(), &scale);

		pInfo->vScale.y = scale.y;
	}

	pInfo->nTriggerType = 2;
}

void i3LevelPrimitive_Capsule::GetScale(VEC3D * pScale)
{
	VEC3D upperScale, cylinderScale;

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);

	i3Matrix::GetScale(pUpperTrans->GetMatrix(), &upperScale);
	i3Matrix::GetScale(pCylinderTrans->GetMatrix(), &cylinderScale);

	pScale->x = upperScale.x;
	pScale->y = cylinderScale.y;
	pScale->z = upperScale.z;
}

void i3LevelPrimitive_Capsule::SetScale(VEC3D * pScale)
{
	VEC3D upperScale, cylinderScale, lowerScale;

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	i3Matrix::SetScale(pUpperTrans->GetMatrix(), pScale->x, pScale->x, pScale->x);
	i3Matrix::SetScale(pCylinderTrans->GetMatrix(), pScale->x , pScale->y, pScale->x);
	i3Matrix::SetScale(pLowerTrans->GetMatrix(), pScale->x, pScale->x, pScale->x);

	i3Matrix::PostTranslate(pUpperTrans->GetMatrix(), 0.0f, pScale->y, 0.0f);
	i3Matrix::PostTranslate(pLowerTrans->GetMatrix(), 0.0f, -pScale->y, 0.0f);

	AdjustPhysixAndBoundShape(pUpperTrans->GetMatrix(), pCylinderTrans->GetMatrix(), 0.0f);
}

void i3LevelPrimitive_Capsule::Scaling(I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap)
{
	MATRIX snapScaleMat;

	bool bX = ( i3Math::abs( x) > I3_EPS);
	bool bY = ( i3Math::abs( y) > I3_EPS);
	bool bZ = ( i3Math::abs( z) > I3_EPS);

	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	i3Matrix::Copy(pUpperTrans->GetMatrix(), &m_SavedUpper);
	i3Matrix::Copy(pCylinderTrans->GetMatrix(), &m_SavedCylinder);
	i3Matrix::Copy(pLowerTrans->GetMatrix(), &m_SavedLower);

	if(bX)
	{
		ScaleCapsuleRadius(x, &snapScaleMat, snap);
	}

	if(bY)
	{
		ScaleCapsuleHeight(y, &snapScaleMat, snap);
	}

	if(bZ)
	{
		ScaleCapsuleRadius(z, &snapScaleMat, snap);
	}

	if(snap > 0.0f)
	{
		VEC3D p[4], diff, orgSize;

		i3Vector::Sub( &orgSize, &m_BoundMax, &m_BoundMin);

		p[0] = VEC3D( m_BoundMin.x, m_BoundMin.y, m_BoundMin.z);
		p[1] = VEC3D( m_BoundMax.x, m_BoundMin.y, m_BoundMin.z);		// X만 다른 경우
		p[2] = VEC3D( m_BoundMin.x, m_BoundMax.y, m_BoundMin.z);		// Y만 다른 경우
		p[3] = VEC3D( m_BoundMin.x, m_BoundMin.y, m_BoundMax.z);		// Z만 다른 경우.

		MATRIX scaleMat;
		VEC3D snapScale;

		REAL32 scaleValue;
		
		i3Vector::Copy(&snapScale, i3Matrix::GetRight(&snapScaleMat));

		if(bX)
		{
			i3Matrix::Mul(&scaleMat, i3LevelElement3D::GetTransform(), pUpperTrans->GetMatrix());
			i3Matrix::TransformPoints( p, p, 4, &scaleMat);

			i3Vector::Sub( &diff, &p[1], &p[0]);

			scaleValue = GetScaleValue(i3Vector::Length(&diff), snapScale.x, orgSize.x, snap);

			ScaleCapsuleRadius(scaleValue, nullptr, snap);
		}
		
		if(bY)
		{
			i3Matrix::Mul(&scaleMat, i3LevelElement3D::GetTransform(), pCylinderTrans->GetMatrix());
			i3Matrix::TransformPoints( p, p, 4, &scaleMat);

			i3Vector::Sub( &diff, &p[2], &p[0]);

			scaleValue = GetScaleValue(i3Vector::Length(&diff), snapScale.y, orgSize.y, snap);

			ScaleCapsuleHeight(scaleValue, nullptr, snap);
		}

		if(bZ)
		{
			i3Matrix::Mul(&scaleMat, i3LevelElement3D::GetTransform(), pUpperTrans->GetMatrix());
			i3Matrix::TransformPoints( p, p, 4, &scaleMat);

			i3Vector::Sub( &diff, &p[3], &p[0]);

			scaleValue = GetScaleValue(i3Vector::Length(&diff), snapScale.z, orgSize.z, snap);

			ScaleCapsuleRadius(scaleValue, nullptr, snap);
		}
	}

	AdjustPhysixAndBoundShape(pUpperTrans->GetMatrix(), pCylinderTrans->GetMatrix(), snap);
}

REAL32 i3LevelPrimitive_Capsule::GetScaleValue(REAL32 diff, REAL32 snapScale, REAL32 origin, REAL32 snap)
{
	REAL32 scaleValue;

	scaleValue = (INT32)(diff / snap) * snap;

	scaleValue = scaleValue / origin - snapScale;

	return scaleValue;
}

REAL32 i3LevelPrimitive_Capsule::ScaleVector(VEC3D * vec, REAL32 value, REAL32 snap)
{
	REAL32 lenOrg, lenNew;

	lenOrg = i3Vector::Normalize( vec, vec);
	lenNew = lenOrg + value;

	if( i3Math::abs( lenNew) < snap)
	{
		// Scale이 Eps보다 작으면 문제가 발생한다.
		(lenNew >= 0.0f) ?	lenNew = snap : lenNew = -snap;
	}

	if(lenNew < 0.0f)
	{
		lenNew = -lenNew;
	}

	i3Vector::Scale( vec, vec, lenNew);

	return lenOrg;
}

void i3LevelPrimitive_Capsule::ScaleCapsuleRadius(REAL32 value, MATRIX * pMat, REAL32 snap)
{
	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	if(pMat != nullptr)
	{
		VEC3D right, up, at;

		ScaleCapsuleTransform(pUpperTrans->GetMatrix(), value, value, snap, &right);
		ScaleCapsuleTransform(pLowerTrans->GetMatrix(), value, value, snap, &up);
		ScaleCapsuleTransform(pCylinderTrans->GetMatrix(), value, 0.0f, snap, &at);

		i3Matrix::SetRight(pMat, &right);
		i3Matrix::SetUp(pMat, &up);
		i3Matrix::SetAt(pMat, &at);
	}
	else
	{
		ScaleCapsuleTransform(pUpperTrans->GetMatrix(), value, value, snap, nullptr);
		ScaleCapsuleTransform(pLowerTrans->GetMatrix(), value, value, snap, nullptr);
		ScaleCapsuleTransform(pCylinderTrans->GetMatrix(), value, 0.0f, snap, nullptr);
	}
}

void i3LevelPrimitive_Capsule::ScaleCapsuleHeight(REAL32 value, MATRIX * pMat, REAL32 snap)
{
	i3Transform2 * pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	i3Transform2 * pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	i3Transform2 * pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	VEC3D len, scale;
	VEC3D * pUp;
	pUp = i3Matrix::GetUp(pCylinderTrans->GetMatrix());

	REAL32 lenOrg = i3Vector::Normalize(&len, pUp);

	ScaleCapsuleTransform(pCylinderTrans->GetMatrix(), 0.0f, value, snap, &scale);

	REAL32 lenNew = i3Vector::Normalize(&len, pUp);

	REAL32 moveLength = lenNew - lenOrg;

	i3Matrix::PostTranslate(pUpperTrans->GetMatrix(), 0.0f, moveLength, 0.0f);
	i3Matrix::PostTranslate(pLowerTrans->GetMatrix(), 0.0f, -moveLength, 0.0f);

	if(pMat != nullptr)
	{
		i3Matrix::SetRight(pMat, &scale);	
	}
}
void i3LevelPrimitive_Capsule::ScaleCapsuleTransform(MATRIX * pMat, REAL32 radius, REAL32 height, REAL32 snap, VEC3D * pVec)
{
	I3ASSERT(pMat != nullptr);

	VEC3D * pRight, * pUp, * pAt;
	REAL32 x, y, z;

	pRight = i3Matrix::GetRight(pMat);
	pUp = i3Matrix::GetUp(pMat);
	pAt = i3Matrix::GetAt(pMat);

	x = ScaleVector(pRight, radius, snap);
	y = ScaleVector(pUp, height, snap);
	z = ScaleVector(pAt, radius, snap);

	if(pVec != nullptr)
	{
		pVec->Set(x, y, z);
	}
}

void i3LevelPrimitive_Capsule::AdjustPhysixAndBoundShape(MATRIX * pHemiSphereMat, MATRIX * pCylinderMat, REAL32 snap)
{
	VEC3D upperScale, cylinderScale;

	i3Matrix::GetScale(pHemiSphereMat, &upperScale);
	i3Matrix::GetScale(pCylinderMat, &cylinderScale);

	i3PhysixShapeSet * pShapeSet = ((i3Transform2 *)GetSymbol())->getShapeSet();

	INT32 nShapeCount = pShapeSet->getNxShapeCount();

	for(INT32 i = 0; i < nShapeCount; i++)
	{
		NxShape * pShape = pShapeSet->getNxShape( i);
		NxCapsuleShape * pCapsuleShape = pShape->isCapsule();

		if(pCapsuleShape != nullptr)
		{
			if( i3Math::abs( cylinderScale.y) < snap)
				cylinderScale.y = snap;

			if( i3Math::abs( upperScale.x) < snap)
				upperScale.x = snap;

			pCapsuleShape->setHeight(cylinderScale.y * 2);
			pCapsuleShape->setRadius(upperScale.x);
		}
	}

	i3Matrix::SetScale(m_pSelBox->GetMatrix(), upperScale.x * 2, (cylinderScale.y + upperScale.x) * 2, upperScale.z * 2);
	i3Matrix::SetScale(m_pTargetBox->GetMatrix(), upperScale.x * 2, (cylinderScale.y + upperScale.x) * 2, upperScale.z * 2);

	i3Vector::Set(&m_BoundMax, upperScale.x, cylinderScale.y + upperScale.x, upperScale.z);
	i3Vector::Set(&m_BoundMin, -upperScale.x,  -(cylinderScale.y + upperScale.x), -upperScale.z);
}

i3Transform2 * i3LevelPrimitive_Capsule::GetTransform(SHAPE_TYPE shapeType)
{
	i3Transform2 * pTrans;
	i3Transform2 * pRootTrans = (i3Transform2 *)GetSymbol();

	switch(shapeType)
	{
	case SHAPE_UPPER_HEMISPHERE:
		pTrans = (i3Transform2 *)pRootTrans->GetChild(0);
		break;

	case SHAPE_CYLINDER:
		pTrans = (i3Transform2 *)pRootTrans->GetChild(1);
		break;

	case SHAPE_LOWER_HEMISPHERE:
		pTrans = (i3Transform2 *)pRootTrans->GetChild(2);
		break;

	default:
		pTrans = nullptr;
		break;
	}

	I3ASSERT(pTrans != nullptr);

	return pTrans;
}

i3Node * i3LevelPrimitive_Capsule::BuildSg()
{
	i3Node * pCylinder;
	i3Node * pUpperHemisphere;
	i3Node * pLowerHemisphere;

	i3Transform2 * pRootTrans;
	i3Transform2 * pCylinderTrans;
	i3Transform2 * pUpperTrans;
	i3Transform2 * pLowerTrans;

	COLORREAL colDiffuse, colAmbient;

	i3Color::Set( &colDiffuse, 0.33f, 0.66f, 0.99f, 1.0f);
	i3Color::Set( &colAmbient, 1.0f, 1.0f, 1.0f, 1.0f);

	pCylinder = i3SceneUtil::BuildCylinderMesh(1.0f, 1.0f, TRUE, TRUE, 24, 10, nullptr, &colAmbient, &colDiffuse);
	pUpperHemisphere = i3SceneUtil::BuildHemiSphereMesh(1.0f, TRUE, TRUE, TRUE, 20, 10, nullptr, &colAmbient, &colDiffuse);
	pLowerHemisphere = i3SceneUtil::BuildHemiSphereMesh(1.0f, TRUE, TRUE, FALSE, 20, 10, nullptr, &colAmbient, &colDiffuse);

	{
		VEC3D center;
		i3PhysixShapeSet * pShapeSet;

		i3Vector::Set(&center, 0.0f, 0.0f, 0.0f);

		pShapeSet = i3PhysixShapeSet::new_object_ref();
		pShapeSet->CreateCapsuleShape(&center, 1.0f, 2.0f, 0);  

		pRootTrans = i3Transform2::new_object_ref();
		pRootTrans->setShapeSet(pShapeSet);

		pCylinderTrans = i3Transform2::new_object_ref();
		pCylinderTrans->AddChild(pCylinder);

		pUpperTrans = i3Transform2::new_object_ref();
		pUpperTrans->AddChild(pUpperHemisphere);

		pLowerTrans = i3Transform2::new_object_ref();
		pLowerTrans->AddChild(pLowerHemisphere);

		i3Matrix::SetTranslate(pUpperTrans->GetMatrix(), 0.0f, 1.0f, 0.0f);
		i3Matrix::SetTranslate(pLowerTrans->GetMatrix(), 0.0f, -1.0f, 0.0f);

		pRootTrans->AddChild(pUpperTrans);
		pRootTrans->AddChild(pCylinderTrans);
		pRootTrans->AddChild(pLowerTrans);

		I3_MUST_RELEASE(pCylinder);
		I3_MUST_RELEASE(pUpperHemisphere);
		I3_MUST_RELEASE(pLowerHemisphere);

		i3Scene::ModifyDynamicState(pRootTrans, I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	return pRootTrans;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct LEVEL_PRIMITIVE_CAPSULE
	{
		UINT8	m_ID[4] = { 'C', 'A', 'P', '1' };
		VEC3D	m_UpperScale;
		VEC3D	m_CylinderScale;

		UINT32	m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3LevelPrimitive_Capsule::OnSave(i3ResourceFile * pRes)
{
	UINT32 Rc, Result = 0;
	pack::LEVEL_PRIMITIVE_CAPSULE info;

	Result = i3LevelPrimitive::OnSave(pRes);
	I3_CHKIO(Result);
	
	VEC3D upperScale, cylinderScale;

	i3Matrix::GetScale(GetTransform(SHAPE_UPPER_HEMISPHERE)->GetMatrix(), &upperScale);
	i3Matrix::GetScale(GetTransform(SHAPE_CYLINDER)->GetMatrix(), &cylinderScale);

	info.m_UpperScale.x = upperScale.x;
	info.m_UpperScale.y = upperScale.y;
	info.m_UpperScale.z = upperScale.z;

	info.m_CylinderScale.x = cylinderScale.x;
	info.m_CylinderScale.y = cylinderScale.y;
	info.m_CylinderScale.z = cylinderScale.y;

	Rc = pRes->Write(&info, sizeof(info));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3LevelPrimitive_Capsule::OnLoad(i3ResourceFile * pRes)
{
	UINT32 Rc, Result = 0;
	pack::LEVEL_PRIMITIVE_CAPSULE info;

	Result = i3LevelPrimitive::OnLoad(pRes);
	I3_CHKIO(Result);

	VEC3D sphereScale, cylinderScale;

	Rc = pRes->Read(&info, sizeof(info));
	I3_CHKIO( Rc);
	Result += Rc;

	sphereScale.x = info.m_UpperScale.x;
	sphereScale.y = info.m_UpperScale.y;
	sphereScale.z = info.m_UpperScale.z;

	cylinderScale.x = info.m_CylinderScale.x;
	cylinderScale.y = info.m_CylinderScale.y;
	cylinderScale.z = info.m_CylinderScale.z;

	i3Transform2 * pUpperTrans;
	i3Transform2 * pCylinderTrans;
	i3Transform2 * pLowerTrans;

	pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	I3ASSERT(pUpperTrans != nullptr);
	I3ASSERT(pCylinderTrans != nullptr);
	I3ASSERT(pLowerTrans != nullptr);

	i3Matrix::SetScale(pUpperTrans->GetMatrix(), sphereScale.x, sphereScale.y, sphereScale.z);
	i3Matrix::SetScale(pCylinderTrans->GetMatrix(), cylinderScale.x, cylinderScale.y, cylinderScale.z);
	i3Matrix::SetScale(pLowerTrans->GetMatrix(), sphereScale.x, sphereScale.y, sphereScale.z);

	return Result;
}

bool i3LevelPrimitive_Capsule::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelPrimitive::OnSaveXML( pFile, pXML);

	if(rv == false)
	{
		return false;
	}

	i3Transform2 * pUpperTrans;
	i3Transform2 * pCylinderTrans;
	i3Transform2 * pLowerTrans;

	pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	i3Matrix::SetToXML(pXML, "UpperTrans", pUpperTrans->GetMatrix());
	i3Matrix::SetToXML(pXML, "CylinderTrans", pCylinderTrans->GetMatrix());
	i3Matrix::SetToXML(pXML, "LowerTrans", pLowerTrans->GetMatrix());
	
	return rv;
}

bool i3LevelPrimitive_Capsule::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelPrimitive::OnLoadXML(pFile, pXML);

	if(rv == false)
	{
		return false;
	}

	i3Transform2 * pUpperTrans;
	i3Transform2 * pCylinderTrans;
	i3Transform2 * pLowerTrans;

	pUpperTrans = GetTransform(SHAPE_UPPER_HEMISPHERE);
	pCylinderTrans = GetTransform(SHAPE_CYLINDER);
	pLowerTrans = GetTransform(SHAPE_LOWER_HEMISPHERE);

	i3Matrix::GetFromXML(pXML, "UpperTrans", pUpperTrans->GetMatrix());
	i3Matrix::GetFromXML(pXML, "CylinderTrans", pCylinderTrans->GetMatrix());
	i3Matrix::GetFromXML(pXML, "LowerTrans", pLowerTrans->GetMatrix());

	AdjustPhysixAndBoundShape(pUpperTrans->GetMatrix(), pCylinderTrans->GetMatrix(), 0.0f);

	return true;
}