#if !defined( __I3_UI_SPRITE_H)
#define __I3_UI_SPRITE_H

typedef UINT8 I3UI_SPRITE_STATE;
#define		I3UI_SPRITE_STATE_ALL			0xFF
#define		I3UI_SPRITE_STATE_MODIFIED		0x01
#define		I3UI_SPRITE_STATE_VALIDATED		0x02
#define		I3UI_SPRITE_STATE_USED			0x04	// 사용 여부
#define		I3UI_SPRITE_STATE_DISABLED		0x08
#define		I3UI_SPRITE_STATE_SETTED		0x10


typedef UINT8 I3UI_SPRITE_STYLE;
#define	I3UI_SPRITE_STYLE_INSTANCE			0x00
#define I3UI_SPRITE_STYLE_NODE				0x01	//<<< i3Sprite2D를 사용한 Rendering을 합니다.
#define I3UI_SPRITE_STYLE_BLOCK_AUTO_TRASH_TEX	0x02


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

class i3UISprite
{
protected:
	VEC3D		m_Center;
	VEC3D		m_Size;				// { Width, Height, Rotate }
	VEC4D		m_TexCoord;			// { u1, v1, u2, v2 }
	VEC4D		m_TexRange;			// { u1, v1, u2, v2 }
	UINT32		m_Color = 0xFFFFFFFF;			// ARGB
	
	INT32		m_idxPrev = -1;			// 
	INT32		m_idxNext = -1;			// 할당/해제를 관리하는 Double-Linked-List의 연결

	INT32		m_idxModifyNext = -1;	// 수정된 Sprite들을 연결한 Single-Linked-List를 위한...

	INT32		m_idxInstance = -1;		// 자신에 해당하는 Instance VA의 Index
	i3Node *	m_pNode = nullptr;			//<< Sprite2D나 3Dview용 Pointer

	INT32		m_idxTex = -1;

	I3UI_SPRITE_STATE	m_State = 0;
	I3UI_SPRITE_STYLE	m_Style = 0;	// Style에 따라 Rendering 방식을 결정합니다.

public:
	VEC3D *		getCenter(void)						{ return &m_Center; }
	void		setCenter( VEC3D * pCenter)			{ i3Vector::Copy( &m_Center, pCenter); }
	void		setCenter( REAL32 x, REAL32 y, REAL32 z)
	{
		m_Center.x = x;
		m_Center.y = y;
		m_Center.z = z;
	}

	VEC3D *		getSizeAndRotate(void)						{ return &m_Size; }
	void		setSizeAndRotate( REAL32 w, REAL32 h, REAL32 rot)
	{
		m_Size.x = w;
		m_Size.y = h;
		m_Size.z = rot;
	}

	VEC4D *		getTexCoord(void)					{ return &m_TexCoord; }
	void		setTexCoord( REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
	{
		/*I3ASSERT( u2 >= u1);
		I3ASSERT( v2 >= v1);
		I3ASSERT( u1 >= 0.0f);
		I3ASSERT( v1 >= 0.0f);*/

		// Scale
		m_TexCoord.x = (u2 - u1);
		m_TexCoord.y = (v2 - v1);

		// Offset
		m_TexCoord.z = u1;
		m_TexCoord.w = v1;

		m_State |= I3UI_SPRITE_STATE_SETTED;
	}

	INT32		getTexIndex( void)					{ return m_idxTex; }
	void		setTexIndex( INT32 idx)				{ m_idxTex = idx; }

	VEC4D *		getTexRange(void)					{ return &m_TexRange; }
	void		setTexRange( REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
	{
		m_TexRange.x = u1;
		m_TexRange.y = v1;
		m_TexRange.z = u2;
		m_TexRange.w = v2;
	}

	UINT32		getColor(void)						{ return m_Color; }
	void		setColor( COLOR * pColor)			{ m_Color = i3Color::ConvertRGBA32( pColor); }

	I3UI_SPRITE_STATE	getState(void)					{ return m_State; }
	void		addState( I3UI_SPRITE_STATE state)		{ m_State |= state; }
	void		removeState( I3UI_SPRITE_STATE state)	{ m_State &= ~state; }
	bool		isState( I3UI_SPRITE_STATE state)		{ return (m_State & state) == state; }
	void		clearState(void)					{ m_State = 0; }

	bool		isStyle( I3UI_SPRITE_STYLE style)	{ return (m_Style & style) == style; }
	void		setStyle( I3UI_SPRITE_STYLE style)	{ m_Style = style; }
	I3UI_SPRITE_STYLE getStyle( void)				{ return m_Style; }
	void		addStyle( I3UI_SPRITE_STYLE style)	{ m_Style |= style; }
	void		removeStyle( I3UI_SPRITE_STYLE style) { m_Style &= ~style; }

	bool		isEnabled(void)						{ return isState( I3UI_SPRITE_STATE_DISABLED) == false; }
	void		setEnableState( bool bState)
	{
		if( bState)
			removeState( I3UI_SPRITE_STATE_DISABLED);
		else
			addState( I3UI_SPRITE_STATE_DISABLED);
	}

	bool		isModified(void)					{ return isState( I3UI_SPRITE_STATE_MODIFIED); }
	void		setModified(void)					{ addState( I3UI_SPRITE_STATE_MODIFIED); }
	void		clearModified(void)					{ removeState( I3UI_SPRITE_STATE_MODIFIED); }

	INT32		getPrev(void)						{ return m_idxPrev; }
	void		setPrev( INT32 idx)					{ m_idxPrev = idx; }

	INT32		getNext(void)						{ return m_idxNext; }
	void		setNext( INT32 idx)					{ m_idxNext = idx; }

	INT32		getModifyNext(void)					{ return m_idxModifyNext; }
	void		setModifyNext( INT32 idx)			{ m_idxModifyNext = idx; }

	INT32		getInstanceIndex(void)				{ return m_idxInstance; }
	void		setInstanceIndex( INT32 idx)		{ m_idxInstance = idx; }

	i3Node *	getNode( void)						{ return m_pNode; }
	void		setNode( i3Node * pNode)			{ m_pNode = pNode; }


#if defined( I3_DEBUG)
protected:
	void *		m_pOwner = nullptr;
#endif

public:
	void		SetOwner( void * pOwner)
	{
#if defined( I3_DEBUG)
		m_pOwner = pOwner;
#endif
	}
	void *		GetOwner( void)
	{
#if defined( I3_DEBUG)
		return m_pOwner;
#else
		return nullptr;
#endif
	}
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
