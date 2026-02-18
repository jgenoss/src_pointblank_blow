#if !defined( __MEDALDEF_CLIENT_H__)
#define __MEDALDEF_CLIENT_H__

#include "../../../Common/CommonSource/Medal/MedalDef.h"

#define	MAX_MEDAL_INFO_PROTOCOL	3
														// 우선 임시로 이렇게 처리..
struct MEDAL_INFO
{
	bool					m_bActiveMedal = false;		// 서버에서 주는 인덱스 메달만 활성화
	UINT16					m_ui16ImageIdx = 0;			// 메달 이미지 인덱스
	UINT16					m_ui16Idx = 0;				// 메달 고유 인덱스.
	UINT16					m_ui16BattleCount = 0;		// 메달 임무 진행 횟수
	bool					m_bAcq = false;				// 메달 획득 여부
	bool					m_bNew = false;				// 새로 획득한 메달 여부(UI용)
	bool					m_bNew2 = false;			// 새로 획득한 메달 여부(결과창 표시용) - 결과창 나오면 갱신
	UINT8					m_ui8CurrentLevel = 0;		// 현재 레벨

	// 레벨별 달성횟수.
	UINT16					m_ui16Count[MEDAL_LEVEL_MAX + 1] = { 0 };
	// 레벨별 보상정보
	MQF_REWARD_INFO			m_eRewardInfo[MEDAL_LEVEL_MAX + 1] = { 0 };
	// 레벨별 보상획득여부
	bool					m_bRewardAcq[MEDAL_LEVEL_MAX] = { false };

	char					m_szNameKey[MAX_STRING_MEDAL_NAME_KEY_COUNT] = { 0 };
	char					m_szDescription[MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT] = { 0 };



	MEDAL_INFO(MQF_MEDAL_INFO * m)
	{
		CopyFrom(m);
	}
	void	CopyFrom(MQF_MEDAL_INFO * m)
	{
		m_bActiveMedal =		false;
		m_ui16Idx =				m->m_ui16Idx;
		m_ui16ImageIdx =		m->m_ui16ImageIdx;
		m_ui16BattleCount =		0;
		m_ui8CurrentLevel =		0;
		m_bNew2 = false;
		setAcquired(false);
		setNewMark(false);
		
		m_ui16Count[0] = 0;
		for(INT32 i=0; i < MEDAL_LEVEL_MAX; ++i)
		{
			m_bRewardAcq[i]	 =	false;
		}
		for(INT32 i=0; i < MEDAL_LEVEL_MAX; ++i)
		{
			m_ui16Count[i+1] =	m->m_ui16Count[i];
			m_eRewardInfo[i] =	m->m_eRewardInfo[i];
		}

		i3::safe_string_copy(m_szNameKey,		m->m_szNameKey,		MAX_STRING_MEDAL_NAME_KEY_COUNT			);
		i3::safe_string_copy(m_szDescription, m->m_szDescription, MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT	);
	}

	void	setBattleCount(UINT16 num, bool inBattle);
	void	setTotalMedalReward(UINT8 reward);
	void	setMedalReward(UINT8 lv, bool bAcq);
	void	setNewMark(bool bNew)		{ m_bNew = bNew;	}
	void	setNewMedal(bool bNew)		{ m_bNew2 = bNew;	}
	void	setAcquired(bool bAcq)		{ m_bAcq = bAcq;	}
	void	setActive(bool bActive)		{ m_bActiveMedal = bActive;	}
	bool	isActive()					{ return m_bActiveMedal;	}
};

struct MedalSetInfo
{
	MEDAL_SET_TYPE	setType;
	UINT16			setIndex;
	INT32			medalSetCount;
	bool			bAquired;
	i3::vector< MEDAL_INFO* >		medalSet;
	MedalSetInfo(MEDAL_SET_TYPE _setType, UINT16 _setIndex, INT32 _medalSetCount, bool _bAquired)
		: setType(_setType), setIndex(_setIndex),  medalSetCount(_medalSetCount), bAquired(_bAquired)
	{}
};

struct MedalCompleteInfo
{
	MEDAL_SET_TYPE	_type = MEDAL_SET_TYPE_NORMAL;	// 메달 타입
	UINT16			_idx = 0;	// 메달 고유 인덱스
	MEDAL_LEVEL		_level = MEDAL_LEVEL_1;	// 메달 레벨

	MedalCompleteInfo() {}

	void Reset(void)
	{
		_type = MEDAL_SET_TYPE_NORMAL;
		_idx = 0;
		_level = MEDAL_LEVEL_1;
	}

	void SetMedalCompleteInfo(MEDAL_SET_TYPE type, UINT16 idx, MEDAL_LEVEL level)
	{
		_type = type;
		_idx = idx;
		_level = level;
	}

	friend bool operator == (const MedalCompleteInfo & lhs, const MedalCompleteInfo & rhs)
	{
		return ( (lhs._type == rhs._type) && (lhs._idx == rhs._idx) && (lhs._level == rhs._level) );
	}
};

struct MedalRewardResult
{
	MedalCompleteInfo	_info;
	T_ItemID			_itemid = 0;
	bool				_success = false;

	MedalRewardResult() {}

	MedalRewardResult(const MedalCompleteInfo & info)
	{
		_info = info;
	}

	friend bool operator == (const MedalRewardResult & lhs, const MedalCompleteInfo & rhs)
	{
		return lhs._info == rhs;
	}
};

namespace medal_util
{
	typedef bool (*pf_direcory)(const i3::string& path);
	typedef void (*pf_file)(const i3::string& file_path, const i3::string& file_name);

	void browse_directory(const char* path, pf_direcory pf_d, pf_file pf_f);
}

#endif