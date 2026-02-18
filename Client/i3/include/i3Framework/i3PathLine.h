#if !defined( __I3_PATH_LINE_H)
#define __I3_PATH_LINE_H

#include "i3PathPoint.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3
{
	namespace pack
	{
		struct PATH_LINK_INFO
		{
			UINT32		m_Style = 0;
			REAL32		m_Cost = 0.0f;

			INT32		m_idxStart = 0;
			INT32		m_idxEnd = 0;

			VEC3D		m_Dir;

			UINT32		pad[12] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_FRAMEWORK i3PathLine : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PathLine, i3ElementBase);

public:
	enum STYLE
	{
		STYLE_BIDIRECTION = 0x00000001,
	};

	#define	MAX_PARAM		4

protected:

	// Persistant members
	REAL32			m_Cost = 0.0f;
	UINT32			m_PathStyle = STYLE::STYLE_BIDIRECTION;
	i3PathPoint *	m_pStart = nullptr;
	i3PathPoint *	m_pEnd = nullptr;

public:
	virtual ~i3PathLine(void);

	REAL32			getCost(void)					{ return m_Cost; }
	void			setCost( REAL32 cost)			{ m_Cost = cost; }

	UINT32			getPathStyle(void)				{ return m_PathStyle; }
	void			setPathStyle( UINT32 style)		{ m_PathStyle = style; }
	void			addPathStyle( UINT32 mask)		{ m_PathStyle |= mask; }
	void			removePathStyle( UINT32 mask)	{ m_PathStyle &= mask; }

	i3PathPoint *	getStart(void)					{ return m_pStart; }
	void			setStart( i3PathPoint * pStart)	{ I3_REF_CHANGE( m_pStart, pStart); }

	i3PathPoint *	getEnd(void)					{ return m_pEnd; }
	void			setEnd( i3PathPoint * pEnd)		{ I3_REF_CHANGE( m_pEnd, pEnd); }

	void			SetTo( i3::pack::PATH_LINK_INFO * pInfo);
};

#endif
