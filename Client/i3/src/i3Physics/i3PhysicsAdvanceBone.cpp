#include "i3PhysicsType.h"
#include "i3PhysicsAdvanceBone.h"

I3_CLASS_INSTANCE( i3PhysicsAdvanceBone, i3ElementBase );

i3PhysicsAdvanceBone::i3PhysicsAdvanceBone()
{
	m_pBone = NULL;
	m_pParentBone = NULL;

	min_rx = 0.0f;
	max_rx = 0.0f;
	min_ry = 0.0f;
	max_ry = 0.0f;
	min_rz = 0.0f;
	max_rz = 0.0f;
	damp_width = 0.0f;
	damp_strength = 0.0f;

	m_Mass = 0.0f;
	m_Friction = 0.0f;
	m_Kfriction = 0.0f;
	m_Elast = 0.0f;
	
	i3Vector::Set( &m_vDir, 0.0f, 0.0f, 0.0f );
}

i3PhysicsAdvanceBone::~i3PhysicsAdvanceBone()
{
}

void i3PhysicsAdvanceBone::Create( i3Transform * pBone, i3Transform * pParentBone )
{
	m_pBone = pBone;

	m_pParentBone = pParentBone;
}

