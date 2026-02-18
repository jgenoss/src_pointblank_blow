#include "pch.h"
#include "TargetObject.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "i3Base/string/ext/contain_string.h"


I3_CLASS_INSTANCE( TargetObject);//, i3Object);

static i3Texture *		s_pFriendTex = nullptr;
static i3Texture *		s_pEnemyTex = nullptr;

TargetObject::TargetObject(void)
{
	m_Type			= TYPE_ENEMY;
	m_ActualType	= TYPE_ENEMY;
	m_Appear		= APT_POPUP;
	m_timeOffset	= 0.0f;
	m_timeStart		= 0.6f;
	m_timeKeep		= 3.0f;
	m_timeEnd		= 0.7f;
	m_bSmall		= false;
	m_bDestroyed	= false;

	m_State			= STATE_SLEEP;
	m_OldState		= (STATE) -1;
	m_timeLocal		= 0.0f;

	m_pAnimStart	= nullptr;
	m_pAnimKeep		= nullptr;
	m_pAnimEnd		= nullptr;
}

TargetObject::~TargetObject(void)
{
}

void TargetObject::Start(void)
{
	if((m_State == STATE_SLEEP) && (getCurrentHP() > 0.0f))
		m_State = STATE_START_ACTIVE;
}

void TargetObject::Reset(void)
{
	i3Texture * pTex = s_pEnemyTex;

	m_ActualType = getType();
	m_bDestroyed = false;

	switch( getType())
	{
		case TYPE_ENEMY :		pTex	= s_pEnemyTex;	break;
		case TYPE_FRIEND :		pTex	= s_pFriendTex;	break;
		case TYPE_RANDOM :	
			if( rand() & 1)
			{
				m_ActualType = TYPE_ENEMY;
				pTex = s_pEnemyTex;
			}
			else
			{
				m_ActualType = TYPE_FRIEND;
				pTex = s_pFriendTex;
			}
			break;
	}

	// 각 State의 Sg.들의 Texture Bind를 수정
	for( INT32 i = 0; i < getStateCount(); i++)
	{
		i3SceneGraphInfo * pSgInfo = getSceneGraphInfo( i);

		i3AttrSet * pAttrSet = (i3AttrSet *) i3Scene::FindNodeByName( pSgInfo->getInstanceSg(), "Target_Tex");
		I3ASSERT( pAttrSet != nullptr);

		i3TextureBindAttr * pBind = (i3TextureBindAttr *) i3Scene::FindAttrByExactType( pAttrSet, i3TextureBindAttr::static_meta(), false);
		I3ASSERT( pBind != nullptr);

		pBind->SetTexture( pTex);
	}
}

void TargetObject::Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet)
{
	if( m_State == STATE_ACTIVE)
	{
		REAL32 scale = 1.0f;

		if((pHitShapeSet != nullptr) && (pHitShapeSet->hasName()))
		{
			// 왜 0번 캐릭터 정보를 가져다가 쓰는것인지?
			// 데미지 비율을 아무거나 가져다 써도 되는것인가요?
			const CCharaInfo * pCharaInfo = (const CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, 0);

			I3ASSERT_RETURN(pCharaInfo);

			// 총알이 박힌 지점의 이름으로 Damage의 량을 조절한다.
			char * pszName = (char *) pHitShapeSet->GetName();

			if( pszName[0] == 'H')
			{
				// Head
				scale = pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_HEAD);
				//I3TRACE( "HIT : HEAD\n");
			}
			else if( pszName[1] == 'B')
			{
				// Body
				scale = pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_BODY);
				//I3TRACE( "HIT : BODY\n");
			}
			else
			{
				// Arm
				scale = pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_ARM);
				//I3TRACE( "HIT : ARM\n");
			}
		}

		dmg *= scale;

		i3Object::Damage( dmg, pEventSender, pHitShapeSet);

		if((getCurrentHP() <= 0.0f) && (m_bDestroyed == false))
		{
			// 파괴되었다.
			INT32 point = 4;
			m_bDestroyed = true;

			if( m_bSmall)
				point = 6;

			if(CHud::i() != nullptr)
			{
				if (m_ActualType == TYPE_ENEMY)
				{
					CHud::i()->CreateInGameMessage((INT32)TargetObject::TYPE_ENEMY, point);
				}
				else
				{
					CHud::i()->CreateInGameMessage((INT32)TargetObject::TYPE_FRIEND, point);
				}
			}
		}
	}
}

void TargetObject::OnSleep( REAL32 tm, bool bFirst)
{
	if( bFirst)
	{
		if( m_pAnimEnd != nullptr)
		{
			PlayAnim( m_pAnimEnd, I3_ANIM_PLAY_MODE_CLAMP);
			getSceneObject()->SetPlayTime( 1.0f);		
		}

		Reset();
	}
}

void TargetObject::OnStartActive( REAL32 tm, bool bFirst)
{
	if( m_timeLocal > (m_timeOffset + m_timeStart))
	{
		m_State = STATE_ACTIVE;
	}
	else if( i3Math::abs( m_timeLocal - m_timeOffset) <= tm)
	{
		PlayAnim( m_pAnimStart, I3_ANIM_PLAY_MODE_CLAMP);

		setTimeScale( 0.66f / m_timeStart);
	}
}

void TargetObject::OnActive( REAL32 tm, bool bFirst)
{
	if( bFirst)
	{
		PlayAnim( m_pAnimKeep, I3_ANIM_PLAY_MODE_CLAMP);
	}

	if( m_timeLocal > m_timeKeep)
	{
		m_State = STATE_START_SLEEP;
	}
}

void TargetObject::OnStartSleep( REAL32 tm, bool bFirst)
{
	if( bFirst)
	{
		PlayAnim( m_pAnimEnd, I3_ANIM_PLAY_MODE_CLAMP);

		setTimeScale( 0.66f / m_timeEnd);
	}

	if( m_timeLocal > m_timeEnd)
	{
		m_State = STATE_FINISH;
	}
}

void TargetObject::OnDestroy( REAL32 tm, bool bFirst)
{
}

void TargetObject::OnUpdate( REAL32 tm)
{
	i3Object::OnUpdate( tm);

	bool bFirst = (m_OldState != m_State);

	m_OldState = m_State;

	if( bFirst)
		m_timeLocal = 0.0f;

	switch( m_State)
	{
		case STATE_SLEEP :			OnSleep( tm, bFirst);	break;
		case STATE_START_ACTIVE :	OnStartActive( tm, bFirst);	break;
		case STATE_ACTIVE :			OnActive( tm, bFirst);	break;
		case STATE_START_SLEEP :	OnStartSleep( tm, bFirst);	break;
		case STATE_DESTROY :		OnDestroy( tm, bFirst);	break;
	}

	m_timeLocal += tm;
}

void TargetObject::OnChangeTime( REAL32 tm)
{
	i3Object::OnChangeTime( tm);
}

void TargetObject::_FindAnimForPopup(void)
{
	INT32 i;
	i3ResourceObject * pObj;

	for( i = 0; i < getRefObjectCount(); i++)
	{
		pObj = getRefObject( i);

		if( i3::same_of<i3Animation*>(pObj))
		{
			i3Animation * pAnim = (i3Animation *) pObj;

			if( i3::contain_string( pAnim->GetName(), "_DOWN") != -1)
			{
				m_pAnimEnd = pAnim;
			}
			else if( i3::contain_string( pAnim->GetName(), "_IDLE") != -1)
			{
				m_pAnimKeep = pAnim;
			}
			else if( i3::contain_string( pAnim->GetName(), "_STAND") != -1)
			{
				m_pAnimStart = pAnim;
			}
		}
		else if( i3::kind_of<i3Texture*>(pObj))
		{
			i3Texture * pTex = (i3Texture *) pObj;

			if( i3::contain_string( pTex->GetName(), "TARGET01") != -1)
			{
				s_pEnemyTex = pTex;
			}
			else if( i3::contain_string( pTex->GetName(), "TARGET02") != -1)
			{
				s_pFriendTex = pTex;
			}
			
		}
	}
}

void TargetObject::_FindAnimForSlide(void)
{
	INT32 i;
	i3ResourceObject * pObj;

	for( i = 0; i < getRefObjectCount(); i++)
	{
		pObj = getRefObject( i);

		if( i3::same_of<i3Animation*>(pObj))
		{
		
			i3Animation * pAnim = (i3Animation *) pObj;

			if( getAppearType() == APT_SLIDE_LEFT)
			{
				if( i3::contain_string( pAnim->GetName(), "_LEFTTOCENTER") != -1)
				{
					m_pAnimEnd = pAnim;
				}
				else if( i3::contain_string( pAnim->GetName(), "_LEFTIDLE") != -1)
				{
					m_pAnimKeep = pAnim;
				}
				else if( i3::contain_string( pAnim->GetName(), "_CENTERTOLEFT") != -1)
				{
					m_pAnimStart = pAnim;
				}
			}
			else
			{
				if( i3::contain_string( pAnim->GetName(), "_RIGHTTOCENTER") != -1)
				{
					m_pAnimEnd = pAnim;
				}
				else if( i3::contain_string( pAnim->GetName(), "_RIGHTIDLE") != -1)
				{
					m_pAnimKeep = pAnim;
				}
				else if( i3::contain_string( pAnim->GetName(), "_CENTERTORIGHT") != -1)
				{
					m_pAnimStart = pAnim;
				}
			}
		}
		else if( i3::kind_of<i3Texture*>(pObj))
		{
			i3Texture * pTex = (i3Texture *) pObj;

			if( i3::contain_string( pTex->GetName(), "TARGET01") != -1)
			{
				s_pEnemyTex = pTex;
			}
			else if( i3::contain_string( pTex->GetName(), "TARGET02") != -1)
			{
				s_pFriendTex = pTex;
			}
			
		}
	}
}

void TargetObject::OnBindResource(void)
{
	i3Object::OnBindResource();

	i3SceneGraphInfo * pSgInfo = getSceneGraphInfo( 0);
	I3ASSERT( pSgInfo != nullptr);

	if( (i3::contain_string( pSgInfo->GetName(), "TARGET02") != -1) ||
		(i3::contain_string( pSgInfo->GetName(), "TARGET04") != -1))
	{
		m_bSmall = true;
	}

	if( m_pAnimStart == nullptr)
	{
		switch( m_Appear)
		{
			case APT_POPUP :
				_FindAnimForPopup();
				break;

			default :
				_FindAnimForSlide();
				break;
		}
	}
}

void TargetObject::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3Object::CopyTo( pObj, method);

	I3ASSERT( i3::same_of<TargetObject*>(pObj));

	TargetObject * pTarget = (TargetObject *) pObj;

	pTarget->setType( getType());
	pTarget->setAppearType( getAppearType());
	pTarget->setOffsetTime( getOffsetTime());
	pTarget->setStartTime( getStartTime());
	pTarget->setKeepTime( getKeepTime());
	pTarget->setEndTime( getEndTime());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct ALIGN4 PERSIST_TARGET_OBJECT
{
	UINT8		m_ID[4];
	INT32		m_Type;
	INT32		m_Appear;
	REAL32		m_timeOffset;
	REAL32		m_timeStart;
	REAL32		m_timeKeep;
	REAL32		m_timeEnd;
	UINT32		pad[32];
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 TargetObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_TARGET_OBJECT data;

	result = i3Object::OnSave( pResFile);
	I3_CHKIO( result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "TOB1", 4);
	data.m_Type			= (INT32) getType();
	data.m_Appear		= (INT32) getAppearType();
	data.m_timeOffset	= getOffsetTime();
	data.m_timeStart	= getStartTime();
	data.m_timeKeep		= getKeepTime();
	data.m_timeEnd		= getEndTime();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 TargetObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_TARGET_OBJECT data;

	result = i3Object::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setType( (TargetObject::TYPE) data.m_Type);
	setAppearType( (TargetObject::APPEAR_TYPE) data.m_Appear);
	setOffsetTime( data.m_timeOffset);
	setStartTime( data.m_timeStart);
	setKeepTime( data.m_timeKeep);
	setEndTime( data.m_timeEnd);

	return result;
}


UINT32 TargetObject::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	UINT32 sz;
	PERSIST_TARGET_OBJECT * pData;

	sz = sizeof(PERSIST_TARGET_OBJECT);

	*ppData = pData = (PERSIST_TARGET_OBJECT *) i3MemAlloc( sz);
	
	i3mem::FillZero( pData, sz);
	i3mem::Copy( pData->m_ID, "TOB1", 4);

	pData->m_Type			= (INT32) getType();
	pData->m_Appear			= (INT32) getAppearType();
	pData->m_timeOffset		= getOffsetTime();
	pData->m_timeStart		= getStartTime();
	pData->m_timeKeep		= getKeepTime();
	pData->m_timeEnd		= getEndTime();

	return sz;
}

UINT32 TargetObject::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	PERSIST_TARGET_OBJECT data;
	i3Stream * pStream = pInfo->getStream();
	UINT32 rc, result = 0;

	rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setType( (TargetObject::TYPE) data.m_Type);
	setAppearType( (TargetObject::APPEAR_TYPE) data.m_Appear);
	setOffsetTime( data.m_timeOffset);
	setStartTime( data.m_timeStart);
	setKeepTime( data.m_timeKeep);
	setEndTime( data.m_timeEnd);

	I3ASSERT( result == sz);

	return result;
}

#if defined(I3_DEBUG)//XML
bool	TargetObject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "TargetType",	(INT32) getType());
	pXML->addAttr( "AppearType",	(INT32) getAppearType());
	pXML->addAttr( "TimeOffset",	getOffsetTime());
	pXML->addAttr( "TimeStart",		getStartTime());
	pXML->addAttr( "TimeKeep",		getKeepTime());
	pXML->addAttr( "TimeEnd",		getEndTime());

	return true;
}

bool	TargetObject::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->getAttr( "TargetType",  (INT32 *)&m_Type);
	pXML->getAttr( "AppearType",	(INT32 *)&m_Appear);
	pXML->getAttr( "TimeOffset",	&m_timeOffset);
	pXML->getAttr( "TimeStart",		& m_timeStart);
	pXML->getAttr( "TimeKeep",		& m_timeKeep);
	pXML->getAttr( "TimeEnd",		& m_timeEnd);

	return true;
}
#endif

