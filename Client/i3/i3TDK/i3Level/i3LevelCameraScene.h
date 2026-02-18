#if !defined( __I3_Level_CAMERA_SCENE_H)
#define __I3_Level_CAMERA_SCENE_H

#include "i3LevelCamera.h"
#include "i3LevelControl.h"

//m_EffectType
#define	I3Level_SCENECAMERA_EFFECT_TREMBLE	0x00000001
#define I3Level_SCENECAMERA_EFFECT_CIRCLE		0x00000002

//m_InterpolateType
#define	I3Level_SCENECAMERA_INTER_LINEAR		0x10000002

//m_AtState
#define	I3Level_SCENECAMERA_AT_HERO			0x20000001
#define	I3Level_SCENECAMERA_AT_POS			0x20000002


#define I3Level_SCENECAMERA_PLAY				1
#define I3Level_SCENECAMERA_INTER				2

///File Header
struct I3_Level_CAMERA_HEADER
{
	INT32		m_Ver = 0; 
	INT32		m_TotalActionCount = 0; 
};

//chunk
struct I3_Level_CAMERA_ACTION
{
	void	*	m_pPrev;			//파일 저장에 필요없지만. 그냥 저장한다.  
	void	*	m_pNext;			//파일 저장에 필요없지만. 그냥 저장한다. 	

    REAL32		m_TotalTime;		
	INT32		m_AtState;			//At Type	
	INT32		m_EffectState;		//Effect
	VEC3D		m_At;				//만약 At에서 히어로를 보면 이건 필요없다. 
	VEC3D		m_AttachPos;		//이건 상대적인 값으로 들어갑니다. 

	REAL32		m_CircleSpeed;		
	REAL32		m_TrembleRand;			
 
	//다음 카메라 액션으로 갈때 
	INT32		m_InterpolateState;	//이거 지금은 필요없지만 나중을 위해서 사용한다. 
	REAL32		m_InterTime;		//I3Level_SCENECAMERA_INTER_JUMP  이면 0으로 한다. 
};

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3
{
	namespace pack
	{
		///File Header
		struct ALIGN4 Level_CAMERA_HEADER
		{
			INT32		m_Ver = 1;
			INT32		m_TotalActionCount = 0;
		};

		//chunk
		struct ALIGN4 Level_CAMERA_ACTION
		{
			REAL32		m_TotalTime = 0.0f;
			INT32		m_AtState = 0;			//At Type	
			INT32		m_EffectState = 0;		//Effect

			REAL32				m_AtX = 0.0f;
			REAL32				m_AtY = 0.0f;
			REAL32				m_AtZ = 0.0f;

			REAL32				m_AttachPosX = 0.0f;
			REAL32				m_AttachPosY = 0.0f;
			REAL32				m_AttachPosZ = 0.0f;

			/*
			i3::pack::VEC3D		m_At;				//만약 At에서 히어로를 보면 이건 필요없다.
			i3::pack::VEC3D		m_AttachPos;		//이건 상대적인 값으로 들어갑니다.
			*/

			REAL32		m_CircleSpeed = 0.0f;
			REAL32		m_TrembleRand = 0.0f;

			//다음 카메라 액션으로 갈때 
			INT32		m_InterpolateState = 0;	//이거 지금은 필요없지만 나중을 위해서 사용한다. 
			REAL32		m_InterTime = 0.0f;		//I3Level_SCENECAMERA_INTER_JUMP  이면 0으로 한다. 
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif
 

class I3_EXPORT_TDK i3LevelSceneCamera : public i3LevelCamera
{
	I3_EXPORT_CLASS_DEFINE( i3LevelSceneCamera, i3LevelCamera );

protected:	
	i3LevelControl *		m_pCameraIcon = nullptr;
	i3LevelControl *		m_pHeroIcon;					// [initialize at constructor]
	
	char						m_szPath[256] = { 0 };
	I3_Level_CAMERA_ACTION	*	m_pStartAction = nullptr;
	I3_Level_CAMERA_ACTION	*	m_pEndAction = nullptr;

	//For Play 
	REAL32						m_LocalTime = 0.0f; 
	I3_Level_CAMERA_ACTION	*	m_NowAction = nullptr;
	MATRIX						m_BeMat; 
	INT32						m_PlayState = 0;
	VEC3D						m_InterStaAt; 
	VEC3D						m_InterStaPos; 
	VEC3D						m_InterEndAt; 
	VEC3D						m_InterEndPos; 

	//파일에 저장되는 녀석 입니다. 
	I3_Level_CAMERA_HEADER	m_HeaderInfo; 
	i3::vector<I3_Level_CAMERA_ACTION*>	m_ActionList; 
	
public:
	void	SetIcon(void); 
	i3LevelControl *	GetIcon(void)					{ return m_pHeroIcon; }

	char * GetPath(void)							{return m_szPath; }
	i3LevelSceneCamera( void );	
	virtual ~i3LevelSceneCamera( void );

	//Edit Scene Camera 
	void OnCreate( REAL32 TotalTime );	
	void DeleteAllAction(void);					
	I3_Level_CAMERA_ACTION * InsertAction(I3_Level_CAMERA_ACTION * pPrevAction = nullptr, I3_Level_CAMERA_ACTION * pNextAction = nullptr); 
	void ChangeAction(I3_Level_CAMERA_ACTION * ,REAL32 Time, INT32 AtState, INT32 EffectState, VEC3D vAt, VEC3D vStartPostion); 
	void DeleteAction(I3_Level_CAMERA_ACTION * pAction); 
	
	//Load And Save 
	BOOL OnLoad( const char * szName ); 
	BOOL OnSave(void); 
	BOOL OnSaveAs( const char * szName ); 
	BOOL OnLoad( i3FileStream * pStream); 
	BOOL OnSave( i3FileStream * pStream); 

	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;

	//Get Infomation 
	I3_Level_CAMERA_ACTION * GetAction(INT32 idx); 
	I3_Level_CAMERA_ACTION * GetStartAction(void) { return m_pStartAction; }
	I3_Level_CAMERA_ACTION * GetEndAction(void)	{ return m_pStartAction; }

	REAL32	GetTotalTime(void);
	INT32	GetTotalActionCount(void)	{ return (INT32)m_ActionList.size(); }

	void	Play(void); 
	void	Pause(void); 
	void	Stop(void); 		

	virtual void	OnDrive( REAL32 time) override;

	//Get State String 
	static void GetActionString(INT32 idx, char * pName, UINT16 nBufSize); 
	static INT32 GetActionIdx(const char * pName); 

};

#endif