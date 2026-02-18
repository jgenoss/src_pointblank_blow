#if !defined(__UTIL_BATTLE_H)
#define __UTIL_BATTLE_H

namespace util_battle {}
namespace ub = util_battle;

namespace util_battle
{
	bool is_live_character(INT32 slot_idx);
	

	bool is_current_toggle_camera_fps();

	void toggle_3p_camera();
	void toggle_observer_fps_camera();				// 스테이지배틀에서 옮겼지만, 현재 쓰이고 있지 않은 카메라 설정함수...

	void toggle_normal_observe_mode(void);
	void toggle_GM_observe_mode(void);				// 스테이지배틀에서 옮겼지만, 현재 쓰이는 곳이 없는 함수입니다. (보존)
	void toggle_flying_camera_mode(void);

	void set_topview_camera_pos(const VEC3D& p);
	void toggle_topview_camera(void);

// ObserverMode
	void	SetToOnObjectCamera(i3Body* pBody, INT32 idx, i3Object* pObj, WeaponBase * pWeapon);
	void	SetToOldCameraIfOnObjectCamera();
	void	SetToObserveCamera();


	//ai
	void SetAiCharaData(INT32 idx, AI_BIND_DATA* pbind_data, INT8 nteam);

	void AddAiChara(INT32 slotIdx);
	void AddAiCharacter(void);						// 현재 이 함수는 폐기 예정입니다..데디케이션서버가 직접 생성해서 클라이언트는 ai생성을 수신만 받습니다..
	void RespawnAiCharacter( INT32 slotIdx);
	void ChangeAiCharacter( void);
	void StartAiForClient( INT32 slotIdx);			// 임시코드로만 쓰이는듯 하고 현재 .. EVENT_START_BATTLE처리에서 주석되어 있음..

	void AddDinoChara();

	void	ApplySkillStatus(INT32 idx, bool);			// 스킬 적용

	// 스킬 시스템 적용
	void	SetSkillsystemValue(INT32 CharaSlotIdx, INT32 SkillSlotIdx);		// 이것은 CommandSet에서 직접 호출
	void	ApplySkillsystem(INT32 idx);

	// 왼손, 오른손 업데이트
	void	ApplyLRHandFromEnvSet();

	void	CheckAIData( AI_BIND_DATA * pAiData);
	
}

AI_BIND_DATA * _SetAiInfo( INT32 slotIdx, INT32 difficultyLevel);


#endif