#ifndef __I3GUI_DEFINES_H__
#define __I3GUI_DEFINES_H__

#if (defined( I3_WINDOWS) || defined(I3_WINCE)) && defined( I3_DLL)
	#if defined( I3_GUI_BUILD)
		#define I3_EXPORT_GUI			__declspec( dllexport)
		#define I3_EXTERN_GUI			extern
	#else
		#define I3_EXPORT_GUI			__declspec( dllimport)
		#define I3_EXTERN_GUI			__declspec( dllimport)
	#endif
#else
	#define I3_EXPORT_GUI
	#define I3_EXTERN_GUI				extern
#endif

#define I3GUI_RESOLUTION_WIDTH	1024.0f
#define I3GUI_RESOLUTION_HEIGHT	768.0f

#define I3_GUI_SHAPE_COUNT		100

#define USE_VERTEX_BANK			1

// Control에 사용되는 정렬규칙입니다.
#define I3_GUI_ALIGN_LEFT	0
#define I3_GUI_ALIGN_CENTER	1
#define I3_GUI_ALIGN_RIGHT	2
#define I3_GUI_ALIGN_TOP	0
#define I3_GUI_ALIGN_MIDDLE	1
#define I3_GUI_ALIGN_BOTTOM	2

#define	I3GUI_DEFAULT_CHARA_SET	"맑은 고딕"

// Control의 모양을 나타냅니다.
// 최소한으로 Control을 사용할때는 I3GUI_CTRL_SHAPE_CENTER만 사용합니다.
enum I3GUI_CONTROL_SHAPE
{	
	I3GUI_CTRL_SHAPE_CENTER = 0,
	I3GUI_CTRL_SHAPE_TOP,
	I3GUI_CTRL_SHAPE_LEFT,
	I3GUI_CTRL_SHAPE_RIGHT,
	I3GUI_CTRL_SHAPE_BOTTOM,
	I3GUI_CTRL_SHAPE_TOP_LEFT,
	I3GUI_CTRL_SHAPE_TOP_RIGHT,
	I3GUI_CTRL_SHAPE_BOTTOM_LEFT,
	I3GUI_CTRL_SHAPE_BOTTOM_RIGHT,

	MAX_I3GUI_CTRL_SHAPE,
};

// Control의 State(상태)를 나타냅니다.
enum GUI_CONTROL_STATE
{
	I3GUI_CONTROL_STATE_NORMAL = 0,
	I3GUI_CONTROL_STATE_CLICKED,			// 마우스 좌측버튼 클릭!
	I3GUI_CONTROL_STATE_FOCUS,
	I3GUI_CONTROL_STATE_ONMOUSE,
	I3GUI_CONTROL_STATE_START,
	I3GUI_CONTROL_STATE_TIMEOUT,
	I3GUI_CONTROL_STATE_DRAG,
	I3GUI_CONTROL_STATE_PROGRESS,
	I3GUI_CONTROL_STATE_DISABLED,

	I3GUI_CONTROL_STATE_ONENABLE,			// 
	I3GUI_CONTROL_STATE_ONDISABLE,			// 
	I3GUI_CONTROL_STATE_R_CLICKED,			// 
	I3GUI_CONTROL_STATE_RESERVED2,			// DOUBLECLICK
	I3GUI_CONTROL_STATE_RESERVED3,
	I3GUI_CONTROL_STATE_RESERVED4,
	I3GUI_CONTROL_STATE_RESERVED5,
	I3GUI_CONTROL_STATE_RESERVED6,
	I3GUI_CONTROL_STATE_RESERVED7,
	I3GUI_CONTROL_STATE_RESERVED8,
	I3GUI_CONTROL_STATE_RESERVED9,

	MAX_I3GUI_CONTROL_STATE,
};

/*
enum GUI_CONTROL_STATE
{
	I3GUI_CONTROL_STATE_NORMAL = 0,
	I3GUI_CONTROL_STATE_CLICKED,			// 마우스 좌측버튼 클릭!
	I3GUI_CONTROL_STATE_FOCUS,
	I3GUI_CONTROL_STATE_ONMOUSE,
	I3GUI_CONTROL_STATE_START,
	I3GUI_CONTROL_STATE_TIMEOUT,
	I3GUI_CONTROL_STATE_DRAG,
	I3GUI_CONTROL_STATE_PROGRESS,
	I3GUI_CONTROL_STATE_DISABLED,

	MAX_I3GUI_CONTROL_STATE,
};
*/

// 입력모드
enum I3GUI_INPUT_MODE
{
	I3GUI_INPUT_MODE_STROKE= 0,
	I3GUI_INPUT_MODE_PRESS ,
	I3GUI_INPUT_MODE_REPEAT,

	I3GUI_INPUT_MODE_DOUNT,
};


// mouse button state
#define I3GUI_MOUSE_STATE_LBUTTON_PRESS		0x00000001	// I3I_MOUSE_LBUTTON와 bit 맞춤
#define I3GUI_MOUSE_STATE_RBUTTON_PRESS		0x00000002	// I3I_MOUSE_RBUTTON와 bit 맞춤
#define I3GUI_MOUSE_STATE_MBUTTON_PRESS		0x00000004	// I3I_MOUSE_MBUTTON와 bit 맞춤

#define I3GUI_MOUSE_STATE_LBUTTON_DOWN		0x00000010
#define I3GUI_MOUSE_STATE_RBUTTON_DOWN		0x00000020
#define I3GUI_MOUSE_STATE_MBUTTON_DOWN		0x00000040
#define I3GUI_MOUSE_STATE_LBUTTON_UP		0x00000080
#define I3GUI_MOUSE_STATE_RBUTTON_UP		0x00000100
#define I3GUI_MOUSE_STATE_MBUTTON_UP		0x00000200
#define I3GUI_MOUSE_STATE_LBUTTON_DCLICK	0x00000400
#define I3GUI_MOUSE_STATE_RBUTTON_DCLICK	0x00000800
#define I3GUI_MOUSE_STATE_MBUTTON_DCLICK	0x00001000
#define I3GUI_MOUSE_STATE_CTRL				0x00002000
#define I3GUI_MOUSE_STATE_SHIFT				0x00004000
#define I3GUI_MOUSE_STATE_ALT				0x00008000
#define I3GUI_MOUSE_STATE_LBUTTON_REPEAT	0x00010000
#define I3GUI_MOUSE_STATE_RBUTTON_REPEAT	0x00020000
#define I3GUI_MOUSE_STATE_MBUTTON_REPEAT	0x00040000
#define I3GUI_MOUSE_STATE_WHEEL_UP			0x10000000
#define I3GUI_MOUSE_STATE_WHEEL_DOWN		0x20000000

#define I3GUI_MOUSE_STATE_MASK_PRESS		(I3GUI_MOUSE_STATE_LBUTTON_PRESS|I3GUI_MOUSE_STATE_RBUTTON_PRESS|I3GUI_MOUSE_STATE_MBUTTON_PRESS)
#define I3GUI_MOUSE_STATE_MASK_MODIFIER		(I3GUI_MOUSE_STATE_CTRL|I3GUI_MOUSE_STATE_SHIFT|I3GUI_MOUSE_STATE_ALT))

#define IS_MOUSE_STATE(state,msid)			((state) & (msid))
#define IS_MOUSE_STATE_NULL(state)			( ((state) | I3GUI_MOUSE_STATE_MASK_MODIFIER) == I3GUI_MOUSE_STATE_MASK_MODIFIER )
/////////////////////////////////////////////////////////////////////////////////////////
// Event에 사용되는 struct들입니다.(시작)
/////////////////////////////////////////////////////////////////////////////////////////

// GUI의 Event의 종류 Flag입니다. (하나의 Window나 Control에 Event Flag가 중복될수 있습니다)
#define		I3GUI_EVENT_MOVE					0x00000001
#define		I3GUI_EVENT_ROTATE					0x00000002
#define		I3GUI_EVENT_SCALE					0x00000004
#define		I3GUI_EVENT_SOUND					0x00000008
#define		I3GUI_EVENT_COUNT					4
// GUI의 Event에 사용되는 움직임의 종류입니다. (단 하나의 움직임 종류만 갖습니다)
#define		I3GUI_EVENT_FACTOR_TYPE_NORMAL		0
#define		I3GUI_EVENT_FACTOR_TYPE_ACCEL		1

// GUI 회전방향
#define		I3GUI_SPIN_CW						0
#define		I3GUI_SPIN_CCW						1

// Scale방향
#define		I3GUI_SCALE_DIR_NONE				0
#define		I3GUI_SCALE_DIR_RIGHT				1
#define		I3GUI_SCALE_DIR_LEFT				2
#define		I3GUI_SCALE_DIR_UP					3
#define		I3GUI_SCALE_DIR_DOWN				4

typedef struct
{
	INT32				m_nFactorType;
	RT_REAL32			m_fCountTime;
	RT_REAL32			m_fDestPosX;
	RT_REAL32			m_fDestPosY;
	RT_REAL32			m_fSpeedX;
	RT_REAL32			m_fSpeedY;
	INT32				pad[10];
	void Init()
	{
		m_nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL;
		m_fCountTime = __RT_ZERO;
		m_fDestPosX = __RT_ZERO;
		m_fDestPosY = __RT_ZERO;
		m_fSpeedX = __RT_ZERO;
		m_fSpeedY = __RT_ZERO;
	}
}I3GUI_EVENT_MOVE_DATA;

typedef struct
{
	INT32				m_nFactorType;
	RT_REAL32			m_fCountTime;
	RT_REAL32			m_fSpeed;
	BOOL				m_bLoopRotate;
	BOOL				m_bSpin;
	INT32				m_nSpinDirection;
	INT32				pad[10];
	void Init()
	{
		m_nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL;
		m_fCountTime = __RT_ZERO;
		m_fSpeed = __RT_ZERO;
		m_bLoopRotate = FALSE;
		m_bSpin = FALSE;
		m_nSpinDirection = I3GUI_SPIN_CW;
	}
}I3GUI_EVENT_ROTATE_DATA;

typedef struct
{
	INT32				m_nFactorType;
	RT_REAL32			m_fCountTime;
	RT_REAL32			m_fDestScaleX;
	RT_REAL32			m_fDestScaleY;
	RT_REAL32			m_fOldScaleX;
	RT_REAL32			m_fOldScaleY;
	RT_REAL32			m_fSpeedScaleX;
	RT_REAL32			m_fSpeedScaleY;
	BOOL				m_bLoopScale;
	INT32				pad[7];

	void Init()
	{
		m_nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL;
		m_fCountTime = __RT_ZERO;
		m_fDestScaleX = __RT(1.0f);
		m_fDestScaleX = __RT(1.0f);
		m_fOldScaleX = __RT(1.0f);
		m_fOldScaleX = __RT(1.0f);
		m_fSpeedScaleX = __RT_ZERO;
		m_fSpeedScaleY = __RT_ZERO;
		m_bLoopScale = FALSE;
	}
}I3GUI_EVENT_SCALE_DATA;

typedef struct
{
	i3Sound	*	m_pSound;
	UINT32		m_Style;
	BOOL		m_bLoop;
	INT32		m_DefVolume;

	RT_REAL32	m_timeStart;
	INT32		m_LoopCount;
	UINT32		pad[10];

	void Init()
	{
		m_pSound	= NULL;
		m_Style		= I3SND_PROP_TYPE_VOICE;
		m_DefVolume	= 100;
		m_timeStart = __RT_ZERO;
		m_bLoop		= FALSE;
		m_LoopCount	= 1;
	}
}I3GUI_EVENT_SOUND_DATA;


typedef struct i3GuiControlNotify
{
	INT32 m_nID;		// Control ID, GuiRoot안에서 유일하면 되나, Proj.안에서 유일함을 권장. non-zero.
	INT32 m_nEvent;		// Control Notify Event
	INT32 m_nPriParam;	// Primary Parameter
	INT32 m_nSecParam;	// Secondary Parameter
} I3GUI_CONTROL_NOTIFY;

class I3_EXPORT_GUI I3GUI_EVENT_DATA : public i3NamedElement
{
	I3_CLASS_DEFINE( I3GUI_EVENT_DATA);

public:
	I3GUI_EVENT_DATA();
	virtual ~I3GUI_EVENT_DATA();

public:
	UINT32					  m_nEventStyle;
	I3GUI_EVENT_MOVE_DATA	* m_pMoveData;
	I3GUI_EVENT_ROTATE_DATA * m_pRotateData;
	I3GUI_EVENT_SCALE_DATA	* m_pScaleData;
	i3SoundNode				* m_pSoundNode;
public:
	void Init();

	virtual void OnBuildObjectList( i3List * pList);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};
/////////////////////////////////////////////////////////////////////////////////////////
// Event에 사용되는 struct들입니다.(끝)
/////////////////////////////////////////////////////////////////////////////////////////


class I3_EXPORT_GUI i3GuiImage : public i3NamedElement
{
	I3_CLASS_DEFINE( i3GuiImage );

public:
	i3GuiImage();
	virtual ~i3GuiImage();

	i3Texture	*m_pTexture;
	REAL32		m_nOriginalX, m_nOriginalY;
	REAL32		m_nOriginalWidth, m_nOriginalHeight;
	REAL32		m_nX, m_nY;				// Image의 위치
	REAL32		m_nWidth, m_nHeight;	// Image의 size
	INT32		m_u,m_v,m_w,m_h;		// Texture의 uv와 size
	BOOL		m_bFlipX, m_bFlipY;		// Image의 반전여부
	BOOL		m_bEnable;

public:
	void SetTexture( i3Texture *pTex );
	void SetTexture( i3Texture *pTex, INT32 u, INT32 v, INT32 w, INT32 h);
	i3Texture * GetTexture(void)								{	return m_pTexture;							}
	void setWidth( REAL32 nWidth)								{ m_nWidth = nWidth;							}
	void setHeight( REAL32 nHeight)								{ m_nHeight = nHeight;							}
	REAL32 getWidth(void)										{ return m_nWidth;								}
	REAL32 getHeight(void)										{ return m_nHeight;								}
	REAL32 getPosX(void)										{ return m_nX;									}
	REAL32 getPosY(void)										{ return m_nY;									}
	REAL32 getOriginalWidth(void)								{ return m_nOriginalWidth;						}
	REAL32 getOriginalHeight(void)								{ return m_nOriginalHeight;						}
	REAL32 getOriginalX(void)									{ return m_nOriginalX;							}
	REAL32 getOriginalY(void)									{ return m_nOriginalY;							}
	void setEnable( BOOL bEnable)								{ m_bEnable = bEnable;							}
	BOOL getEnable(void)										{ return m_bEnable;								}

	INT32 getPixelU(void)										{ return m_u;					}
	INT32 getPixelV(void)										{ return m_v;					}
	INT32 getPixelWidth(void)									{ return m_w;					}
	INT32 getPixelHeight(void)									{ return m_h;					}

	void Reset(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

class I3_EXPORT_GUI i3GuiTemplateElement : public i3NamedElement
{
	I3_CLASS_DEFINE( i3GuiTemplateElement );

protected:
	i3List			m_ImageList;

	BOOL			m_bFixedCountShapeType;
//	i3GuiImage*		m_pImage[MAX_I3GUI_CTRL_STATES];

public:
	i3GuiTemplateElement();
	virtual ~i3GuiTemplateElement();
	
	INT32			getShapeCount()								{ return m_ImageList.GetCount();					}
	
	i3GuiImage*		getImage(INT32 nIndex)						{ return (i3GuiImage*)(m_ImageList.GetItem(nIndex));}

	void			setFixedCountShapeType( BOOL bFixed)		{ m_bFixedCountShapeType = bFixed;}
	BOOL			isFixedCountShapeType(void)					{ return m_bFixedCountShapeType;}
	
	void			AddShape();
	void			AddShape(i3GuiImage * pImage);
	void			RemoveShape(INT32 nIndex);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

// ============================================================================
// GUI Control Data
// ============================================================================
class I3_EXPORT_GUI i3GuiControlData : public i3NamedElement
{
	I3_CLASS_DEFINE(i3GuiControlData);

public:
	i3GuiTemplateElement*	m_pShape;
	I3COLOR					m_Color[I3_GUI_SHAPE_COUNT];

	BOOL					m_bEnable;
	INT32					m_nAlignX,			m_nAlignY;
	RT_REAL32				m_fScaleX,			m_fScaleY;
	RT_REAL32				m_fRotate;
	RT_REAL32				m_fRotateCenterX,	m_fRotateCenterY;
	BOOL					m_bFlipX,			m_bFlipY;	// U,v 좌표를 뒤집음.

public:
	i3GuiControlData();
	virtual	~i3GuiControlData(void); 

	void					Init();
	void					SetShape(i3GuiTemplateElement* pShape);
	i3GuiTemplateElement *	GetShape(void)		{ return m_pShape;		}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};


#endif //__I3GUI_DEFINES_H__
