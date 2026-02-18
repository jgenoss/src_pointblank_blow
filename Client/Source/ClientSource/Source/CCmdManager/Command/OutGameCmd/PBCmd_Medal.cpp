#include "pch.h"
#include "PBCmd_Medal.h"
#include "Cheatkey_LimitPred.h"
#include "../../Cheatkey.h"

#include "../TBoundChk.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Cheatkey
{
	//--------------------------------------------------
	// 메달 카운트 변경.
	// ex) /MEDALC 메달타입 메달인덱스 메달진행횟수
	//--------------------------------------------------
	bool ChangeMedalCount::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() != 3) return false;

		m_type = static_cast<UINT8>(::atoi(params[0].c_str()));
		m_idx = static_cast<UINT16>(::atoi(params[1].c_str()));
		m_count = static_cast<UINT16>(::atoi(params[2].c_str()));

		if (!TBoundChk<UINT8>(m_type, 0, MEDAL_COMMAND_TYPE_MAX-1)) return false;
		if (!TBoundChk<UINT16>(m_idx, 0, std::numeric_limits<UINT16>::max())) return false;
		if (!TBoundChk<UINT16>(m_count, 0, std::numeric_limits<UINT16>::max())) return false;
		return true;
	}

	bool ChangeMedalCount::_Execute()
	{
		MEDAL_COMMAND medalInfo;	
		i3mem::FillZero(&medalInfo, sizeof(MEDAL_COMMAND));

		medalInfo.m_ui8CommandType = MEDAL_COMMAND_TYPE_COUNT;
		medalInfo.m_ui8MedalType = m_type;
		medalInfo.m_ui16Idx = m_idx;
		medalInfo.m_ui16Count = m_count;

		GameEventSender::i()->SetEvent(EVENT_MEDAL_COMMAND, &medalInfo);

		return true;
	}

	ICommand* ChangeMedalCount::Creator()
	{
		return new ChangeMedalCount(new LPred_PermittedAccountOnly(new LPred_OutgameOnly));
	}

	//--------------------------------------------------
	// 메달 세트 변경.
	// ex) /MEDALS 메달셋트타입 메달셋트인덱스 1(의미없는값 아무거나)
	//--------------------------------------------------
	bool ChangeMedalSet::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() < 2) return false;

		m_type = static_cast<UINT8>(::atoi(params[0].c_str()));
		m_idx = static_cast<UINT16>(::atoi(params[1].c_str()));

		if (!TBoundChk<UINT8>(m_type, 0, MEDAL_COMMAND_TYPE_MAX-1)) return false;
		if (!TBoundChk<UINT16>(m_idx, 0, std::numeric_limits<UINT16>::max())) return false;
		return true;
	}

	bool ChangeMedalSet::_Execute()
	{
		MEDAL_COMMAND medalInfo;	
		i3mem::FillZero(&medalInfo, sizeof(MEDAL_COMMAND));

		medalInfo.m_ui8CommandType = MEDAL_COMMAND_TYPE_CHANGE_MEDALSET;
		medalInfo.m_ui8MedalType = m_type;
		medalInfo.m_ui16Idx = m_idx;
		medalInfo.m_ui16Count = 1;

		GameEventSender::i()->SetEvent(EVENT_MEDAL_COMMAND, &medalInfo);

		return true;
	}

	ICommand* ChangeMedalSet::Creator()
	{
		return new ChangeMedalSet(new LPred_PermittedAccountOnly(new LPred_OutgameOnly));
	}
}