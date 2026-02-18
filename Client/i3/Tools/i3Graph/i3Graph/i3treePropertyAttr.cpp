#include "stdafx.h"
#include "i3treeProperty.h"
#include "i3treePropertyBase.h"
#include "PropertyTextureBindType.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"

////////////////////////////////////////////////////////////////////////////
//Attr Base 
void PropertyBoard::SetAttr(void)
{
	//For Property
	I3_P_STRING		* pPropertyString; 		
	
	//For Class Name 0
	pPropertyString = new I3_P_STRING; 
	pPropertyString->SetName("ClassName"); 
	pPropertyString->SetValue(m_pSelObj->GetMeta()->GetClassName()); 
	pPropertyString->AddStyle(I3_READONLY); 
	m_PropertyList.Add((void *)pPropertyString); 
}

BOOL PropertyBoard::ChangeAttr(void)
{
	//For Class name - ReadOnly 0
	return FALSE; 
}

////////////////////////////////////////////////////////////////////////////
//EnableAttr
void PropertyBoard::SetEnableAttr(void)
{
	I3_P_BOOL		* pPropertyBool; 

	i3AbstractBoolAttr * pBoolAttr; 

	//For Enable Attr TRUE or FALSE 1

	pBoolAttr = (i3AbstractBoolAttr *)m_pSelObj; 
	pPropertyBool = new I3_P_BOOL; 
	pPropertyBool->SetName("Enable");
	pPropertyBool->SetValue(pBoolAttr->Get());	
	m_PropertyList.Add((void *)pPropertyBool); 		

}

BOOL PropertyBoard::ChangeEnableAttr(void)
{
	CpropertyBool * pPropertyBool; 	
	i3AbstractBoolAttr * pBoolAttr; 

	//For Enable Attr TRUE or FALSE 1
	if(m_SelectLine  ==  1)
	{
		pPropertyBool = (CpropertyBool *)GetProperty();		
		pBoolAttr = (i3AbstractBoolAttr *)m_pSelObj; 		
		pBoolAttr->Set(pPropertyBool->GetValue()); 
		return TRUE; 
	}

	return FALSE; 
}

////////////////////////////////////////////////////////////////////////////
//MaterialAttr 
void PropertyBoard::SetMaterialAttrList(void)
{
	I3_P_COLORREAL * pPropertyColor; 
	I3_P_REAL32 * pPropertyReal32; 

	//Get MaterialAttrList Property 
	i3MaterialAttr * pMaterialAttr; 
	pMaterialAttr = (i3MaterialAttr *)m_pSelObj; 

	//For Diffuse Color 2
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Diffuse"); 
	pPropertyColor->SetValue(* (pMaterialAttr->GetDiffuse())); 
	m_PropertyList.Add((void *)pPropertyColor);

	//For Specular Color 3
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Specular"); 
	pPropertyColor->SetValue(*(pMaterialAttr->GetSpecular())); 
	m_PropertyList.Add((void *)pPropertyColor);

	//For Emissive Color 4
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Emissive"); 
	pPropertyColor->SetValue(*(pMaterialAttr->GetEmissive())); 
	m_PropertyList.Add((void *)pPropertyColor);

	//For Shininess 5
	pPropertyReal32 = new I3_P_REAL32; 
	pPropertyReal32->SetName("Shininess");
	pPropertyReal32->SetValue(pMaterialAttr->GetShininess());	
	m_PropertyList.Add((void *)pPropertyReal32); 			
}

void PropertyBoard::ChangeMaterialAttrList(void)
{
	i3MaterialAttr * pMaterialAttr; 
	pMaterialAttr = (i3MaterialAttr *)m_pSelObj; 

	I3_P_COLORREAL * pPropertyColor; 
	I3_P_REAL32 * pPropertyReal; 
	
	switch(m_SelectLine)
	{
	case 1: 
		//Diffuse
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pMaterialAttr->SetDiffuse(&(pPropertyColor->GetValue())); 		
		break; 
	case 2: 
		//Specular
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pMaterialAttr->SetSpecular(&(pPropertyColor->GetValue())); 		
		break; 
	case 3: 
		//Emissive
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pMaterialAttr->SetEmissive(&(pPropertyColor->GetValue())); 		
		break; 
	case 4: 
		//Shininess		
		pPropertyReal = (I3_P_REAL32*)GetProperty(); 
		pMaterialAttr->SetShininess( pPropertyReal->GetValue() ); 		
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//Color Attr
void PropertyBoard::SetColorAttrList(void)
{
	I3_P_COLORREAL * pPropertyColor; 

	//Get MaterialAttrList Property 
	i3ColorAttr * pColorAttr; 
	pColorAttr = (i3ColorAttr *)m_pSelObj; 

	//For Ambient Color 1
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Color"); 
	COLORREAL ColorReal;
	i3Color::Set( &ColorReal, i3Color::GetR( pColorAttr->Get()), i3Color::GetG( pColorAttr->Get()), i3Color::GetB( pColorAttr->Get()), i3Color::GetA( pColorAttr->Get())); 
	pPropertyColor->SetValue(ColorReal); 
	m_PropertyList.Add((void *)pPropertyColor);
}

void PropertyBoard::ChangeColorAttrList(void)
{
	i3ColorAttr * pColorAttr; 
	pColorAttr = (i3ColorAttr *)m_pSelObj; 

	I3_P_COLORREAL * pPropertyColor; 

	switch(m_SelectLine)
	{
	case 1: 
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pColorAttr->Set(&(pPropertyColor->GetValue())); 		
		break; 
	}

}

////////////////////////////////////////////////////////////////////////////
//AlphaFucAttr
void PropertyBoard::SetAlphaFuncAttrList()
{
	I3_P_INT * pPropertyInt; 
	I3_P_COMPFUNC * pPropertyCompFunc; 
	i3AlphaFuncAttr * pAlphaFuncAttr; 

	pAlphaFuncAttr = (i3AlphaFuncAttr *)m_pSelObj; 

	//For Function 1
	pPropertyCompFunc = new I3_P_COMPFUNC;
	pPropertyCompFunc->SetName("Function"); 
	pPropertyCompFunc->SetValue(pAlphaFuncAttr->GetFunction()); 
	m_PropertyList.Add((void *)pPropertyCompFunc);

	//For Ref 2
	pPropertyInt = new I3_P_INT; 
	pPropertyInt->SetName("AlphaRef"); 
	pPropertyInt->SetValue( pAlphaFuncAttr->GetRefValue()); 
	m_PropertyList.Add((void *)pPropertyInt);
}

void PropertyBoard::ChangeAlphaFuncAttrList()
{
	i3AlphaFuncAttr * pAlphaFuncAttr; 
	I3_P_INT * pPropertyInt; 
	I3_P_COMPFUNC * pPropertyCompFunc; 

	pAlphaFuncAttr = (i3AlphaFuncAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Function 
		{
			pPropertyCompFunc = (I3_P_COMPFUNC *)GetProperty(); 
			pAlphaFuncAttr->SetFunction(pPropertyCompFunc->GetValue()); 			
		}
		break;	
	case 2: //For Ref
		{
			pPropertyInt = (I3_P_INT *)GetProperty(); 
			pAlphaFuncAttr->SetRefValue(pPropertyInt->GetValue()); 
		}
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//BlendModeAttr
void PropertyBoard::SetBlendModeAttrList()
{
	I3_P_BLEND * pBlend; 
	i3BlendModeAttr * pBlendModeAttr; 
	pBlendModeAttr = (i3BlendModeAttr *)m_pSelObj; 

	//Source		1
	pBlend = new I3_P_BLEND; 
	pBlend->SetName("Source"); 
	pBlend->SetValue(pBlendModeAttr->GetSource()); 
	m_PropertyList.Add((void *)pBlend);

	//Destination	2
	pBlend = new I3_P_BLEND; 
	pBlend->SetName("Destination"); 
	pBlend->SetValue(pBlendModeAttr->GetDestination()); 
	m_PropertyList.Add((void *)pBlend);

}
void PropertyBoard::ChangeBlendModeAttrList()
{
	I3_P_BLEND * pBlend; 
	i3BlendModeAttr * pBlendModeAttr; 
	pBlendModeAttr = (i3BlendModeAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Source 
		{
			pBlend = (I3_P_BLEND *)GetProperty(); 
			pBlendModeAttr->SetSource(pBlend->GetValue()); 			
		}
		break;	
	case 2: //For Destination
		{
			pBlend = (I3_P_BLEND *)GetProperty(); 
			pBlendModeAttr->SetDestination(pBlend->GetValue());             
		}
		break; 
	}

}

////////////////////////////////////////////////////////////////////////////
//FaceCullModeAttr
void PropertyBoard::SetFaceCullModeAttrList()
{
	I3_P_FACECULL * pFaceCull;
	i3FaceCullModeAttr * pFaceCullModeAttr; 
	pFaceCullModeAttr = (i3FaceCullModeAttr *)m_pSelObj; 

	//Mode			1
	pFaceCull = new I3_P_FACECULL; 
	pFaceCull->SetName("FaceCullMode"); 
	pFaceCull->SetValue(pFaceCullModeAttr->Get()); 
	m_PropertyList.Add((void *)pFaceCull);
}

void PropertyBoard::ChangeFaceCullModeAttrList()
{
	I3_P_FACECULL * pFaceCull;
	i3FaceCullModeAttr * pFaceCullModeAttr; 
	pFaceCullModeAttr = (i3FaceCullModeAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1:				//Mode
		{
			pFaceCull = (I3_P_FACECULL *)GetProperty(); 
			pFaceCullModeAttr->Set(pFaceCull->GetValue());             
		}
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//FogColorAttr
void PropertyBoard::SetFogColorAttrList()
{
	I3_P_COLOR * pPropertyColor; 
	i3FogColorAttr * pFogColorAttr; 
	pFogColorAttr = (i3FogColorAttr *)m_pSelObj; 

	//Color		1
	pPropertyColor = new I3_P_COLOR; 
	pPropertyColor->SetName("Fog Color"); 
	pPropertyColor->SetValue( *(pFogColorAttr->Get()) ); 
	m_PropertyList.Add((void *)pPropertyColor);
//*/
}

void PropertyBoard::ChangeFogColorAttrList()
{
	i3FogColorAttr * pFogColorAttr; 
	I3_P_COLOR	* pPropertyColor; 
	pFogColorAttr = (i3FogColorAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1:		//Color
		{
			pPropertyColor = (I3_P_COLOR *)GetProperty(); 
			pFogColorAttr->Set(&(pPropertyColor->GetValue()));
		}
		break; 
	}
//*/
}

////////////////////////////////////////////////////////////////////////////
//FogDensityAttr
void PropertyBoard::SetFogDensityAttrList()
{
	i3FogDensityAttr * pFogDensityAttr; 
	I3_P_REAL32 * pPropertyReal; 
	pFogDensityAttr = (i3FogDensityAttr *)m_pSelObj; 

	//For Density					1	
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("");
	pPropertyReal->SetValue(pFogDensityAttr->Get()); 
	m_PropertyList.Add((void *)pPropertyReal);
}

void PropertyBoard::ChangeFogDensityAttrList()
{
	i3FogDensityAttr * pFogDensityAttr; 
	I3_P_REAL32 * pPropertyReal; 
	pFogDensityAttr = (i3FogDensityAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1:		//Density
		{
			pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
			pFogDensityAttr->Set( pPropertyReal->GetValue() );
		}
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//FogModeAttr
void PropertyBoard::SetFogModeAttrList()
{
	I3_P_FOGMODE * pFogMode; 
	i3FogModeAttr * pFogModeAttr;
	pFogModeAttr = (i3FogModeAttr *)m_pSelObj; 

	//For Fog Mode			1
	pFogMode = new I3_P_FOGMODE; 
	pFogMode->SetName("FogMode"); 
	pFogMode->SetValue(pFogModeAttr->Get()); 
	m_PropertyList.Add((void *)pFogMode);
}

void PropertyBoard::ChangeFogModeAttrList()
{
	I3_P_FOGMODE * pFogMode; 
	i3FogModeAttr * pFogModeAttr;
	pFogModeAttr = (i3FogModeAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1:		//FogMode
		{
			pFogMode = (I3_P_FOGMODE *)GetProperty(); 
			pFogModeAttr->Set(pFogMode->GetValue());
		}
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//FogNearFarAttr
void PropertyBoard::SetFogNearFarAttrList()
{
	I3_P_REAL32 * pPropertyReal; 
	i3FogNearFarAttr * pFogNearFarAttr; 
	pFogNearFarAttr = (i3FogNearFarAttr *)m_pSelObj; 

	//For Near				1
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("Near"); 
	pPropertyReal->SetValue(pFogNearFarAttr->GetNear()); 
	m_PropertyList.Add((void *)pPropertyReal);

	//For Far				2
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("Far"); 
	pPropertyReal->SetValue(pFogNearFarAttr->GetFar()); 
	m_PropertyList.Add((void *)pPropertyReal);
}

void PropertyBoard::ChangeFogNearFarAttrList()
{
	I3_P_REAL32 * pPropertyReal; 
	i3FogNearFarAttr * pFogNearFarAttr; 
	pFogNearFarAttr = (i3FogNearFarAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Near		
		pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
		pFogNearFarAttr->SetNear(pPropertyReal->GetValue());
		break; 
	case 2: //For Far
		pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
		pFogNearFarAttr->SetFar(pPropertyReal->GetValue());
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//GeometryAttr
void PropertyBoard::SetGeometryAttrList()
{
	//For Attr //For Proeprtys 
	i3GeometryAttr * pGeometryAttr; 
	pGeometryAttr = (i3GeometryAttr *)m_pSelObj;     
	I3_P_INT		* pInt; 
	I3_P_PRIMITIVE	* pPrimitive; 
	I3_P_GEOMETRY	* pGeometry;

	//For Primitive Type		1
	pPrimitive = new I3_P_PRIMITIVE; 
	pPrimitive->SetName("Primitve Type"); 
	pPrimitive->AddStyle(I3_READONLY); 
	pPrimitive->SetValue(pGeometryAttr->GetPrimitiveType()); 
	m_PropertyList.Add((void *)pPrimitive);

	//For Primitive Count		2
	pInt = new I3_P_INT; 
	pInt->SetName("Primitive Count"); 
	pInt->AddStyle(I3_READONLY); 
	pInt->SetValue(pGeometryAttr->GetPrimitiveCount()); 
	m_PropertyList.Add((void *)pInt);

	//For Geometry Dialog
	pGeometry = new I3_P_GEOMETRY;
	pGeometry->m_pGeometry = pGeometryAttr; 
	pGeometry->SetName("GeometryInfo"); 
	m_PropertyList.Add((void *)pGeometry);

}

void PropertyBoard::ChangeGeometryAttrList()
{
	i3GeometryAttr * pGeometryAttr; 
	pGeometryAttr = (i3GeometryAttr *)m_pSelObj; 
}

////////////////////////////////////////////////////////////////////////////
//LightAttr
void PropertyBoard::SetLightAttrList()
{
	i3LightAttr * pLightAttr; 
	pLightAttr = (i3LightAttr *)m_pSelObj; 	

	I3_P_LIGHTTYPE	* pPropertyLightType; 
	I3_P_COLORREAL	* pPropertyColor; 
	I3_P_REAL32		* pPropertyReal; 

	//For Light Type		1
	pPropertyLightType = new I3_P_LIGHTTYPE; 
	pPropertyLightType->SetName("Type"); 
	pPropertyLightType->SetValue(pLightAttr->getLightType()); 
	m_PropertyList.Add((void *)pPropertyLightType);

	//For Diffuse Color		3
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Diffuse"); 
	pPropertyColor->SetValue(* (pLightAttr->getDiffuse())); 
	m_PropertyList.Add((void *)pPropertyColor);

	//For Specular Color	4
	pPropertyColor = new I3_P_COLORREAL; 
	pPropertyColor->SetName("Specular"); 
	pPropertyColor->SetValue(*(pLightAttr->getSpecular())); 
	m_PropertyList.Add((void *)pPropertyColor);

	//For CutoffAngle		5
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("InnerAngle"); 
	pPropertyReal->SetValue(pLightAttr->getInnerAngle()); 
	m_PropertyList.Add((void *)pPropertyReal);

	//For SpotLightAngle	6
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("OuterAngle"); 
	pPropertyReal->SetValue(pLightAttr->getOuterAngle()); 
	m_PropertyList.Add((void *)pPropertyReal);

	//For Range				7
	pPropertyReal = new I3_P_REAL32; 
	pPropertyReal->SetName("Range"); 
	pPropertyReal->SetValue(pLightAttr->getRange()); 
	m_PropertyList.Add((void *)pPropertyReal);

}

void PropertyBoard::ChangeLightAttrList()
{
	i3LightAttr * pLightAttr; 
	pLightAttr = (i3LightAttr *)m_pSelObj; 

	I3_P_LIGHTTYPE	* pPropertyLightType; 
	I3_P_COLORREAL	* pPropertyColor; 
	I3_P_REAL32		* pPropertyReal; 

	switch(m_SelectLine)
	{
	case 1: //For Light Type 
		pPropertyLightType = (I3_P_LIGHTTYPE *)GetProperty(); 
		pLightAttr->setLightType( pPropertyLightType->GetValue()); 
		break; 
	case 2: 
		//Diffuse
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pLightAttr->setDiffuse(&(pPropertyColor->GetValue())); 		
		break; 
	case 3: 
		//Specular
		pPropertyColor = (I3_P_COLORREAL *)GetProperty();		
		pLightAttr->setSpecular(&(pPropertyColor->GetValue())); 		
		break; 
	case 4: 
		//For Inner-Angle		
		pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
		pLightAttr->setInnerAngle( pPropertyReal->GetValue()); 
		break; 
	case 5: 
		//For Outer-Angle	
		pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
		pLightAttr->setOuterAngle( pPropertyReal->GetValue()); 
		break; 
	case 6: 
		//For Range				
		pPropertyReal = (I3_P_REAL32 *)GetProperty(); 
		pLightAttr->setRange( pPropertyReal->GetValue()); 
		break; 
	}
}


////////////////////////////////////////////////////////////////////////////
//void SetTextureFilterAttrList(void); 
//	void ChangeTextureFilterAttrList(void); 

void PropertyBoard::SetTextureFilterAttrList(void)
{
	I3_P_TEXTUREFILTER  * pPropertyTexture; 
	i3TextureFilterAttr * pTextureFilterAttr; 
	pTextureFilterAttr = (i3TextureFilterAttr *)m_pSelObj; 

	//For Mag				1
	pPropertyTexture = new I3_P_TEXTUREFILTER; 
	pPropertyTexture->SetName("MagFilter"); 
	pPropertyTexture->SetValue(pTextureFilterAttr->GetMagFilter()); 
	m_PropertyList.Add((void *)pPropertyTexture);

	//For Min				2				
	pPropertyTexture = new I3_P_TEXTUREFILTER; 
	pPropertyTexture->SetName("MinFilter"); 
	pPropertyTexture->SetValue(pTextureFilterAttr->GetMinFilter()); 
	m_PropertyList.Add((void *)pPropertyTexture);

}

void PropertyBoard::ChangeTextureFilterAttrList(void)
{
	I3_P_TEXTUREFILTER  * pPropertyTexture; 
	i3TextureFilterAttr * pTextureFilterAttr; 
	pTextureFilterAttr = (i3TextureFilterAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Mag
		pPropertyTexture = (I3_P_TEXTUREFILTER *)GetProperty(); 
		pTextureFilterAttr->SetMagFilter(pPropertyTexture->GetValue());
		break; 
	case 2: //For Near
		pPropertyTexture = (I3_P_TEXTUREFILTER *)GetProperty(); 
		pTextureFilterAttr->SetMinFilter(pPropertyTexture->GetValue());
		break; 
	}
}

void PropertyBoard::SetTextureWrapAttrList(void)
{
	I3_P_TEXTUREWRAP  * pPropertyTexture; 
	i3TextureWrapAttr * pTextureWrapAttr; 
	pTextureWrapAttr = (i3TextureWrapAttr *)m_pSelObj; 

	//For hor				1
	pPropertyTexture = new I3_P_TEXTUREWRAP; 
	pPropertyTexture->SetName("Horz"); 
	pPropertyTexture->SetValue(pTextureWrapAttr->GetHorzWrap()); 
	m_PropertyList.Add((void *)pPropertyTexture);

	//For Ver				2				
	pPropertyTexture = new I3_P_TEXTUREWRAP; 
	pPropertyTexture->SetName("Ver"); 
	pPropertyTexture->SetValue(pTextureWrapAttr->GetVertWrap()); 
	m_PropertyList.Add((void *)pPropertyTexture);


}
void PropertyBoard::ChangeTextureWrapAttrList(void)
{
	I3_P_TEXTUREWRAP  * pPropertyTexture; 
	i3TextureWrapAttr * pTextureWrapAttr; 
	pTextureWrapAttr = (i3TextureWrapAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Mag
		pPropertyTexture = (I3_P_TEXTUREWRAP *)GetProperty(); 
		pTextureWrapAttr->SetHorzWrap(pPropertyTexture->GetValue());
		break; 
	case 2: //For Near
		pPropertyTexture = (I3_P_TEXTUREWRAP *)GetProperty(); 
		pTextureWrapAttr->SetVertWrap(pPropertyTexture->GetValue());
		break; 
	}

}

void PropertyBoard::SetZfuncAttrList(void)
{
	I3_P_COMPFUNC  * pPropertyZFunc; 
	i3ZFuncAttr * pZFuncAttr; 
	pZFuncAttr = (i3ZFuncAttr *)m_pSelObj; 

	//For Func				1
	pPropertyZFunc = new I3_P_COMPFUNC; 
	pPropertyZFunc->SetName("Func"); 
	pPropertyZFunc->SetValue(pZFuncAttr->Get()); 
	m_PropertyList.Add((void *)pPropertyZFunc);
}

void PropertyBoard::ChangeZfuncAttrList(void)
{
	I3_P_COMPFUNC  * pPropertyZFunc; 
	i3ZFuncAttr * pZFuncAttr; 
	pZFuncAttr = (i3ZFuncAttr *)m_pSelObj; 
	switch(m_SelectLine)
	{
	case 1: //For Func
		pPropertyZFunc = (I3_P_COMPFUNC *)GetProperty(); 
		pZFuncAttr->Set(pPropertyZFunc->GetValue());
		break; 
	}
}

void PropertyBoard::SetShadeModeList(void)
{
	I3_P_SHADEMODE * pPropertyShadeMode; 
	i3ShadeModeAttr * pShademode; 
	pShademode = (i3ShadeModeAttr *)m_pSelObj; 

	//For Shade Mode		1
	pPropertyShadeMode = new I3_P_SHADEMODE; 
	pPropertyShadeMode->SetName("ShadeMode"); 
	pPropertyShadeMode->SetValue(pShademode->Get()); 
	m_PropertyList.Add((void *)pPropertyShadeMode);
}

void PropertyBoard::ChangeShadeModeList(void)
{
	I3_P_SHADEMODE * pPropertyShadeMode; 
	i3ShadeModeAttr * pShademode; 
	pShademode = (i3ShadeModeAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Shade Mode
		pPropertyShadeMode = (I3_P_SHADEMODE *)GetProperty(); 
		pShademode->Set(pPropertyShadeMode->GetValue());
		break; 
	}

}

void PropertyBoard::SetRenderModeList(void)
{
	I3_P_RENDERMODE * pPropertyRender; 
	i3RenderModeAttr * pRenderModeAttr; 
	pRenderModeAttr = (i3RenderModeAttr *) m_pSelObj; 

	//For RenderMode			1
	pPropertyRender = new I3_P_RENDERMODE; 
	pPropertyRender->SetName("RenderMode"); 
	pPropertyRender->SetValue(pRenderModeAttr->Get()); 
	m_PropertyList.Add((void *)pPropertyRender); 
}

void PropertyBoard::ChangeRenderModeList(void)
{
	I3_P_RENDERMODE * pPropertyRender; 
	i3RenderModeAttr * pRenderModeAttr; 
	pRenderModeAttr = (i3RenderModeAttr *) m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1: //For Shade Mode
		pPropertyRender = (I3_P_RENDERMODE *)GetProperty(); 
		pRenderModeAttr->Set(pPropertyRender->GetValue());
		break; 
	}

}
void PropertyBoard::SetTextureBindList(void)
{
	i3TextureBindAttr * pTextureBindAttr; 
	i3Texture *			pTexture; 
	I3_P_INT *			pInt; 
	I3_P_STRING *		pString;
	I3_P_IMAGE *		pImage; 
	I3_P_IMAGEFORMAT *	pImageFormat; 
	pTextureBindAttr = (i3TextureBindAttr *) m_pSelObj; 
	pTexture = pTextureBindAttr->GetTexture(); 
	
	{
		CPropertyTextureBindType * pProp = new CPropertyTextureBindType;

		pProp->SetName( "Bind Type");
		pProp->SetValue( pTextureBindAttr->GetBindType());
		m_PropertyList.Add( pProp);
	}
	
	if( pTexture != NULL)
	{
		//UINT32				GetWidth(void)							{ return m_Width; }
		pInt = new I3_P_INT; 
		pInt->SetName("Width"); 
		pInt->SetValue(pTexture->GetWidth()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetHeight(void)							{ return m_Height; }
		pInt = new I3_P_INT; 
		pInt->SetName("Height"); 
		pInt->SetValue(pTexture->GetHeight()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//I3G_IMAGE_FORMAT	GetFormat(void)							{ return m_Format; }
		pImageFormat = new I3_P_IMAGEFORMAT; 
		pImageFormat->SetName("ImageFormat"); 
		pImageFormat->SetValue(pTexture->GetFormat()); 
		pImageFormat->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pImageFormat); 

		//UINT32				GetLevelCount(void)						{ return m_LevelCount; }
		pInt = new I3_P_INT; 
		pInt->SetName("LevelCount"); 
		pInt->SetValue(pTexture->GetLevelCount()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetBitsPerPixel(void)					{ return m_BitsPerPixel; }
		pInt = new I3_P_INT; 
		pInt->SetName("BitsPerPixel"); 
		pInt->SetValue(i3Gfx::GetImageFormatBitCount( pTexture->GetPersistFormat()));
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetStride(void)							{ return m_Stride; }
		pInt = new I3_P_INT; 
		pInt->SetName("Stride"); 
		pInt->SetValue(pTexture->GetStride()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetPitch(void)							{ return m_Pitch; }
		pInt = new I3_P_INT; 
		pInt->SetName("Pitch"); 
		pInt->SetValue(pTexture->GetPitch()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetDataSize(void)						{ return m_DataSize; }
		pInt = new I3_P_INT; 
		pInt->SetName("DataSize"); 
		pInt->SetValue(pTexture->GetDataSize()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		if( pTexture->hasName())
		{
			char szPath[MAX_PATH];
			BOOL bRelative = g_pMainWnd->GetRelativePath( szPath, pTexture->GetName() );

			// External Image
			pString = new I3_P_STRING; 
			if( bRelative )
			{
				pString->SetName("Relative Path"); 
			}
			else
			{
				pString->SetName("Absolute Path"); 
			}

			pString->SetValue( szPath ); 
			pString->AddStyle( I3_READONLY ); 
			m_PropertyList.Add( pString ); 
		}
	}

	pImage = new I3_P_IMAGE; 
	pImage->SetTexture( pTexture );
	pImage->SetName("Image"); 
	m_PropertyList.Add((void*)pImage); 
}

void PropertyBoard::ChangeTextureBindList(void)
{
	I3_P_IMAGE*	pImage;
	i3TextureBindAttr * pTextureBindAttr; 
	pTextureBindAttr = (i3TextureBindAttr *) m_pSelObj; 
	
	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );
	if( _stricmp( "Image", (char*)pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pTextureBindAttr->GetTexture())//변경사항이 있다면
		{
			pTextureBindAttr->SetTexture(pImage->GetTexture());
			SetBoard( m_pSelObj );
		}
	}
	else if( i3String::Compare( pProp->GetName(), "Bind Type") == 0)
	{
		CPropertyTextureBindType * pTexBindProp = (CPropertyTextureBindType *) pProp;

		pTextureBindAttr->SetBindType( pTexBindProp->GetValue());
	}
}


////////////////////////////////////////////////////////////////////////////
//For TextureMatrixAttrList
void PropertyBoard::SetTextureMatrixAttrList(void)
{
	I3_P_VEC4D	* pVec; 
	I3_P_BOOL	* pBool; 
	I3_P_INT	* pInt;
	i3TextureMatrixAttr * TextureMatrix; 
	MATRIX	* pMatrix; 
	VEC4D	Vec; 
	TextureMatrix = (i3TextureMatrixAttr *)m_pSelObj; 
	pMatrix = TextureMatrix->GetMatrix();

	//For Right			1
	pVec = new I3_P_VEC4D;
	pVec->SetName("1"); 	
	i3Matrix::GetRow(pMatrix, 0, &Vec); 
	pVec->SetValue(Vec); 
	m_PropertyList.Add((void *)pVec); 
	//For Up			2
	pVec = new I3_P_VEC4D; 
	pVec->SetName("2"); 	
	i3Matrix::GetRow(pMatrix, 1, &Vec); 
	pVec->SetValue(Vec); 
	m_PropertyList.Add((void *)pVec); 

	//For At			3
	pVec = new I3_P_VEC4D; 
	pVec->SetName("3"); 	
	i3Matrix::GetRow(pMatrix, 2, &Vec); 
	pVec->SetValue(Vec); 
	m_PropertyList.Add((void *)pVec); 

	//For Position		4
	pVec = new I3_P_VEC4D; 
	pVec->SetName("4"); 	
	i3Matrix::GetRow(pMatrix, 3, &Vec); 
	pVec->SetValue(Vec); 
	m_PropertyList.Add((void *)pVec); 
		
	//For Projective	5
	pBool = new I3_P_BOOL; 
	pBool->SetName("IsProjective"); 
	pBool->SetValue(TextureMatrix->IsProjective()); 
	m_PropertyList.Add((void *)pBool); 

	//For Out Element Count	6
	pInt = new I3_P_INT; 
	pInt->SetName("OutputElementCount"); 
	pInt->SetValue(TextureMatrix->GetOutputElementCount()); 
	m_PropertyList.Add((void *)pInt); 

	//For Bind Type		7
	{
		CPropertyTextureBindType * pProp;

		pProp = new CPropertyTextureBindType;
		pProp->SetName("Bind Type"); 
		pProp->SetValue( TextureMatrix->GetBindType()); 
		m_PropertyList.Add((void *)pProp); 
	}
}

void PropertyBoard::ChangeTextureMatrixAttrList(void)
{
	I3_P_VEC4D	*	pVec; 
	I3_P_BOOL	*	pBool; 
	I3_P_INT	*	pInt; 
	MATRIX		*	pMatrix; 
	VEC4D			Vec; 
	i3TextureMatrixAttr * TextureMatrix; 

	TextureMatrix = (i3TextureMatrixAttr *) m_pSelObj; 
	pMatrix = TextureMatrix->GetMatrix();
	switch(m_SelectLine)
	{
	case 1:				//For Right
		pVec = (I3_P_VEC4D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	case 2:				//For Up
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break;	
	case 3:				//For At
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	case 4:				//For Pos
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	case 5:				//For Projective
		pBool = (I3_P_BOOL *)GetProperty(); 
		TextureMatrix->SetProjectiveState( pBool->GetValue() ); 
		break; 
	case 6:				//For Out Element Count
		pInt = (I3_P_INT *)GetProperty(); 
		TextureMatrix->SetOutputElementCount( pInt->GetValue() ); 
		break; 
	case 7:				//For Stage Index
		{
			CPropertyTextureBindType * pProp = (CPropertyTextureBindType *) GetProperty();
		
			TextureMatrix->SetBindType( pProp->GetValue());
		}
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//For TextureCoordsourceAttrList
void PropertyBoard::SetTextureCoordsourceAttrList(void)
{
	i3TextureCoordSourceAttr	* TextureCoord; 
	I3_P_TEXTURE_COORDSOURCE	* pCoordSource; 
	TextureCoord = (i3TextureCoordSourceAttr *) m_pSelObj; 

	//For Stage Index 1
	{
		CPropertyTextureBindType * pProp;

		pProp = new CPropertyTextureBindType;
		pProp->SetName("Bind Type"); 
		pProp->SetValue( TextureCoord->GetBindType()); 
		m_PropertyList.Add( pProp); 
	}

	//For Mode		2	
	pCoordSource = new I3_P_TEXTURE_COORDSOURCE; 
	pCoordSource->SetName("Mode"); 
	pCoordSource->SetValue(TextureCoord->GetMode()); 
	m_PropertyList.Add((void *)pCoordSource); 


}

void PropertyBoard::ChangeTextureCoordsourceAttrList(void)
{
	I3_P_TEXTURE_COORDSOURCE	* pCoordSource;

	i3TextureCoordSourceAttr	* TextureCoord; 
	TextureCoord = (i3TextureCoordSourceAttr *) m_pSelObj; 

	switch(m_SelectLine)
	{			
	case 1:		//For BindType			
		{
			CPropertyTextureBindType * pProp = (CPropertyTextureBindType *) GetProperty();
		
			TextureCoord->SetBindType( pProp->GetValue());
		}
		break; 
	case 2:		//For Mode
		pCoordSource = (I3_P_TEXTURE_COORDSOURCE *)GetProperty(); 
		TextureCoord->SetMode(pCoordSource->GetValue());         
		break; 
	}
}

////////////////////////////////////////////////////////////////////////////
//For TextureFuctionAttrList
void PropertyBoard::SetTextureFuctionAttrList(void)
{
	I3_P_TEXTURE_FUNCTION	*	pTextureFunction; 
	i3TextureFunctionAttr	*	TextureFuction; 
	
	TextureFuction = (i3TextureFunctionAttr *)m_pSelObj; 

	//For Stage Index		1
	{
		CPropertyTextureBindType * pProp;

		pProp = new CPropertyTextureBindType;
		pProp->SetName("Bind Type"); 
		pProp->SetValue( TextureFuction->GetBindType()); 
		m_PropertyList.Add( pProp); 
	}

	//For Texture Function	2
	pTextureFunction = new I3_P_TEXTURE_FUNCTION; 
	pTextureFunction->SetName("Function"); 
	pTextureFunction->SetValue(TextureFuction->GetFunction()); 
	m_PropertyList.Add((void *)pTextureFunction);
}

void PropertyBoard::ChangeTextureFuctionAttrList(void)
{
	I3_P_TEXTURE_FUNCTION	*	pTextureFunction; 
	i3TextureFunctionAttr * TextureFuction; 
	TextureFuction = (i3TextureFunctionAttr *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 1:		//For BindType
		{
			CPropertyTextureBindType * pProp = (CPropertyTextureBindType *) GetProperty();
		
			TextureFuction->SetBindType( pProp->GetValue());
		}
		break; 
	case 2: 
		pTextureFunction = (I3_P_TEXTURE_FUNCTION *)GetProperty(); 
		TextureFuction->SetFunction(pTextureFunction->GetValue());         
		break; 
	}

}

////////////////////////////////////////////////////////////////////////////
// i3TextureEnableAttr
void PropertyBoard::SetTextureEnableAttr(void)
{
	I3_P_BOOL		* pPropertyBool; 
	i3TextureEnableAttr * pAttr; 

	//For Enable Attr TRUE or FALSE 1
	pAttr = (i3TextureEnableAttr *)m_pSelObj;

	pPropertyBool = new I3_P_BOOL; 
	pPropertyBool->SetName("Enable");
	pPropertyBool->SetValue( pAttr->Get());	
	m_PropertyList.Add((void *)pPropertyBool); 		

}

void PropertyBoard::ChangeTextureEnableAttr(void)
{
	CpropertyBool * pPropertyBool; 	
	i3TextureEnableAttr * pAttr = (i3TextureEnableAttr *) m_pSelObj;

	switch( m_SelectLine)
	{
		case 1 :
			{
				pPropertyBool = (CpropertyBool *)GetProperty();		
				
				pAttr->Set( pPropertyBool->GetValue()); 
			}
			break;
	}
}

////////////////////////////////////////////////////////////////////////////
//For ProjectMatrixAttrList
void PropertyBoard::SetProjectMatrixAttrList(void)
{
	i3ProjectMatrixAttr * ProjectMatrix; 
	MATRIX	* pMatrix; 
	ProjectMatrix = (i3ProjectMatrixAttr *)m_pSelObj; 
	pMatrix = ProjectMatrix->GetMatrix();

	InsertMatrixInfo(pMatrix);
}

void PropertyBoard::ChangeProjectMatrixAttrList(void)
{
	MATRIX		*	pMatrix; 
	i3ProjectMatrixAttr * ProjectMatrix; 

	ProjectMatrix = (i3ProjectMatrixAttr *) m_pSelObj; 
	pMatrix = ProjectMatrix->GetMatrix();

	ChangeMatrixValue(pMatrix,m_SelectLine);
}


////////////////////////////////////////////////////////////////////////////
//For SetViewMatrixAttrList
void PropertyBoard::SetViewMatrixAttrList(void)
{
	i3ViewMatrixAttr * ProjectMatrix; 
	MATRIX	* pMatrix; 

	ProjectMatrix = (i3ViewMatrixAttr *)m_pSelObj; 
	pMatrix = ProjectMatrix->GetMatrix();

	InsertMatrixInfo(pMatrix);
}

void PropertyBoard::ChangeViewMatrixAttrList(void)
{
	MATRIX		*	pMatrix; 
	i3ViewMatrixAttr * ProjectMatrix; 

	ProjectMatrix = (i3ViewMatrixAttr *) m_pSelObj; 
	pMatrix = ProjectMatrix->GetMatrix();

	ChangeMatrixValue(pMatrix,m_SelectLine);
}

//2005.01.05 raja
////////////////////////////////////////////////////////////////////////////
//For i3_stencinFuncAttr
void PropertyBoard::SetStencilFuncAttrList(void)
{
	I3_P_COMPFUNC*			m_pStencilFunc;
	I3_P_INT*				m_StencilRef;
	I3_P_INT*				m_ReadMask;
	I3_P_INT*				m_WriteMask;

	i3StencilFuncAttr*		m_pStencilFuncAttr;
	m_pStencilFuncAttr = (i3StencilFuncAttr*)m_pSelObj;

	//stencil-compfunc
	m_pStencilFunc = new I3_P_COMPFUNC;
	m_pStencilFunc->SetName("StencilCompFunc");
	m_pStencilFunc->SetValue(m_pStencilFuncAttr->GetFunction());
	m_PropertyList.Add((void*)m_pStencilFunc);

	//stencil - ref
	m_StencilRef = new I3_P_INT;
	m_StencilRef->SetName("stencilRef");
	m_StencilRef->SetValue((INT32)m_pStencilFuncAttr->GetRefValue());
	m_PropertyList.Add((void*)m_StencilRef);

	//stencil - readmask
	m_ReadMask = new I3_P_INT;
	m_ReadMask->SetName("ReadMask");
	m_ReadMask->SetValue((INT32)m_pStencilFuncAttr->GetReadMask());
	m_PropertyList.Add((void*)m_ReadMask);

	//stencil - writemask
	m_WriteMask = new I3_P_INT;
	m_WriteMask->SetName("WriteMask");
	m_WriteMask->SetValue((INT32)m_pStencilFuncAttr->GetWriteMask());
	m_PropertyList.Add((void*)m_WriteMask);
}

void PropertyBoard::ChangeStencilFuncAttrList(void)
{
	I3_P_INT *				pInt;
	INT32					TempInt;
	I3_P_COMPFUNC*			pCompFunc;

	i3StencilFuncAttr*		m_pStencilFuncAttr;
	m_pStencilFuncAttr = (i3StencilFuncAttr*)m_pSelObj;

	switch(m_SelectLine)
	{
	case	1:	//comp_func
		pCompFunc = (I3_P_COMPFUNC*)GetProperty();
		m_pStencilFuncAttr->SetFunction(pCompFunc->GetValue());
		break;
		
	case	2:	//stencil ref
		pInt = (I3_P_INT*)GetProperty();
		TempInt = pInt->GetValue();
		m_pStencilFuncAttr->SetRefValue((UINT8)TempInt);
		break;
		
	case	3:	//readmask
		pInt = (I3_P_INT*)GetProperty();
		TempInt = pInt->GetValue();
		m_pStencilFuncAttr->SetReadMask((UINT8)TempInt);
		break;
		
	case	4:	//writemask
		pInt = (I3_P_INT*)GetProperty();
		TempInt = pInt->GetValue();
		m_pStencilFuncAttr->SetWriteMask((UINT8)TempInt);
		break;	
	}
}


////////////////////////////////////////////////////////////////////////////
//for i3StencilOperationAttr
void PropertyBoard::SetStencilOPAttrList(void)
{
	I3_P_STENCIL_OP*			pStencilOPSPass;
	I3_P_STENCIL_OP*			pStencilOPSFail;
	I3_P_STENCIL_OP*			pStencilOPZFail;
	
	i3StencilOperationAttr*		pStencilOPAttr;
	pStencilOPAttr = (i3StencilOperationAttr*)m_pSelObj;

	//stencilop_SPass
	pStencilOPSPass = new I3_P_STENCIL_OP;
	pStencilOPSPass->SetName("STENCIL_OP_SPASS");
	pStencilOPSPass->SetValue(pStencilOPAttr->GetSPass());
	m_PropertyList.Add((void *)pStencilOPSPass);

	//stencilop_SFail
	pStencilOPSFail = new I3_P_STENCIL_OP;
	pStencilOPSFail->SetName("STENCIL_OP_SFAIL");
	pStencilOPSFail->SetValue(pStencilOPAttr->GetSFail());
	m_PropertyList.Add((void *)pStencilOPSFail);

	//stencilop_ZFail
	pStencilOPZFail = new I3_P_STENCIL_OP;
	pStencilOPZFail->SetName("STENCIL_OP_ZFail");
	pStencilOPZFail->SetValue(pStencilOPAttr->GetZFail());
	m_PropertyList.Add((void *)pStencilOPZFail);
}

void PropertyBoard::ChangeStencilOPAttrList(void)
{
	I3_P_STENCIL_OP*		pTempOP;

	i3StencilOperationAttr*		pStencilOPAttr;
	pStencilOPAttr = (i3StencilOperationAttr*)m_pSelObj;

	switch(m_SelectLine)
	{
	case	1:
		pTempOP = (I3_P_STENCIL_OP*)GetProperty();
		pStencilOPAttr->SetSPass(pTempOP->GetValue());
		break;
	case	2:
		pTempOP = (I3_P_STENCIL_OP*)GetProperty();
		pStencilOPAttr->SetSFail(pTempOP->GetValue());
		break;
	case	3:
		pTempOP = (I3_P_STENCIL_OP*)GetProperty();
		pStencilOPAttr->SetZFail(pTempOP->GetValue());
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// ChangeMatirxValue
// : 하나의 matrix만 있다는전제로 변경될 매트릭스와 라인을 받아서 변경
void PropertyBoard::ChangeMatrixValue(MATRIX * pMatrix , INT32 pSelLine)
{
	I3_P_VEC4D				*pVec; 
	VEC4D					Vec; 

	switch(pSelLine)
	{
	case 1:				//For Right
		pVec = (I3_P_VEC4D *)GetProperty(); 		
		Vec = pVec->GetValue(); 
		i3Matrix::SetRight(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	case 2:				//For Up
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetUp(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break;	
	case 3:				//For At
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetAt(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	case 4:				//For Pos
		pVec = (I3_P_VEC4D *)GetProperty(); 
		Vec = pVec->GetValue(); 
		i3Matrix::SetPos(pMatrix,i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),i3Vector::GetZ(&Vec), i3Vector::GetW(&Vec)); 
		break; 
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// matrix 하나를 display가능하도록 만든다.
void PropertyBoard::InsertMatrixInfo(MATRIX* pMatrix)
{
	I3_P_VEC4D*				pVec4;
	VEC4D					vec;

	pVec4 = new I3_P_VEC4D;	
	pVec4->SetName("1");
	i3Matrix::GetRow(pMatrix,0,&vec);
	pVec4->SetValue(vec);
	m_PropertyList.Add((void*)pVec4);

	pVec4 = new I3_P_VEC4D;	
	pVec4->SetName("2");
	i3Matrix::GetRow(pMatrix,1,&vec);
	pVec4->SetValue(vec);
	m_PropertyList.Add((void*)pVec4);

	pVec4 = new I3_P_VEC4D;	
	pVec4->SetName("3");
	i3Matrix::GetRow(pMatrix,2,&vec);
	pVec4->SetValue(vec);
	m_PropertyList.Add((void*)pVec4);

	pVec4 = new I3_P_VEC4D;	
	pVec4->SetName("4");
	i3Matrix::GetRow(pMatrix,3,&vec);
	pVec4->SetValue(vec);
	m_PropertyList.Add((void*)pVec4);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for i3ModelViewMatrixAttr
void	PropertyBoard::ChangeModelViewMatrixAttrList(void)
{
	MATRIX					*pMatrix; 
	i3ModelViewMatrixAttr	*ProjectMatrix; 

	ProjectMatrix = (i3ModelViewMatrixAttr *) m_pSelObj; 
	pMatrix = ProjectMatrix->GetMatrix();

	ChangeMatrixValue(pMatrix,m_SelectLine);
}

void	PropertyBoard::SetModelViewMatrixAttrList(void)
{
	i3ModelViewMatrixAttr*	pViewMatrixAttr;
	MATRIX*					pMatrix;
	
    pViewMatrixAttr = (i3ModelViewMatrixAttr*)m_pSelObj;
	pMatrix			= pViewMatrixAttr->GetMatrix();

	InsertMatrixInfo(pMatrix);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PropertyBoard::SetNormalMapBindAttr(void)
{
	i3NormalMapBindAttr * pAttr = (i3NormalMapBindAttr *) m_pSelObj; 
	i3Texture *			pTexture; 
	I3_P_INT *			pInt; 
	I3_P_STRING *		pString;
	I3_P_IMAGE *		pImage; 
	I3_P_IMAGEFORMAT *	pImageFormat; 

	pTexture = pAttr->GetTexture(); 
	
	if( pTexture != NULL && pTexture->GetWidth() > 0 && pTexture->GetHeight() > 0)
	{
		//UINT32				GetWidth(void)							{ return m_Width; }
		pInt = new I3_P_INT; 
		pInt->SetName("Width"); 
		pInt->SetValue(pTexture->GetWidth()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetHeight(void)							{ return m_Height; }
		pInt = new I3_P_INT; 
		pInt->SetName("Height"); 
		pInt->SetValue(pTexture->GetHeight()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//I3G_IMAGE_FORMAT	GetFormat(void)							{ return m_Format; }
		pImageFormat = new I3_P_IMAGEFORMAT; 
		pImageFormat->SetName("ImageFormat"); 
		pImageFormat->SetValue(pTexture->GetFormat()); 
		pImageFormat->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pImageFormat); 

		//UINT32				GetLevelCount(void)						{ return m_LevelCount; }
		pInt = new I3_P_INT; 
		pInt->SetName("LevelCount"); 
		pInt->SetValue(pTexture->GetLevelCount()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetBitsPerPixel(void)					{ return m_BitsPerPixel; }
		pInt = new I3_P_INT; 
		pInt->SetName("BitsPerPixel"); 
		pInt->SetValue(i3Gfx::GetImageFormatBitCount( pTexture->GetPersistFormat()));
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetStride(void)							{ return m_Stride; }
		pInt = new I3_P_INT; 
		pInt->SetName("Stride"); 
		pInt->SetValue(pTexture->GetStride()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetPitch(void)							{ return m_Pitch; }
		pInt = new I3_P_INT; 
		pInt->SetName("Pitch"); 
		pInt->SetValue(pTexture->GetPitch()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetDataSize(void)						{ return m_DataSize; }
		pInt = new I3_P_INT; 
		pInt->SetName("DataSize"); 
		pInt->SetValue(pTexture->GetDataSize()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		if( pTexture->hasName())
		{
			char szPath[MAX_PATH];
			BOOL bRelative = g_pMainWnd->GetRelativePath( szPath, pTexture->GetName() );

			// External Image
			pString = new I3_P_STRING; 
			if( bRelative )
			{
				pString->SetName("Relative Path"); 
			}
			else
			{
				pString->SetName("Absolute Path"); 
			}

			pString->SetValue( szPath ); 
			pString->AddStyle( I3_READONLY ); 
			m_PropertyList.Add( pString ); 
		}
	}

	pImage = new I3_P_IMAGE; 
	pImage->SetTexture( pTexture );
	pImage->SetName("Image"); 
	m_PropertyList.Add((void*)pImage); 
}

void PropertyBoard::ChangeNormalMapBindAttr(void)
{
	I3_P_IMAGE*	pImage;
	i3NormalMapBindAttr * pAttr = (i3NormalMapBindAttr *) m_pSelObj; 
	
	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( _stricmp( "Image", (char*)pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pAttr->GetTexture())//변경사항이 있다면
		{
			pAttr->SetTexture( pImage->GetTexture());
			SetBoard( m_pSelObj );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PropertyBoard::SetSpecularMapBindAttr(void)
{
	i3SpecularMapBindAttr * pAttr = (i3SpecularMapBindAttr *) m_pSelObj; 
	i3Texture *			pTexture; 
	I3_P_INT *			pInt; 
	I3_P_STRING *		pString;
	I3_P_IMAGE *		pImage; 
	I3_P_IMAGEFORMAT *	pImageFormat; 

	pTexture = pAttr->GetTexture(); 
	
	if( pTexture != NULL )
	{
		//UINT32				GetWidth(void)							{ return m_Width; }
		pInt = new I3_P_INT; 
		pInt->SetName("Width"); 
		pInt->SetValue(pTexture->GetWidth()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetHeight(void)							{ return m_Height; }
		pInt = new I3_P_INT; 
		pInt->SetName("Height"); 
		pInt->SetValue(pTexture->GetHeight()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//I3G_IMAGE_FORMAT	GetFormat(void)							{ return m_Format; }
		pImageFormat = new I3_P_IMAGEFORMAT; 
		pImageFormat->SetName("ImageFormat"); 
		pImageFormat->SetValue(pTexture->GetFormat()); 
		pImageFormat->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pImageFormat); 

		//UINT32				GetLevelCount(void)						{ return m_LevelCount; }
		pInt = new I3_P_INT; 
		pInt->SetName("LevelCount"); 
		pInt->SetValue(pTexture->GetLevelCount()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetBitsPerPixel(void)					{ return m_BitsPerPixel; }
		pInt = new I3_P_INT; 
		pInt->SetName("BitsPerPixel"); 
		pInt->SetValue(i3Gfx::GetImageFormatBitCount( pTexture->GetPersistFormat()));
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetStride(void)							{ return m_Stride; }
		pInt = new I3_P_INT; 
		pInt->SetName("Stride"); 
		pInt->SetValue(pTexture->GetStride()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetPitch(void)							{ return m_Pitch; }
		pInt = new I3_P_INT; 
		pInt->SetName("Pitch"); 
		pInt->SetValue(pTexture->GetPitch()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetDataSize(void)						{ return m_DataSize; }
		pInt = new I3_P_INT; 
		pInt->SetName("DataSize"); 
		pInt->SetValue(pTexture->GetDataSize()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		if( pTexture->hasName())
		{
			char szPath[MAX_PATH];
			BOOL bRelative = g_pMainWnd->GetRelativePath( szPath, pTexture->GetName() );

			// External Image
			pString = new I3_P_STRING; 
			if( bRelative )
			{
				pString->SetName("Relative Path"); 
			}
			else
			{
				pString->SetName("Absolute Path"); 
			}

			pString->SetValue( szPath ); 
			pString->AddStyle( I3_READONLY ); 
			m_PropertyList.Add( pString ); 
		}
	}

	pImage = new I3_P_IMAGE; 
	pImage->SetTexture( pTexture );
	pImage->SetName("Image"); 
	m_PropertyList.Add((void*)pImage); 
}

void PropertyBoard::ChangeSpecularMapBindAttr(void)
{
	I3_P_IMAGE*	pImage;
	i3SpecularMapBindAttr * pAttr = (i3SpecularMapBindAttr *) m_pSelObj; 
	
	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( _stricmp( "Image", (char*)pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pAttr->GetTexture())//변경사항이 있다면
		{
			pAttr->SetTexture( pImage->GetTexture());
			SetBoard( m_pSelObj );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PropertyBoard::SetLuxMapBindAttr(void)
{
	i3LuxMapBindAttr * pAttr = (i3LuxMapBindAttr *) m_pSelObj; 
	i3Texture *			pTexture; 
	I3_P_INT *			pInt; 
	I3_P_STRING *		pString;
	I3_P_IMAGE *		pImage; 
	I3_P_IMAGEFORMAT *	pImageFormat; 

	pTexture = pAttr->GetTexture(); 
	
	if( pTexture != NULL )
	{
		//UINT32				GetWidth(void)							{ return m_Width; }
		pInt = new I3_P_INT; 
		pInt->SetName("Width"); 
		pInt->SetValue(pTexture->GetWidth()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetHeight(void)							{ return m_Height; }
		pInt = new I3_P_INT; 
		pInt->SetName("Height"); 
		pInt->SetValue(pTexture->GetHeight()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//I3G_IMAGE_FORMAT	GetFormat(void)							{ return m_Format; }
		pImageFormat = new I3_P_IMAGEFORMAT; 
		pImageFormat->SetName("ImageFormat"); 
		pImageFormat->SetValue(pTexture->GetFormat()); 
		pImageFormat->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pImageFormat); 

		//UINT32				GetLevelCount(void)						{ return m_LevelCount; }
		pInt = new I3_P_INT; 
		pInt->SetName("LevelCount"); 
		pInt->SetValue(pTexture->GetLevelCount()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetBitsPerPixel(void)					{ return m_BitsPerPixel; }
		pInt = new I3_P_INT; 
		pInt->SetName("BitsPerPixel"); 
		pInt->SetValue(i3Gfx::GetImageFormatBitCount( pTexture->GetPersistFormat()));
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetStride(void)							{ return m_Stride; }
		pInt = new I3_P_INT; 
		pInt->SetName("Stride"); 
		pInt->SetValue(pTexture->GetStride()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetPitch(void)							{ return m_Pitch; }
		pInt = new I3_P_INT; 
		pInt->SetName("Pitch"); 
		pInt->SetValue(pTexture->GetPitch()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		//UINT32				GetDataSize(void)						{ return m_DataSize; }
		pInt = new I3_P_INT; 
		pInt->SetName("DataSize"); 
		pInt->SetValue(pTexture->GetDataSize()); 
		pInt->AddStyle(I3_READONLY); 
		m_PropertyList.Add((void *)pInt); 

		if( pTexture->hasName())
		{
			char szPath[MAX_PATH];
			BOOL bRelative = g_pMainWnd->GetRelativePath( szPath, pTexture->GetName() );

			// External Image
			pString = new I3_P_STRING; 
			if( bRelative )
			{
				pString->SetName("Relative Path"); 
			}
			else
			{
				pString->SetName("Absolute Path"); 
			}

			pString->SetValue( szPath ); 
			pString->AddStyle( I3_READONLY ); 
			m_PropertyList.Add( pString ); 
		}
	}

	pImage = new I3_P_IMAGE; 
	pImage->SetTexture( pTexture );
	pImage->SetName("Image"); 
	m_PropertyList.Add((void*)pImage); 
}

void PropertyBoard::ChangeLuxMapBindAttr(void)
{
	I3_P_IMAGE*	pImage;
	i3LuxMapBindAttr * pAttr = (i3LuxMapBindAttr *) m_pSelObj; 
	
	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( _stricmp( "Image", (char*)pProp->GetName() ) == 0 )
	{
		pImage = (I3_P_IMAGE*)GetProperty();
		if(pImage->GetTexture() != pAttr->GetTexture())//변경사항이 있다면
		{
			pAttr->SetTexture( pImage->GetTexture());
			SetBoard( m_pSelObj );
		}
	}
}

//I3_P_COMPFUNC
void PropertyBoard::SetFresnelAttr(void)
{
	i3FresnelAttr * pAttr = (i3FresnelAttr *) m_pSelObj;

	//For Enable Attr TRUE or FALSE 1
	{
		I3_P_BOOL		* pProperty;

		pProperty = new I3_P_BOOL; 
		pProperty->SetName("Enable");
		pProperty->SetValue( pAttr->isEnable());
		m_PropertyList.Add( pProperty);
	}

	{
		I3_P_REAL32 * pProperty;

		pProperty = new I3_P_REAL32;
		pProperty->SetName( "Index of Reflect");
		pProperty->SetValue( pAttr->getIOR());
		m_PropertyList.Add( pProperty);
	}

	{
		I3_P_COLORREAL * pProperty;

		pProperty = new I3_P_COLORREAL;
		pProperty->SetName( "Color");
		pProperty->SetValue( * pAttr->getColor());
		m_PropertyList.Add( pProperty);
	}
}

void PropertyBoard::ChangeFresnelAttr(void)
{
	i3FresnelAttr * pAttr = (i3FresnelAttr *) m_pSelObj; 
	
	CpropertyBase *pProp = (CpropertyBase *)m_PropertyList.GetItem( m_SelectLine );

	if( _stricmp( "Enable", (char*)pProp->GetName() ) == 0 )
	{
		I3_P_BOOL * pProperty;

		pProperty = (I3_P_BOOL *) GetProperty();

		pAttr->setEnable( pProperty->GetValue() == TRUE);
		SetBoard( m_pSelObj );
	}
	else if( _stricmp( "Index Of Reflect", (char*)pProp->GetName() ) == 0 )
	{
		I3_P_REAL32 * pProperty;

		pProperty = (I3_P_REAL32 *) pProp;

		pAttr->setIOR( pProperty->GetValue());

		SetBoard( m_pSelObj );
	}
	else if( _stricmp( "Color", (char*)pProp->GetName() ) == 0 )
	{
		I3_P_COLORREAL * pProperty;

		pProperty = (I3_P_COLORREAL *) pProp;

		pAttr->setColor( & pProperty->GetValue());
		
		SetBoard( m_pSelObj );
	}
}
