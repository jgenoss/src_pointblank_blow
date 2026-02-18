#include "StdAfx.h"
#include "Framework.h"

I3_CLASS_INSTANCE( CFramework, i3ViewerFramework);

CFramework::CFramework(void)
{
	m_iShapeMode = GIC_SHAPE_MODE_NONE;

}

CFramework::~CFramework(void)
{
	
}

void CFramework::OnCreate()
{
	i3ViewerFramework::OnCreate();
}
