#include "pch.h"
#include "GameCharaMultiWeaponContext.h"
#include "GameCharaWeaponContext.h"
#include "UserInfoContext.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudMultiWeapon.h"
#include "UserInfoContext.h"

#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaMultiWeaponContext);

CGameCharaMultiWeaponContext::CGameCharaMultiWeaponContext() : m_pLeftWeaponSlot(nullptr), m_pRightWeaponSlot(nullptr), m_pCurrentWeaponSlot(nullptr), m_nQuickChangeIdx(-1), m_bPickupChange(false), m_bDropedChange(false), m_bHUDSelectedChange(false), m_bQuickChange(false), m_bDropProhibit(false), m_bPickupProhibit(false)
{
	m_MultiWeaponList.clear();
}

CGameCharaMultiWeaponContext::~CGameCharaMultiWeaponContext()
{
	I3TRACE("###################################################################\n");
	I3TRACE("CharaIdx : %d, CGameCharaMultiWeaponContext::OnDestroy\n", m_pOwner->getCharaNetIndex());

	OnDestroy();
}

void CGameCharaMultiWeaponContext::OnCreate(CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	CGameCharaContextBase::OnCreate(pOwner, defaultView, defaultNetwork);
}

void CGameCharaMultiWeaponContext::OnDestroy()
{
	ClearWeaponSlotList();

	CGameCharaContextBase::OnDestroy();
}

void CGameCharaMultiWeaponContext::ClearWeaponSlotList()
{
#if defined(I3_DEBUG)
	I3TRACE("CGameCharaMultiWeaponContext::ClearWeaponSlotList - %d\n", m_MultiWeaponList.size());
	I3TRACE("###################################################################\n");
#endif

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if (info->pWeapon != nullptr)
		{
			info->pWeapon->ReturnToReady();
			info->pWeapon->DetachNode();
		}
	}

	if ( m_pOwner->getWeaponContext() != nullptr)
	{
		m_pOwner->getWeaponContext()->_Delete_Weapon(WEAPON_SLOT_PRIMARY);
	}

	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	// Context 해제 시, 배틀 종료 시 리스트 삭제
	i3::cu::for_each_SafeFree_clear(m_MultiWeaponList);
}

void CGameCharaMultiWeaponContext::InitMultiWeaponList()
{
	m_pOwner->getWeaponContext()->_Delete_Weapon( WEAPON_SLOT_PRIMARY);

	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	m_MultiWeaponList.clear();

	ResetChange();

	EmptyQuickChangeIdx();
}

void CGameCharaMultiWeaponContext::DeleteMultiWeapon( WeaponBase* pWeapon)
{
	MULTIWEAPON_INFO* pDeleteInfo = nullptr;
	UINT8 nDeleteIdx = 0;

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		// UI Index가 일치할 경우 - 삭제
		// 불일치하지만 UI Index가 크면 -1 해준다
		// 나머지는 Skip
		if (info->pWeapon == pWeapon)
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			info->pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
			I3TRACE("다중무기 %d슬롯 삭제 - %s\n", i, wstrWeaponName);
#endif

			if ( m_pCurrentWeaponSlot == nullptr)
			{
				m_pOwner->getWeaponContext()->NoUseWeapon(info->pWeapon);
				I3TRACE("문제 발생! 꼭 문의 바람\n");
			}
			
			EmptyQuickChangeIdx();

			pDeleteInfo = info;
			nDeleteIdx = static_cast<UINT8>(i);

			m_pCurrentWeaponSlot = nullptr;

			I3TRACE("다중무기리스트를 비울 캐릭터 Index :%d\n", m_pOwner->getCharaNetIndex());
		}
	}

	// 지울 것이 있으면 지운다
	if (pDeleteInfo != nullptr)
	{
		I3MEM_SAFE_FREE(pDeleteInfo);
		i3::vu::erase_index(m_MultiWeaponList, static_cast<size_t>(nDeleteIdx));

		for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
		{
			MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

			if ( info->nSlotNumber > nDeleteIdx)
			{
				I3TRACE("%d Slot -> ", info->nSlotNumber);
				info->nSlotNumber -= 1;
				I3TRACE("%d Slot\n", info->nSlotNumber);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// 여기서 데이터 셋팅하면 안됨! 하지만 몇번째 슬롯으로  셋팅 할지는 결정해도 될지도...
	// 1개일 경우 - Null 처리
	// 삭제한 슬롯이 마지막 번호의 슬롯이었을 경우 1번 슬롯 기준으로 데이터 셋팅
	// 그 외의 경우 삭제한 슬롯 번호를 기준으로 데이터를 셋팅
	if (m_MultiWeaponList.size() < 1)
	{
		ClearWeaponSlotList();
		I3TRACE("다중무기리스트가 비었습니다.\n");
	}
	else if ( nDeleteIdx == GetMultiWeaponCount() + 1)
	{
		SetMultiWeaponSlot(1);
	}
	else
	{
		SetMultiWeaponSlot( nDeleteIdx);
	}
}

// 다중무기를 추가하는 함수
void CGameCharaMultiWeaponContext::AddMultiWeapon(WeaponBase* pWeapon, UINT8 nSyncNumber)
{
	I3ASSERT( pWeapon != nullptr);

	// 슬롯 최대 갯수만큼 사용 중이면 추가할 수 없음
	if ( CheckEmptySlot() == FALSE)
		return;

	MULTIWEAPON_INFO * pInfo = (MULTIWEAPON_INFO*)i3MemAlloc( sizeof(MULTIWEAPON_INFO));

	i3mem::FillZero( pInfo, sizeof(MULTIWEAPON_INFO));

	pInfo->nSlotNumber = (UINT8)m_MultiWeaponList.size() + 1;
	pInfo->pWeapon = pWeapon;
	pInfo->nSyncNumber = nSyncNumber;

	m_MultiWeaponList.push_back(pInfo);

	// 리스폰 후 주무기를 다중무기에 등록할 때는 기준으로 셋팅
	if ( m_pCurrentWeaponSlot == nullptr)
	{
		SetMultiWeaponSlot(1);
	}

#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName(wstrWeaponName);
	I3TRACE("AddMultiWeapon %d Index - %s(%d)\n", pInfo->nSlotNumber, wstrWeaponName, nSyncNumber);
#endif
}

bool CGameCharaMultiWeaponContext::CheckEmptySlot()
{
	if ( UserInfoContext::i()->GetMaxMultiWeapon() <= GetMultiWeaponCount())
		return false;

	return true;
}

// 네트워크 유저의 다중무기를 변경하는 함수
void CGameCharaMultiWeaponContext::ChangeMultiWeapon(WeaponBase* pWeapon)
{
	I3ASSERT(pWeapon != nullptr);

	// 내 캐릭터의 다중무기 변경은 Cmd_ChangeWeapon으로 진행
	if ( m_pOwner->isLocal())
		return;

	// 슬롯 최대 갯수만큼 사용 중이면 추가할 수 없음
	if ( CheckEmptySlot() == FALSE)
		return;

	// 변경하려는 무기를 다중무기 리스트에서 찾아본다 
	UINT8 nMultiWeaponIdx = SearchItemIDMultiWeapon( pWeapon);

	if ( nMultiWeaponIdx == 0)
	{
		// 해당하는 무기 슬롯이 없을 경우 다중무기 리스트에 무기를 추가 - 다른 유저의 Sync 번호는 관리할 필요 없음
		AddMultiWeapon( pWeapon);
		I3TRACE("다중무기를 변경하려고 하였으나 리스트에 존재하지 않아서 추가\n");
	}
	else
	{
		// 현재 장착하고 있는 다중무기가 있을 경우 날려버림
		if ( m_pCurrentWeaponSlot != nullptr)
		{
			m_pOwner->getWeaponContext()->NoUseWeapon(m_pCurrentWeaponSlot->pWeapon);
		}

		// 추가된 슬롯을 기준으로 데이터 셋팅
		SetMultiWeaponSlot( nMultiWeaponIdx);
	}

#if defined(I3_DEBUG)
	i3::rc_wstring wstrWeaponName;
	pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
	I3TRACE("ChangeMultiWeapon - %s\n", wstrWeaponName);
#endif
}

UINT8 CGameCharaMultiWeaponContext::SearchItemIDMultiWeapon(WeaponBase* pWeapon)
{
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( pWeapon->getWeaponInfo()->GetItemID() == info->pWeapon->getWeaponInfo()->GetItemID())
		{
			return info->nSlotNumber;
		}
	}

	return 0;
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::GetPrevMultiWeaponInfo(UINT8 idx)
{
	if ( GetMultiWeaponCount() <= 1)
		return nullptr;
	else if ( idx == GetMultiWeaponCount())
		return SearchIndexSlot( 1);
	else
		return SearchIndexSlot( idx + 1);
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::GetNextMultiWeaponInfo(UINT8 idx)
{
	if ( idx > 1)
		return SearchIndexSlot( idx - 1);
	else if ( idx == 1)
		return SearchIndexSlot( GetMultiWeaponCount());
	else
		return nullptr;
}

bool CGameCharaMultiWeaponContext::IsUsingMultiWeapon( WeaponBase* pWeapon)
{
	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->pWeapon == pWeapon)
		{
			if ( info->pWeapon->getOwner() != nullptr)
			{
				//if ( info->pWeapon->getOwner()->isLocal())
				{
					return true;
				}
			}
		}
	}

	return false;
}

// idx로 받은 슬롯 기준으로 데이터를 셋팅하는 함수 - Drop, Add, Select
void CGameCharaMultiWeaponContext::SetMultiWeaponSlot(UINT8 idx)
{
	// 혹시 모를 버그 방지를 위해 셋팅 전 데이터를 초기화
	m_pCurrentWeaponSlot = nullptr;
	m_pLeftWeaponSlot = nullptr;
	m_pRightWeaponSlot = nullptr;

	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		// 현재 다중무기 슬롯
		if ( info->nSlotNumber == idx)
		{
			m_pCurrentWeaponSlot = info;

			if ( m_pOwner->isLocal())
			{
				UIHudMultiWeapon * pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
				I3ASSERT( pHud != nullptr);

				pHud->SetCurrentSlotIdx( idx);
			}
		}

		// 좌측 다중무기 슬롯(2개 이상 다중무기가 있을 경우만 존재) - 다음 번호
		if ( GetMultiWeaponCount() > 1)
		{
			// 기준이 마지막 슬롯일 경우 1번 슬롯을 셋팅
			// 그 외에는 기준 Idx + 1 슬롯을 셋팅
			if ( idx == GetMultiWeaponCount() && info->nSlotNumber == 1)
				m_pLeftWeaponSlot = info;
			else if ( idx != GetMultiWeaponCount() && info->nSlotNumber == idx + 1)
				m_pLeftWeaponSlot = info;
		}

		// 우측 다중무기 슬롯(3개 이상 다중무기가 있을 경우만 존재) - 이전 번호
		if ( GetMultiWeaponCount() > 2)
		{
			// 기준이 1번 슬롯일 경우 마지막 슬롯을 셋팅
			// 그 외에는 기준 Idx - 1 슬롯을 셋팅
			if ( idx == 1 && info->nSlotNumber == GetMultiWeaponCount())
				m_pRightWeaponSlot = info;
			else if ( idx != 1 && info->nSlotNumber == idx - 1)
				m_pRightWeaponSlot = info;
		}
	}
}

UINT8 CGameCharaMultiWeaponContext::GetMultiWeaponIdx(UINT8 nSyncNum)
{
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->nSyncNumber == nSyncNum)
			return info->nSlotNumber;
	}

	return 0;
}

void CGameCharaMultiWeaponContext::SetQuickChangeIdx()
{
	if ( GetCurrSlotInfo() == nullptr)
		return;

	m_nQuickChangeIdx = GetCurrSlotInfo()->nSlotNumber;
	I3TRACE("SetQuickChangeIdx : %d\n", m_nQuickChangeIdx);
}

WeaponBase* CGameCharaMultiWeaponContext::GetQuickChangeWeapon()
{
	if ( SearchIndexSlot( static_cast<UINT8>(GetQuickChangeIdx())) != nullptr)
	{
		return SearchIndexSlot( static_cast<UINT8>(GetQuickChangeIdx()))->pWeapon;
	}

	return nullptr;
}

WeaponBase* CGameCharaMultiWeaponContext::GetMultiWeapon( UINT8 index)
{
	// 범위 체크 추가

	for (size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if (info->nSlotNumber == index)
			return info->pWeapon;
	}

	return nullptr;
}

WeaponBase* CGameCharaMultiWeaponContext::GetSwapMultiWeapon( bool bReset)
{
	// 무기교체 방법에 따라 다중무기가 달라진다

	// 1. 주무기 획득으로 인한 무기 교체일 경우 - 무조건  다중무기 리스트에서 마지막에 들고 있는 다중무기
	// HUD 선택으로 교체 후 가능 - HUDChange 리셋
	// 일반적인 무기 교체 후 가능 - 고려할 사항 없음
	// 빠른 무기 교체 후 가능 - QuickChange 리셋
	// 주무기 드랍 교체 후 가능할지도... - DropedChange 리셋
	if ( IsPickupChange())
	{
		UINT8 nLastMultiWeaponSlot = GetMultiWeaponCount();

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - PickupChange\n");

			ResetChange();

			SetMultiWeaponSlot( nLastMultiWeaponSlot);

			if ( m_pOwner->isLocal())
			{
				UIHudMultiWeapon* pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
				I3ASSERT(pHud != nullptr);

				pHud->UpdateMultiWeaponSlot();
				pHud->ShowMultiWeaponSlot(true);
			}
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - PickupChange\n");
		}

		return GetMultiWeapon( nLastMultiWeaponSlot);
	}

	// 2. 다중무기 HUD에서 선택으로 인한 무기 교체일 경우 - HUD가 가리키는 슬롯의 다중무기
	// 주무기 획득 교체 후 가능할지도... - PickupChange 리셋
	// 일반적인 무기 교체 후 조건적(주무기로 교체)으로 가능할지도 - 고려할 사항 없음
	// 주무기 드랍 교체 후 가능할지도... - DropedChange 리셋
	// 빠른 무기 교체 후 가능할지도... - QuickChange 리셋
	if ( IsHUDSelectedChange())
	{
		UIHudMultiWeapon* pHud = (UIHudMultiWeapon*)HUD::instance()->GetHud(HUD::MULTIWEAPON);
		I3ASSERT( pHud != nullptr);

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - HUDSelectedChange\n");

			ResetChange();

			SetMultiWeaponSlot( pHud->GetCurrentSlotIdx());
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - HUDSelectedChange\n");
		}

		return GetMultiWeapon( pHud->GetCurrentSlotIdx());
	}

	// 3. 주무기 드랍으로 인한 무기 교체일 경우 - 드랍 시 셋팅된 슬롯의 다중무기
	// 주무기 획득 교체 후 가능 - PickupChange 리셋
	// 일반적인 무기 교체 후 가능 - 고려할 사항 없음
	// 빠른 무기 교체 후 가능 - QuickChange 리셋
	// HUD 선택으로 교체 후 가능 - HUDChange 리셋
	if ( IsDropedChange())
	{
		UINT8 nNextMultiWeaponSlot = GetNextSlotIndex();

		// 다중무기가 없는데 다중무기로 교체 시도
		if ( nNextMultiWeaponSlot == 0)
		{
			if ( bReset)
			{
				ResetChange();
			}

			return getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
		}

		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - DropedChange\n");

			ResetChange();

			SetMultiWeaponSlot( nNextMultiWeaponSlot);
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - DropedChange\n");
		}

		return GetMultiWeapon( nNextMultiWeaponSlot);

	}

	// 4. 빠른 무기 교체로 인한 무기 교체일 경우(주무기-주무기 교체만) - 이전에 장착한 다중무기
	if ( IsQuickChange())
	{
		if ( bReset)
		{
			I3TRACE("OnSelect GetSwapMultiWeapon - QuickChange\n");

			ResetChange();

			UINT8 nQuickChangeIdx = static_cast<UINT8>(GetQuickChangeIdx());
			m_pOwner->getMultiWeaponContext()->SetQuickChangeIdx();

			SetMultiWeaponSlot( nQuickChangeIdx);

			return GetMultiWeapon( nQuickChangeIdx);
		}
		else
		{
			I3TRACE("PreOnSelect GetSwapMultiWeapon - QuickChange\n");
		}

		return GetMultiWeapon( static_cast<UINT8>( GetQuickChangeIdx()) );
	}

	// 5. 일반적인 무기교체로 인한 다중무기 교체
	// 주무기 획득 교체 후 불가능할 듯
	// 주무기 드랍 교체 후 불가능할 듯
	// 빠른 무기 교체 후 불가능할 듯
	// HUD 선택으로 교체 후 불가능할 듯
	if ( bReset)
	{
		I3TRACE("OnSelect GetSwapMultiWeapon - WeaponChange\n");
		ResetChange();
	}
	else
	{
		I3TRACE("PreOnSelect GetSwapMultiWeapon - WeaponChange\n");
	}

	return getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY);
}

void CGameCharaMultiWeaponContext::ResetChange()
{
	ResetPickupChange();
	ResetHUDSelectedChange();
	ResetDropedChange();
	ResetQuickChange();
	SetDropProhibit(false);
	SetPickupProhibit(false);
}

void CGameCharaMultiWeaponContext::HasDropedChange(UINT8 index /* = 0 */)
{
	m_bDropedChange = true;
	m_nNextSlotIndex = index;
}

// 현재 장착한 다중무기를 삭제하는 함수
void CGameCharaMultiWeaponContext::DropedMultiWeapon()
{
	I3TRACE("CGameCharaMultiWeaponContext::DropedMultiWeapon() - ");
	// 현재 장착 중인 다중무기가 없을 경우 리턴
	if ( m_pCurrentWeaponSlot == nullptr)
		return;

	UINT8 idx = m_pCurrentWeaponSlot->nSlotNumber;
	I3TRACE("삭제할 UI Idx : %d\n", idx);

	MULTIWEAPON_INFO* pDeleteInfo = nullptr;
	size_t nDeleteIdx = 0;

	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		I3TRACE("%d번째 슬롯 검사 : %d\n", info->nSlotNumber);

		// UI Index가 일치할 경우 - 삭제
		// 불일치하지만 UI Index가 크면 -1 해준다
		// 나머지는 Skip
		if ( info->nSlotNumber == idx)
		{
#if defined( I3_DEBUG)
			i3::rc_wstring wstrWeaponName;
			info->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
			I3TRACE("다중무기 %d슬롯 삭제 - %s\n", idx, wstrWeaponName);
#endif

			if ( m_pCurrentWeaponSlot == nullptr)
			{
				m_pOwner->getWeaponContext()->NoUseWeapon( info->pWeapon);
				I3TRACE("문제 발생! 꼭 문의 바람\n");
			}

			if ( GetQuickChangeIdx() == idx)
				EmptyQuickChangeIdx();

			pDeleteInfo = info;
			nDeleteIdx = i;

			m_pCurrentWeaponSlot = nullptr;

			I3TRACE("다중무기리스트를 비울 캐릭터 Index :%d\n", m_pOwner->getCharaNetIndex());
		}
		else if ( info->nSlotNumber > idx)
		{
			// 삭제하는 슬롯보다 슬롯번호가 클 경우 1씩 줄여줌
			I3TRACE("%d Slot -> ",info->nSlotNumber);
			info->nSlotNumber -= 1;
			I3TRACE("%d Slot\n", info->nSlotNumber);
		}
	}

	// 지울 것이 있으면 지운다
	if ( pDeleteInfo != nullptr)
	{
		I3MEM_SAFE_FREE( pDeleteInfo);
		i3::vu::erase_index( m_MultiWeaponList, nDeleteIdx);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// 여기서 데이터 셋팅하면 안됨! 하지만 몇번째 슬롯으로  셋팅 할지는 결정해도 될지도...
	// 1개일 경우 - Null 처리
	// 삭제한 슬롯이 마지막 번호의 슬롯이었을 경우 1번 슬롯 기준으로 데이터 셋팅
	// 그 외의 경우 삭제한 슬롯 번호를 기준으로 데이터를 셋팅
	if ( m_MultiWeaponList.size() < 1)
	{
		HasDropedChange();
		I3TRACE("다중무기리스트가 비었습니다.\n");
	}
	else if( idx  == GetMultiWeaponCount() + 1)
	{
		HasDropedChange(1);
	}
	else
	{
		HasDropedChange(idx);
	}
}

MULTIWEAPON_INFO* CGameCharaMultiWeaponContext::SearchIndexSlot(UINT8 uiIdx)
{
	I3TRACE("SearchIndexSlot\n");
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info->nSlotNumber == uiIdx)
			return info;
	}

	return nullptr;
}

#if defined(I3_DEBUG)
void CGameCharaMultiWeaponContext::OutputMulitWeaponList()
{
	I3TRACE("\n##################################\nm_MultiWeaponList Size : %d\n", m_MultiWeaponList.size());

	I3TRACE("[%d] 캐릭터 : ", m_pOwner->getCharaNetIndex());
	for ( size_t i = 0; i < m_MultiWeaponList.size(); i++)
	{
		MULTIWEAPON_INFO* info = m_MultiWeaponList.at(i);

		if ( info == nullptr)
			continue;

		i3::rc_wstring wstrWeaponName;
		info->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);

		I3TRACE("[%d 슬롯 - %s (%d)] ", (INT32)info->nSlotNumber, wstrWeaponName, info->nSyncNumber);
	}
	I3TRACE("\n");

	if ( m_pCurrentWeaponSlot != nullptr)
	{
		i3::rc_wstring wstrWeaponName;
		m_pCurrentWeaponSlot->pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
		I3TRACE("Current Weapon - %s\n", wstrWeaponName);
	}
	else
	{
		I3TRACE("현재 다중무기 슬롯이 비어있음!!!\n");
	}
	I3TRACE("##################################\n\n");
}
#endif