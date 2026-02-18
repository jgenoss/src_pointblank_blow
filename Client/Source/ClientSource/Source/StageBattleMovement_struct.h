#if !defined( __CSTAGE_BATTLE_MOVEMENT_STRUCT__ )
#define __CSTAGE_BATTLE_MOVEMENT_STRUCT__

#include "GameCharaDef.h"

typedef struct CStageBtattleMovement
{
	//StageBattle에 캐릭터에 이동관련 적용될 값들.

	REAL32  AccelerationTime;
	REAL32  Acceleration_Down_Speed_late;
	REAL32  Acceleration_Up_Speed_late;
	REAL32  m_rAddMovementSpeed[MST_COUNT][MDT_COUNT];
	
	REAL32  m_rGravity;
	REAL32  m_rChara_Down_Gravity_Speed_late;
	REAL32  m_rChara_Up_Gravity_Speed_late;
	REAL32	m_rChara_Fall_Gravity_Speed_late;

	REAL32	m_rTempGravity;

	void	init(i3RegKey* Key)
	{
		AccelerationTime	 = 1.0f;
		Acceleration_Down_Speed_late = 1.0f;
		Acceleration_Up_Speed_late = 1.0f;
		for (int x = 0; x < MST_COUNT; x++)
		{
			for (int y = 0; y < MDT_COUNT; y++)
			{
				m_rAddMovementSpeed[x][y] = 0.0f;
			}
		}

		m_rGravity								= 9.0f;
		m_rChara_Down_Gravity_Speed_late		= 1.005f;
		m_rChara_Up_Gravity_Speed_late			= 1.005f;
		m_rChara_Fall_Gravity_Speed_late		= 1.005f;
		m_rTempGravity							= 13.0f;

		i3RegKey * pChildKey = Config::FindKey("Movement", Key);
		if (pChildKey != nullptr)
		{
			FIND_REG_DATA(pChildKey, "AccelerationTime",				&AccelerationTime);
			FIND_REG_DATA(pChildKey, "Acceleration_Down_Speed_late",	&Acceleration_Down_Speed_late);
			FIND_REG_DATA(pChildKey, "Acceleration_Up_Speed_late",		&Acceleration_Up_Speed_late);

			FIND_REG_DATA(pChildKey, "Add_BackMoveSpeed",				&m_rAddMovementSpeed[MST_RUN][MDT_BACK]);
			FIND_REG_DATA(pChildKey, "Add_DiagonalMoveSpeed",			&m_rAddMovementSpeed[MST_RUN][MDT_DIAGONAL]);
			FIND_REG_DATA(pChildKey, "Add_FrontMoveSpeed",				&m_rAddMovementSpeed[MST_RUN][MDT_FRONT]);
			FIND_REG_DATA(pChildKey, "Add_SideMoveSpeed",				&m_rAddMovementSpeed[MST_RUN][MDT_SIDE]);

			FIND_REG_DATA(pChildKey, "Add_WalkBackMove",				&m_rAddMovementSpeed[MST_WALK][MDT_BACK]);
			FIND_REG_DATA(pChildKey, "Add_WalkDiagonalMove",			&m_rAddMovementSpeed[MST_WALK][MDT_DIAGONAL]);
			FIND_REG_DATA(pChildKey, "Add_WalkFrontMove",				&m_rAddMovementSpeed[MST_WALK][MDT_FRONT]);
			FIND_REG_DATA(pChildKey, "Add_WalkSideMove",				&m_rAddMovementSpeed[MST_WALK][MDT_SIDE]);

			FIND_REG_DATA(pChildKey, "Add_JumpRunBackMoveSpeed",		&m_rAddMovementSpeed[MST_JUMP_RUN][MDT_BACK]);
			FIND_REG_DATA(pChildKey, "Add_JumpRunDiagonalMoveSpeed",	&m_rAddMovementSpeed[MST_JUMP_RUN][MDT_DIAGONAL]);
			FIND_REG_DATA(pChildKey, "Add_JumpRunFrontMoveSpeed",		&m_rAddMovementSpeed[MST_JUMP_RUN][MDT_FRONT]);
			FIND_REG_DATA(pChildKey, "Add_JumpRunSideMoveSpeed",		&m_rAddMovementSpeed[MST_JUMP_RUN][MDT_SIDE]);

			FIND_REG_DATA(pChildKey, "Add_JumpWalkBackMove",			&m_rAddMovementSpeed[MST_JUMP_WALK][MDT_BACK]);
			FIND_REG_DATA(pChildKey, "Add_JumpWalkDiagonalMove",		&m_rAddMovementSpeed[MST_JUMP_WALK][MDT_DIAGONAL]);
			FIND_REG_DATA(pChildKey, "Add_JumpWalkFrontMove",			&m_rAddMovementSpeed[MST_JUMP_WALK][MDT_FRONT]);
			FIND_REG_DATA(pChildKey, "Add_JumpWalkSideMove",			&m_rAddMovementSpeed[MST_JUMP_WALK][MDT_SIDE]);

			FIND_REG_DATA(pChildKey, "Gravity",							&m_rGravity);
			FIND_REG_DATA(pChildKey, "Chara_Down_Gravity_Speed_late",	&m_rChara_Down_Gravity_Speed_late);
			FIND_REG_DATA(pChildKey, "Chara_Up_Gravity_Speed_late",		&m_rChara_Up_Gravity_Speed_late);
			FIND_REG_DATA(pChildKey, "Chara_Fall_Gravity_Speed_late",	&m_rChara_Fall_Gravity_Speed_late);

			FIND_REG_DATA(pChildKey, "GravityRestore",					&m_rTempGravity);
		}
	}

	REAL32 GetStageAddMoveSpeed( EMoveSpeedType eMST, EMoveDirectionType eMDT) const
	{
		return m_rAddMovementSpeed[eMST][eMDT];
	}

}CSBM;

#endif