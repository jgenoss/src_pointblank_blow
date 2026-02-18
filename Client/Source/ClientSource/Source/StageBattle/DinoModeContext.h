#if !defined( __ESCAPE_MODE_CONTEXT_H)
#define __ESCAPE_MODE_CONTEXT_H

struct DinoModeContext : public i3::shared_ginst<DinoModeContext>
{
	//Escape Mode
	INT16 TouchDownCount;
	INT32 TouchDownSlotIdx;

	UINT16 RedTouchDownCount;
	UINT16 BlueTouchDownCount;
	
	REAL32 rDelayTime;
	TOUCHDOWN_BLOCK_STATE eEscapeBlock;

	//
	UINT8 CurrentDinoRound;
	DINO_ROOM_INFO DinoSlot;

	DinoModeContext()
	{
		RedTouchDownCount = 0;
		BlueTouchDownCount =0;
		eEscapeBlock = TOUCHDOWN_BLOCK_STATE_INIT;

		CurrentDinoRound = 0;
	}

	CHARA_RES_ID GetDinoResType( INT32 idx);
};

#endif