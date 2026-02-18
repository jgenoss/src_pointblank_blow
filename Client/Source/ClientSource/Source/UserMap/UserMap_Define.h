#if !defined( __USERMAP_DEFINE_H)
#define __USERMAP_DEFINE_H

#include "UserMap_Block.h"
#include "UserMap_Block_Geometry.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Stage
//
namespace USMAP
{
	namespace STAGE
	{
		typedef UINT32		STYLE;
		
		const STYLE		STYLE_INTERCEPTABLE		= 0x00000001;				// 난입 가능
		const STYLE		STYLE_ROUND				= 0x00000002;				// Round 제
		const STYLE		STYLE_RESPAWN_USER		= 0x00000004;				// 반복해서 Respawn 된.

		struct INFO
		{
			INT8				m_ID[4];
			UINT64				m_MD5[2];
			
			char				m_Name[128];						// Map 이름
			UINT32				m_Author;							// 제작자 ID
			UINT16				m_Version;							// Version Number (Ordinal)
			UINT32				m_PlayCount;						// Play 회수
			INT32				m_RegCount;							// Server 등록 회수
			INT8				m_LastEditDate[6];					// 최종 수정 일자.

			STYLE				m_Style;

			UINT8				m_FrameCX;
			UINT8				m_FrameCZ;							// Map의 크기.

			INT16				m_BlockCount;						// 배치된 Block의 개수
			INT32				m_BlockOffset;						// Block Data의 상대 Offset

			INT16				m_ConditionCount;					// Condition의 개수
			INT32				m_ConditionOffset;					// Condition Data의 상대 Offset

			INT8				m_Sky;

			INT8				m_bFogEnable;						// Fog 가/불
			UINT32				m_FogColor;							// Fog Color
			REAL32				m_FogNear;							// Fog Near
			REAL32				m_FogFar;							// Fog Far

			REAL32				m_Gravity;							// 중력
			INT8				m_MaxRedUserCount;					// RED Team 최대 유저 수
			INT8				m_MaxBlueUserCount;					// BLUE Team 최대 유저 수
			INT8				m_TimeLimit;						// 제한 시간 (분 단위)
			INT8				m_RoundLimit;						// 제한 Round 수
			REAL32				m_DamageFactor;						// 데미지의 적용 수준. (0~1)
			
			INT8				pad[44];							// 256 크기를 맞추기 위한 pad

			void				init(void);
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Object
	//
	namespace OBJECT
	{
		typedef UINT16			STYLE;
		
		const STYLE		STYLE_DYNAMIC			= 0x0001;
		const STYLE		STYLE_TARGET_RED		= 0x0002;
		const STYLE		STYLE_TARGET_BLUE		= 0x0004;
		const STYLE		STYLE_NETSYNC			= 0x0008;
		const STYLE		STYLE_BREAKABLE			= 0x0010;
		const STYLE		STYLE_RESPAWN			= 0x0020;
		const STYLE		STYLE_NO_LIGHT			= 0x0040;

		struct INFO
		{
			UINT16						m_ID;						// Object의 Type ID
			STYLE						m_Style;					// Object의 유형
			INT16						m_Pos[3];					// 위치 정보		(0-bits Fixed-Point format)
			INT8						m_Rot[3];					// 회전 정보		(8-bits Fixed-Point format)
			INT8						m_Scale[3];
			UINT8						m_TexID;					// Texture ID / Trigger ID / Respawn Team;
			INT8						m_Group;

			void	init(void);

			void	setScale( REAL32 sx, REAL32 sy, REAL32 sz);
			REAL32	getScale( INT32 idx)								{ return m_Scale[idx] * 0.5f; }
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Action
	//
	namespace ACTION
	{
		enum ACTION_TYPE
		{
			ACTION_NONE = 0,
			ACTION_FIN_BATTLE,						// 전투 종료 및 결과			(-)
			ACTION_FIN_ROUND,						// Round의 종료					(-)
			ACTION_BREAK_OBJECT,					// 특정 Object의 파괴			(Data[0] : Block ID)
			ACTION_RESPAWN,							// 지정 User의 Respawn			(Data[0] : USMAP_COND_TARGET)
			ACTION_KILL,							// 지정 User를 Kill				(Data[0] : USMAP_COND_TARGET)
			ACTION_ENABLE_TRIGGER,					// 특정 Trigger의 활성화		(Data[0] : Block(Trigger) ID)
			ACTION_DISABLE_TRIGGER,					// 특정 Trigger의 비활성화		(Data[0] : Block(Trigger) ID)
			ACTION_ADD_TIME,						// 제한 시간의 증가/감소		(Data[0] : 변화치)
			ACTION_ADD_REDKILL,						// RED KillCount의 증가/감소	(Data[0] : 변화치)
			ACTION_ADD_BLUEKILL,					// BLUE KillCount의 증가/감소	(Data[0] : 변화치)
			ACTION_ADD_ALLYKILL,					// 아군 KillCount의 증가/감소	(Data[0] : 변화치)
			ACTION_ADD_ENEMYKILL,					// 아군 KillCount의 증가/감소	(Data[0] : 변화치)
			ACTION_ADD_NPC,							// NPC 추가.
			ACTION_ALARM,							// Alarm 울리기					(Data[0] : Alarm ID)
			ACTION_SHOW_BOMB_INSTALL_UI,			// 폭탄 설치 UI 표시			(-)
		};
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Condition
	//
	namespace COND
	{
		typedef UINT16			STYLE;
	
		const STYLE		STYLE_START			= 0x0001;
		const STYLE		STYLE_ALWAYS		= 0x0002;
		const STYLE		STYLE_ROUNDEND		= 0x0004;

		enum PARAM
		{
			PARAM_TIME			= 0,			// PlayTime의 초단위. 만약 Round제라면 Round Play Time으로 간주.	(Value[0])
			PARAM_ALIVECOUNT,					// 살아있는 유저 수													(Target[0], Value[0])
			PARAM_DEADCOUNT,					// 죽은 유저 수														(Target[0], Value[0])
			PARAM_RESPAWNCOUNT,					// 지정 User의 Respawn 회수											(Target[0], Value[0])
			PARAM_ROUNDCOUNT,					// 진행한 Round 회수												(Value[0])
			PARAM_TARGETHP,						// Target Object의 HP												(Target[0], Value[0])
			PARAM_USERHP,						// 지정 User의 HP													(Target[0], Value[0])
			PARAM_TARGETCOUNT,					// Target Object 중, 파괴되지 않은 Object의 개수					(Target[0], Value[0])
			PARAM_USERTRIGGER,					// 지정 User가 진입한 Trigger ID									(Target[0], Value[0])
			PARAM_OBJECTTRIGGER,				// 지정 Object가 진입한 Trigger ID									(Target[0], Value[0])
			PARAM_KILLCOUNT,					// Kill Count														(Target[0], Value[0])
			PARAM_TOTALKILLCOUNT,				// 전체의 Kill Count												(Target[0], Value[0])
			PARAM_INSTALLED_BOMB_COUNT,			// 설치된 폭탄의 개수												(Value[0])
			PARAM_DEFUSED_BOMB_COUNT,			// 해제된 폭탄의 개수												(Value[0])
		};

		enum TARGET
		{
			TARGET_PLAYER = 1,					// Player에게 적용되는 조건
			TARGET_ALL_ALLY,					// 자신을 포함한 모든 아군에 적용되는 조건
			TARGET_ALL_ENEMY,					// 모든 적군에게 적용되는 조건
			TARGET_RED_TARGET,					// RED Team Target Object에 적용되는 조건.
			TARGET_BLUE_TARGET,					// BLUE Team Target Object에 적용되는 조건.
			TARGET_ALLY_TARGET,					// 아군 Target Object에 적용되는 조건
			TARGET_ENEMY_TARGET,				// 아군 Target Object에 적용되는 조건
			TARGET_DEAD_ALLY,					// 죽은 아군 중, 첫 번째 1명
			TARGET_ALL_HEAD_ALLY,				// 죽은 모든 아군
			TARGET_DEAD_ENEMY,					// 죽은 적군 중, 첫 번째 1명
			TARGET_ALL_DEAD_ENEMY,				// 죽은 모든 적군.
		};

		struct INFO
		{
			STYLE							m_Style;				// 유형
			INT8							m_AndLink;				// Single-Linked-List의 Next Index. (-1 == 연결 없음)
			PARAM							m_Param;
			TARGET							m_Target;
			INT32							m_Value[2];
			USMAP::ACTION::ACTION_TYPE		m_Action[4];
			TARGET							m_ActionTarget[4];
			INT32							m_ActionData[4];

			void				init(void);
		};
	};

	////////////////////////////////////////////////////////////////////////////////////
	//
	// Blocks
	namespace BLOCK
	{
		const INT32				ICON_CX		= 38;
		const INT32				ICON_CY		= 37;

		typedef UINT32			EDIT_STYLE;

		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_X		= 0x00000001;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_Y		= 0x00000002;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_MOVE_Z		= 0x00000004;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_X		= 0x00000008;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_Y		= 0x00000010;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_ROTATE_Z		= 0x00000020;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_X		= 0x00000040;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_Y		= 0x00000080;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_SCALE_Z		= 0x00000100;
		const EDIT_STYLE		EDIT_STYLE_DISABLE_TEX			= 0x00000200;

		typedef UINT16			ID;

		const ID				ID_NONE							= 0;
		const ID				ID_CUBE							= 1;
		const ID				ID_SPHERE						= 2;
		const ID				ID_SLOPE						= 3;
		const ID				ID_TRAP							= 4;
		const ID				ID_PYRAMID						= 5;
		const ID				ID_ROAD1						= 6;
		const ID				ID_HOLE1						= 7;
		const ID				ID_WALL_DOOR1					= 8;
		const ID				ID_WALL_HOLE1					= 9;
		const ID				ID_WALL_WINDOW1					= 10;
		const ID				ID_STEP1						= 11;
		const ID				ID_STEP2						= 12;
		const ID				ID_BARICATE						= 13;
		const ID				ID_CAR1							= 14;
		const ID				ID_DRUM1						= 15;
		const ID				ID_CONTAINER1					= 16;
		const ID				ID_BOX_WOOD						= 17;

		struct INFO
		{
			char				m_szName[64];
			char				m_szRes[ 128];
			ID					m_ID;
			i3GameResObject *	m_pRes;
			CUserMap_Block *	m_pBlock;
			UINT32				m_EditStyle;

			char				m_szTex[128];
			i3Texture *			m_pIconTex;
			VEC2D				m_UV;
		};

		INT32					getCount(void);
		INFO *					getInfo( INT32 idx);
		INFO *					FindInfo( UINT16 id);
	};

	//////////////////////////////////////////////////////////////////////////////////
	// Sky
	namespace SKY
	{
		typedef INT32			TYPE;

		typedef struct _tagInfo
		{
			char				m_szRes[128];
			char				m_szName[128];
			i3SceneGraphInfo *	m_pSg;
			COLORREAL			m_LightDiffuse;
			COLORREAL			m_LightSpecular;
			VEC3D				m_LightDir;
		} INFO;

		INT32					getCount(void);
		INFO *					getInfo( INT32 idx);
	};
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
