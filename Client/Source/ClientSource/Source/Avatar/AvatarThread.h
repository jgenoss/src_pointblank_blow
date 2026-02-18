#if !defined( __AVATAR_THREAD_H__)
#define __AVATAR_THREAD_H__

#include "Avatar.h"
#include "GameThread.h"

class cThreadAvatar;

enum eAVATAR_STATE
{
	E_AVATAR_STATE_NA = 0,			// 아무런 상태도 아니다.
	E_AVATAR_STATE_COMMAND,			// 로드 명령을 받은 상태
	E_AVATAR_STATE_LOADING,			// 로드 중이다.
	E_AVATAR_STATE_ERROR,			// 로드 실패
	E_AVATAR_STATE_LOADED,			// 로드 완료
	E_AVATAR_STATE_FLUSH,			// 로딩중이지만 쓰지 않는다 해제시켜주자
};

enum AVATAR_THREAD_STATE
{
	ATS_WAIT,
	ATS_RUN,
	ATS_LOST,	// Lost Device 상태 로딩을 멈춘다.
	ATS_CLEAR,	// 외부에서 종료하는 경우
	ATS_FLUSH,	// 중간에 삭제예정된 경우 (ReturnInstance를 해야한다)
};

struct AVATAR_LOAD_INFO
{
	INT32						_netIndex = -1;
	GLOBAL_CHARA_INFO 			_CharaInfo;		// 로딩용
	GLOBAL_CHARA_INFO *			_pCharaInfo = nullptr;	// 외부 CharaInfo
	Avatar*						_pLoadedAvatar = nullptr;
	cThreadAvatar *				_pThread = nullptr;
	eAVATAR_STATE				_state = E_AVATAR_STATE_NA;
	i3Stage *					_pStage = nullptr;		//Avatar의 InstanceManager가 같은지 여부를 판단하기 위함. InstanceManager가 해제되었다면 아바타를 삭제시 바로 해제시켜야한다.

	void	Reset()
	{
		_state = E_AVATAR_STATE_NA;
		_netIndex = -1;
		_CharaInfo.Reset();
		_pCharaInfo = nullptr;
		_pThread = nullptr;
		_pLoadedAvatar = nullptr;
		_pStage = nullptr;
	}

	void	Done( void);
	void	Flush( void);
};

class cThreadAvatar : public CGameThread
{
	I3_CLASS_DEFINE( cThreadAvatar, CGameThread);

public:
	cThreadAvatar();
	virtual ~cThreadAvatar();

	virtual void	Run( void *pData) override;

	bool			SetAvatarInfo( AVATAR_LOAD_INFO * pInfo);

	Avatar*			GetAvatar()									{ return m_pAvatar; }

	void			SetAvatarState(AVATAR_THREAD_STATE eState)	{ m_eState = eState; }
	AVATAR_THREAD_STATE	GetAvatarState()						{ return m_eState; }

	REAL32			GetSleepTime( void)							{ return m_SleepTime; }

private:
	Avatar*			m_pAvatar = nullptr;
	AVATAR_THREAD_STATE	m_eState = ATS_WAIT;

	i3Timer *		m_pTimer;			// [initialize at constructor]
	REAL32			m_SleepTime = 0.0f;	// 잠자고 있는 시간
};

#endif
