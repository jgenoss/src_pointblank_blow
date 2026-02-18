#if !defined( __I3_PATHPOINT_H)
#define __I3_PATHPOINT_H

class i3PathLine;

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3
{
	namespace pack
	{
		struct PATH_POINT_INFO
		{
			UINT32		m_Style = 0;
			VEC3D		m_Pos;
			INT32		m_Priority = 0;
			COLORREAL	m_Color;

			INT32		m_idxLink = 0;
			INT32		m_cntLink = 0;

			// Volatile
			UINT32		m_idFind = 0;			// 길찾기를 할 때, 방문한 Node인지를 표시하기 위한 변수

			UINT32		pad[10] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

// 아래의 Class는 Tool용이다. 즉, Game에서는 직접 사용되는 경우가 없도록한다.

class I3_EXPORT_FRAMEWORK i3PathPoint : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PathPoint, i3ElementBase);
protected:
	VEC3D		m_Pos;
	INT32		m_Priority = 0;
	//  의미 없는 색으로 설정
	COLORREAL	m_Color = { 0.0f, 0.4f, 1.0f, 1.0f };

public:
	VEC3D *			getPos(void)					{ return & m_Pos; }
	void			setPos( VEC3D * pPos)			{ i3Vector::Copy( &m_Pos, pPos); }

	INT32			getPriority(void)				{ return m_Priority; }
	void			setPriority( INT32 prio)		{ m_Priority = prio; }

	COLORREAL *		getColor(void)					{ return &m_Color; }
	void			setColor( COLORREAL * pColor)	{ i3Color::Set( &m_Color, pColor); }

	void			SetTo( i3::pack::PATH_POINT_INFO * pInfo);
};

#endif
