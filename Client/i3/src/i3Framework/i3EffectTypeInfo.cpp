#include "i3FrameworkPCH.h"
#include "i3EFfectTypeInfo.h"
#include "i3GameResSceneGraph.h"
#include "i3GameResSound.h"
#include "i3Framework.h"
#include "i3Game.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/algorithm/to_upper.h"

#define DECAL_POINTS_COUNT 32

I3_CLASS_INSTANCE( i3EffectTypeInfo);

i3EffectTypeInfo::i3EffectTypeInfo(void)
{
	m_pTypeRoot = i3Node::new_object();
	m_pTypeRoot->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
}

i3EffectTypeInfo::~i3EffectTypeInfo(void)
{
	DeleteAll();

	I3_SAFE_RELEASE( m_pTypeRoot);
}

void i3EffectTypeInfo::SetName( const char * pszName)
{
	char conv[MAX_PATH];
	i3::safe_string_copy( conv, pszName, MAX_PATH);
	i3::to_upper( conv);

	m_Name = pszName;

	m_HashCode = CRC32( 0xFFFFFFFF, (UINT8 *) conv, i3::generic_string_size( conv));
}

void i3EffectTypeInfo::_UnlinkFromUsedList( i3TimeStamp * pNode)
{
	i3TimeStamp * pTemp;

	I3ASSERT( pNode != nullptr);

	if( pNode->getPrev() != nullptr)
	{
		pTemp = (i3TimeStamp *) pNode->getPrev();

		pTemp->setNext( pNode->getNext());
	}
	else
	{
		// Head인 경우
		m_pUsedHead = (i3TimeStamp *) pNode->getNext();
	}

	if( pNode->getNext() != nullptr)
	{
		pTemp = (i3TimeStamp *) pNode->getNext();

		pTemp->setPrev( pNode->getPrev());
	}
	else
	{
		// Tail인 경우
		m_pUsedTail = (i3TimeStamp *) pNode->getPrev();
	}

	pNode->setPrev(nullptr);
	pNode->setNext(nullptr);
}

void i3EffectTypeInfo::_LinkToUsedList( i3TimeStamp * pNode)
{
	I3ASSERT( pNode != nullptr);

	if( m_pUsedHead != nullptr)
	{
		m_pUsedHead->setPrev( pNode);
	}

	if( m_pUsedTail == nullptr)
	{
		m_pUsedTail = pNode;
	}

	pNode->setPrev(nullptr);
	pNode->setNext( m_pUsedHead);

	m_pUsedHead = pNode;
}

void i3EffectTypeInfo::_UnlinkFromFreeList( i3TimeStamp * pNode)
{
	i3TimeStamp * pTemp;

	I3ASSERT( pNode != nullptr);

	if( pNode->getPrev() != nullptr)
	{
		pTemp = (i3TimeStamp *) pNode->getPrev();

		pTemp->setNext( pNode->getNext());
	}
	else
	{
		// Free Head...
		m_pFreeHead = (i3TimeStamp *) pNode->getNext();
	}

	if( pNode->getNext() != nullptr)
	{
		pTemp = (i3TimeStamp *) pNode->getNext();

		pTemp->setPrev( pNode->getPrev());
	}

	pNode->setNext(nullptr);
	pNode->setPrev(nullptr);
}

void i3EffectTypeInfo::_LinkToFreeList( i3TimeStamp * pNode)
{
	I3ASSERT( pNode != nullptr);

	if( m_pFreeHead != nullptr)
		m_pFreeHead->setPrev( pNode);

	pNode->setPrev(nullptr);
	pNode->setNext( m_pFreeHead);

	m_pFreeHead = pNode;
}

void i3EffectTypeInfo::Reset(void)
{
	i3TimeStamp * pStamp = m_pUsedHead;
	i3TimeStamp * pNext;

	while( pStamp != nullptr)
	{
		pNext = (i3TimeStamp *) pStamp->getNext();

		pStamp->DeactivateNode();

		pStamp->setRefObject(nullptr);

		_LinkToFreeList( pStamp);

		pStamp = pNext;
	}

	m_pTypeRoot->RemoveAllChild();

	m_pUsedHead = m_pUsedTail = nullptr;
}

void i3EffectTypeInfo::Create( i3GameResSceneGraph * pRes, UINT32 count, bool bDecal )
{
	INT32 i;
	INT32 cnt;
	i3TimeStamp * pTimeStamp;
	i3Node * pCloneSg;
	bool bAdd;

	cnt = getInstanceCount() - (INT32) count;

	if( cnt > 0)
	{
		bAdd = false;
	}
	else
	{
		cnt = -cnt;
		bAdd = true;
	}

	m_pTypeRoot->SetName( pRes->GetNameString());

	if( bAdd)
	{
		// 모자라는 개수만큼 추가한다.
		_InitSoundNode( pRes->getScene());

		// 데칼이면 Sprite를 Decal로 변경해줍니다.
		
		if( bDecal )
		{
			for( i = 0; i < cnt; i++)
			{
				pTimeStamp = i3TimeStamp::new_object();
				pTimeStamp->DeactivateNode();

				{
					i3MatrixObjectProxy * pProxy = i3MatrixObjectProxy::new_object_ref();
					pTimeStamp->SetMatrixObject( pProxy);
				}

				pCloneSg = i3Scene::CreateClone( pRes->getScene(), I3_COPY_INSTANCE);

				pTimeStamp->AddChild( pCloneSg);

				_LinkToFreeList( pTimeStamp);
			}
		}
		else
		{
			for( i = 0; i < cnt; i++)
			{
				pTimeStamp = i3TimeStamp::new_object();
				pTimeStamp->DeactivateNode();
				
				{
					i3MatrixObjectProxy * pProxy = i3MatrixObjectProxy::new_object_ref();
					pTimeStamp->SetMatrixObject( pProxy);
				}

				pCloneSg = i3Scene::CreateClone( pRes->getScene(), I3_COPY_INSTANCE);

				pTimeStamp->AddChild( pCloneSg);

				_LinkToFreeList( pTimeStamp);
			}
		}
	}
	else
	{
	//	cnt = -cnt;		// 위에 이미 처리가 있고, 이걸 쓰게 되면 아래 루틴이 안도므로 주석을 건다. (16.08.05.수빈)

		for( i = 0; i < cnt; i++)
		{
			pTimeStamp = m_pFreeHead;

			_UnlinkFromFreeList( pTimeStamp);
			I3_MUST_RELEASE( pTimeStamp);
		}
	}
}

void i3EffectTypeInfo::DeleteAll(void)
{
	i3TimeStamp * pNode, * pNext;

	// Used List
	pNode = m_pUsedHead;

	while( pNode != nullptr)
	{
		pNext = (i3TimeStamp *) pNode->getNext();

		I3_MUST_RELEASE( pNode);
		pNode = pNext;
	}

	// Free List
	pNode = m_pFreeHead;
	while( pNode != nullptr)
	{
		pNext = (i3TimeStamp *) pNode->getNext();

		I3_MUST_RELEASE( pNode);
		pNode = pNext;
	}

	m_pUsedHead = m_pUsedTail = m_pFreeHead = nullptr;
}

#define ENABLED_RESTORE_SNDPLAYINFO
void i3EffectTypeInfo::_InitSoundNode( i3Node * pEffNode)
{
#if defined( ENABLED_RESTORE_SNDPLAYINFO)
	char	szTemp[MAX_PATH];
	i3::vector<i3Node*>	lstSndPlay;
	lstSndPlay.reserve(16);

	i3Scene::FindNodeByExactType( pEffNode, i3SoundNode::static_meta(), lstSndPlay);

	INT32 nSoundNodeCount = INT32(lstSndPlay.size());

	for( INT32 nidx = 0; nidx < nSoundNodeCount; ++nidx)
	{
		i3SoundNode * pNode = (i3SoundNode*)lstSndPlay[nidx];
		i3SoundPlayInfo * pSndInfo = pNode->getSoundPlayInfo();

		if( pSndInfo == nullptr)
			continue;

		i3Sound * pSound = pSndInfo->getSound();
		if( pSound != nullptr && (pSound->hasName()))
		{
			i3GameResSound * pRes = (i3GameResSound *) i3Game::getFramework()->FindResourceByPath( pSound->GetName());
			if( pRes == nullptr)
			{
				i3::extract_filename(pSound->GetNameString(), szTemp);
				pRes = (i3GameResSound *) i3Game::getFramework()->FindResourceByOnlyName( szTemp);
			}

			if( pRes)
			{
				i3SoundPlayInfo * pSndResInfo = pRes->getSound();
				if( pSndResInfo != nullptr)
				{
					pSndInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);

					pSndInfo->RemoveStyle( I3SND_PROP_TYPE_BGM | I3SND_PROP_TYPE_VOICE);
					pSndInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);

					pSndInfo->SetDistanceInfo( pSndResInfo->getMaxDistance(),
												pSndResInfo->getMinDistance(), 
												pSndResInfo->getVolumeType());
				}
			}
		}
	}
#endif
}

i3TimeStamp * i3EffectTypeInfo::FindFree( REAL32 timeStamp, bool bForce, EffectCancelProc pProc)
{
	i3TimeStamp * pNode = nullptr;

	if( m_pFreeHead != nullptr)
	{
		pNode = m_pFreeHead;

		m_pFreeHead = (i3TimeStamp *) pNode->getNext();
		
		if( m_pFreeHead != nullptr)
			m_pFreeHead->setPrev(nullptr);
	}
	else
	{
		if( (bForce) && ( m_pUsedTail != nullptr))
		{
			pNode = m_pUsedTail;

			m_pUsedTail = (i3TimeStamp *) m_pUsedTail->getPrev();

			if( m_pUsedTail != nullptr)
			{
				m_pUsedTail->setNext(nullptr);
			}

			if( pNode == m_pUsedHead)
				m_pUsedHead = nullptr;

			if( pProc )
				pProc( pNode);
		}
	}

	if( pNode)
	{
		pNode->RemoveFromParent();
		pNode->DeactivateNode();
		pNode->setTimeStamp( timeStamp);

		pNode->setPrev(nullptr);
		pNode->setNext( m_pUsedHead);

		if( m_pUsedHead != nullptr)
			m_pUsedHead->setPrev( pNode);
		else
			m_pUsedTail = pNode;

		m_pUsedHead = pNode;

		{
			i3MatrixObjectProxy * pProxy = (i3MatrixObjectProxy*)pNode->GetMatrixObject();
			pProxy->SetSourceMatrix(nullptr);
			pProxy->SetLocalOffsetTransform(nullptr);
			pProxy->SetPositionSource(nullptr);
		}
	}

	return pNode;
}

i3TimeStamp *	i3EffectTypeInfo::Emit( REAL32 timeStamp, bool bForce, EffectCancelProc pProc)
{
	i3TimeStamp * pTimeStamp = FindFree( timeStamp, bForce, pProc);

	if( pTimeStamp != nullptr)
	{
		if( pTimeStamp->GetParent() == nullptr)
		{
			m_pTypeRoot->AddChild( pTimeStamp);
		}

		pTimeStamp->ActivateNode();

		pTimeStamp->OnChangeTime( 0.0f);
	}

	return pTimeStamp;
}

INT32 i3EffectTypeInfo::OnUpdate( REAL32 tm)
{
	i3TimeStamp * pTemp = m_pUsedHead, * pNext;
	i3Node * pNode;
	i3TimeSequence * pTimeSeq;
	INT32 cnt = 0;

	while( pTemp != nullptr)
	{
		cnt++;
		pNext = (i3TimeStamp *) pTemp->getNext();

		pNode = pTemp->GetChild( 0);

		if( i3::same_of<i3TimeSequence* >(pNode))
		{
			pTimeSeq = (i3TimeSequence*)pNode;

			if( ! pTimeSeq->IsActive())
			{
				pTemp->RemoveFromParent();
				pTemp->DeactivateNode();

				pTemp->setRefObject(nullptr);

				// Used-List에서 제거한다.
				{
					if( pTemp->getPrev() != nullptr)
					{
						i3TimeStamp * pPrev = (i3TimeStamp *) pTemp->getPrev();

						pPrev->setNext( pTemp->getNext());
					}
					else
					{
						// Head인 경우
						m_pUsedHead = (i3TimeStamp *) pTemp->getNext();

					}

					if( pTemp->getNext() != nullptr)
					{
						i3TimeStamp * pNext = (i3TimeStamp *) pTemp->getNext();

						pNext->setPrev( pTemp->getPrev());
					}
					else
					{
						// Tail인 경우
						m_pUsedTail = (i3TimeStamp *) pTemp->getPrev();
					}
				}

				// Free List로 추가
				{
					pTemp->setPrev(nullptr);
					pTemp->setNext( m_pFreeHead);

					m_pFreeHead = pTemp;
				}
			}
		}

		pTemp = pNext;
	}

	return cnt;
}
