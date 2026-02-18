#include "stdafx.h"
#include "GICDiagramAI.h"

//I3_CONCRETE_CLASS_INSTANCE( GICDiagramAI, i3GICDiagram);
I3_CLASS_INSTANCE( GICDiagramAI);
//

GICDiagramAI::GICDiagramAI(void)
{
	m_pAI = NULL;
}

GICDiagramAI::~GICDiagramAI(void)
{
}

void GICDiagramAI::OnDetachShape( i3GICShape * pShape)
{
	GICShapeAI * pAI = (GICShapeAI *) pShape;

	m_pAI->getAIContext()->RemoveAIState( pAI->getAIState());
}
