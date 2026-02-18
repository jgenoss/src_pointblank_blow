#include "i3FrameworkPCH.h"
#include "i3ActionEmitEffect.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3ActionEmitEffect);

i3ActionEmitEffect::i3ActionEmitEffect(void)
{
	
	
	addStyle( I3_ACTION_STYLE_NEEDCOPYINST);
}

i3ActionEmitEffect::~i3ActionEmitEffect(void)
{
	I3_SAFE_RELEASE( m_pEffectName);
	I3_SAFE_RELEASE( m_pAttachBone);
}

void i3ActionEmitEffect::SetEffectName( const char * pszName)
{
	UINT32 len;

	if( m_pEffectName == nullptr)
	{
		m_pEffectName = i3MemoryBuffer::new_object();
	}

	len = i3::generic_string_size( pszName);

	m_pEffectName->Create( len + 1);

	i3::safe_string_copy( (char *)m_pEffectName->getBuffer(), pszName, m_pEffectName->GetSize() );

	m_idxEffect = -1;
}

void i3ActionEmitEffect::SetAttachBoneName( const char * pszName)
{
	UINT32 len;

	if( m_pAttachBone == nullptr)
	{
		m_pAttachBone = i3MemoryBuffer::new_object();
	}

	len = i3::generic_string_size( pszName);

	m_pAttachBone->Create( len + 1);

	m_pBoneMatrix = nullptr;

	i3::safe_string_copy( (char *) m_pAttachBone->getBuffer(), pszName, m_pAttachBone->GetSize() );
}

void i3ActionEmitEffect::setOffset( VEC3D * pOff)
{
	i3Vector::Copy( &m_Offset, pOff);

	i3Matrix::SetPos( &m_LocalTx, pOff);
}

void i3ActionEmitEffect::_updateLocalTx(void)
{
	i3Matrix::SetRotateX( &m_LocalTx, m_Rho);
	i3Matrix::PostRotateY( &m_LocalTx, m_Theta);
	i3Matrix::SetPos( &m_LocalTx, &m_Offset);
}

void i3ActionEmitEffect::setEmitTheta( REAL32 th)
{
	m_Theta = th;

	_updateLocalTx();
}

void i3ActionEmitEffect::setEmitRho( REAL32 rho)
{
	m_Rho = rho;

	_updateLocalTx();
}

void i3ActionEmitEffect::reset(void)
{
	m_timeLife = 0.0f;
	m_timeInterval = 0.0f;
}

void i3ActionEmitEffect::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Effect 발사", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Effect를 Scene에 발사(추가)한다..", len);
	}
}

void i3ActionEmitEffect::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pEffectName != nullptr)
	{
		m_pEffectName->OnBuildObjectList( List);
	}

	if( m_pAttachBone != nullptr)
	{
		m_pAttachBone->OnBuildObjectList( List);
	}

	i3Action::OnBuildObjectList( List);
}

void i3ActionEmitEffect::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionEmitEffect * pEffect = (i3ActionEmitEffect *) pDest;

	I3_REF_CHANGE( pEffect->m_pEffectName, m_pEffectName);
	I3_REF_CHANGE( pEffect->m_pAttachBone, m_pAttachBone);
	pEffect->m_State			= m_State;
	pEffect->m_idxEffect		= m_idxEffect;
	pEffect->m_pBoneMatrix		= m_pBoneMatrix;
	pEffect->m_RepeatDuration	= m_RepeatDuration;
	pEffect->m_RepeatInterval	= m_RepeatInterval;
	pEffect->m_nEmitStyle		= m_nEmitStyle;

	pEffect->setOffset( getOffset());
	pEffect->setEmitTheta( getEmitTheta());
	pEffect->setEmitRho( getEmitRho());
	pEffect->setScale( getScale());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_EMITEFFECT
	{
		UINT8	m_ID[4] = { 'A', 'E', 'E', '1' };
		OBJREF	m_pEffectName = 0;
		INT32	m_nEmitStyle = 0;
		OBJREF	m_pAttachBone = 0;
		UINT32	pad[4] = { 0 };
	};

	struct ALIGN4 ACTION_EMITEFFECT2
	{
		UINT8			m_ID[4] = { 'A', 'E', 'E', '2' };
		OBJREF			m_pEffectName = 0;
		INT32			m_nEmitStyle = 0;
		OBJREF			m_pAttachBone = 0;
		i3::pack::VEC3D	m_Offset;
		REAL32			m_Theta = 0.0f;
		REAL32			m_Rho = 0.0f;
		REAL32			m_RepeatDuration = 0.0f;
		REAL32			m_RepeatInterval = 0.0f;
		REAL32			m_Scale = 0.0f;
		UINT32			pad = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ActionEmitEffect::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_EMITEFFECT2 data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pEffectName		= pResFile->GetObjectPersistID( m_pEffectName);
	data.m_pAttachBone		= pResFile->GetObjectPersistID( m_pAttachBone);
	data.m_nEmitStyle		= (INT32) m_nEmitStyle;
	data.m_RepeatDuration	= getRepeatDuration();
	data.m_RepeatInterval	= getRepeatInterval();

	data.m_Offset.x			= m_Offset.x;
	data.m_Offset.y			= m_Offset.y;
	data.m_Offset.z			= m_Offset.z;

	data.m_Theta			= m_Theta;
	data.m_Rho				= m_Rho;
	data.m_Scale			= getScale();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionEmitEffect::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_EMITEFFECT2 data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	char id[4];

	rc = pResFile->Read( id, 4);

	I3_CHKIO( rc );

	pResFile->GetStream()->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "AEE2", 4) != 0)
	{
		pack::ACTION_EMITEFFECT old;

		rc = pResFile->Read( &old, sizeof(old));
		I3_CHKIO( rc);
		result += rc;

		data.m_nEmitStyle = old.m_nEmitStyle;
		data.m_pAttachBone = old.m_pAttachBone;
		data.m_pEffectName = old.m_pEffectName;

		data.m_RepeatDuration = 0.0f;
		data.m_RepeatInterval = 0.0f;

		data.m_Offset.x = 0.0f;
		data.m_Offset.y = 0.0f;
		data.m_Offset.z = 0.0f;

		data.m_Theta = 0.0f;
		data.m_Rho = 0.0f;
		data.m_Scale = 1.0f;
	}
	else
	{
		rc = pResFile->Read( &data, sizeof(data));
		I3_CHKIO( rc);
		result += rc;
	}

	if( data.m_pEffectName != 0)
	{
		i3MemoryBuffer * pBuffer = (i3MemoryBuffer *) pResFile->FindObjectByID( data.m_pEffectName);

		I3_REF_CHANGE( m_pEffectName, pBuffer);
	}

	if( data.m_pAttachBone != 0)
	{
		i3MemoryBuffer * pBuffer = (i3MemoryBuffer *) pResFile->FindObjectByID( data.m_pAttachBone);

		I3_REF_CHANGE( m_pAttachBone, pBuffer);
	}

	m_nEmitStyle = data.m_nEmitStyle;

	m_Offset.x = data.m_Offset.x;
	m_Offset.y = data.m_Offset.y;
	m_Offset.z = data.m_Offset.z;

	m_Theta = data.m_Theta;
	m_Rho = data.m_Rho;

	if( data.m_Scale > I3_EPS)
		setScale( data.m_Scale);
	else
		setScale( 1.0f);

	setRepeatDuration( data.m_RepeatDuration);
	setRepeatInterval( data.m_RepeatInterval);

	_updateLocalTx();

	addStyle( I3_ACTION_STYLE_NEEDCOPYINST);

	return result;
}
