#if !defined( __I3_ATTR_STACK_H)
#define __I3_ATTR_STACK_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3AttrSet.h"

//
// Traversal을 하는 동안에 여러 Render State 들의 변경 사항을 Stack 형식으로
// 관리하기 위한 Class이다.
#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct I3ATTRSTACKINFO
{
	i3RenderAttr *	m_pAttr = nullptr;
	INT8			m_Priority = 0;
} ;

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

#define		I3SG_ATTRSTACK_STATE_DISABLED			0x00000001
#define		I3SG_ATTRSTACK_STATE_LINKED				0x00000002
#define		I3SG_ATTRSTACK_STATE_LINKED_ALPHA		0x00000004

class I3_EXPORT_SCENE i3AttrStack : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AttrStack, i3ElementBase);

protected:
	I3ATTRSTACKINFO		*		m_pStack = nullptr;
	INT32						m_Count = 0;
	INT32						m_MaxCount = 0;
	INT32						m_AllocUnit;
	i3RenderAttr *				m_pDefaultAttr = nullptr;
	UINT32						m_State = 0;
	bool						m_bLastTraceMode = false;
	INT32						m_ID = -1;

	i3AttrStack *				m_apNext[2] = { nullptr, nullptr };

protected:
	void						ResizeStack( INT32 MaxCount);

public:
	i3AttrStack( INT32 AllocUnit = 20);
	virtual ~i3AttrStack(void);

	inline void setNext( i3AttrStack * pNext, bool bAlphaSort)
	{
		m_apNext[bAlphaSort] = pNext;
	}

	inline i3AttrStack * getNext(bool bAlphaSort) const	{ return m_apNext[bAlphaSort]; }

	inline bool isLinked(bool bAlphaSort) const
	{
		return (m_State & ( I3SG_ATTRSTACK_STATE_LINKED << int(bAlphaSort) ) ) != 0;
	}

	inline void LinkState(bool bAlphaSort) 	{ m_State |= ( I3SG_ATTRSTACK_STATE_LINKED << int(bAlphaSort) ); }
	inline void UnlinkState(bool bAlphaSort)   { m_State &= ~( I3SG_ATTRSTACK_STATE_LINKED << int(bAlphaSort) ); }

	INT32		getID(void)					{ return m_ID; }
	void		setID( INT32 id)			{ m_ID = id; }

	bool	IsEnabled(void)					{ return (m_State & I3SG_ATTRSTACK_STATE_DISABLED) == 0; }
	void	SetEnable( bool bFlag)
	{
		if( bFlag)
			m_State &= ~I3SG_ATTRSTACK_STATE_DISABLED;
		else
			m_State |= I3SG_ATTRSTACK_STATE_DISABLED;
	}

	bool	GetTraceMode(void)				{ return m_bLastTraceMode; }
	void	SetTraceMode( bool bFlag)		{ m_bLastTraceMode = bFlag; }

	void	SetDefaultAttr( i3RenderAttr * pAttr)
	{
		I3_REF_CHANGE( m_pDefaultAttr, pAttr);
	}

	i3RenderAttr *	GetCurrentAttr(void)
	{
		if( m_Count <= 0)
		{
			return m_pDefaultAttr;
		}

		return m_pStack[ m_Count - 1].m_pAttr;
	}

	INT32			GetCurrentPriority(void)
	{
		if( m_Count <= 0)
			return I3_DEF_ATTR_PRIORITY;

		return m_pStack[ m_Count - 1].m_Priority;
	}


	i3RenderAttr * GetDefaultAttr(void)
	{
		return m_pDefaultAttr;
	}


	bool	IsEmpty(void)
	{
		return (m_Count == 0);
	}


	void	Push( i3RenderAttr * pAttr, INT32 Priority = I3_DEF_ATTR_PRIORITY)
	{
		I3ASSERT( Priority > -127);
		I3ASSERT( Priority < 127);

		if( m_bLastTraceMode == false)
		{
			if( m_MaxCount < (m_Count + 1))
			{
				#if defined( I3_DEBUG)
				I3TRACE( "i3AttrStack::Push(%d, %s) - Number of pushed attr is exceed attribute stack size : %d\n", 
					pAttr->GetID(), 
					pAttr->meta()->class_name().c_str()			//pAttr->GetMeta()->GetClassName()
					, m_Count);
				#endif

				ResizeStack( m_MaxCount + m_AllocUnit);
			}

			m_pStack[ m_Count].m_pAttr = pAttr;
			m_pStack[ m_Count].m_Priority = (INT8) Priority;

			m_Count++;
		}
		else
		{
			m_pStack[0].m_pAttr = pAttr;
			m_pStack[0].m_Priority = (INT8) Priority;

			m_Count = 1;
		}
	}

	
	void	Pop(void)
	{
		if( m_bLastTraceMode == false)
		{
			if( m_Count > 0)
				m_Count--;
		}
		else
		{
			m_Count = 0;
		}
	}

	void	PopToCount( INT32 count);

	void	Reset(void);

	
	i3RenderAttr * GetTop(void)
	{
		return GetCurrentAttr();
	}
};

#endif
