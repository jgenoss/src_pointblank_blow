#if !defined( __I3_MATRIX_STACK_H)
#define __I3_MATRIX_STACK_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "i3MathType.h"
#include "i3Matrix.h"

#define		MAX_MATRIXSTACK_DEPTH		40

class I3_EXPORT_MATH i3MatrixStack : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3MatrixStack, i3ElementBase);

protected:
	bool			m_bModified = true;

	MATRIX			* m_Stack[MAX_MATRIXSTACK_DEPTH] = { nullptr };
	INT32			m_Index = 0;
	INT32			m_SavedIndex = -1;

public:
	i3MatrixStack(void);

	void	PushPost( MATRIX * pOut, MATRIX * pMatrix);
	void	PushPre( MATRIX * pOut, MATRIX * pMatrix);

	//void	MulPost( MATRIX * pMatrix);
	//void	MulPre( MATRIX * pMatrix);

	void	PushIdentity(void);
	void	Push( MATRIX * pOut, MATRIX * pMatrix);
	void	Pop(void);
	void	Pop( INT32 iCount);

	MATRIX *	GetTop(void)				{ return m_Stack[ m_Index]; }

	void		SetTop( MATRIX * pMatrix);

	void	Reset(void);

	bool	IsModified(void)				{ return m_bModified; }
	void	SetModified( bool bFlag)		{ m_bModified = bFlag; }

	void	SetBookmark(void)				{ m_SavedIndex = m_Index; }
	void	RestoreBookmark(void)			{ m_Index = m_SavedIndex; }
};

#endif
