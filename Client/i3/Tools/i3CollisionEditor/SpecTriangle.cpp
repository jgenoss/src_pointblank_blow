// SpecTriangle.cpp : implementation file
//

#include "stdafx.h"
#include "i3CollisionEditor.h"
#include "SpecTriangle.h"
#include "Collidee.h"
#include "GlobalVar.h"

// CSpecTriangle dialog

IMPLEMENT_DYNAMIC(CSpecTriangle, i3TDKDialogBase)
CSpecTriangle::CSpecTriangle(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecTriangle::IDD, pParent)
{
}

CSpecTriangle::~CSpecTriangle()
{
}

void CSpecTriangle::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_TERRAIN, m_TerrainCtrl);
	DDX_Control(pDX, IDC_CB_REVERB, m_ReverbCtrl);
	DDX_Control(pDX, IDC_ED_ELASTICITY, m_ElasticityCtrl);
}

void CSpecTriangle::SetObject( i3ElementBase * pObj)
{
	INT32 i, j, selCount = 0;
	UINT8 terrain = 0, reverb = 0;
	INT8	elast = 0;
	bool bSameTerrain, bSameReverb, bSameElast;
	Collidee * pCollidee;
	I3_COLLIDEE_TRI32 * pTri;
	bool bFirst = true;

	bSameTerrain = true;
	bSameReverb = true;
	bSameElast = true;

	for( i = 0; i < g_pProject->getCollideeCount(); i++)
	{
		pCollidee = g_pProject->getCollidee( i);

		if( pCollidee->getCollidee()->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
			continue;
		
		i3CollideeMesh * pMesh = (i3CollideeMesh *) pCollidee->getCollidee();

		for( j = 0; j < (INT32) pMesh->GetTriangleCount(); j++)
		{
			pTri = pMesh->GetTriangle( j);

			if( pTri->pad != 0)
			{
				selCount++;
				if( bFirst)
				{
					bFirst = false;

					terrain = pTri->m_Terrain;
					reverb = pTri->m_Reverb;
					elast = pTri->m_Elasticity;
				}
				else
				{
					if( bSameTerrain == true)
					{
						if( terrain != pTri->m_Terrain)
							bSameTerrain = false;
					}

					if( bSameReverb == true)
					{
						if( reverb != pTri->m_Reverb)
							bSameReverb = false;
					}

					if( bSameElast == true)
					{
						if( elast != pTri->m_Elasticity)
							bSameElast = false;
					}
				}
				
			}
		}
	}

	// Terrain
	if( (selCount > 0) && (bSameTerrain))
	{
		m_TerrainCtrl.SetCurSel( terrain);
	}
	else
	{
		m_TerrainCtrl.SetCurSel( -1);
	}

	// Reverb
	if( (selCount > 0) && (bSameReverb))
	{
		m_ReverbCtrl.SetCurSel( reverb);
	}
	else
	{
		m_ReverbCtrl.SetCurSel( -1);
	}

	// Elasticity
	if( (selCount > 0) && (bSameElast))
	{
		char conv[128];

		i3String::ftoa( elast * 0.007874015748031496062992125984252f, conv);
		m_ElasticityCtrl.SetWindowText( conv);
	}
	else
	{
		m_ElasticityCtrl.SetWindowText( "");
	}
}

BEGIN_MESSAGE_MAP(CSpecTriangle, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_TERRAIN, OnCbnSelchangeCbTerrain)
	ON_CBN_SELCHANGE(IDC_CB_REVERB, OnCbnSelchangeCbReverb)
	ON_EN_KILLFOCUS(IDC_ED_ELASTICITY, OnEnKillfocusEdElasticity)
END_MESSAGE_MAP()


// CSpecTriangle message handlers

void CSpecTriangle::OnCbnSelchangeCbTerrain()
{
	INT32 i, j;
	I3_COLLIDEE_TRI32 * pTri;
	Collidee * pCollidee;
	UINT8 terrain;

	if( m_TerrainCtrl.GetCurSel() == -1)
		return;

	terrain = (UINT8) m_TerrainCtrl.GetCurSel();

	for( i = 0; i < g_pProject->getCollideeCount(); i++)
	{
		pCollidee = g_pProject->getCollidee( i);

		for( j = 0; j < pCollidee->getTriangleCount(); j++)
		{
			pTri = pCollidee->getTriangle( j);

			if( pTri->pad == TRUE)
			{
				pTri->m_Terrain = terrain;
			}
		}
	}
}

void CSpecTriangle::OnCbnSelchangeCbReverb()
{
	INT32 i, j;
	I3_COLLIDEE_TRI32 * pTri;
	Collidee * pCollidee;
	UINT8 reverb;

	if( m_ReverbCtrl.GetCurSel() == -1)
		return;

	reverb  = (UINT8) m_ReverbCtrl.GetCurSel();

	for( i = 0; i < g_pProject->getCollideeCount(); i++)
	{
		pCollidee = g_pProject->getCollidee( i);

		if( pCollidee->getCollidee()->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
			continue;

		i3CollideeMesh * pMesh = (i3CollideeMesh *) pCollidee->getCollidee();

		for( j = 0; j < (INT32) pMesh->GetTriangleCount(); j++)
		{
			pTri = pMesh->GetTriangle( j);

			if( pTri->pad == TRUE)
			{
				pTri->m_Reverb = reverb;
			}
		}
	}
}

void CSpecTriangle::OnEnKillfocusEdElasticity()
{
	INT32 i, j;
	I3_COLLIDEE_TRI32 * pTri;
	Collidee * pCollidee;
	INT8 elast;

	{
		char conv[128];

		m_ElasticityCtrl.GetWindowText( conv, sizeof(conv) - 1);
		elast = (INT8)( atof( conv) * 127);
	}

	for( i = 0; i < g_pProject->getCollideeCount(); i++)
	{
		pCollidee = g_pProject->getCollidee( i);

		if( pCollidee->getCollidee()->IsTypeOf( i3CollideeMesh::GetClassMeta()) == FALSE)
			continue;

		i3CollideeMesh * pMesh = (i3CollideeMesh *) pCollidee->getCollidee();

		for( j = 0; j < (INT32) pMesh->GetTriangleCount(); j++)
		{
			pTri = pMesh->GetTriangle( j);

			if( pTri->pad == TRUE)
			{
				pTri->m_Elasticity = elast;
			}
		}
	}
}

void CSpecTriangle::OnOK()
{
	OnEnKillfocusEdElasticity();
}

void CSpecTriangle::OnCancel()
{
}
