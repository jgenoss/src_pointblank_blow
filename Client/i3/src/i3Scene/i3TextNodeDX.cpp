#include "i3SceneDef.h"
#include "i3TextNodeDX.h"

I3_CLASS_INSTANCE( i3TextNodeDX);

i3TextNodeDX::~i3TextNodeDX(void)
{
	I3_SAFE_RELEASE( m_pTextAttrDX);
	I3_SAFE_RELEASE( m_pGeomery);
}


bool i3TextNodeDX::Create( const char* pszFont, INT32 nTextMaxCount, INT32 nSize, UINT32 nWeight, bool bItalic, DWORD dwQuality)
{
	I3ASSERT( m_pTextAttrDX == nullptr);
	m_pTextAttrDX = i3TextAttrDX::new_object();
	I3ASSERT( m_pTextAttrDX != nullptr);
	if( ! m_pTextAttrDX->Create( pszFont, nTextMaxCount, nSize, nWeight, bItalic, dwQuality) )
	{
		I3TRACE( "%s (%d) : [i3TextNodeDX::Create]\n", __FILE__, __LINE__ );
		I3ASSERT_0;
		return false;
	}

	I3ASSERT( m_pGeomery == nullptr);
	m_pGeomery = i3Geometry::new_object();
	I3ASSERT( m_pGeomery != nullptr);
	AddChild(m_pGeomery);
	m_pGeomery->AppendGeometryAttr(m_pTextAttrDX);

	return true;
}
