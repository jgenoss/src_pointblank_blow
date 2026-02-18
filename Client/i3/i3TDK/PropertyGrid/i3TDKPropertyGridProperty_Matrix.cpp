#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Matrix.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Matrix, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Matrix::i3TDKPropertyGridProperty_Matrix( PROPERTYINFO* pInfoArg)
:	i3TDKPropertyGridProperty( ToMFCString(pInfoArg->m_pszName), 0, FALSE), m_pData( (MATRIX*)(pInfoArg->m_pData))
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
	
	i3::vector<PROPERTYINFO*> temp;

	AddMFCProperty_Vec4D( temp, &m_pData->vec[0], "row1", pInfoArg->m_pszDesc, m_fMin, m_fMax);		// new
	AddMFCProperty_Vec4D( temp, &m_pData->vec[1], "row2", pInfoArg->m_pszDesc, m_fMin, m_fMax);		// new
	AddMFCProperty_Vec4D( temp, &m_pData->vec[2], "row3", pInfoArg->m_pszDesc, m_fMin, m_fMax);		// new
	AddMFCProperty_Vec4D( temp, &m_pData->vec[3], "row4", pInfoArg->m_pszDesc, m_fMin, m_fMax);		// new
	
	m_pRow1 = new i3TDKPropertyGridProperty_Vec4D( temp[0]);
	m_pRow2 = new i3TDKPropertyGridProperty_Vec4D( temp[1]);
	m_pRow3 = new i3TDKPropertyGridProperty_Vec4D( temp[2]);
	m_pRow4 = new i3TDKPropertyGridProperty_Vec4D( temp[3]);
	
	AddSubItem( m_pRow1);
	AddSubItem( m_pRow2);
	AddSubItem( m_pRow3);
	AddSubItem( m_pRow4);

	for( size_t i=0; i< temp.size(); i++)
	{
		PROPERTYINFO* pInfo = temp[i]; 
		I3MEM_SAFE_FREE( pInfo);
	}

	Expand(FALSE);
	AllowEdit( FALSE);	
}

void i3TDKPropertyGridProperty_Matrix::UpdateFromProperty( void* pData)
{
}

#endif