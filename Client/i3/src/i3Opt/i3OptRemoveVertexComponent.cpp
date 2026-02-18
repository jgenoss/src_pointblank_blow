#include "i3OptPCH.h"
#include "i3OptRemoveVertexComponent.h"

//	Property Field Name
#define FIELD_REMOVE_NORMAL		"RemoveNormal"
#define FIELD_REMOVE_COLOR		"RemoveColor"
#define FIELD_REMOVE_TEXCOORD	"RemoveTexcoord"

I3_CLASS_INSTANCE(i3OptRemoveVertexComponent);

i3OptRemoveVertexComponent::i3OptRemoveVertexComponent(void)
{
	setDefaultInfo();

	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
	m_Class = CLASS_GEOMETRY;
}

void i3OptRemoveVertexComponent::setDefaultInfo(void)
{
	m_OptInfo._bRemoveNormal = false;
	m_OptInfo._bRemoveColor = false;
	m_OptInfo._bRemoveTexcoord = false;
	m_OptInfo._bRemoveTangent = false;
	m_OptInfo._bRemoveAllTexcoord = false;
}

i3VertexArray * i3OptRemoveVertexComponent::rebuildVertexArray(i3VertexArray * pSrcVA, i3VertexFormat * pFormat)
{
	i3VertexArray * pNewVA;

	pNewVA = i3VertexArray::new_object();

	//  Create new vertex array
	pNewVA->Create(pFormat, pSrcVA->GetCount(), 0);

	//	Copy the Vertices components
	UINT32		i = 0;

	pSrcVA->Lock(I3G_LOCK_READONLY);
	pNewVA->Lock();

	for (i = 0; i < pSrcVA->GetCount(); i++)
	{
		pNewVA->CopyVertexFrom(i, pSrcVA, i);
	}

	pNewVA->Unlock();
	pSrcVA->Unlock();

	return pNewVA;
}

void i3OptRemoveVertexComponent::copyVertexComponent(i3VertexArray * pDstVA, i3VertexArray * pSrcVA)
{
	if (pDstVA == nullptr || pSrcVA == nullptr) return;
}

void	i3OptRemoveVertexComponent::Trace(i3Node * pRoot)
{
	INT32 i;

	SetTraceRoot(pRoot);

	GatherAllVertexArray(pRoot);

	ReportStart(getVACount());

	// Build new vertex arrays
	for (i = 0; i < getVACount(); i++)
	{
		ReportProgress(i);

		VAINFO * pInfo = getVA(i);
		i3VertexArray * pOldVA = pInfo->m_pOldVA;
		i3VertexFormat fmt;

		fmt = *pOldVA->GetFormat();

		//	Remove normal component in geometry
		if (m_OptInfo._bRemoveNormal && fmt.GetHasNormal())
		{
			fmt.SetHasNormal(false);
			fmt.SetHasTangent(false);
			fmt.SetHasBinormal(false);
		}

		if (m_OptInfo._bRemoveTangent && fmt.GetHasTangent())
		{
			fmt.SetHasTangent(false);
			fmt.SetHasBinormal(false);
		}

		//	Remove color component in geometry
		if (m_OptInfo._bRemoveColor && fmt.GetHasColor())
		{
			fmt.SetHasColor(false);
		}

		//	Remove Texture Coord component in geometry
		if (m_OptInfo._bRemoveAllTexcoord && fmt.GetTextureCoordSetCount() > 0)
		{
			fmt.SetTextureCoordSetCount(0);
			fmt.SetTextureCoordCount(0);
		}
		else
		if( m_OptInfo._bRemoveTexcoord && ( fmt.GetTextureCoordSetCount() > 1))
		{
			fmt.SetTextureCoordSetCount( 1);
		}

		//	Rebuild vertex array
		if( fmt != *pOldVA->GetFormat())
		{
			pInfo->m_pNewVA = rebuildVertexArray( pOldVA, &fmt);
		}
	}

	// Replace all reference of vertex array from i3GeometryAttr
	for( i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pGeoAttr = getGeoAttr( i);

		INT32 idx = FindVA( pGeoAttr->GetVertexArray());
		if( idx == -1)
			continue;
		
		VAINFO * pInfo = getVA( idx);

		if( pInfo->m_pNewVA != nullptr)
			pGeoAttr->SetVertexArray( pInfo->m_pNewVA);
	}

	RemoveAllVA();
	RemoveAllGeo();

	ReportEnd();
}

const char * i3OptRemoveVertexComponent::getDescName(void)
{
	return "Remove vertex components";
}

bool i3OptRemoveVertexComponent::SaveToXML( i3XMLElement * pXML)
{
	pXML->addAttr( "RemoveNormal", m_OptInfo._bRemoveNormal);
	pXML->addAttr( "RemoveColor", m_OptInfo._bRemoveColor);
	pXML->addAttr( "RemoveTexCoord", m_OptInfo._bRemoveTexcoord);
	pXML->addAttr( "RemoveTangent", m_OptInfo._bRemoveTangent);

	pXML->addAttr("RemoveAllTexCoord", m_OptInfo._bRemoveAllTexcoord);


	return true;
}

bool i3OptRemoveVertexComponent::LoadFromXML( i3XMLElement * pXML)
{
	pXML->getAttr( "RemoveNormal", &m_OptInfo._bRemoveNormal);
	pXML->getAttr( "RemoveColor", &m_OptInfo._bRemoveColor);
	pXML->getAttr( "RemoveTexCoord", &m_OptInfo._bRemoveTexcoord);
	pXML->getAttr( "RemoveTangent", &m_OptInfo._bRemoveTangent);

	if (pXML->getAttr("RemoveAllTexCoord", &m_OptInfo._bRemoveAllTexcoord) == nullptr)
	{
		m_OptInfo._bRemoveAllTexcoord = false;
	}

	return true;
}
