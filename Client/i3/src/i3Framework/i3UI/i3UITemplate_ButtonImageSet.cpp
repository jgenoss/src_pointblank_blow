#include "i3FrameworkPCH.h"
#include "i3UITemplate_ButtonImageSet.h"
#include "i3UIButtonImageSet.h"

//I3_CLASS_INSTANCE( i3UITemplate_ButtonImageSet, i3UITemplate_User);
I3_CLASS_INSTANCE( i3UITemplate_ButtonImageSet);

i3UITemplate_ButtonImageSet::i3UITemplate_ButtonImageSet()
{
	m_pIB[I3UI_IMAGESET_IB_OVER] = i3UITemplate_ImageBox::new_object();
	m_pIB[I3UI_IMAGESET_IB_OVER]->SetName( "MouseOver");
	m_pIB[I3UI_IMAGESET_IB_OVER]->setBuiltInSubTemplate( true);

	m_pIB[I3UI_IMAGESET_IB_DOWN] = i3UITemplate_ImageBox::new_object();
	m_pIB[I3UI_IMAGESET_IB_DOWN]->SetName( "MouseDown");
	m_pIB[I3UI_IMAGESET_IB_DOWN]->setBuiltInSubTemplate( true);

	m_pIB[I3UI_IMAGESET_IB_DISABLED] = i3UITemplate_ImageBox::new_object();
	m_pIB[I3UI_IMAGESET_IB_DISABLED]->SetName( "Disabled");
	m_pIB[I3UI_IMAGESET_IB_DISABLED]->setBuiltInSubTemplate( true);
}

i3UITemplate_ButtonImageSet::~i3UITemplate_ButtonImageSet()
{
	for( INT32 i=0; i<getSubTemplateCount(); ++i) {
		I3_SAFE_RELEASE( m_pIB[i]);
	}	
}


INT32 i3UITemplate_ButtonImageSet::getSubTemplateCount( void )
{
	return I3UI_IMAGESET_IB_COUNT;
}

INT32 i3UITemplate_ButtonImageSet::addSubTemplate( i3UITemplate * pTemplate)
{
	INT32 idx = i3UITemplate::addSubTemplate( pTemplate);

	setSubTemplate( idx, pTemplate);

	return idx;
}

i3UITemplate * i3UITemplate_ButtonImageSet::getSubTemplate( INT32 idx )
{
	I3_BOUNDCHK( idx, I3UI_IMAGESET_IB_COUNT);

	return (i3UITemplate*)m_pIB[ idx];
}

void i3UITemplate_ButtonImageSet::setSubTemplate( INT32 idx, i3UITemplate * pTemplate )
{
	I3_BOUNDCHK( idx, I3UI_IMAGESET_IB_COUNT);

#if defined( I3_DEBUG)
	if( pTemplate != nullptr)
		I3ASSERT( i3::kind_of<i3UITemplate_ImageBox*>(pTemplate));
#endif

	I3_REF_CHANGE( m_pIB[idx], (i3UITemplate_ImageBox*)pTemplate);
}

bool i3UITemplate_ButtonImageSet::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 totalCount = getShapeCount();
	
	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( INT32 i=0; i < totalCount; ++i)
	{
 		i3::pack::RECT* pRect = _getTargetRect( i, pRects);

		pRect->left = 0.0f;
		pRect->top	= 0.0f;

		pRect->right = cx;
		pRect->bottom = cy;
	}

	return true;
}

i3ClassMeta	* i3UITemplate_ButtonImageSet::GetInstanceMeta()
{
	return i3UIButtonImageSet::static_meta();
}