#include "i3FrameworkPCH.h"
#include "i3ActionCollision.h"

#include "i3Scene.h"
#include "i3Chara.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3ActionCollision);

i3ActionCollision::~i3ActionCollision()
{
	_ReleaseShapeInfo();
}

void i3ActionCollision::_ReleaseShapeInfo( void)
{
	I3MEM_SAFE_FREE( m_pShapeInfo);
	m_ShapeType = SHAPE_TYPE_NONE;

	I3MEM_SAFE_FREE( m_pAttachedBoneName);
}

void i3ActionCollision::SetPlane( VEC3D ** pVArray)
{
	INT32 i;

	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_PLANE;

	I3_ACTION_COLLISION_INFO_PLANE * pInfo = (I3_ACTION_COLLISION_INFO_PLANE*) i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_PLANE));

	for( i = 0; i < 4; i++)
	{
		i3Vector::Copy( &pInfo->_v[i], pVArray[i]);
	}

	m_pShapeInfo = (void*) pInfo;
}

void i3ActionCollision::SetBox( VEC3D ** pVArray)
{
	INT32 i;

	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_BOX;

	I3_ACTION_COLLISION_INFO_BOX * pInfo = (I3_ACTION_COLLISION_INFO_BOX*) i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_BOX));

	for( i = 0; i < 8; i++)
	{
		i3Vector::Copy( &pInfo->_v[i], pVArray[i]);
	}

	m_pShapeInfo = (void*) pInfo;
}

void i3ActionCollision::SetSphere( VEC3D * pCenter, REAL32 r)
{
	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_SPHERE;

	I3_ACTION_COLLISION_INFO_SPHERE * pInfo = (I3_ACTION_COLLISION_INFO_SPHERE*) i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_SPHERE));

	i3Vector::Copy( &pInfo->_vCenter, pCenter);
	pInfo->_r = r;

	m_pShapeInfo = (void*) pInfo;
}

void i3ActionCollision::SetCone( VEC3D * pDir, REAL32 angle, REAL32 height)
{
	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_CONE;

	I3_ACTION_COLLISION_INFO_CONE * pInfo = (I3_ACTION_COLLISION_INFO_CONE*) i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_CONE));

	i3Vector::Copy( &pInfo->_vDir, pDir);
	pInfo->_rAngle = angle;
	pInfo->_rHeight = height;

	m_pShapeInfo = (void*) pInfo;
}

void i3ActionCollision::SetLine( VEC3D * pOffset, VEC3D * pDir, REAL32 length)
{
	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_LINE;

	I3_ACTION_COLLISION_INFO_LINE * pInfo = (I3_ACTION_COLLISION_INFO_LINE*) i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_LINE));

	i3Vector::Copy( &pInfo->_vOffset, pOffset);
	i3Vector::Copy( &pInfo->_vDir, pDir);
	pInfo->_Length = length;

	m_pShapeInfo = (void*) pInfo;
}

void i3ActionCollision::SetConLine( VEC3D * pOffset, VEC3D * pDir, REAL32 length, REAL32 angle)
{
	_ReleaseShapeInfo();

	m_ShapeType = SHAPE_TYPE_CONLINE;

	I3_ACTION_COLLISION_INFO_CONLINE * pInfo = (I3_ACTION_COLLISION_INFO_CONLINE*)i3MemAlloc( sizeof( I3_ACTION_COLLISION_INFO_CONLINE));

	i3Vector::Copy( &pInfo->_vOffset, pOffset);
	i3Vector::Copy( &pInfo->_vDir, pDir);
	pInfo->_Length = length;
	pInfo->_angle	= angle;

	m_pShapeInfo = (void*)pInfo;
}

void i3ActionCollision::SetAttachBoneName( const char * pszName)
{
	if( pszName != nullptr)
	{
		UINT32 len;

		I3MEM_SAFE_FREE( m_pAttachedBoneName);

		len = i3::generic_string_size( pszName);

		m_pAttachedBoneName = (char *) i3MemAlloc( sizeof( char) * (len +1));

		i3::string_ncopy_nullpad( m_pAttachedBoneName, pszName, len + 1);

		m_bAttachedBone = true;
	}
	else
	{
		m_bAttachedBone = false;
	}
}

void i3ActionCollision::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::safe_string_copy( pszStr, "Emit Collision", len);
	}
	else
	{
		i3::safe_string_copy( pszStr, "Collision Shape을 활성화합니다.", len);
	}
}
#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ACTION_COLLISION
	{
		UINT32	_ID[4] = { 'A', 'T', 'C', '3' };
		UINT32	_ShapeType = 0;
		UINT32	_CollisionGroup = 0;
		UINT32	_bAttached = 0;
		char	_szAttachedBoneName[MAX_NAME] = { 0 };
		REAL32	m_fDuration = 0.0f;
		REAL32	m_fStartTime = 0.0f;
		bool	m_bMultiHit = false;
		bool	m_bDirectionalHit = false;

		UINT32	m_pad[10] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

/*virtual*/ UINT32 i3ActionCollision::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ACTION_COLLISION info;
	i3::pack::VEC3D v;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3Action::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	//memcpy( info._ID, "ATC3", 4);

	info._ShapeType = (UINT32) getShapeType();
	info._CollisionGroup = getCollisionGroup();
	if( isAttached() == 0)	info._bAttached = 0;
	else
	{
		info._bAttached = 1;
		i3::safe_string_copy( info._szAttachedBoneName, getAttachBoneName(), MAX_NAME);
	}
	
	info.m_fStartTime = m_StartTime;
	info.m_fDuration = m_Duration;
	info.m_bMultiHit = m_bMultiHit;
	info.m_bDirectionalHit = m_bDirectionalHit;

	Rc = pStream->Write( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	if( getShapeType() == SHAPE_TYPE_SPHERE)
	{
		Rc = pStream->Write( &getSphere()->_r, sizeof( getSphere()->_r));
		I3_CHKIO( Rc);
		Result += Rc;

		v.x = getX( &getSphere()->_vCenter);
		v.y = getY( &getSphere()->_vCenter);
		v.z = getZ( &getSphere()->_vCenter);

		Rc = pStream->Write( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else if( getShapeType() == SHAPE_TYPE_LINE)
	{
		// offset
		v.x = getX( &getLine()->_vOffset);
		v.y = getY( &getLine()->_vOffset);
		v.z = getZ( &getLine()->_vOffset);

		Rc = pStream->Write( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		// direction
		v.x = getX( &getLine()->_vDir);
		v.y = getY( &getLine()->_vDir);
		v.z = getZ( &getLine()->_vDir);

		Rc = pStream->Write( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		// length
		Rc = pStream->Write( &getLine()->_Length, sizeof( getLine()->_Length));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else if( getShapeType() == SHAPE_TYPE_CONLINE)
	{
		// offset
		v.x = getX( &getConLine()->_vOffset);
		v.y = getY( &getConLine()->_vOffset);
		v.z = getZ( &getConLine()->_vOffset);

		Rc = pStream->Write( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		// direction
		v.x = getX( &getConLine()->_vDir);
		v.y = getY( &getConLine()->_vDir);
		v.z = getZ( &getConLine()->_vDir);

		Rc = pStream->Write( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		// length
		Rc = pStream->Write( &getConLine()->_Length, sizeof( getConLine()->_Length));
		I3_CHKIO( Rc);
		Result += Rc;

		// div
		Rc = pStream->Write( &getConLine()->_angle, sizeof( getConLine()->_angle));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

/*virtual*/ UINT32 i3ActionCollision::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ACTION_COLLISION info;
	i3::pack::VEC3D v;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3Action::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	if( i3::generic_is_niequal( (const char*)info._ID, "ATC3", 4) )
	{
		m_StartTime = info.m_fStartTime;
		m_Duration = info.m_fDuration;
		m_bMultiHit = info.m_bMultiHit;
		m_bDirectionalHit = info.m_bDirectionalHit;
	}

	setCollisionGroup( info._CollisionGroup);

	if( info._ShapeType == SHAPE_TYPE_SPHERE)
	{
		REAL32 r;
		VEC3D vCenter;

		Rc = pStream->Read( &r, sizeof( r));
		I3_CHKIO( Rc);
		Result += Rc;

		Rc = pStream->Read( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;
		
		i3Vector::Set( &vCenter, v.x, v.y, v.z);

		SetSphere( &vCenter, r);
	}
	else if( info._ShapeType == SHAPE_TYPE_LINE)
	{
		REAL32 length;
		VEC3D vOffset, vDir;

		// offset
		Rc = pStream->Read( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;
		
		i3Vector::Set( &vOffset, v.x, v.y, v.z);

		// direction
		Rc = pStream->Read( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		i3Vector::Set( &vDir, v.x, v.y, v.z);

		// length
		Rc = pStream->Read( &length, sizeof( length));
		I3_CHKIO( Rc);
		Result += Rc;

		SetLine( &vOffset, &vDir, length);
	}
	else if( info._ShapeType == SHAPE_TYPE_CONLINE)
	{
		REAL32 length, angle;
		VEC3D vOffset, vDir;

		// offset
		Rc = pStream->Read( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;
		
		i3Vector::Set( &vOffset, v.x, v.y, v.z);

		// direction
		Rc = pStream->Read( &v, sizeof( v));
		I3_CHKIO( Rc);
		Result += Rc;

		i3Vector::Set( &vDir, v.x, v.y, v.z);

		// length
		Rc = pStream->Read( &length, sizeof( length));
		I3_CHKIO( Rc);
		Result += Rc;

		// div
		Rc = pStream->Read( &angle, sizeof( angle));
		I3_CHKIO( Rc);
		Result += Rc;

		SetConLine( &vOffset, &vDir, length, angle);
	}

	if( info._bAttached)
	{
		SetAttachBoneName( info._szAttachedBoneName);
	}

	return Result;
}

/*virtual*/ bool i3ActionCollision::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3Action::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( "ShapeType", (UINT32)getShapeType());
	pXML->addAttr( "CollisionGroup", getCollisionGroup());
	pXML->addAttr( "Duration", m_Duration);
	pXML->addAttr( "StartTime", m_StartTime);
	pXML->addAttr( "MultipleHit", (INT32) m_bMultiHit);
	pXML->addAttr( "DirectionalHit", (INT32) m_bDirectionalHit);

	if( getShapeType() == SHAPE_TYPE_SPHERE)
	{
		pXML->addAttr( "Sphere_Radius", getSphere()->_r);
		pXML->addAttr( "Sphere_CenterX", getX( &getSphere()->_vCenter));
		pXML->addAttr( "Sphere_CenterY", getY( &getSphere()->_vCenter));
		pXML->addAttr( "Sphere_CenterZ", getZ( &getSphere()->_vCenter));
	}
	else if( getShapeType() == SHAPE_TYPE_LINE)
	{
		pXML->addAttr( "Line_OffsetX", getX( &getLine()->_vOffset));
		pXML->addAttr( "Line_OffsetY", getY( &getLine()->_vOffset));
		pXML->addAttr( "Line_OffsetZ", getZ( &getLine()->_vOffset));

		pXML->addAttr( "Line_DirectionX", getX( &getLine()->_vDir));
		pXML->addAttr( "Line_DirectionY", getY( &getLine()->_vDir));
		pXML->addAttr( "Line_DirectionZ", getZ( &getLine()->_vDir));

		pXML->addAttr( "Line_Length", getLine()->_Length);
	}
	else if( getShapeType() == SHAPE_TYPE_CONLINE)
	{
		pXML->addAttr( "Line_OffsetX", getX( &getConLine()->_vOffset));
		pXML->addAttr( "Line_OffsetY", getY( &getConLine()->_vOffset));
		pXML->addAttr( "Line_OffsetZ", getZ( &getConLine()->_vOffset));

		pXML->addAttr( "Line_DirectionX", getX( &getConLine()->_vDir));
		pXML->addAttr( "Line_DirectionY", getY( &getConLine()->_vDir));
		pXML->addAttr( "Line_DirectionZ", getZ( &getConLine()->_vDir));

		pXML->addAttr( "Line_Length", getConLine()->_Length);
		pXML->addAttr( "Check_Angle", getConLine()->_angle);
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "not yet defined data..");
	}
	
	pXML->addAttr( "Attach", (UINT32) isAttached());
	if( m_pAttachedBoneName != nullptr)
		pXML->addAttr( "AttachedBoneName", m_pAttachedBoneName);

	return true;
}

/*virtual*/ bool i3ActionCollision::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3Action::OnLoadXML( pFile, pXML) == false)
		return false;

	UINT32 shapeType, collisionGroup, attach;

	pXML->getAttr( "ShapeType", &shapeType);

	pXML->getAttr( "CollisionGroup", &collisionGroup);
	setCollisionGroup( collisionGroup);

	pXML->getAttr( "Duration", &m_Duration);
	pXML->getAttr( "StartTime", &m_StartTime);
	pXML->getAttr( "MultipleHit", (INT32*) &m_bMultiHit);
	pXML->getAttr( "DirectionalHit", (INT32*) &m_bDirectionalHit);

	if( shapeType == SHAPE_TYPE_SPHERE)
	{
		REAL32 r, x, y, z;
		pXML->getAttr( "Sphere_Radius", &r);
		pXML->getAttr( "Sphere_CenterX", &x);
		pXML->getAttr( "Sphere_CenterY", &y);
		pXML->getAttr( "Sphere_CenterZ", &z);

		VEC3D v;
		i3Vector::Set( &v, x, y, z);
		SetSphere( &v, r);
	}
	else if( shapeType == SHAPE_TYPE_LINE)
	{
		VEC3D vOffset, vDir;
		REAL32 length, x, y, z;

		pXML->getAttr( "Line_OffsetX", &x);
		pXML->getAttr( "Line_OffsetY", &y);
		pXML->getAttr( "Line_OffsetZ", &z);

		i3Vector::Set( &vOffset, x, y, z);

		pXML->getAttr( "Line_DirectionX", &x);
		pXML->getAttr( "Line_DirectionY", &y);
		pXML->getAttr( "Line_DirectionZ", &z);

		i3Vector::Set( &vDir, x, y, z);

		pXML->getAttr( "Line_Length", &length);
		
		SetLine( &vOffset, &vDir, length);
	}
	else if( shapeType == SHAPE_TYPE_CONLINE)
	{
		VEC3D vOffset, vDir;
		REAL32 length, angle, x, y, z;

		pXML->getAttr( "Line_OffsetX", &x);
		pXML->getAttr( "Line_OffsetY", &y);
		pXML->getAttr( "Line_OffsetZ", &z);

		i3Vector::Set( &vOffset, x, y, z);

		pXML->getAttr( "Line_DirectionX", &x);
		pXML->getAttr( "Line_DirectionY", &y);
		pXML->getAttr( "Line_DirectionZ", &z);

		i3Vector::Set( &vDir, x, y, z);

		pXML->getAttr( "Line_Length", &length);
		pXML->getAttr( "Check_Angle", &angle);
		
		SetConLine( &vOffset, &vDir, length, angle);
	}

	pXML->getAttr( "Attach", &attach);
	if( attach)
	{
		char szBoneName[ MAX_NAME];
		i3::safe_string_copy( szBoneName, pXML->getAttr( "AttachedBoneName"), sizeof( szBoneName));
		SetAttachBoneName( szBoneName);
	}

	return true;
}

#if defined( I3_DEBUG)

//static i3::vector<i3::rc_string> s_CollisionShapeTypeList;			// 쓰이지 않는듯 하여 주석처리..

void i3ActionCollision::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3Action::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3ActionCollision");

	//{	// Shape Type

	//	if( s_CollisionShapeTypeList.GetCount() == 0)
	//	{
	//		s_CollisionShapeTypeList.Add( L"none");
	//		s_CollisionShapeTypeList.Add( L"Plane");
	//		s_CollisionShapeTypeList.Add( L"Box");
	//		s_CollisionShapeTypeList.Add( L"Sphere");
	//		s_CollisionShapeTypeList.Add( L"Cone");
	//		s_CollisionShapeTypeList.Add( L"Line");
	//	}

	//	AddMFCProperty_Combo( pList, s_CollisionShapeTypeList, &m_ShapeType, L"Shape Type");
	//}

	//if( m_ShapeType == SHAPE_TYPE_SPHERE)
	//{
	//	I3_ACTION_COLLISION_INFO_SPHERE* pSphere = (I3_ACTION_COLLISION_INFO_SPHERE*)m_pShapeInfo;
	//	
	//	AddMFCProperty_Real32( pList, &(pSphere->_r), L"Radius");
	//	AddMFCProperty_Vec3D( pList, &(pSphere->_vCenter), L"Center Pos");
	//}


	//{	// Renderable Shape
	//	AddMFCProperty_Bool( pList, &m_bEnableRenderCollision, L"Render Shape");

	//	{	// Shape Color
	//		AddMFCProperty_Color( pList, &m_LineColor, L"Render Shape Color");
	//	}
	//}

	//if( getEventReceiver() != nullptr &&
	//	getEventReceiver()->IsTypeOf( i3Chara::GetClassMeta()) )
	//{
	//	INT32 i;
	//	i3Body * pBody = ((i3Chara*)getEventReceiver())->getSceneObject()->GetBody();

	//	m_BoneNameList.Clear();

	//	if( getAttachBoneName() != nullptr)
	//	{
	//		m_SelectedBoneIdx = pBody->FindBoneByName( getAttachBoneName());
	//	}
	//	else
	//	{
	//		m_SelectedBoneIdx = -1;
	//	}

	//	for( i = 0; i < pBody->getFrameLOD()->getBoneCount(); i++)
	//	{
	//		i3Bone * pBone = pBody->getFrameLOD()->getSkeleton()->getBone( i);

	//		m_BoneNameList.Add( pBone->getName());
	//	}
	//	
	//	AddMFCProperty_Combo( pList, &m_BoneNameList, &m_SelectedBoneIdx, L"Attached Bone Name");
	//	
	//}

	AddMFCProperty_EndGroup( List);
}

void i3ActionCollision::OnTDKPropertyChanged( const char * szName)
{
	if( i3::generic_is_iequal( szName, "Radius") )
	{
		//s_TESProc_SetSphereRadius
	}
	else if( i3::generic_is_iequal( szName, "Center Pos") )
	{
		//s_TESProc_SetSphereCenter
	}
	

	{
		i3Action::OnTDKPropertyChanged( szName);
	}
}


#endif

