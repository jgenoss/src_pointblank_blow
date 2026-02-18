#include "PCH.h"
#include "UserMap_Block_Geometry.h"

I3_CLASS_INSTANCE( CUserMap_Block_Geometry, i3ElementBase);

CUserMap_Block_Geometry::CUserMap_Block_Geometry(void)
{
	m_pVtx = NULL;
	m_Count = 0;
	m_pSrcGeo = NULL;
}

CUserMap_Block_Geometry::~CUserMap_Block_Geometry(void)
{
	I3MEM_SAFE_FREE( m_pVtx);
	I3_SAFE_RELEASE( m_pSrcGeo);
}

void CUserMap_Block_Geometry::Create( i3GeometryAttr * pGeo)
{
	I3ASSERT( m_pVtx == NULL);
	I3ASSERT( pGeo != NULL);

	i3VertexArray * pVA = pGeo->GetVertexArray();

	I3ASSERT( pVA->GetFormat()->GetHasNormal());
	I3ASSERT( pVA->GetFormat()->GetTextureCoordSetCount() == 1);
	
	if( pVA->GetFormat()->GetHasTangent() == FALSE)
	{
		I3TRACE( "Tangent Component°¡ ¾øÀ½.\n");
		return;
	}

	I3_REF_CHANGE( m_pSrcGeo, pGeo);

	m_Count = pVA->GetCount();

	m_pVtx = (BLOCKVTX *) i3MemAlloc( sizeof(BLOCKVTX) * m_Count);
	I3ASSERT( m_pVtx != NULL);

	INT32 i;

	pVA->Lock( I3G_LOCK_READONLY);

	for( i = 0; i < m_Count; i++)
	{
		pVA->GetPosition(			i, & m_pVtx[i].m_Pos);
		pVA->GetTextureCoord( 0,	i, & m_pVtx[i].m_Tex);
		pVA->GetNormal(				i, & m_pVtx[i].m_Normal);
		pVA->GetTangent(			i, & m_pVtx[i].m_Tangent);
		pVA->GetBinormal(			i, & m_pVtx[i].m_Binormal);
	}
	
	pVA->Unlock();
}

