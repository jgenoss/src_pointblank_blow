#if !defined( __I3_GIC_DEFINE_H)
#define __I3_GIC_DEFINE_H

#if defined( I3_DEBUG)

class i3GICShape;

#define		CPSZ			4			///< Control Point size

/** \defgroup I3GIC_CP I3GIC_CP */
//\{
typedef UINT32 I3GIC_CP_STYLE;

#define	I3GIC_CP_RESIZE_LEFT		0x00000001
#define I3GIC_CP_RESIZE_RIGHT		0x00000002
#define	I3GIC_CP_RESIZE_UP			0x00000004
#define	I3GIC_CP_RESIZE_DOWN		0x00000008
#define	I3GIC_CP_RESIZE_ALL			0x0000000F

#define	I3GIC_CP_MOVE_LEFT			0x00000010
#define	I3GIC_CP_MOVE_RIGHT			0x00000020
#define I3GIC_CP_MOVE_TOP			0x00000040
#define I3GIC_CP_MOVE_DOWN			0x00000080
#define I3GIC_CP_MOVE_ALL			0x000000F0

#define I3GIC_CP_MOVE_LINESTART		0x00000100
#define I3GIC_CP_MOVE_LINEEND		0x00000200
#define I3GIC_CP_MOVE_LINEALL		0x00000F00

#define I3GIC_CP_MOVE_POINT1		0x00001000
#define I3GIC_CP_MOVE_POINT2		0x00002000
#define I3GIC_CP_MOVE_POINT3		0x00004000
#define I3GIC_CP_MOVE_POINT4		0x00008000
#define I3GIC_CP_MOVE_POINTALL		0x0000F000

#define I3GIC_CP_VALUE_P1			0x00010000			///< Value Bar를 Control하는 Ponit
#define I3GIC_CP_VALUE_P2			0x00020000
#define I3GIC_CP_VALUE_ALL			0x000F0000

#define I3GIC_CP_LINKER				0x80000000
#define I3GIC_CP_POINTER			0x10000000

//\}

enum I3GIC_CORNER
{
	I3GIC_CORNER_TOP = 0,
	I3GIC_CORNER_LEFT_TOP,
	I3GIC_CORNER_LEFT,
	I3GIC_CORNER_LEFT_BOTTOM,
	I3GIC_CORNER_BOTTOM,
	I3GIC_CORNER_RIGHT_BOTTOM,
	I3GIC_CORNER_RIGHT,
	I3GIC_CORNER_RIGHT_TOP,

	I3GIC_CORNER_MAX
};

/** \defgroup I3GIC_STYLE I3GIC_STYLE	*/
//\{
typedef UINT32 I3GIC_STYLE;
#define I3GIC_STYLE_MASK_TYPE				0x0000FFFF
#define	I3GIC_STYLE_CONTROL					0x00000001			///< Type Control
#define I3GIC_STYLE_LINKER					0x00000002			///< Type Linker
#define I3GIC_STYLE_LINE					0x00000004			///< Type Line
#define I3GIC_STYLE_BEZIERLINE				0x00000008			///< Type Bezier Line
#define I3GIC_STYLE_RECT					0x00000010			///< Type Rectangle
#define I3GIC_STYLE_RECTBAR					0x00000020			///< Type Rectangle with Bar
#define I3GIC_STYLE_TEXT					0x00000040			///< Type Text
#define I3GIC_STYLE_TEXTURE					0x00000080			///< Type Renderable Texture
#define I3GIC_STYLE_CIRCLE					0x00000100			///< Type Circle
#define I3GIC_STYLE_CIRCLEBAR				0x00000200			///< Type Circle with Bar
#define I3GIC_STYLE_TRIANGLE				0x00000400			///< Type Triagle
#define I3GIC_STYLE_POLYGON					0x00000800			///< Type Polygon

#define I3GIC_STYLE_MASK_FLAG				0xFFFF0000
#define I3GIC_STYLE_NOBORDERLINE			0x00010000			///< Border line을 그리지 않는다.
#define I3GIC_STYLE_NOFILL					0x00020000			///< 칠하지 않는다.
#define I3GIC_STYLE_LOCK					0x00040000			///< Parent Node에 붙어있고 움직일 수 없다.
#define I3GIC_STYLE_GDIPLUS					0x00080000			///< GDI+를 이용해서 Drawing합니다. 필요하지 않는 Object는 생성할 필요가 없다.
#define I3GIC_STYLE_NOSELECT				0x00100000			///< 선택할 수 없는 Shape
#define I3GIC_STYLE_INVISIBLE				0x00200000			///< 그리지 않는다.
#define I3GIC_STYLE_NOUPDATE_SIDE_LINK		0x00400000			///< 다른 Link Point에 대해서 Update를 하지 않는다.
#define I3GIC_STYLE_NORESCALE				0x00800000			///< Resize 불가
#define I3GIC_STYLE_NOHITTEST				0x01000000			///< Hit Test를 하지 않는다.
#define I3GIC_STYLE_NOSELECT_ONLY			0x02000000			///< Hierachy 구조에 상관 없이 해당 Shape만 선택하지 못한다.

//\}

/** \defgroup I3GIC_STATE I3GIC_STATE	*/
//\{
typedef UINT32 I3GIC_STATE;
#define	I3GIC_STATE_SELECTED				0x00000001
#define	I3GIC_STATE_DISABLED				0x00000002
#define	I3GIC_STATE_DRAGGING				0x00000004
#define I3GIC_STATE_REDRAW					0x00000008			///< 다시 그려야 하는 상태
#define I3GIC_STATE_LINKTOSHAPE				0x00000010			///< Movement에 의해서 다른 Shape에 Link된 시점. MouseUp이 호출되기 전까지 유지된다.
																	///< 내부 함수에서 Linker를 Update할 시 제외시키기 위한 용도
//\}

enum GIC_MOUSEMODE
{
	GIC_MOUSEMODE_NONE = 0,
	GIC_MOUSEMODE_MULTISEL,
	GIC_MOUSEMODE_CREATE,
	GIC_MOUSEMODE_MOVE_OBJ,
	GIC_MOUSEMODE_MOVE_CP,
	GIC_MOUSEMODE_LINKCREATE,
	GIC_MOUSEMODE_LINKMOVE,

	GIC_MOUSEMODE_PICKEDSHAPE,		///< Tree Design용 모드

	GIC_MOUSEMODE_SCROLL,			///< Middle Button에 의한 Scroll
	GIC_MOUSEMODE_MULTISELECTED,
	GIC_MOUSEMODE_MOVE_MULTIOBJ,
	GIC_MOUSEMODE_CONTROL_VALUE,
};

enum GIC_MOUSE_CURSOR
{
	GIC_CURSOR_NA = -1,
	GIC_CURSOR_SELECT = 0,
	GIC_CURSOR_DISABLE,
	GIC_CURSOR_CREATE,
	GIC_CURSOR_RESIZE_NS,
	GIC_CURSOR_RESIZE_WE,
	GIC_CURSOR_RESIZE_NESW,
	GIC_CURSOR_RESIZE_NWSE,
	GIC_CURSOR_MOVE_ALL,
	GIC_CURSOR_LINK,
	GIC_CURSOR_CONTROL_VALUE,

	GIC_CURSOR_SCROLL,

	GIC_CURSOR_MAX
};


enum GIC_SHAPE_BORDER_STYLE
{
	GIC_SHAPE_BORDER_STYLE_SOLID = 0,			///< PS_SOLID		
	GIC_SHAPE_BORDER_STYLE_DASH,				///< PS_DASH		/* -------  */
	GIC_SHAPE_BORDER_STYLE_DOT,					///< PS_DOT			/* .......  */
	GIC_SHAPE_BORDER_STYLE_DASHDOT,				///< PS_DASHDOT		/* _._._._  */
	GIC_SHAPE_BORDER_STYLE_DASHDOTDOT,			///< PS_DASHDOTDOT	/* _.._.._  */
};

enum GIC_LINE_STYLE
{
	GIC_LINE_STYLE_SOLID = 0,			///< PS_SOLID		
	GIC_LINE_STYLE_DASH,				///< PS_DASH		/* -------  */
	GIC_LINE_STYLE_DOT,					///< PS_DOT			/* .......  */
	GIC_LINE_STYLE_DASHDOT,				///< PS_DASHDOT		/* _._._._  */
	GIC_LINE_STYLE_DASHDOTDOT,			///< PS_DASHDOTDOT	/* _.._.._  */
};


struct GIC_HIT_RESULT
{
	I3GIC_CP_STYLE	m_Semantic = 0;
	i3GICShape *	m_pHitShape = nullptr;
	REAL32			m_rDepth = -100000.0f;

	void reset()
	{
		m_Semantic = 0;
		m_pHitShape = nullptr;
		m_rDepth = -100000.0f;
	}
} ;

class i3GICControlPoint;
class i3GICLinkPoint;

struct I3GIC_POINT
{
	VEC2D				m_Point;	///< Persist
	I3GIC_STATE			m_State = 0;	///< Persist

	VEC2D				m_vSavedPos;	///< Volatile
	i3GICControlPoint *	m_pCtrl = nullptr;		///< Volatile
	i3GICLinkPoint *	m_pLink = nullptr;
} ;

#endif	// I3_DEBUG

#endif
