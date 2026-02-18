#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Vec3D.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Vec3D, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Vec3D::i3TDKPropertyGridProperty_Vec3D( PROPERTYINFO* pInfoArg)
:	i3TDKPropertyGridProperty( ToMFCString(pInfoArg->m_pszName), 0, TRUE),
	m_pData( (VEC3D*)(pInfoArg->m_pData))
{
	m_bHasRange = ((pInfoArg->m_nFlag & PROPERTYFLAG_HASRANGE) != 0) ? true : false;
	
	if( m_bHasRange)
	{
		REAL32* pRange = (REAL32*)(pInfoArg->m_pUserData);
		m_fMin = pRange[0];
		m_fMax = pRange[1];

		delete [] pRange;
	}

	I3ASSERT( m_pData != nullptr);
	
	i3::vector<PROPERTYINFO*> vec3d;

	AddMFCProperty_Real32( vec3d, &(m_pData->x), "x", pInfoArg->m_pszDesc, m_fMin, m_fMax);
	AddMFCProperty_Real32( vec3d, &(m_pData->y), "y", pInfoArg->m_pszDesc, m_fMin, m_fMax);
	AddMFCProperty_Real32( vec3d, &(m_pData->z), "z", pInfoArg->m_pszDesc, m_fMin, m_fMax);

	m_pX = new i3TDKPropertyGridProperty_Real32( vec3d[0]);
	m_pY = new i3TDKPropertyGridProperty_Real32( vec3d[1]);
	m_pZ = new i3TDKPropertyGridProperty_Real32( vec3d[2]);

	AddSubItem( m_pX);
	AddSubItem( m_pY);
	AddSubItem( m_pZ);

	for( size_t i=0; i< vec3d.size(); i++)
	{
		PROPERTYINFO* pInfo = vec3d[i];
		I3MEM_SAFE_FREE( pInfo);
	}

	Expand(FALSE);
}

void i3TDKPropertyGridProperty_Vec3D::UpdateFromProperty( void* pData)
{
}


BOOL i3TDKPropertyGridProperty_Vec3D::OnUpdateValue()
{
	BOOL bResult = CMFCPropertyGridProperty::OnUpdateValue();

	m_pX->UpdateFromProperty();
	m_pY->UpdateFromProperty();
	m_pZ->UpdateFromProperty();

	return bResult;
}

#endif