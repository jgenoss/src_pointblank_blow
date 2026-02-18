#if !defined( __GIC_AI_DIAGRAM_H)
#define __GIC_AI_DIAGRAM_H

#include "GICShapeAI.h"

class AI;

class GICDiagramAI : public i3GICDiagram
{
	I3_CONCRETE_CLASS_DEFINE( GICDiagramAI, i3GICDiagram);
protected:
	AI *		m_pAI;

public:
	GICDiagramAI(void);
	virtual ~GICDiagramAI(void);

	AI *			getAI(void)					{ return m_pAI; }
	void			setAI( AI * pAI)			{ m_pAI = pAI; }

	virtual void	OnDetachShape( i3GICShape * pShape);
};

#endif
