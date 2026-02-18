#include "pch.h"
#include "AvatarManager.h"
#include "Avatar.h"
#include "../../Avatar/AvatarGen.h"
#include "AvatarThread.h"

#if 0
#define AV_TRACE	I3TRACE
#else
#define AV_TRACE	__noop
#endif

I3_CLASS_INSTANCE( cAvatarManager);

cAvatarManager::cAvatarManager(void)
{
	m_pMacroTexThread = AvatarLoadMacroTexThread::new_object();
	m_pMacroTexThread->Create( "LoadMacroTexThread");

	m_AvatarLoadingList.reserve( 16);
}

cAvatarManager::~cAvatarManager(void)
{
	for( UINT32 i = 0; i < m_AvatarLoadedList.size(); i++)
	{
		AVATAR_LOAD_INFO * pInfo = m_AvatarLoadedList.at(i);
		pInfo->_pLoadedAvatar->ReturnInstance();
	}
	m_AvatarLoadedList.clear();

	UINT32 cnt = m_ThreadList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		cThreadAvatar * pThread = m_ThreadList.at( i);
		I3_SAFE_RELEASE( pThread);
	}
	m_ThreadList.clear();

	i3Framework::FlushDXForThread();

	FlushMacroTexQue();

	I3_SAFE_RELEASE( m_pMacroTexThread);

	I3_SAFE_RELEASE( m_pTeamBandTexture[ TEAM_BLUE]);
	I3_SAFE_RELEASE( m_pTeamBandTexture[ TEAM_RED]);

	//i3GfxResource::Dump();
}

void cAvatarManager::_CreateThread( void)
{
	if( m_ThreadList.size() < m_MaxThreadCount)
	{
		cThreadAvatar * pThread = cThreadAvatar::new_object();

		char szName[ 128];
		i3::sprintf( szName, "AvatarThread%d", m_ThreadList.size());
		pThread->Create( szName);
		
		m_ThreadList.push_back( pThread);
	}
}

cThreadAvatar * cAvatarManager::_FindWaitingThread( void)
{
	UINT32 cnt = m_ThreadList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		cThreadAvatar * pThread = m_ThreadList.at(i);

		if( pThread->GetAvatarState() == ATS_WAIT &&
			pThread->GetAvatar() == nullptr )
		{
			return pThread;
		}
	}

	return nullptr;
}

INT32 cAvatarManager::_FindEmptyInfo( INT32 idx, CHARA_PARTS_INFO * pInfo)
{
	for( INT32 i = 0; i < MAX_AVATAR_INFO_COUNT; i++)
	{
		if( m_pLoadInfo[i]._state == E_AVATAR_STATE_NA)
			return i;
	}

	return -1;
}


void cAvatarManager::_LoadTeamBandTexture( void)
{
	char szPath[ MAX_PATH];
	
	i3::safe_string_copy( szPath, "Avatar/Teamband_Diff_BLUE.i3i", sizeof(szPath));
	m_pTeamBandTexture[ TEAM_BLUE] = g_pFramework->GetResourceManager()->LoadTexture( szPath);
	I3ASSERT( m_pTeamBandTexture[ TEAM_BLUE] != nullptr);
	
	i3::safe_string_copy( szPath, "Avatar/Teamband_Diff_RED.i3i", sizeof(szPath));
	m_pTeamBandTexture[ TEAM_RED] = g_pFramework->GetResourceManager()->LoadTexture( szPath);
	I3ASSERT( m_pTeamBandTexture[ TEAM_RED] != nullptr);
}

void cAvatarManager::Create( void)
{
	UINT32 cpuCount = i3CPUGetCount();
	m_MaxThreadCount = 1;

	if( cpuCount < 2)
		m_MaxThreadCount = 1;
	else if( cpuCount < 3)
		m_MaxThreadCount = 2;
	else if( cpuCount < 5)
		m_MaxThreadCount = 4;
	else
		m_MaxThreadCount = 8;

	_LoadTeamBandTexture();
}

void cAvatarManager::OnUpdate( void)
{
	if( !m_qAvatarCon.empty())
	{
		AVATAR_LOAD_INFO * pInfo = m_qAvatarCon.front();

		I3ASSERT( pInfo->_state == E_AVATAR_STATE_COMMAND);

		cThreadAvatar * pThread = _FindWaitingThread();

		if( pThread == nullptr)
		{
			_CreateThread();
			pThread = _FindWaitingThread();
		}

		if( pThread != nullptr)
		{
			pInfo->_state = E_AVATAR_STATE_LOADING;
			pInfo->_pThread = pThread;

			pThread->SetAvatarInfo( pInfo);	// ref++

			m_AvatarLoadingList.push_back( pInfo);
			AV_TRACE( "Avatar Loading %d\n", pInfo->_netIndex);
			
			m_qAvatarCon.pop();
		}
	}

	for( UINT32 i = 0; i < m_AvatarLoadingList.size(); i++)
	{
		AVATAR_LOAD_INFO * pInfo = m_AvatarLoadingList[i];

		switch( pInfo->_state)
		{
		case E_AVATAR_STATE_LOADING :			// 로드 중이다.
			{
				I3ASSERT( pInfo->_pLoadedAvatar != nullptr);

				if( pInfo->_pLoadedAvatar->GetLoadedParts() == true)
				{
					// ref = 1
					pInfo->_state = E_AVATAR_STATE_LOADED;
					pInfo->_pThread = nullptr;
					AV_TRACE( "Avatar Loaded %d %d\n", pInfo->_netIndex, pInfo->_pLoadedAvatar->GetRefCount());

					m_AvatarLoadedList.push_back( pInfo);
					i3::vu::erase_index(m_AvatarLoadingList, i);
					i--;
				}
				else
				{
					i3Framework::UpdateDXForThread();
				}
			}
			break;

		case E_AVATAR_STATE_FLUSH :
			I3ASSERT( pInfo->_pThread != nullptr);
			
			if( pInfo->_pLoadedAvatar->GetLoadedParts() == true)
			{// Thread가 종료되었으면 삭제하자!!!
				i3::vu::erase_index(m_AvatarLoadingList, i);
				pInfo->Done();
				i--;
			}
			else
			{
				pInfo->_pThread->Wakeup();
				i3Framework::UpdateDXForThread();
			}
			break;
		}
	}

	UpdateLoadMacroTex();

	// 10초간 사용하지 않는 Thread는 삭제
	//for( UINT32 i = 0; i < m_ThreadList.size(); i++)
	//{
	//	cThreadAvatar * pThread = m_ThreadList.at(i);
	//	if( pThread->GetSleepTime() > 10.f && pThread->GetAvatar() == nullptr)
	//	{
	//		I3_SAFE_RELEASE( pThread);
	//		m_ThreadList.erase( m_ThreadList.begin() + i);
	//		i--;
	//	}
	//}
}

void cAvatarManager::LeaveStage( void)
{
	while( m_qAvatarCon.empty() == false)
	{
		AVATAR_LOAD_INFO * pInfo = m_qAvatarCon.front();
		pInfo->Done();
		m_qAvatarCon.pop();
	}

	FlushMacroTexQue();

	while( m_AvatarLoadingList.size() > 0)
	{
		for( UINT32 i = 0; i < m_AvatarLoadingList.size(); i++)
		{
			AVATAR_LOAD_INFO * pInfo = m_AvatarLoadingList.at(i);

			if( pInfo->_pLoadedAvatar->GetLoadedParts() == false)
			{
				if( pInfo->_pThread != nullptr)
					pInfo->_pThread->Wakeup();
				i3Framework::UpdateDXForThread();
				FlushMacroTexQue();
			}
			else
			{
				// Thread가 종료되었으면 삭제하자!!!
				i3::vu::erase_index(m_AvatarLoadingList, i);
				pInfo->Done();
			}
		}
	}

	// 로딩은 완료되었지만 아직 바인드 안된 것이 있을 수 있다.
	for( UINT32 i = 0; i < m_AvatarLoadedList.size(); i++)
	{
		AVATAR_LOAD_INFO * pInfo = m_AvatarLoadedList.at(i);
		pInfo->Done();
	}
	m_AvatarLoadedList.clear();

	// 아직 쓰레드에 바인드된게 있다면 해제시킨다.
	for( UINT32 i = 0; i < m_ThreadList.size(); i++)
	{
		cThreadAvatar * pThread = m_ThreadList.at(i);
		I3_SAFE_RELEASE( pThread);
	}
	m_ThreadList.clear();

	FlushMacroTexQue();
}

void cAvatarManager::LostDevice( void)
{
	WaitingForLoadAvatar();
}

void cAvatarManager::Revive( i3RenderContext * pCtx)
{
	
}

void cAvatarManager::WaitingForLoadAvatar( void)
{
	// 로딩이 완료되기를 기다린다.
	for( UINT32 i = 0; i < m_AvatarLoadingList.size(); i++)
	{
		AVATAR_LOAD_INFO * pInfo = m_AvatarLoadingList.at(i);
		if( pInfo->_pThread != nullptr)
		{
			while( pInfo->_pLoadedAvatar->GetLoadedParts() == false)
			{
				if( pInfo->_pThread != nullptr)
					pInfo->_pThread->Wakeup();
				i3Framework::UpdateDXForThread();
			}

			// ref = 1
			pInfo->_state = E_AVATAR_STATE_LOADED;
			pInfo->_pThread = nullptr;

			m_AvatarLoadedList.push_back( pInfo);
			i3::vu::erase_index(m_AvatarLoadingList, i);
			i--;
		}
	}
}

INT32 cAvatarManager::LoadAvatarQue( void (*fn)(void*,INT32,AI_BIND_DATA*), GLOBAL_CHARA_INFO* pCharaInfo,
									INT32 idx, bool bUseAvatar)
{
	INT32 i;
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	if( pStage == nullptr)
		return -1;

	i3InstanceManager * pManager = pStage->GetInstanceManager( CGameCharaBase::static_meta());
	if( pManager == nullptr)
		return -1;

	I3ASSERT( pCharaInfo->GetParts()->IsValid() == true);

	UINT32 cnt = 0;
	while( !m_qAvatarCon.empty() && cnt < m_qAvatarCon.size())
	{
		AVATAR_LOAD_INFO * pInfo = m_qAvatarCon.front();
		m_qAvatarCon.pop();

		if( pInfo->_netIndex == idx)
		{
			if( pInfo->_CharaInfo.GetParts()->IsSame( pCharaInfo->GetParts()) == false
				|| pInfo->_CharaInfo.GetTeam() != pCharaInfo->GetTeam())
			{
				g_pCharaManager->RemoveCharacter_Impl( pInfo->_pLoadedAvatar);
				pInfo->Done();
				break;
			}
			else
			{
				m_qAvatarCon.push( pInfo);
				// 동일한 정보로 다시 명령이 들어왔다.
				return -1;
			}
		}
		else
			m_qAvatarCon.push( pInfo);
		
		cnt++;
	}

	// 같은 유저의 경우 이전 캐릭터와 같은 경우인지 확인
	for( i = 0; i < MAX_AVATAR_INFO_COUNT; i++)
	{
		if( m_pLoadInfo[i]._netIndex == idx )
		{
			if( m_pLoadInfo[ i]._state == E_AVATAR_STATE_LOADING)
			{
				I3ASSERT( m_pLoadInfo[i]._pLoadedAvatar != nullptr);

				if( (m_pLoadInfo[i]._CharaInfo.GetParts()->IsSame( pCharaInfo->GetParts()) == false)
					|| m_pLoadInfo[i]._CharaInfo.GetTeam() != pCharaInfo->GetTeam() )	// 팀이 변경되도 다시 로딩
				{// 다르다면 다시 생성 필요
					// 이전 캐릭터는 버리자!!
					m_pLoadInfo[i]._state = E_AVATAR_STATE_FLUSH;
					g_pCharaManager->RemoveCharacter_Impl( m_pLoadInfo[i]._pLoadedAvatar);

					AV_TRACE( "Avatar Flush(Loading) %d %d\n", m_pLoadInfo[i]._netIndex,
						m_pLoadInfo[i]._pLoadedAvatar->GetRefCount());
				}
				else
				{
					// 같다면 아직 로딩중이다.
					return -1;
				}
			}
			else if( m_pLoadInfo[i]._state == E_AVATAR_STATE_LOADED)
				return i;
		}
	}

	// 새로이 생성되는 경우
	for( i = 0; i < MAX_AVATAR_INFO_COUNT; i++)
	{
		if( m_pLoadInfo[ i]._state == E_AVATAR_STATE_NA)
		{
			I3ASSERT( m_pLoadInfo[ i]._pLoadedAvatar == nullptr);
			m_pLoadInfo[ i]._netIndex = idx;
			m_pLoadInfo[ i]._pCharaInfo = pCharaInfo;
			pCharaInfo->CopyTo( &m_pLoadInfo[ i]._CharaInfo);
			m_pLoadInfo[ i]._state = E_AVATAR_STATE_COMMAND;
			
			pManager->setInstanceMeta( Avatar::static_meta());

			m_pLoadInfo[ i]._pStage = pStage;
			m_pLoadInfo[ i]._pLoadedAvatar = (Avatar *) pManager->GetFreeInstance();	// ref 2

			m_pLoadInfo[ i]._pLoadedAvatar->SetCallBack( fn);
			m_pLoadInfo[ i]._pLoadedAvatar->SetThreadMode( bUseAvatar);
			m_pLoadInfo[ i]._pLoadedAvatar->Create( m_pLoadInfo[ i]._pCharaInfo);	//생성
			
			if( bUseAvatar )
			{
				m_qAvatarCon.push( &m_pLoadInfo[i]);
			}
			else
				m_pLoadInfo[ i]._state = E_AVATAR_STATE_LOADED;

			return i;
		}
	}

	return -1;
}


Avatar * cAvatarManager::GetLoadAvatarQue( INT32 loadIndex, bool bUseAvatar)
{
	I3_BOUNDCHK( loadIndex, MAX_AVATAR_INFO_COUNT);

	AVATAR_LOAD_INFO * pInfo = &m_pLoadInfo[ loadIndex];
	Avatar * pAvatar = pInfo->_pLoadedAvatar;
	
	if( bUseAvatar == false)
	{	// 아바타는 사용하지 않는다.	// 기본 더미로 사용
		pInfo->Done();
	}

	return pAvatar;
}

Avatar * cAvatarManager::GetLoadedAvatar()
{
	for( UINT32 i = 0; i < m_AvatarLoadedList.size(); i++)
	{
		AVATAR_LOAD_INFO * pInfo = m_AvatarLoadedList[i];

		if( pInfo->_state == E_AVATAR_STATE_LOADED)
		{
			// ref = 2
			Avatar * pAvatar = pInfo->_pLoadedAvatar;
			i3::vu::erase_index(m_AvatarLoadedList, i);
			pInfo->Done();

			return pAvatar;
		}
	}

	return nullptr;
}


void cAvatarManager::_LoadMacroTexQue( AvatarGen * pOwner, i3Texture * pSrc, i3Texture * pDest,
									  INT32 x, INT32 y, INT32 width, INT32 height, bool bCommonRes,
									  const AVATAR::TEXINFO * pTexInfo )
{
	tagLOADMACROTEXDATA * pNewInfo = &m_LoadMacroTexQueBuffer[ m_MacrotexQueWrite % LOADMACROTEXQUE_COUNT];
	
	pNewInfo->_lodStep = 0;
	pNewInfo->_offsetX = x;
	pNewInfo->_offsetY = y;
	pNewInfo->_destWidth = width;
	pNewInfo->_destHeight = height;
	pNewInfo->_bUseMacroTexture = bCommonRes;
	pNewInfo->_pTexInfo = pTexInfo;

	pNewInfo->_pSrcTex = pSrc;
	I3_MUST_ADDREF(pSrc);
	pNewInfo->_pDestTex = pDest;
	I3_MUST_ADDREF(pDest);
	pNewInfo->_pOwner = pOwner;
	I3_MUST_ADDREF(pOwner);

	m_MacrotexQueWrite++;

	AV_TRACE( "Push MacroTexQue %s(%d,%d):%d -> %s(%d,%d)\n", pSrc->GetName(), pSrc->GetWidth(), pSrc->GetHeight(), pSrc->GetRefCount(), pDest->GetName(), pDest->GetWidth(), pDest->GetHeight());
}

void cAvatarManager::LoadMacroTexQue( AvatarGen * pOwner, i3Texture * pSrc, i3Texture * pDest,
									 INT32 x, INT32 y, INT32 width, INT32 height, bool bInThread, bool bCommonRes,
									 const AVATAR::TEXINFO * pTexInfo )
{
	if( bInThread)
	{
		i3Monitor2 mon( &m_Mutex);

		while( m_MacrotexQueWrite - m_MacrotexQueRead > LOADMACROTEXQUE_COUNT)
		{// Mainthread에서 완료될때까지 대기
			i3Timer::Sleep( 10);
		}

		_LoadMacroTexQue( pOwner, pSrc, pDest, x, y, width, height, bCommonRes, pTexInfo);
	}
	else
	{
		_LoadMacroTexQue( pOwner, pSrc, pDest, x, y, width, height, bCommonRes, pTexInfo);
	}
}

void cAvatarManager::UpdateLoadMacroTex( void)
{
	if( m_pMacroTexThread->IsEmpty())
	{
		UINT32 cnt = m_MacrotexQueWrite - m_MacrotexQueRead;

		if( cnt > 0)
		{
			tagLOADMACROTEXDATA * pInfo = &m_LoadMacroTexQueBuffer[ m_MacrotexQueRead % LOADMACROTEXQUE_COUNT];

			m_pMacroTexThread->SetLoad( pInfo);

			m_MacrotexQueRead++;
		}
	}
}

void cAvatarManager::FlushMacroTexQue( void)
{
	UINT32 cnt = m_MacrotexQueWrite - m_MacrotexQueRead;

	for( UINT32 i = 0; i < cnt; i++)
	{
		tagLOADMACROTEXDATA * pInfo = &m_LoadMacroTexQueBuffer[ m_MacrotexQueRead % LOADMACROTEXQUE_COUNT];
		if( pInfo->_pOwner != nullptr)
		{
			AvatarGen * pGen = reinterpret_cast<AvatarGen*>(pInfo->_pOwner);
			I3_MUST_RELEASE(pGen);
			pInfo->_pOwner = nullptr;
		}

		pInfo->Clear();

		m_MacrotexQueRead++;
	}
}

void cAvatarManager::PauseThread( void)
{
	UINT32 cnt = m_ThreadList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		cThreadAvatar * pThread = m_ThreadList.at( i);
		pThread->Pause();
	}
}

void cAvatarManager::ResumeThread( void)
{
	UINT32 cnt = m_ThreadList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		cThreadAvatar * pThread = m_ThreadList.at( i);
		pThread->Resume();
	}
}

void cAvatarManager::CopyResizingTexture( i3Texture * pDestTex, i3Texture * pSrcTex, UINT32 destLV, INT32 lvGap)
{
	char * pDest = pDestTex->GetLockedBuffer( destLV);
	char * pSrc = pSrcTex->GetLockedBuffer( 0);

	UINT32 pixelSize = pSrcTex->GetDataSize( 0) / (pSrcTex->GetWidth() * pSrcTex->GetHeight());

	//UINT32 destlines = (lvGap * pSrcTex->GetWidth());	// dest의 line size

	UINT32 destPitch = i3Gfx::CalcRasterBufferSize( pDestTex->GetWidth() / (1<<destLV), 1, 4, pDestTex->GetFormat());

	UINT32 srcHeight = pSrcTex->GetHeight();
	UINT32 srcWidth = pSrcTex->GetWidth();

	for( UINT32 h = 0; h < srcHeight; h++)
	{
		UINT32 offset = (h * srcWidth);

		for( UINT32 w = 0; w < srcWidth; w++)
		{
			UINT32 srcIdx = w + offset;

			for( INT32 hh = 0; hh < lvGap; hh++)
			{
				UINT32 destOffset = (hh * destPitch);

				for( INT32 ww = 0; ww < lvGap; ww++)
				{
					UINT32 destIdx = ((w*lvGap) + ww) + destOffset;

					i3mem::Copy( &pDest[ (destIdx * pixelSize) ], &pSrc[ (srcIdx * pixelSize)], pixelSize);
				}
			}
		}
	}

	
}

void cAvatarManager::CopyTexture( i3Texture * pDestTex, i3Texture * pSrcTex, UINT32 destLV, INT32 srcLV, INT32 x, INT32 y, INT32 w, INT32 h)
{
	char * pDest = pDestTex->GetLockedBuffer(destLV);
	char * pSrc = pSrcTex->GetLockedBuffer(srcLV);

	/*I3ASSERT( pDestTex->GetWidth() == (UINT32) w);
	I3ASSERT( pDestTex->GetHeight() == (UINT32) h);*/

	INT32 width = (pDestTex->GetWidth() / (1<<destLV));
	INT32 height = (pDestTex->GetHeight() / (1<<destLV));

	INT32 X = x / (1<<srcLV);
	INT32 Y = y / (1<<srcLV);

	// DXT는 4bytes align을 사용합니다.
	// 4bytes에 맞춰 복사할 수 있도록..
	UINT32 pitch = i3Gfx::CalcRasterBufferSize( pSrcTex->GetWidth() / (1<<srcLV), 1, 4, pSrcTex->GetFormat());
	UINT32 destPitch = i3Gfx::CalcRasterBufferSize( width, 1, 4, pDestTex->GetFormat());

	UINT32 offset = X * 4 + Y * pitch;
	pSrc += offset;

	for( INT32 hh = 0; hh < (height>>2); hh++)
	{
		i3mem::Copy( &pDest[ hh * destPitch * 4], &pSrc[hh * pitch * 4], destPitch * 4);
	}
}



