#include "i3FrameworkPCH.h"
#include "i3ActionExplosion.h"
#include "i3Game.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionExplosion);

i3ActionExplosion::i3ActionExplosion()
{
	m_Style = I3_ACTION_STYLE_OWNPROCESS;
}

i3ActionExplosion::~i3ActionExplosion(void)
{
	I3_SAFE_RELEASE( m_pAttachBone);
}

void	i3ActionExplosion::OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code)
{
	i3Framework * pFramework = i3Game::getFramework();

	if( pFramework != nullptr)
	{
		pFramework->OnEvent( I3_EVT_ACTION, this, (UINT32) pObj, code);
	}
}

void i3ActionExplosion::SetAttachBoneName( const char * pszName)
{
	UINT32 len;

	if( m_pAttachBone == nullptr)
	{
		m_pAttachBone = i3MemoryBuffer::new_object();
	}

	len = i3::generic_string_size( pszName);

	m_pAttachBone->Create( len + 1);

	i3::safe_string_copy( (char *) m_pAttachBone->getBuffer(), pszName, m_pAttachBone->GetSize() );
}

void i3ActionExplosion::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Explosion", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "폭발.", len);
	}
}

	
void i3ActionExplosion::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pAttachBone != nullptr)
	{
		m_pAttachBone->OnBuildObjectList( List);
	}

	i3Action::OnBuildObjectList( List);
}


void i3ActionExplosion::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionExplosion * pExplosion = (i3ActionExplosion *) pDest;

	I3_REF_CHANGE( pExplosion->m_pAttachBone, m_pAttachBone);

	pExplosion->m_bAttachedBone = m_bAttachedBone;
	pExplosion->m_Range		= m_Range;
	pExplosion->m_Damage	= m_Damage;
	pExplosion->m_Force		= m_Force;
	pExplosion->m_Slope		= m_Slope;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_EXPLOSION
	{
		UINT8		m_ID[4] = { 'A', 'E', 'X', '1' };
		REAL32		m_Range = 0.0f;
		REAL32		m_Damage = 0.0f;
		REAL32		m_Force = 0.0f;
		REAL32		m_Slope = 0.0f;
		OBJREF		m_pAttachBone = 0;
		UINT32		m_bAttachedBone = 0;
	};
}

#if defined(I3_COMPILER_VC)&& defined(PACKING_RIGHT)
#pragma pack(pop)			// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionExplosion::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_EXPLOSION data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_Range = getRange();
	data.m_Damage = getDamage();
	data.m_Force = getForce();
	data.m_Slope = getSlope();
	data.m_pAttachBone = (OBJREF)pResFile->GetObjectPersistID( m_pAttachBone);
	data.m_bAttachedBone = (getAttachEnable())? 1 : 0;
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionExplosion::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_EXPLOSION data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setRange( data.m_Range);
	setDamage( data.m_Damage);
	setForce( data.m_Force);
	setSlope( data.m_Slope);

	if( data.m_pAttachBone != 0)
	{
		i3MemoryBuffer * pBuffer = (i3MemoryBuffer *) pResFile->FindObjectByID( data.m_pAttachBone);

		I3_REF_CHANGE( m_pAttachBone, pBuffer);
	}

	setAttachEnable( (data.m_bAttachedBone > 0)? true : false);

	addStyle( I3_ACTION_STYLE_OWNPROCESS);

	return result;
}
