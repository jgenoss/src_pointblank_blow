#ifndef __I3UI_DEFINES_H__
#define __I3UI_DEFINES_H__

#define I3UI_RESOLUTION_WIDTH	1024.0f
#define I3UI_RESOLUTION_HEIGHT	768.0f

#define USE_VERTEX_BANK			1

#define	I3UI_DEFAULT_CHARA_SET	"맑은 고딕"

// Control의 모양을 나타냅니다.
// 최소한으로 Control을 사용할때는 I3UI_CTRL_SHAPE_CENTER만 사용합니다.
enum I3UI_CTRL_SHAPE
{	
	I3UI_CTRL_SHAPE_NONE = -1,
	I3UI_CTRL_SHAPE_CLIENT = 0,
	I3UI_CTRL_SHAPE_TOP,
	I3UI_CTRL_SHAPE_LEFT,
	I3UI_CTRL_SHAPE_RIGHT,
	I3UI_CTRL_SHAPE_BOTTOM,
	I3UI_CTRL_SHAPE_TOP_LEFT,
	I3UI_CTRL_SHAPE_TOP_RIGHT,
	I3UI_CTRL_SHAPE_BOTTOM_LEFT,
	I3UI_CTRL_SHAPE_BOTTOM_RIGHT,
	I3UI_CTRL_SHAPE_CAPTION,
	I3UI_CTRL_SHAPE_CLOSE,
	I3UI_CTRL_SHAPE_HSCROLL,
	I3UI_CTRL_SHAPE_VSCROLL,
	I3UI_CTRL_SHAPE_SIZE,

	MAX_I3UI_CTRL_SHAPE,
};

// control style
typedef UINT32 I3UI_STYLE;
#define I3UI_STYLE_ALIGN_LEFT			0x00000000	//	가로정렬 왼쪽
#define I3UI_STYLE_ALIGN_MIDDLE			0x00000001	//	가로정렬 중간
#define I3UI_STYLE_ALIGN_RIGHT			0x00000002	//	가로정렬 오른쪽
#define	I3UI_STYLE_ALIGN_HMASK			0x0000000F	//	가로정렬 마스크

#define	I3UI_STYLE_ALIGN_TOP			0x00000000	//	세로정렬 위
#define	I3UI_STYLE_ALIGN_CENTER			0x00000010	//	세로정렬 중간
#define	I3UI_STYLE_ALIGN_BOTTOM			0x00000020	//	세로정렬 아래
#define	I3UI_STYLE_ALIGN_VMASK			0x000000F0	//	세로정렬 마스크

#define	I3UI_STYLE_SIZE_PIXEL			0x00000000	//	고정 픽셀 사이즈 방식
#define	I3UI_STYLE_SIZE_RATIO			0x00000100	//	해상도와 연관된 값이 곱해진 크기가 됨)
#define	I3UI_STYLE_SIZE_MASK			0x00000F00	//	마스크

#define I3UI_STYLE_POS_ALIGN_MASK		0x0003F000	//	Screen 위치에 대한 정렬
#define I3UI_STYLE_POS_ALIGN_LEFT		0x00000000	
#define I3UI_STYLE_POS_ALIGN_CENTER		0x00001000
#define I3UI_STYLE_POS_ALIGN_RIGHT		0x00002000
#define I3UI_STYLE_POS_ALIGN_FORCE_LEFT 0x00010000

#define I3UI_STYLE_POS_ALIGN_TOP		0x00000000
#define I3UI_STYLE_POS_ALIGN_MIDDLE		0x00004000
#define I3UI_STYLE_POS_ALIGN_BOTTOM		0x00008000
#define I3UI_STYLE_POS_ALIGN_FORCE_TOP  0x00020000

#define	I3UI_STYLE_SLAVE				0x10000000	//	Eable/Disable/Size/move등등을 Owner Control에서 직접 설정해준다.
#define I3UI_STYLE_LOCK					0x20000000	//	Lock
#define I3UI_STYLE_NO_VT				0x40000000	//	Virtual Texture를 사용하지 않고 일반 Texture를 이용하여 Rendering. 3DView, 2D Old
#define I3UI_STYLE_IGNOREINPUT			0x80000000	//	Input을 처리 하지 않는다.
#define I3UI_STYLE_HIDE					0x01000000	//	Shape을 강제로 Hide시킨다.
#define I3UI_STYLE_MANUAL_ORDER			0x02000000	// Manual로 SpriteOrder를 한다.. 
#define I3UI_STYLE_AUTOALIGN			0x00400000	//	Align 기능 사용
#define I3UI_STYLE_BLOCK_AUTO_TRASH_TEX	0x08000000  // 
#define I3UI_STYLE_TOP_DRAW				0x0F000000  // 

// control state
typedef UINT32 I3UI_STATE;
#define I3UI_STATE_DEACTIVATED			0x00000001		
#define I3UI_STATE_FOCUSED				0x00000002
#define I3UI_STATE_ONMOUSE				0x00000004
#define I3UI_STATE_SELECTED				0x00000008		// Button, Checkbox, etc...
#define I3UI_STATE_HIDED				0x00000010
#define I3UI_STATE_NO_NOTIFY			0x00000020
#define I3UI_STATE_MODAL				0x00000040		// FrameWnd Only
#define	I3UI_STATE_WAITLOAD				0x00000080		// Texture Loading이 완료되기를 기다리는 상태..
#define I3UI_STATE_INVALIDSPRITE		0x00000100		// 생성된 후, 아직 Sprite이 제대로 설정되지 않았음을 의미.
#define I3UI_STATE_REORDERING			0x00000200		// Order를 다시 설정합니다.
#define I3UI_STATE_PUSHED				0x00000400		// 눌려진 상태 확인
#define I3UI_STATE_HOLD_IMAGE			0x00000800		//
#define I3UI_STATE_LBTN_DRAG			0x00001000		// L Btn Drag 상태
#define I3UI_STATE_RBTN_DRAG			0x00002000		// R Btn Drag 상태

#define I3UI_STATE_NEED_UPDATE			0x80000000		// Control의 변화가 생길경우 해당내용을 Update합니다.

#define I3UI_STATE_VOLATILE_MASK	(I3UI_STATE_DEACTIVATED | I3UI_STATE_FOCUSED | I3UI_STATE_SELECTED | I3UI_STATE_ONMOUSE | I3UI_STATE_MODAL)

enum I3UI_MODALRESULT
{
	I3UI_MODALRESULT_CANCEL = 0,
	I3UI_MODALRESULT_YES,
	I3UI_MODALRESULT_NO
};

// Control의 State(상태)를 나타냅니다.
enum I3UI_EVT
{
	I3UI_EVT_NORMAL = 0,
	I3UI_EVT_CREATE,	
	I3UI_EVT_DESTROY,
	I3UI_EVT_SHOW,
	I3UI_EVT_HIDE,
	I3UI_EVT_ACTIVATE,
	I3UI_EVT_INACTIVATE,
	I3UI_EVT_ENABLE,
	I3UI_EVT_DISABLE,
	I3UI_EVT_SETFOCUS,
	I3UI_EVT_KILLFOCUS,
	I3UI_EVT_ENTERMOUSE,
	I3UI_EVT_LEAVEMOUSE,

	////////////////////////////////////////////////////
	// I3UI_EVT_CLICKED
	//
	// Desc			Mouse LButton이 클릭되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_DBL_CLICKED
	//
	// Desc			Mouse LButton이 Double Click되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_DBL_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_R_CLICKED
	//
	// Desc			Mouse RButton이 Click 되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_R_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_R_DBL_CLICKED
	//
	// Desc			Mouse RButton이 Double Click되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_R_DBL_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_M_CLICKED
	//
	// Desc			Mouse MButton이 Click 되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_M_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_M_DBL_CLICKED
	//
	// Desc			Mouse MButton이 Double Click되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_M_DBL_CLICKED,

	////////////////////////////////////////////////////
	// I3UI_EVT_DRAGSTART, I3UI_EVT_DRAGGING, I3UI_EVT_DRAGEND
	//
	// Desc			Dragging
	// Param1		x position of mouse
	// Param2		y position of mouse
	I3UI_EVT_DRAGSTART,
	I3UI_EVT_DRAGGING,
	I3UI_EVT_DRAGEND,
	
	I3UI_EVT_DRAGCANCEL,

	I3UI_EVT_SIZING,
	
	////////////////////////////////////////////////////
	// I3UI_EVT_SIZE
	//
	// Desc			OnSize
	// Param1 cx
	// Param2 cy
	////////////////////////////////////////////////////
	I3UI_EVT_SIZE,		

	I3UI_EVT_MOVING,
	I3UI_EVT_MOVE,

	I3UI_EVT_BTN_PUSHED,

	////////////////////////////////////////////////////
	// I3UI_EVT_IME
	//
	// Desc			IME에서 상태가 변경되었을 때 Notify된다.
	//
	// Param1		Event Code ( i3InputIME의 EDIT_STATE를 참조)
	// Param2		Unused
	I3UI_EVT_IME,

	////////////////////////////////////////////////////
	// I3UI_EVT_CHANGED
	//
	// Desc			값이 변경되었을 때 전달된다.
	//
	// Param1		Unused
	// Param2		Unused
	I3UI_EVT_CHANGED,

	////////////////////////////////////////////////////
	// I3UI_EVT_MODALRESULT
	//
	// Desc			Modal Dialog의 동작 결과를 Owner Window에게 전달한다.
	//
	// Param1		i3UIFrameWnd *
	// Param2		I3UI_MODALRESULT
	I3UI_EVT_MODALRESULT,

	//////////////////////////////////////////////////////
	// I3UI_EVT_SCROLL
	// Desc				Scroll Position이 변경되었음을 알린다.
	// Param1			Current Position
	// Param2			Unused
	I3UI_EVT_SCROLL,

	//////////////////////////////////////////////////////
	// I3UI_EVT_SELITEM
	// Desc				List Box의 아이템이 선택되었음을 알린다.
	// Param1			Item Idx
	// Param2			Unused
	I3UI_EVT_SELITEM,


	I3UI_EVT_NOTIFY,
	
	//////////////////////////////////////////////////////
	// I3UI_EVT_MOUSEWHEEL
	// Desc				Mouse Wheel이 스크롤되었음을 알린다.( I3UI_EVT_SCROLL과 다름)
	// Param1			OnMouseWheel에서의 zDelta(short)
	// Param2			Unused
	I3UI_EVT_MOUSEWHEEL,
	
	I3UI_EVT_UPDATE_STATE,
	
	//////////////////////////////////////////////////////
	// I3UI_EVT_TOOLTIP
	// Desc				ToolTip을 보여주거나 숨겨야함을 알린다. 
	// Param1			1이면 Show, 0이면 Hide
	// Param2			Unused
	I3UI_EVT_TOOLTIP,		//지워도 괜찮은 이벤트 입니다.
	
	//////////////////////////////////////////////////////
	// I3UI_EVT_KEYDOWN
	// Desc				KeyDown Event를 보냅니다.
	// Param1			key
	// Param2			Unused
	I3UI_EVT_KEYDOWN,
	
	I3UI_EVT_COMBOBOX_UNFOLDED,
	I3UI_EVT_COMBOBOX_FOLDED,
	I3UI_EVT_COMBOBOX_SELITEM,

	I3UI_EVT_RADIO_CHECKED, 
	I3UI_EVT_RADIO_UNCHECKED,
	
	I3UI_EVT_LVH_PUSHED,

	I3UI_EVT_RESERVED9,

	I3UI_EVT_RESERVED10,
	I3UI_EVT_RESERVED11,
	I3UI_EVT_RESERVED12,
	I3UI_EVT_RESERVED13,
	I3UI_EVT_RESERVED14,
	I3UI_EVT_RESERVED15,
	I3UI_EVT_RESERVED16,
	I3UI_EVT_RESERVED17,
	I3UI_EVT_RESERVED18,
	I3UI_EVT_RESERVED19,

	// desc Load 완료시 이벤트입니다.
	I3UI_EVT_COMPLETE_LOAD,

	I3UI_EVT_MAXCOUNT
};

struct I3UI_EVENT_INFO
{
	I3UI_EVT		m_Code;
	const char*		m_pszEvent;		// 기본적으로 문자열리터럴을 취득해 쓰기 때문에 일단 방치합니다...
	const char*		m_pszCode;		// For 'Dlg_InsertEventHandler" (intellisense??)
};

extern I3_EXPORT_FRAMEWORK I3UI_EVENT_INFO s_EventInfoTable[I3UI_EVT_MAXCOUNT];

class i3UIControl;
class i3UIFrameWnd;
class i3UIAnimation2DSprite;

struct I3UI_PICK_INFO
{
	REAL32				m_X = 0.0f;
	REAL32				m_Y = 0.0f;
	i3UIControl *		m_pPickedCtrl = nullptr;
	I3UI_CTRL_SHAPE	m_ShapeType = I3UI_CTRL_SHAPE::I3UI_CTRL_SHAPE_NONE;

	void				init( REAL32 x, REAL32 y)
	{
		m_X = x;		m_Y = y;
		m_pPickedCtrl = nullptr;
		m_ShapeType = I3UI_CTRL_SHAPE_NONE;
	}
};


// 입력모드
enum I3UI_INPUT_MODE
{
	I3UI_INPUT_MODE_STROKE= 0,
	I3UI_INPUT_MODE_PRESS ,
	I3UI_INPUT_MODE_REPEAT,

	I3UI_INPUT_MODE_COUNT,
};


// mouse button state
#define I3UI_MOUSE_STATE_LBUTTON_PRESS		0x00000001	// I3I_MOUSE_LBUTTON와 bit 맞춤
#define I3UI_MOUSE_STATE_RBUTTON_PRESS		0x00000002	// I3I_MOUSE_RBUTTON와 bit 맞춤
#define I3UI_MOUSE_STATE_MBUTTON_PRESS		0x00000004	// I3I_MOUSE_MBUTTON와 bit 맞춤

#define I3UI_MOUSE_STATE_LBUTTON_DOWN		0x00000010
#define I3UI_MOUSE_STATE_RBUTTON_DOWN		0x00000020
#define I3UI_MOUSE_STATE_MBUTTON_DOWN		0x00000040
#define I3UI_MOUSE_STATE_LBUTTON_UP			0x00000080
#define I3UI_MOUSE_STATE_RBUTTON_UP			0x00000100
#define I3UI_MOUSE_STATE_MBUTTON_UP			0x00000200
#define I3UI_MOUSE_STATE_LBUTTON_DCLICK		0x00000400
#define I3UI_MOUSE_STATE_RBUTTON_DCLICK		0x00000800
#define I3UI_MOUSE_STATE_MBUTTON_DCLICK		0x00001000
#define I3UI_MOUSE_STATE_CTRL				0x00002000
#define I3UI_MOUSE_STATE_SHIFT				0x00004000
#define I3UI_MOUSE_STATE_ALT				0x00008000
#define I3UI_MOUSE_STATE_LBUTTON_REPEAT		0x00010000
#define I3UI_MOUSE_STATE_RBUTTON_REPEAT		0x00020000
#define I3UI_MOUSE_STATE_MBUTTON_REPEAT		0x00040000
#define I3UI_MOUSE_STATE_WHEEL_UP			0x10000000
#define I3UI_MOUSE_STATE_WHEEL_DOWN			0x20000000

#define I3UI_MOUSE_STATE_MASK_PRESS			(I3UI_MOUSE_STATE_LBUTTON_PRESS|I3UI_MOUSE_STATE_RBUTTON_PRESS|I3UI_MOUSE_STATE_MBUTTON_PRESS)
#define I3UI_MOUSE_STATE_MASK_MODIFIER		(I3UI_MOUSE_STATE_CTRL|I3UI_MOUSE_STATE_SHIFT|I3UI_MOUSE_STATE_ALT))

struct I3UI_CONTROL_NOTIFY
{
	INT32			m_nID = 0;				// Control ID, GuiRoot안에서 유일하면 되나, Proj.안에서 유일함을 권장. non-zero.
	I3UI_EVT		m_nEvent = I3UI_EVT_NORMAL;			// Control Notify Event
	INT32			m_nPriParam = 0;		// Primary Parameter
	INT32			m_nSecParam = 0;		// Secondary Parameter
	i3UIControl *	m_pCtrl = nullptr;			// Control의 포인터로써 ID로 활용
	i3UIFrameWnd *	m_pFrameWnd = nullptr;		// Control을 포함한 FrameWnd의 Pointer;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define	I3UI_SHAPE_STYLE_FIXED_WIDTH	0x00000001
#define	I3UI_SHAPE_STYLE_FIXED_HEIGHT	0x00000002
#define	I3UI_SHAPE_STYLE_FLIP_X			0x00010000
#define I3UI_SHAPE_STYLE_FLIP_Y			0x00020000

class I3_EXPORT_FRAMEWORK i3UIShape : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3UIShape, i3NamedElement );

protected:
	UINT32				m_nStyle = 0;								// style

	i3::pack::RECT		m_Rect = { 0.0f, 0.0f, 10.0f, 10.0f };									// shape의 x, y, width, height
	i3::pack::RECT		m_TexCoord;								// shape의 texutre coordinate ( u, v, u1, v1)
	i3::pack::POINT2D	m_VTexOffset;							// virtual texture내에서의 reftexture의 offset.
	REAL32				m_RotationRad = 0.0f;							// Rotation value
	INT32				m_TextAlignOffsetX = 0;
	INT32				m_TextAlignOffsetY = 0;

	// for tool
	i3::rc_string		m_strTexFile;							// Texture의 원본 파일명 입니다.
	COLOR				m_ColorKey = { 0, 0, 0, 0 };			// 원본 파일에서 UV검출을 위한 Color Key

	bool				m_bEnable = true;


	i3::rc_string		m_strAnimationName = "";
	INT32				m_nAnimationIndex = 0;
	bool				m_bAnimationRoof = false;
	i3::vector<i3::rc_string>	m_AnimationList;
public:
	

	virtual void	OnUpdate(REAL32 rDeltaSeconds) {}

	/// style
	void	setStyle( UINT32 nStyle)	{	m_nStyle = nStyle;}
	UINT32	getStyle(void)				{	return m_nStyle;}
	void	addStyle( UINT32 nAdd)		{	m_nStyle |= nAdd;}
	void	removeStyle( UINT32 nRemove){	m_nStyle &= ~nRemove;}
	bool	isStyle( UINT32 nStyle)		{	return (m_nStyle & nStyle) != 0;}

	void	enableStyle( bool bEnable, UINT32 nStyle);

	void	setFlipX( bool bFlip)		{	enableStyle( bFlip, I3UI_SHAPE_STYLE_FLIP_X);}
	void	setFlipY( bool bFlip)		{	enableStyle( bFlip, I3UI_SHAPE_STYLE_FLIP_Y);}

	///
	void	setEnable( bool bEnable)	{	m_bEnable = bEnable;	}
	bool	getEnable(void)				{	return m_bEnable;		}

	///
	bool	hasTexture(void)			{	return !m_strTexFile.empty(); }  

	/// rect
	const i3::pack::RECT *	getRect(void) const		{	return &m_Rect;}
	void		setRect( i3::pack::RECT * pRect)	{	m_Rect = *pRect;}

	REAL32	getPosY(void)				{	return m_Rect.top;}
	REAL32	getPosX(void)				{	return m_Rect.left;}
	REAL32	getWidth(void)				{	return m_Rect.right;}
	REAL32	getHeight(void)				{	return m_Rect.bottom;}

	void setPosX( REAL32 x)				{	m_Rect.left = x;}
	void setPosY( REAL32 y)				{	m_Rect.top = y;}
	void setWidth( REAL32 Width)		{	m_Rect.right = Width;}
	void setHeight( REAL32 Height)		{	m_Rect.bottom = Height;}

	REAL32	getRotation( void)			{ return m_RotationRad; }
	void	setRotation( REAL32 rad)	{ m_RotationRad = rad; }

	/// texcoord
	i3::pack::RECT	*	getTextureCoord(void)	{ return &m_TexCoord;}
	void		SetTextureCoord( i3::pack::RECT * pCoord)	{	m_TexCoord = *pCoord;}

	REAL32 getCoordU(void)					{ return m_TexCoord.left;}
	REAL32 getCoordV(void)					{ return m_TexCoord.top;}
	REAL32 getCoordW(void)					{ return m_TexCoord.right;}
	REAL32 getCoordH(void)					{ return m_TexCoord.bottom;}

	void	setCoordU( REAL32 u)				{	m_TexCoord.left		= u;}
	void	setCoordV( REAL32 v)				{	m_TexCoord.top		= v;}
	void	setCoordW( REAL32 w)				{	m_TexCoord.right	= w;}
	void	setCoordH( REAL32 h)				{	m_TexCoord.bottom	= h;}

	void	setTexcoordOffset( i3::pack::POINT2D *offset)		{	m_VTexOffset = *offset;}
	void	setTexcoordOffset( INT32 x, INT32 y)	{	m_VTexOffset.x = x;	m_VTexOffset.y= y;}
	i3::pack::POINT2D	*getTexCoordOffset(	void)				{	return &m_VTexOffset;}

	/// res texture info
	const char* getTextureResPath(void)	const		{	return m_strTexFile.c_str();}
	COLOR	*getColorKey(void)						{	return &m_ColorKey;}

	void setTextureResPath(const char * pszPath);// {	m_strTexFile = pszPath; }
	void setColorKey( COLOR *col)					{	i3Color::Copy( &m_ColorKey, col);}

	INT32	getTextOffsetX()						{	return m_TextAlignOffsetX;}
	INT32	getTextOffsetY()						{	return m_TextAlignOffsetY;}
	void	SetAnimationName(i3::rc_string chName)			{ m_strAnimationName = chName; }
	i3::rc_string	GetAnimationName()				{ return m_strAnimationName; }
	bool			IsAnimationRoof()				{ return m_bAnimationRoof;  }

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List);
	virtual void	OnTDKPropertyChanged( const char* szName);
#endif	// I3_DEBUG

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};


#endif //__I3UI_DEFINES_H__
