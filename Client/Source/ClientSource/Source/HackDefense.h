#if !defined(__HACK_DEFENSE_H)
#define __HACK_DEFENSE_H


namespace hack_defense {}
namespace hd = hack_defense;

namespace hack_defense
{
	class check_observer
	{
	public:
		static check_observer*	i() { return tinst<check_observer>(); }
		void					ProcessCreateStage();
		void					init_roominfo();
		void					check();
	private:
		ROOM_INFO_ADD	m_roomInfo;		// 해킹방어 기능으로 추가된 변수
	};	

	class check_speedhack				// //speed hack// 현재는 구현상 주석처리되어있지만, StageBattle에서 구현을 옮김.
	{
	public:
		static check_speedhack*	i() { return tinst<check_speedhack>(); }
		void					ProcessCreateStage();
		void					init_pos();
		void					checkSpeedPos();
		void					checkMaxSpeed(REAL32 fSpeed);
		bool					getFirstRespawn()const{return m_bFirstSpeedCheck;}
	private:
		VEC3D					m_vPos;
		bool					m_bFirstSpeedCheck;
	};

	
	class check_UnknwonPos				// //UnknwonPos// 캐릭터 위치를 기준으로 바닥에 히트컬리젼이 없을 경우 감지
	{
	public:
		static check_UnknwonPos*	i() { return tinst<check_UnknwonPos>(); }
		void					check(i3CollideeLine * pLine);
	private:

	};

	class check_PhysixDll				// //PhysixDll// 피직스 관련 DLL 파일 변조 감지.
	{
	public:
		static check_PhysixDll*	i() { return tinst<check_PhysixDll>(); }
		void					check();
	private:

	};
	
	// 아래 맵데이터 체크는 루틴은 존재하는데, 데브버전 기준으로 실제로 구동되지 않고 있음...(스테이지배틀에서 옮김)
	class check_mapdata
	{
	public:
		static check_mapdata* i() { return tinst<check_mapdata>(); }
		void					ProcessCreateStage();
		void					check(INT32	nStageIndex);
		bool					is_hacked() const { return !m_IsStageDataSafe; }
	private:
		bool					m_IsStageDataSafe;
	};

	class check_elapsedsynctimer
	{
	public:
		static check_elapsedsynctimer* i() { return tinst<check_elapsedsynctimer>(); }
		void					ProcessCreateStage();
		void					Update(REAL32 rDeltaSeconds);
		void					ResetTimer() { m_fElapsedTimerCheck = 0.f; }
		void					ResetGravityTimer() { m_fElapsedGravityTimerCheck = 0.f; }

//		void					InitMagnetStartTime();
//		void					InitMagnet();

	private:
	
		REAL32					m_fElapsedTimerCheck;
		REAL32					m_fElapsedSpeedTimerCheck;
		REAL32					m_fElapsedGravityTimerCheck;
	};



	
}



#endif