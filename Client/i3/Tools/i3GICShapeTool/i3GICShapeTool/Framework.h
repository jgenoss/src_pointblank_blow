#pragma once

#include "CommonDef.h"

class CFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( CFramework );

protected:
	GIC_SHAPE_MODE		m_iShapeMode;
	PANE_MODE			m_iPaneMode;

public:
	GIC_SHAPE_MODE		getShapeMode( void)						{ return m_iShapeMode; }
	void				setShapeMode( GIC_SHAPE_MODE mode)		{ m_iShapeMode = mode; }

	PANE_MODE			getPaneMode( void)						{ return m_iPaneMode; }
	void				setPaneMode( PANE_MODE mode)			{ m_iPaneMode = mode; }

public:
	CFramework(void);
	virtual ~CFramework(void);

	virtual void	OnCreate( void);
};
