#include "stdafx.h"
#include "i3TDKOptSpecs.h"
#include "i3TDKOptSpecGatherUpAttribute.h"
#include "i3TDKOptSpecResizeImage.h"
#include "i3TDKOptSpecConvertDXTTexture.h"
#include "i3TDKOptSpecBSPBuilder.h"
#include "i3TDKOptSpecNodeFlag.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKOptSpecMacroTexture.h"
#include "i3TDKOptSpecSpatialPartition.h"
#include "i3TDKOptSpecChangeCollisionGroup.h"
#include "i3TDKOptSpecMipmapGen.h"
#include "i3TDKOptSpecOctreeBuilder.h"
#include "i3TDKOptSpec_RemoveVertexComponent.h"

I3_EXPORT_TDK
bool	i3TDK::PopupOptimizerSpecDialog( i3SceneOptimizer * pOpt, CWnd * pParentWnd)
{
	i3TDKOptSpecBase * pDlg = nullptr;
	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( g_hInstTDK);

	if( i3::same_of< i3OptGatherUpAttribute*>(pOpt))
	{
		pDlg = new i3TDKOptSpecGatherUpAttribute;
	}
	else if( i3::same_of< i3OptResizeTexture*>(pOpt))
	{
		pDlg = new i3TDKOptSpecResizeImage;
	}
	else if( i3::same_of< i3OptConvertDXTTexture*>(pOpt))
	{
		pDlg = new i3TDKOptSpecConvertDXTTexture;
	}
	else if( i3::same_of< i3OptBSPBuilder*>(pOpt))
	{
		pDlg = new i3TDKOptSpecBSPBuilder;
	}
	else if( i3::same_of< i3OptNodeFlag*>(pOpt))
	{
		pDlg = new i3TDKOptSpecNodeFlag;
	}
	else if( i3::same_of< i3OptMacroTexture*>(pOpt))
	{
		pDlg = new i3TDKOptSpecMacroTexture;
	}
	else if( i3::same_of< i3OptSpatialPartition*>(pOpt))
	{
		pDlg = new i3TDKOptSpecSpatialPartition;
	}
	else if( i3::same_of< i3OptChangeCollisionGroup*>(pOpt))
	{
		pDlg = new i3TDKOptSpecChangeCollisionGroup;
	}
	else if( i3::same_of< i3OptMipmapGen*>(pOpt))
	{
		pDlg = new i3TDKOptSpecMipmapGen;
	}
	else if( i3::same_of< i3OptBuildOcTree*>(pOpt))
	{
		pDlg = new i3TDKOptSpecOctreeBuilder;
	}
	else if( i3::same_of< i3OptRemoveVertexComponent*>(pOpt))
	{
		pDlg = new i3TDKOptSpec_RemoveVertexComponent;
	}

	if( pDlg != nullptr)
	{
		pDlg->SetOptimizer( pOpt);

		if( pDlg->DoModal() == IDCANCEL)
		{
			AfxSetResourceHandle( hOld);

			delete pDlg;
			return false;
		}
		else
		{
			delete pDlg;
		}
	}

	AfxSetResourceHandle( hOld);

	return true;
}