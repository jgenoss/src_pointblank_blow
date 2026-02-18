#if !defined( __I3_ATTR_STACK_H)
#define __I3_ATTR_STACK_H

#include "i3Base.h"
#include "i3RenderAttr.h"
#include "i3AttrSet.h"

//
// Traversal을 하는 동안에 여러 Render State 들의 변경 사항을 Stack 형식으로
// 관리하기 위한 Class이다.

typedef struct _tagI3AttrStackInfo
{
	INT32			m_Priority;
	i3RenderAttr *	m_pAttr;
} I3ATTRSTACKINFO;

#define		I3SG_ATTRSTACK_STATE_DISABLED	0x00000001
#define		I3SG_ATTRSTACK_STATE_LINKED		0x00000002

class I3_EXPORT_SCENE i3AttrStack : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AttrStack);

protected:
	I3ATTRSTACKINFO		*		m_pStack;
	INT32						m_Count;
	INT32						m_MaxCount;
	INT32						m_AllocUnit;
	i3RenderAttr *				m_pDefaultAttr;
	UINT32						m_State;
	BOOL						m_bLastTraceMode;
	INT32						m_ID;

	i3AttrStack *				m_pNext;

protected:
	void						ResizeStack( INT32 MaxCount);

public:
	i3AttrStack( INT32 AllocUnit = 20);
	virtual ~i3AttrStack(void);

	inline void setNext( i3AttrStack * pNext)
	{
		m_pNext = pNext;
	}

	inline i3AttrStack * getNext(void)		{ return m_pNext; }

	inline bool isLinked(void)
	{
		return (m_State & I3SG_ATTRSTACK_STATE_LINKED) != 0;
	}

	inline void LinkState(void)				{ m_State |= I3SG_ATTRSTACK_STATE_LINKED; }
	inline void UnlinkState(void)			{ m_State &= ~I3SG_ATTRSTACK_STATE_LINKED; }

	INT32		getID(void)					{ return m_ID; }
	void		setID( INT32 id)			{ m_ID = id; }

	BOOL	IsEnabled(void)					{ return (m_State & I3SG_ATTRSTACK_STATE_DISABLED) == 0; }
	void	SetEnable( BOOL bFlag)
	{
		if( bFlag)
			m_State &= ~I3SG_ATTRSTACK_STATE_DISABLED;
		else
			m_State |= I3SG_ATTRSTACK_STATE_DISABLED;
	}

	BOOL	GetTraceMode(void)				{ return m_bLastTraceMode; }
	void	SetTraceMode( BOOL bFlag)		{ m_bLastTraceMode = bFlag; }

	void	SetDefaultAttr( i3RenderAttr * pAttr)
	{
		m_pDefaultAttr = pAttr;

		m_pDefaultAttr->AddRef();
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
		if( m_bLastTraceMode == FALSE)
		{
			if( m_MaxCount < (m_Count + 1))
			{
				#if defined( I3_DEBUG)
				I3TRACE( "i3AttrStack::Push(%d, %s) - Number of pushed attr is exceed attribute stack size : %d\n", 
					pAttr->GetID(), pAttr->GetMeta()->GetClassName(), m_Count);
				#endif

				ResizeStack( m_MaxCount + m_AllocUnit);
			}

			m_pStack[ m_Count].m_pAttr = pAttr;
			m_pStack[ m_Count].m_Priority = Priority;

			m_Count++;
		}
		else
		{
			m_pStack[0].m_pAttr = pAttr;
			m_pStack[0].m_Priority = Priority;

			m_Count = 1;
		}
	}

	
	void	Pop(void)
	{
		if( m_bLastTraceMode == FALSE)
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
