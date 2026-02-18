#include "StdAfx.h"
#include "i3LevelHitPathLink.h"

I3_CLASS_INSTANCE(i3LevelHitPathLink);

i3LevelHitPathLink::i3LevelHitPathLink()
{
	i3LevelPathLink::SetMaterialDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
}


void i3LevelHitPathLink::UpdateLink(void)
{
	VEC3D pos;
	VEC3D dir;

	VEC3D StartPos = *m_pStart->getPos();
	VEC3D EndPos = *m_pEnd->getPos();

	StartPos += m_Offset;
	EndPos += m_Offset;

	i3Vector::Sub(&dir, &StartPos, &EndPos);
	i3Vector::Normalize(&dir, &dir);
	i3Vector::Scale(&dir, &dir, 0.25f);

	i3Vector::Copy(&pos, &StartPos);
	i3Vector::Sub(&pos, &pos, &dir);
	m_pBillboard->SetStart(0, &pos);

	i3Vector::Copy(&pos, &EndPos);
	i3Vector::Add(&pos, &pos, &dir);
	m_pBillboard->SetEnd(0, &pos);

	i3Vector::Minimize(&m_BoundMin, &StartPos, &EndPos);
	i3Vector::Maximize(&m_BoundMax, &StartPos, &EndPos);
}

void i3LevelHitPathLink::OnSelected(BOOL bSelected)
{
	i3LevelControl::OnSelected(bSelected);

	if (bSelected)
		SetMaterialDiffuse(0.0f, 0.0f, 1.0f, 1.0f);
	else
		SetMaterialDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
}