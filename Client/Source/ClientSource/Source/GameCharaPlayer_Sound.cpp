#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaPlayer.h"

/*
static bool _SetSoundNodeProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( pNode->IsExactTypeOf( i3AIContext::GetClassMeta()) == false)
		return true;

	BOOL * isOwner = (BOOL*)pUserData;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == NULL)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if((pInfo->GetNode() != NULL) && (pInfo->GetNode()->IsTypeOf( i3SoundNode::GetClassMeta())))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == NULL) 
					continue;

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == NULL)
					continue;

				UINT32 n3DMode = I3SND_RT_3DMODE_WORLD_SPACE;
				//Listener Sound의 속성을 가지고 있으면서 Owner속성인경우만 positional sound를 disable시킨다.
				if( *isOwner == TRUE)
				{
					n3DMode = I3SND_RT_3DMODE_DISABLE;
				}

				pPlayInfo->set3DMode( n3DMode);
			}
		}
	}

	return true;
}

bool CGameCharaPlayer::PlayFootStep( I3_TERRAIN_TYPE nType,  bool bLeftfoot, BOOL bListener)
{
	I3ASSERT( g_pViewer != NULL);
	I3ASSERT( g_pSndMng != NULL);

	if( !isAlive())
		return false;

	INT32 nRand = i3Math::Rand()%2;

	INT32 nFootStepIdx = GTBT_MOVE_LEFT_1;
	if( !bLeftfoot)
	{
		nFootStepIdx += 2;
	}

	nFootStepIdx += nRand;

	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nType, nFootStepIdx, GetPos(), bListener);

	return true;
}

BOOL CGameCharaPlayer::PlayCharaDamageSound( CHARA_HIT_PART part, BOOL bImmediately)
{	//	데미지 신음소리 플레이~	
	CHARA_SOUND soundIdx = CHARA_SOUND_DAMAGE_DEFAULT;

	switch( part)
	{	
	case CHARA_HIT_HEAD		:
	case CHARA_HIT_BODY1	:
	case CHARA_HIT_BODY2	:
	case CHARA_HIT_BODY3	:		//	몸통을 맞았다.
	case CHARA_HIT_LCLAVICLE	:
	case CHARA_HIT_RCLAVICLE	:
	case CHARA_HIT_NECK		:		
		soundIdx = CHARA_SOUND_DAMAGE_CHEST;
		break;

	case CHARA_HIT_LUARM	:		//	왼쪽 팔을 맞았다.
	case CHARA_HIT_LDARM	:
	case CHARA_HIT_LHAND	:
		soundIdx = CHARA_SOUND_DAMAGE_LEFTARM;
		break;

	case CHARA_HIT_RUARM	:		//	오른쪽 팔을 맞았다.
	case CHARA_HIT_RDARM	:
	case CHARA_HIT_RHAND	:
		soundIdx = CHARA_SOUND_DAMAGE_RIGHTARM;
		break;

	case CHARA_HIT_ROOT		:
	case CHARA_HIT_LULEG	:		//	다리를 맞았다. 
	case CHARA_HIT_LDLEG	:
	case CHARA_HIT_RULEG	:
	case CHARA_HIT_RDLEG	:
	case CHARA_HIT_LFOOT	:
	case CHARA_HIT_RFOOT	:
		soundIdx = CHARA_SOUND_DAMAGE_DEFAULT;
		break;

	default :	I3TRACE( "[%s] : WARNING! Hit part(%d) is Invalid!\n", __FUNCTION__, part ); 
		soundIdx = CHARA_SOUND_DAMAGE_DEFAULT;
		break;
	}

	//	데미지 신음소리 플레이~	
	//PlayCharaSound( soundIdx, TRUE);

	return TRUE;
}

void CGameCharaPlayer::_UpdateCharaSound( REAL32 tm)
{
}


void CGameCharaPlayer::setSoundOwner()
{
	BOOL bOwner = is1PV();
	//Sound에 Owner에따라서 3dmode를 실행 할런지 말런지 결정한다.
	{
		i3GameNodeTraversal * pTraverse = i3GameNodeTraversal::NewObject();

		pTraverse->setUserData( &bOwner);
		pTraverse->setUserProc( _SetSoundNodeProc);

		pTraverse->Traverse( this);

		I3_SAFE_RELEASE( pTraverse);
	}
}
*/
