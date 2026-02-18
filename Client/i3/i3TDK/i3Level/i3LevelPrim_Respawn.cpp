#include "stdafx.h"
#include "i3LevelPrim_Respawn.h"
#include "i3LevelResPrimitive_Respawn.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelRespawn);

i3LevelRespawn::i3LevelRespawn()
{
	i3::safe_string_copy( m_szInstanceClass, "i3RespawnObj", 128);

	AddStyle( I3_LEVEL_STYLE_NO_FILERES);
}

void i3LevelRespawn::setRespawnStyle( UINT32 nStyle)
{
	m_nRespawnStyle = nStyle;

	switch( nStyle)
	{
		case 0:	SetMaterialDiffuse( 1.0f, 0.0f, 0.0f, 1.0f);	break;
		case 1:	SetMaterialDiffuse( 0.0f, 0.0f, 1.0f, 1.0f);	break;
		case 2:	SetMaterialDiffuse( 0.5f, 0.0f, 0.5f, 1.0f);	break;
		case 3:	SetMaterialDiffuse( 0.0f, 0.8f, 0.8f, 1.0f);	break;
		case 4:	SetMaterialDiffuse( 0.0f, 1.0f, 0.0f, 1.0f);	break;
	}
}

void i3LevelRespawn::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	pInfo->nHashCodeResPath		= 0xFFFFFFFF;
	pInfo->nHashCodeInstClass	= CRC32( 0xFFFFFFFF, (UINT8 *) m_szInstanceClass, strlen( m_szInstanceClass));

	// Name
	i3::safe_string_copy( pInfo->m_szName, GetName(), _countof( pInfo->m_szName));

	// Translation
	pInfo->vPos.x = getPos()->x;
	pInfo->vPos.y = getPos()->y;
	pInfo->vPos.z = getPos()->z;

	// Rotation
	{
		QUATERNION rot;
		MATRIX tmp;
		VEC3D vec;

		
		i3Vector::Normalize( &vec, i3Matrix::GetRight(GetTransform()));
		i3Matrix::SetRight( &tmp, vec.x, vec.y, vec.z, 0.0f);

		i3Vector::Normalize( &vec, i3Matrix::GetUp(GetTransform()));
		i3Matrix::SetUp( &tmp, vec.x, vec.y, vec.z, 0.0f);

		i3Vector::Normalize( &vec, i3Matrix::GetAt(GetTransform()));
		i3Matrix::SetAt( &tmp, vec.x, vec.y, vec.z, 0.0f);

		i3Matrix::GetRotateQuat( &rot, &tmp);
		i3Quat::Normalize( &rot, &rot);

		pInfo->qRot.x = rot.x;
		pInfo->qRot.y = rot.y;
		pInfo->qRot.z = rot.z;
		pInfo->qRot.w = rot.w;
	}

	// Scale
	VEC3D scale;

	GetScale( &scale);
	pInfo->vScale.x			= scale.x;
	pInfo->vScale.y			= scale.y;
	pInfo->vScale.z			= scale.z;

	pInfo->bInitEnable		= (IsStyle( I3_LEVEL_STYLE_INIT_DISABLE) == FALSE);

	pInfo->nIDPortal		= 0;

	pInfo->nDynamicState	= I3GAME_DYNAMIC_STATE_NONE;

	pInfo->nParam1			= m_nRespawnStyle;
}

void i3LevelRespawn::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelPrimitive::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelRespawn* >(pDest));

	i3LevelRespawn * pRespawn = (i3LevelRespawn*)pDest;

	pRespawn->setRespawnStyle( getRespawnStyle());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 RESPAWN
	{
		UINT8		m_ID[4] = { 'T', 'O', 'B', '1' };
		UINT32		m_nRespawnStyle = 0;
		UINT32		m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelRespawn::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::RESPAWN Info;

	Result = i3LevelPrimitive::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	Info.m_nRespawnStyle = m_nRespawnStyle;

	Rc  = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelRespawn::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3LevelRespawn::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::RESPAWN Info;

	Result = i3LevelPrimitive::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelRespawn::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	setRespawnStyle( Info.m_nRespawnStyle);

	return Result;
}

bool i3LevelRespawn::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelPrimitive::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->addAttr( "RespawnStyle", m_nRespawnStyle);

	return true;
}

bool i3LevelRespawn::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3LevelPrimitive::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->getAttr( "RespawnStyle", (INT32 *) & m_nRespawnStyle);

	return true;
}

void i3LevelRespawn::CreateSymbol(void)
{
	i3Node * pNode = BuildSg();
	I3ASSERT( pNode != nullptr);

	SetSymbol( pNode);
}

i3Node * i3LevelRespawn::BuildSg(void)
{
	i3LevelResSceneGraph * pRes = g_pScene->GetCommonStartPointResource();

	i3Node * pClone = i3Scene::CreateClone( pRes->GetSceneRoot(), I3_COPY_INSTANCE);

	return pClone;
}
