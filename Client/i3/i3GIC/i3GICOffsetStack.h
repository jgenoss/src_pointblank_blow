#if !defined( __I3_GIC_OFFSET_STACK_H)
#define __I3_GIC_OFFSET_STACK_H

#if defined( I3_DEBUG)

#define		MAX_STACK_DEPTH			32

/** \brief OffsetStack	????	*/

class I3_EXPORT_GIC i3GICOffsetStack : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GICOffsetStack, i3ElementBase);
protected:
	VEC2D		m_Stack[ MAX_STACK_DEPTH];
	INT32		m_Index;

public:
	i3GICOffsetStack(void);
	virtual ~i3GICOffsetStack(void) {}

	/** \brief 스택에 좌표를 Push합니다. 
		\param x x좌표
		\param y y좌표
		\return none		*/
	void		push( REAL32 x, REAL32 y)
	{
		I3ASSERT( m_Index < MAX_STACK_DEPTH);

		m_Stack[ m_Index].x = m_Stack[ m_Index - 1].x + x;
		m_Stack[ m_Index].y = m_Stack[ m_Index - 1].y + y;

		m_Index++;
	}

	/** \brief 스택에 좌표를 Push합니다. 
		\param [in] pVec x,y좌표를 성분으로하는 2D벡터의 포인터
		\return none		*/
	void		push( VEC2D * pVec)					{	push( pVec->x, pVec->y);	}

	/** \brief 스택에 좌표를 Push합니다. 
		\param [in] pPt x,y좌표를 담고있는 POINT 구조체의 포인터
		\return none		*/
	void		push( POINT * pPt)					{	push( (REAL32) pPt->x, (REAL32) pPt->y); }

	/** \brief 스택에 좌표를 Push합니다. 
		\param x x좌표(정수)
		\param y y좌표(정수)
		\return none		*/
	void		push( INT32 x, INT32 y)				{	push( (REAL32)x, (REAL32)y); }

	/** \brief 스택에서 좌표를 Pop합니다. 
		\return none		*/
	void		pop(void)
	{
		I3ASSERT( m_Index > 0);

		m_Index--;
	}

	/** \brief 스택의 Top원소의 주소를 반환합니다. 
		\return Top원소의 포인터		*/
	VEC2D *		getTop(void)						
	{ 
		//I3ASSERT( m_Index > 0);		/* 검사해주어야 하는것 아닌가요 ????	*/
		return &m_Stack[ m_Index - 1]; 
	}

	/** \brief 스택을 초기화합니다. 
		\param initX x좌표의 초기값
		\param initY y좌표의 초기값
		\return none		*/
	void		reset( REAL32 initX = 0.0f, REAL32 initY = 0.0f)
	{
		m_Stack[0].x = initX;
		m_Stack[0].y = initY;
		m_Index = 1;
	}
	
};

#endif	// I3_DEBUG

#endif
