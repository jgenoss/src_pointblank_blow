#include "i3SceneDef.h"
#include "i3ProjectOffsetAttr.h"


I3_CLASS_INSTANCE(i3ProjectOffsetAttr);

i3ProjectOffsetAttr::i3ProjectOffsetAttr(void)
{
	SetID(I3_ATTRID_PROJECT_OFFSET);

	m_vec.x = m_vec.y = m_vec.z = 0;
}

i3ProjectOffsetAttr::~i3ProjectOffsetAttr(void)
{
}

void i3ProjectOffsetAttr::Apply(i3RenderContext * pContext)
{
	pContext->SetProjectOffset(m_vec);
}

void i3ProjectOffsetAttr::CopyTo(i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo(pObj, method);

	((i3ProjectOffsetAttr *)pObj)->Set(Get());
}

bool i3ProjectOffsetAttr::IsSame(i3RenderAttr * pAttr)
{
	return i3Vector::IsSame(&m_vec, ((i3ProjectOffsetAttr *)pAttr)->Get());
}
