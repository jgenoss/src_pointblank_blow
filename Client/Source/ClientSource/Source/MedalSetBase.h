#if !defined( __MEDAL_SET_BASE_H__)
#define __MEDAL_SET_BASE_H__

#include <unordered_map>

struct MedalSetInfo;
struct MEDAL_INFO;

namespace MedalSetProc
{
	bool file_find_in_pack( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData);
}

class MedalSetBase
{
	friend bool MedalSetProc::file_find_in_pack(INT32 Level, char * pszPath, void * pFileInfo, void * pUserData);

public:
	MedalSetBase();
	virtual ~MedalSetBase();

public:
	virtual void	BindMedal(MQF_MEDAL_INFO* pData)			= 0;
	virtual void	BindMedal(MQF_DAILY_MEDAL_SET_INFO* pData)	= 0;
	virtual bool	LoadMedalMQF()								= 0;
	bool			LoadMedalMQFDir(const char * path);
	bool			LoadMedalMQFinPack(const char * path);

	
	virtual INT32	getMedalSetCount();
	virtual INT32	getMedalTypeByIndex(UINT16) = 0;
	// For Network
	virtual void	setMedalInfo(MEDAL& pMedal);
	virtual void	setMedalInfo(CUR_MEDAL_SET& pMedal);
	virtual void	setMedalInfo(COM_MEDAL_SET& pMedal);
	virtual void	setBattleInfo(const MEDAL_STATE & pMedal);
	virtual void	setMedalReward(UINT16 idx, UINT8 lv) = 0;

	void			resetMedalInfo(void);

	// For UI : UITabProfileMedal //
	MEDAL_INFO*		getMedal(UINT16 idx);
	UINT16			getMedalIdx(INT32 idx);
	INT32			getMedalImageIndex(UINT16 idx);
	INT32			getMedalImageIndex(INT32 idx);
	INT32			getMedalTotalCount()			{	return m_viewMedalList.size();	}
	INT32			getMedalProcessCnt(INT32 idx);
	UINT16			getMedalProcessCnt(UINT16 idx);
	UINT8			getCurrentLevel(INT32 idx);
	UINT8			getCurrentLevel(UINT16 idx);
	UINT8			getMaxLevel(INT32 idx);		//현재 매달이 갖는 최대 레벨을 리턴해줍니다.
	INT32			getLevelProcessCnt(MEDAL_INFO * pMedal, INT32 idx);
	INT32			getLevelProcessCnt(INT32 idx, UINT8 lv);
	float			getLevelRate(INT32 idx, UINT8 lv);

	i3::rc_wstring	getMedalName(INT32 idx) const;
	const char*		getMedalDesc(INT32 idx);
	const char*		getMedalDesc(UINT16 idx);
	
	bool			isActivated(INT32 idx);
	bool			isAcquired(INT32 idx);
	bool			isNew(INT32 idx);
	bool			isNew(UINT16 idx);
	void			setNew(INT32 idx, bool bEnable);
	bool			hasNew();

	

	const MQF_REWARD_INFO*	getMedalReward(INT32 idx, UINT8 lv);
	const MQF_REWARD_INFO*	getMedalReward(UINT16 idx, UINT8 lv);
	bool			isGetReward(INT32 idx, UINT8 lv);

protected:
	i3::vector< MedalSetInfo* >					m_MedalList;
	i3::vector< MEDAL_INFO* >					m_viewMedalList;
	i3::unordered_map< UINT16, MEDAL_INFO * >	m_MedalMap;
};

#endif