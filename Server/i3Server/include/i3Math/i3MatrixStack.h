#if !defined( __I3_MATRIX_STACK_H)
#define __I3_MATRIX_STACK_H

#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3ElementBase.h"
#include "../i3Base/i3MetaTemplate.h"
#include "i3MathType.h"
#include "i3Matrix.h"

#define		MAX_MATRIXSTACK_DEPTH		40

class I3_EXPORT_MATH i3MatrixStack : public i3ElementBase
{
	I3_CLASS_DEFINE( i3MatrixStack);

protected:
	BOOL			m_bModified;

	MATRIX			* m_Stack[MAX_MATRIXSTACK_DEPTH];
	INT32			m_Index;

	INT32			m_SavedIndex;

public:
	i3MatrixStack(void);
	virtual ~i3MatrixStack(void);

	void	PushPost( MATRIX * pOut, MATRIX * pMatrix);
	void	PushPre( MATRIX * pOut, MATRIX * pMatrix);

	void	MulPost( MATRIX * pMatrix);
	void	MulPre( MATRIX * pMatrix);

	void	PushIdentity(void);
	void	Push( MATRIX * pOut, MATRIX * pMatrix);
	void	Pop(void);

	MATRIX *	GetTop(void)				{ return m_Stack[ m_Index]; }

	void		SetTop( MATRIX * pMatrix);

	void	Reset(void);

	BOOL	IsModified(void)				{ return m_bModified; }
	void	SetModified( BOOL bFlag)		{ m_bModified = bFlag; }

	void	SetBookmark(void)				{ m_SavedIndex = m_Index; }
	void	RestoreBookmark(void)			{ m_Index = m_SavedIndex; }
};

#endif
