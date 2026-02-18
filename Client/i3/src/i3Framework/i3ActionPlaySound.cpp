#include "i3FrameworkPCH.h"
#include "i3ActionPlaySound.h"
#include "i3Game.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionPlaySound);

i3ActionPlaySound::i3ActionPlaySound(void)
{
	m_Style		=  I3_ACTION_STYLE_OWNPROCESS | I3_ACTION_STYLE_NEEDCOPYINST;
}

i3ActionPlaySound::~i3ActionPlaySound(void)
{
	I3_SAFE_RELEASE( m_pSndNode);
	I3_SAFE_RELEASE( m_pAttachBone);
}

void i3ActionPlaySound::OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code)
{
	if( m_pSndNode == nullptr)
		return;

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( m_bEnableRandom == true)
		{
			UINT32 fac = (m_nRandomFactor <= 1)? 1 : m_nRandomFactor;
			INT32 nRand = i3Math::Rand()%fac;
			if( nRand != 0)
				return;
		}

		if(i3::kind_of<i3GameObj*>(pObj))
		{
			i3GameObj * pGameObj = (i3GameObj*)pObj;
			i3SceneObject * pSceneObj = pGameObj->getSceneObject();

			if( pSceneObj == nullptr)
				return;

			if( m_pSndNode->GetParent() == nullptr)
			{
				if( getAttachEnable())
				{
					i3Transform * pTrans = pSceneObj->FindBoneByName( getAttachBoneName());

					if( pTrans != nullptr)
					{
						pTrans->AddChild( m_pSndNode);
					}
					else
					{
						pSceneObj->AddChild( m_pSndNode);
					}
				}
				else
				{
					pSceneObj->AddChild( m_pSndNode);
				}
			}

			m_pSndNode->DeactivateNode();
			m_pSndNode->ActivateNode();
		}
	}
	else
	{
		m_pSndNode->DeactivateNode();
		m_pSndNode->RemoveFromParent();
	}
}

void i3ActionPlaySound::SetAttachBoneName( const char * pszName)
{
	I3ASSERT( pszName != nullptr);

	UINT32 len;

	if( m_pAttachBone == nullptr)
	{
		m_pAttachBone = i3MemoryBuffer::new_object();
	}

	len = i3::generic_string_size( pszName);

	m_pAttachBone->Create( len + 1);

	i3::safe_string_copy( (char *) m_pAttachBone->getBuffer(), pszName, m_pAttachBone->GetSize() );
}

void i3ActionPlaySound::OnChangeTime( REAL32 tm)
{
	if( m_pSndNode == nullptr)
		return;

	if( tm == 0.0f)
	{
		i3SoundPlayInfo * pInfo = m_pSndNode->getSoundPlayInfo();
		if( pInfo != nullptr)
		{
			if( pInfo->isStopOnReset())
			{
				m_pSndNode->DeactivateNode();
				m_pSndNode->RemoveFromParent();
			}
		}
	}

	i3Action::OnChangeTime( tm);
}

void i3ActionPlaySound::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionPlaySound * pAction = (i3ActionPlaySound *) pDest;

	i3SoundNode * pSrcNode = getSoundNode();
	i3SoundNode * pNode = nullptr;

	if( pSrcNode != nullptr )
	{	
		pNode = i3SoundNode::new_object_ref();

		pSrcNode->CopyTo( pNode, I3_COPY_INSTANCE);
	}

	I3_REF_CHANGE( pAction->m_pAttachBone, m_pAttachBone);

	pAction->setAttachEnable( getAttachEnable());
	pAction->setSoundNode( pNode );
	pAction->setRandomEnable( isRandomEnable());
	pAction->setRandomFactor( getRandomFactor());
}

void i3ActionPlaySound::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		i3::string_ncopy_nullpad( pszStr, "Play Sound", len);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Sound를 재생합니다.", len);
	}
}

void i3ActionPlaySound::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pAttachBone != nullptr)
	{
		m_pAttachBone->OnBuildObjectList( List);
	}

	if( m_pSndNode != nullptr)
	{
		m_pSndNode->OnBuildObjectList( List);
	}

	i3Action::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_PLAYSOUND
	{
		UINT8		m_ID[4] = { 'A', 'P', 'S', '1' };
		UINT32		m_HashCode_Deprecated = 0;	//	m_HashCode;		// 이름을 바꿔서 쓰지 않음을 확인..(2012.08.08)
		OBJREF64	m_pSndNode = 0;
		UINT8		m_nRandomEnable = 0;
		UINT8		m_bAttachBone = 0;
		UINT16		m_nRandomFactor = 0;
		OBJREF		m_pAttachBone = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3ActionPlaySound::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PLAYSOUND data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);
	
	data.m_HashCode_Deprecated = 0 ; //GetClassMeta()->getHashCode();				// 다행이도 쓰이지 않는것 같다..호환성을 위해 공간이 있어야한다..(12.08.08.수빈)
	data.m_pSndNode			= (OBJREF64)pResFile->GetObjectPersistID( getSoundNode());
	data.m_pAttachBone		= pResFile->GetObjectPersistID( m_pAttachBone);
	data.m_nRandomEnable	= ( isRandomEnable())? 1 : 0;
	data.m_bAttachBone		= (getAttachEnable())? 1 : 0;
	data.m_nRandomFactor	= getRandomFactor();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionPlaySound::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_PLAYSOUND data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( i3::generic_is_niequal( (char*)data.m_ID, "APS1", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Invalid Action( PlaySound)");
		return STREAM_ERR;
	}

	//data.m_HashCode
	if( data.m_pSndNode > 0)
	{
		i3SoundNode * pNode = (i3SoundNode *)pResFile->FindObjectByID( (OBJREF)data.m_pSndNode);
		I3ASSERT( pNode != nullptr);

		setSoundNode( pNode);
	}

	if( data.m_pAttachBone != 0)
	{
		i3MemoryBuffer * pBuffer = (i3MemoryBuffer *) pResFile->FindObjectByID( data.m_pAttachBone);

		I3_REF_CHANGE( m_pAttachBone, pBuffer);
	}

	setAttachEnable( (data.m_bAttachBone > 0)? true : false);

	setRandomEnable( (data.m_nRandomEnable > 0)? true : false);
	setRandomFactor( data.m_nRandomFactor);

	addStyle( I3_ACTION_STYLE_OWNPROCESS | I3_ACTION_STYLE_NEEDCOPYINST);

	return result;
}