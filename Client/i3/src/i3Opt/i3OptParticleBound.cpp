#include "i3OptPCH.h"
#include "i3OptParticleBound.h"

I3_CLASS_INSTANCE( i3OptParticleBound);

i3OptParticleBound::i3OptParticleBound(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool i3OptParticleBound::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of< i3Particle*>(pNode))
	{ 
		i3Particle * pParticle = (i3Particle *) pNode;

		pParticle->CalcBoundBox();
	}

	return true;
}
