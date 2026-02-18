#include "stdafx.h"
#include "i3treePropertyBase.h"
#include "ColoDlg.h"
#include "ImageDlg.h"
#include "SetNodeFlagDlg.h"
#include "../include/i3Base/i3Lexer.h"
#include "GeometryDlg.h"
#include "ColliderStyle.h"

///////////////////////////////////////////////////////////////////////////////////////////
/////// Base Proeprty

CpropertyBase::CpropertyBase()
{
	m_Style = 0; 
}

CpropertyBase::~CpropertyBase()
{

}


///////////////////////////////////////////////////////////////////////////////////////////
/////// INT Property 
//I3_EDIT
CpropertyInt::CpropertyInt()
{
	m_value = 0; 
	m_Style |= I3_EDIT;
}

CpropertyInt::~CpropertyInt()
{

}

void CpropertyInt::Get( char * pszValueStr)
{
	sprintf(pszValueStr,"%d", m_value);
}

void CpropertyInt::Set( const char * pszValueStr)
{
	m_value = atoi(pszValueStr);	
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// BOOL Property 
CpropertyBool::CpropertyBool()
{
	m_value = FALSE; 
	m_Style |= I3_LIST; 
}

CpropertyBool::~CpropertyBool()
{

}

void CpropertyBool::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"True")== 0)	m_value = TRUE; 
	else													m_value = FALSE; 
}

void CpropertyBool::Get(char * pszValueStr)
{
	if(m_value == TRUE)
	{
		strcpy( pszValueStr, "True"); 
	}else 
	{
        strcpy(pszValueStr, "False"); 
	}
}

void CpropertyBool::GetValueList(i3StringList * pList)
{
	pList->Add("True");
	pList->Add("False");
}

void CpropertyBool::OnChangeList(void)
{
	switch(m_value)
	{
	case TRUE:		m_value = FALSE;	break; 
	case FALSE:		m_value = TRUE;		break; 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// REAL Property 
CpropertyReal32::CpropertyReal32()
{
	m_value = 0.f;
	m_Style |= I3_EDIT;
}

CpropertyReal32::~CpropertyReal32()
{
}

void CpropertyReal32::Get( char * pszValueStr)
{
	sprintf(pszValueStr,"%.2f",m_value);
}

void CpropertyReal32::Set(const char * pszValueStr)
{
	m_value = (REAL32)atof(pszValueStr);	
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// COLOR Property  
CpropertyColor::CpropertyColor()
{
	m_Style = m_Style | I3_USER_DRAW | I3_DIALOG; 
}
CpropertyColor::~CpropertyColor()
{

}
void CpropertyColor::Get( char * pszValueStr)
{

}

void CpropertyColor::Set( const char * pszValueStr)
{

}

void CpropertyColor::UserDraw(CPaintDC * pDc, RECT *pRect)					
{
	RECT Rect;
	HBRUSH OldBrush, NewBrush;
	HPEN NewPen, OldPen;
	char buffer[16];	
	COLOR iColor; 
	COLORREF Color;	
	UINT8 Red, Green, Blue, Alpha; 

	//Change Int Color 
	i3Color::Set(&iColor, &m_value); 
	Red		= i3Color::GetR(&iColor); 
	Green	= i3Color::GetG(&iColor); 
	Blue	= i3Color::GetB(&iColor); 
	Alpha	= i3Color::GetA(&iColor); 

	//For Color Rect
	Color = RGB( Red, Green, Blue);
	NewBrush = CreateSolidBrush(Color);
	
	ASSERT(NewBrush);

	OldBrush = (HBRUSH) pDc->SelectObject(NewBrush);
	NewPen = (HPEN)GetStockObject(BLACK_PEN);

	ASSERT(NewPen);

	OldPen = (HPEN) pDc->SelectObject(NewPen);

	Rect = *pRect; 
	Rect.top = Rect.top + 2; 
	Rect.bottom = Rect.bottom - 2;
	Rect.right = Rect.right - 2;
	pDc->Rectangle(&Rect);
	
	pDc->SetBkColor(Color); //Bk Color 

	Alpha = (UINT32)((100 * Alpha) / 255);

	Rect.top	= Rect.top + 1;
	Rect.bottom = Rect.bottom -1;
	Rect.left	= Rect.left + 4;

	sprintf( buffer, "%d %%", Alpha);

	//Set Text Color 
	{
		INT32 temp; 
		temp = (INT32)((Red + Green + Blue) / 3); 
		if(temp > 127 ) pDc->SetTextColor(RGB( 0, 0, 0));
		else			pDc->SetTextColor(RGB( 255, 255, 255));
	}

	pDc->DrawText(buffer, (int) strlen((const char *)buffer), &Rect, DT_NOPREFIX | DT_SINGLELINE);

	pDc->SelectObject(OldPen);
	pDc->SelectObject(OldBrush);
	DeleteObject( NewBrush);
	
	pDc->SetBkColor(GetSysColor( COLOR_BTNFACE));
}

void CpropertyColor::PopUpDialog(void)
{
	//View Color Table
	i3TDKColorPicker dlg;
	COLORREAL iColor; 

	//Set Color 
	i3Color::Set(&iColor, &m_value); 

	//Open Dialog Box
	if( dlg.Execute( &iColor) == true)
	{
		//Save Color Proeprty
		i3Color::Set( &iColor, dlg.getSelectedColor());
		i3Color::Set(&m_value,&iColor); 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//for ColorReal
///////////////////////////////////////////////////////////////////////////////////////////
/////// COLOR Property  
CpropertyColorReal::CpropertyColorReal()
{
	m_Style = m_Style | I3_USER_DRAW | I3_DIALOG; 
}
CpropertyColorReal::~CpropertyColorReal()
{

}
void CpropertyColorReal::Get( char * pszValueStr)
{

}

void CpropertyColorReal::Set( const char * pszValueStr)
{

}

void CpropertyColorReal::UserDraw(CPaintDC * pDc, RECT *pRect)					
{
	RECT Rect;
	HBRUSH OldBrush, NewBrush;
	HPEN NewPen, OldPen;
	char buffer[16];	
	COLOR iColor; 
	COLORREF Color;	
	UINT8 Red, Green, Blue, Alpha; 

	//Change Int Color 
	i3Color::Set(&iColor, &m_value); 
	Red		= i3Color::GetR(&iColor); 
	Green	= i3Color::GetG(&iColor); 
	Blue	= i3Color::GetB(&iColor); 
	Alpha	= i3Color::GetA(&iColor); 

	//For Color Rect
	Color = RGB( Red, Green, Blue);
	NewBrush = CreateSolidBrush(Color);
	
	ASSERT(NewBrush);

	OldBrush = (HBRUSH) pDc->SelectObject(NewBrush);
	NewPen = (HPEN)GetStockObject(BLACK_PEN);

	ASSERT(NewPen);

	OldPen = (HPEN) pDc->SelectObject(NewPen);

	Rect = *pRect; 
	Rect.top = Rect.top + 2; 
	Rect.bottom = Rect.bottom - 2;
	Rect.right = Rect.right - 2;
	pDc->Rectangle(&Rect);
	
	pDc->SetBkColor(Color); //Bk Color 

	Alpha = (UINT32)((100 * Alpha) / 255);

	Rect.top	= Rect.top + 1;
	Rect.bottom = Rect.bottom -1;
	Rect.left	= Rect.left + 4;

	sprintf( buffer, "%d %%", Alpha);

	//Set Text Color 
	{
		INT32 temp; 
		temp = (INT32)((Red + Green + Blue) / 3); 
		if(temp > 127 ) pDc->SetTextColor(RGB( 0, 0, 0));
		else			pDc->SetTextColor(RGB( 255, 255, 255));
	}

	pDc->DrawText(buffer, (int) strlen((const char *)buffer), &Rect, DT_NOPREFIX | DT_SINGLELINE);

	pDc->SelectObject(OldPen);
	pDc->SelectObject(OldBrush);
	DeleteObject( NewBrush);
	
	pDc->SetBkColor(GetSysColor( COLOR_BTNFACE));
}

void CpropertyColorReal::PopUpDialog(void)
{	
	//View Color Table
	i3TDKColorPicker dlg;
	COLORREAL iColor; 

	//Set Color 
	i3Color::Set(&iColor, &m_value); 

	//Open Dialog Box
	if( dlg.Execute( &iColor))
	{
		//Save Color Proeprty
		i3Color::Set( &iColor, dlg.getSelectedColor());
		i3Color::Set(&m_value,&iColor); 
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
/////// VEC2d Property
CpropertyVec2d::CpropertyVec2d()
{
	m_Style |= I3_EDIT;
}

CpropertyVec2d::~CpropertyVec2d()
{

}

void CpropertyVec2d::Get(char * pszValueStr)
{
	sprintf( pszValueStr, "( %f, %f )", getX(&m_value), getY(&m_value));
}

void CpropertyVec2d::Set(const char * pszValueStr)
{
	i3LineLexer Lexer;
	char szToken[128];
	char sep;
	int NumCount = 0;

	Lexer.SetMasterString( (char *)pszValueStr);

	while( Lexer.GetNextToken( (char *)szToken, &sep, (char *)" ,)}]|"))
	{
		if( i3String::IsDigitString( szToken))
		{
			REAL32 val = (REAL32) atof( szToken);

			switch( NumCount)
			{
				case 0 :		setU( &m_value, val);	break;
				case 1 :		setV( &m_value, val);	break;
				default :
					return;
			}

			NumCount++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// VEC Property
CpropertyVec3d::CpropertyVec3d()
{
	m_Style |= I3_EDIT;
}

CpropertyVec3d::~CpropertyVec3d() 
{

}

void CpropertyVec3d::Get( char * pszValueStr)
{
	sprintf( pszValueStr, "( %f, %f, %f )", getX(&m_value), getY(&m_value), getZ(&m_value));
}

void CpropertyVec3d::Set( const char * pszValueStr)
{
	i3LineLexer Lexer;
	char szToken[128];
	char sep;
	int NumCount = 0;

	Lexer.SetMasterString( (char *)pszValueStr);

	while( Lexer.GetNextToken( (char *)szToken, &sep, (char *)" ,)}]|"))
	{
		if( i3String::IsDigitString( szToken))
		{
			REAL32 val = (REAL32) atof( szToken);

			switch( NumCount)
			{
				case 0 :		setX( &m_value, val);	break;
				case 1 :		setY( &m_value, val); break;
				case 2 :		setZ( &m_value, val); break;
				default :
					return;
			}

			NumCount++;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
/////// MATRIX Property 
CpropertyMatrix::CpropertyMatrix()
{

}
CpropertyMatrix::~CpropertyMatrix()
{

}

void CpropertyMatrix::Get( char * pszValueStr)
{

}
void CpropertyMatrix::Set( const char * pszValueStr)
{

}

///////////////////////////////////////////////////////////////////////////////////////////
/////// STRING Property 
CpropertyString::CpropertyString()
{
	strcpy(m_value,""); 
	m_Style |= I3_EDIT;
}

CpropertyString::~CpropertyString()
{

}

///////////////////////////////////////////////////////////////////////////////////////////
/////// CompFunc Property 
CpropertyCompFunc::CpropertyCompFunc()
{
	m_Style |= I3_LIST; 
}

CpropertyCompFunc::~CpropertyCompFunc()
{

}

void CpropertyCompFunc::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Never") == 0)	   m_value = I3G_COMP_NEVER; 
	else if(i3String::Compare((char *)pszValueStr,"Less") == 0)m_value = I3G_COMP_LESS; 
	else if(i3String::Compare((char *)pszValueStr,"Equal") == 0)m_value = I3G_COMP_EQUAL; 
	else if(i3String::Compare((char *)pszValueStr,"Lequal") == 0)m_value = I3G_COMP_LEQUAL; 
	else if(i3String::Compare((char *)pszValueStr,"Greater") == 0)m_value = I3G_COMP_GREATER; 
	else if(i3String::Compare((char *)pszValueStr,"Notequal") == 0)m_value = I3G_COMP_NOTEQUAL; 
	else if(i3String::Compare((char *)pszValueStr,"Gequal") == 0)m_value = I3G_COMP_GEQUAL; 
	else if(i3String::Compare((char *)pszValueStr,"Always") == 0)m_value = I3G_COMP_ALWAYS; 
}

void CpropertyCompFunc::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_COMP_NEVER		: strcpy( pszValueStr, "Never"); break; 
	case I3G_COMP_LESS		: strcpy( pszValueStr, "Less"); break; 
	case I3G_COMP_EQUAL		: strcpy( pszValueStr, "Equal"); break; 
	case I3G_COMP_LEQUAL	: strcpy( pszValueStr, "Lequal"); break; 
	case I3G_COMP_GREATER	: strcpy( pszValueStr, "Greater"); break; 
	case I3G_COMP_NOTEQUAL	: strcpy( pszValueStr, "Notequal"); break; 
	case I3G_COMP_GEQUAL	: strcpy( pszValueStr, "Gequal"); break; 
	case I3G_COMP_ALWAYS	: strcpy( pszValueStr, "Always"); break; 
	}
}

void CpropertyCompFunc::GetValueList(i3StringList * pList)
{
	pList->Add("Never");
	pList->Add("Less");
	pList->Add("Equal");
	pList->Add("Lequal");
	pList->Add("Greater");
	pList->Add("Notequal");
	pList->Add("Gequal");
	pList->Add("Always");
}

void CpropertyCompFunc::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_BLEND Property 
CpropertyBlend::CpropertyBlend()
{
	m_Style |= I3_LIST; 
}

CpropertyBlend::~CpropertyBlend()
{

}

void CpropertyBlend::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Zero") == 0)					m_value = I3G_BLEND_ZERO;			
	else if(i3String::Compare((char *)pszValueStr,"One") == 0)				m_value = I3G_BLEND_ONE;			
	else if(i3String::Compare((char *)pszValueStr,"Srccolor") == 0)			m_value = I3G_BLEND_SRCCOLOR;		
	else if(i3String::Compare((char *)pszValueStr,"Invsrccolor") == 0)		m_value = I3G_BLEND_INVSRCCOLOR;	
	else if(i3String::Compare((char *)pszValueStr,"Srcalpha") == 0)			m_value = I3G_BLEND_SRCALPHA;		 
	else if(i3String::Compare((char *)pszValueStr,"Invsrcalpha") == 0)		m_value = I3G_BLEND_INVSRCALPHA; 
	else if(i3String::Compare((char *)pszValueStr,"Destalpha") == 0)		m_value = I3G_BLEND_DESTALPHA;		
	else if(i3String::Compare((char *)pszValueStr,"Invdestalpha") == 0)		m_value = I3G_BLEND_INVDESTALPHA;	
	else if(i3String::Compare((char *)pszValueStr,"Destcolor") == 0)		m_value = I3G_BLEND_DESTCOLOR;		
	else if(i3String::Compare((char *)pszValueStr,"Invdestcolor") == 0)		m_value = I3G_BLEND_INVDESTCOLOR;	
	else if(i3String::Compare((char *)pszValueStr,"Srcalphasat") == 0)		m_value = I3G_BLEND_SRCALPHASAT;	
}

void CpropertyBlend::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_BLEND_ZERO			: strcpy( pszValueStr, "Zero");			break; 
	case I3G_BLEND_ONE			: strcpy( pszValueStr, "One");			break; 
	case I3G_BLEND_SRCCOLOR		: strcpy( pszValueStr, "Srccolor");		break; 
	case I3G_BLEND_INVSRCCOLOR	: strcpy( pszValueStr, "Invsrccolor");	break; 
	case I3G_BLEND_SRCALPHA		: strcpy( pszValueStr, "Srcalpha");		break; 
	case I3G_BLEND_INVSRCALPHA	: strcpy( pszValueStr, "Invsrcalpha");	break; 
	case I3G_BLEND_DESTALPHA	: strcpy( pszValueStr, "Destalpha");	break; 
	case I3G_BLEND_INVDESTALPHA	: strcpy( pszValueStr, "Invdestalpha"); break; 
	case I3G_BLEND_DESTCOLOR	: strcpy( pszValueStr, "Destcolor");	break; 
	case I3G_BLEND_INVDESTCOLOR	: strcpy( pszValueStr, "Invdestcolor"); break; 
	case I3G_BLEND_SRCALPHASAT	: strcpy( pszValueStr, "Srcalphasat");	break; 
	}
}

void CpropertyBlend::GetValueList(i3StringList * pList)
{
	pList->Add("Zero");
	pList->Add("One");
	pList->Add("Srccolor");
	pList->Add("Invsrccolor");
	pList->Add("Srcalpha");
	pList->Add("Invsrcalpha");
	pList->Add("Destalpha");
	pList->Add("Invdestalpha");
	pList->Add("Destcolor");
	pList->Add("Invdestcolor");
	pList->Add("Srcalphasat");
}

void CpropertyBlend::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_BLEND Property 
CpropertyFaceCull::CpropertyFaceCull()
{
	m_Style |= I3_LIST; 
}

CpropertyFaceCull::~CpropertyFaceCull()
{

}

void CpropertyFaceCull::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)					m_value = I3G_FACE_CULL_NONE;			
	else if(i3String::Compare((char *)pszValueStr,"Back") == 0)				m_value = I3G_FACE_CULL_BACK;			
	else if(i3String::Compare((char *)pszValueStr,"Front") == 0)			m_value = I3G_FACE_CULL_FRONT;		
}

void CpropertyFaceCull::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_FACE_CULL_NONE	: strcpy( pszValueStr, "None");			break; 
	case I3G_FACE_CULL_BACK	: strcpy( pszValueStr, "Back");			break; 
	case I3G_FACE_CULL_FRONT	: strcpy( pszValueStr, "Front");		break; 
	}
}

void CpropertyFaceCull::GetValueList(i3StringList * pList)
{
	pList->Add("None");
	pList->Add("Back");
	pList->Add("Front");
}

void CpropertyFaceCull::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_FOG_MODE Property 
CpropertyFogMode::CpropertyFogMode()
{
	m_Style |= I3_LIST; 
}

CpropertyFogMode::~CpropertyFogMode()
{

}

void CpropertyFogMode::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)				m_value = I3G_FOG_NONE;			
	else if(i3String::Compare((char *)pszValueStr,"Linear") == 0)		m_value = I3G_FOG_LINEAR;			
	else if(i3String::Compare((char *)pszValueStr,"Exp") == 0)			m_value = I3G_FOG_EXP;		
	else if(i3String::Compare((char *)pszValueStr,"Exp2") == 0)			m_value = I3G_FOG_EXP2;		
}

void CpropertyFogMode::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_FOG_NONE	: strcpy( pszValueStr, "None");			break;
	case I3G_FOG_LINEAR	: strcpy( pszValueStr, "Linear");		break;
	case I3G_FOG_EXP	: strcpy( pszValueStr, "Exp");			break;
	case I3G_FOG_EXP2	: strcpy( pszValueStr, "Exp2");			break;
	}
}

void CpropertyFogMode::GetValueList(i3StringList * pList)
{
	pList->Add("None");
	pList->Add("Linear");
	pList->Add("Exp");
	pList->Add("Exp2");
}

void CpropertyFogMode::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_LIGHT_TYPE Property 
CpropertyLightType::CpropertyLightType()
{
	m_Style |= I3_LIST; 
}

CpropertyLightType::~CpropertyLightType()
{

}

void CpropertyLightType::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)				m_value = I3G_LIGHT_NONE;			
	else if(i3String::Compare((char *)pszValueStr,"Directional") == 0)	m_value = I3G_LIGHT_DIRECTIONAL;			
	else if(i3String::Compare((char *)pszValueStr,"Spot") == 0)			m_value = I3G_LIGHT_SPOT;		
	else if(i3String::Compare((char *)pszValueStr,"Point") == 0)		m_value = I3G_LIGHT_POINT;		
}

void CpropertyLightType::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_LIGHT_NONE			: strcpy( pszValueStr, "None");			break;
	case I3G_LIGHT_DIRECTIONAL	: strcpy( pszValueStr, "Directional");	break;
	case I3G_LIGHT_SPOT			: strcpy( pszValueStr, "Spot");			break;
	case I3G_LIGHT_POINT		: strcpy( pszValueStr, "Point");		break;
	}
}

void CpropertyLightType::GetValueList(i3StringList * pList)
{
	pList->Add("None");
	pList->Add("Directional");
	pList->Add("Spot");
	pList->Add("Point");
}

void CpropertyLightType::OnChangeList(void)
{
	//미정
}


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_FILTER Property 
CpropertyTextureFilter::CpropertyTextureFilter()
{
	m_Style |= I3_LIST; 
}

CpropertyTextureFilter::~CpropertyTextureFilter()
{

}

void CpropertyTextureFilter::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)		m_value = I3G_TEXTURE_FILTER_NONE					;			
	else if(i3String::Compare((char *)pszValueStr,"Point") == 0)		m_value = I3G_TEXTURE_FILTER_POINT				;			
	else if(i3String::Compare((char *)pszValueStr,"Linear") == 0)		m_value = I3G_TEXTURE_FILTER_LINEAR				;		
	else if(i3String::Compare((char *)pszValueStr,"PointMapmapPoint") == 0)		m_value = I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT	;		
	else if(i3String::Compare((char *)pszValueStr,"PointMipmapLinear") == 0)		m_value = I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR	;			
	else if(i3String::Compare((char *)pszValueStr,"LenearMipmapPoint") == 0)		m_value = I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT	;		
	else if(i3String::Compare((char *)pszValueStr,"LinearMipmapLinear") == 0)		m_value = I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR ;
	else if(i3String::Compare((char *)pszValueStr,"Anisotropic") == 0)				m_value = I3G_TEXTURE_FILTER_ANISOTROPIC;
	else if(i3String::Compare((char *)pszValueStr,"Pyramidal Quad") == 0)			m_value = I3G_TEXTURE_FILTER_PYRAMIDAL_QUAD;
	else if(i3String::Compare((char *)pszValueStr,"Gaussian Quad") == 0)			m_value = I3G_TEXTURE_FILTER_GAUSSIAN_QUAD;
}

void CpropertyTextureFilter::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_TEXTURE_FILTER_NONE						: strcpy( pszValueStr, "None");				break;
	case I3G_TEXTURE_FILTER_POINT					: strcpy( pszValueStr, "Point");			break;
	case I3G_TEXTURE_FILTER_LINEAR					: strcpy( pszValueStr, "Linear");			break;
	case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT		: strcpy( pszValueStr, "PointMapmapPoint");	break;
	case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR		: strcpy( pszValueStr, "PointMipmapLinear");break;
	case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT		: strcpy( pszValueStr, "LenearMipmapPoint");break;
	case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR 	: strcpy( pszValueStr, "LinearMipmapLinear");break;
	case I3G_TEXTURE_FILTER_ANISOTROPIC				: strcpy( pszValueStr, "Anisotropic");break;
	case I3G_TEXTURE_FILTER_PYRAMIDAL_QUAD			: strcpy( pszValueStr, "Pyramidal Quad");	break;
	case I3G_TEXTURE_FILTER_GAUSSIAN_QUAD			: strcpy( pszValueStr, "Gaussian Quad");	break;
	}
}

void CpropertyTextureFilter::GetValueList(i3StringList * pList)
{
	pList->Add("None");				
	pList->Add("Point");			
	pList->Add("Linear");			
	pList->Add("PointMapmapPoint");	
	pList->Add("PointMipmapLinear");
	pList->Add("LenearMipmapPoint");
	pList->Add("LinearMipmapLinear");
	pList->Add("Anisotropic");
	pList->Add( "Pyramidal Quad");
	pList->Add( "Gaussian Quad");
}

void CpropertyTextureFilter::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_WRAP Property 
CpropertyTextureWarp::CpropertyTextureWarp()
{
	m_Style |= I3_LIST; 
}

CpropertyTextureWarp::~CpropertyTextureWarp()
{

}

void CpropertyTextureWarp::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Wrap") == 0)				m_value = I3G_TEXTURE_WRAP_WRAP;			
	else if(i3String::Compare((char *)pszValueStr,"Clamp") == 0)		m_value = I3G_TEXTURE_WRAP_CLAMP;			
}

void CpropertyTextureWarp::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_TEXTURE_WRAP_WRAP		: strcpy( pszValueStr, "Wrap");				break;
	case I3G_TEXTURE_WRAP_CLAMP		: strcpy( pszValueStr, "Clamp");			break;
	}
}

void CpropertyTextureWarp::GetValueList(i3StringList * pList)
{
	pList->Add("Wrap");				
	pList->Add("Clamp");			
}

void CpropertyTextureWarp::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_WRAP Property 
CpropertyShadeMode::CpropertyShadeMode()
{
	m_Style |= I3_LIST; 
}

CpropertyShadeMode::~CpropertyShadeMode()
{

}

void CpropertyShadeMode::Set(const char * pszValueStr)
{
	if( i3String::Compare((char *)pszValueStr,"Flat") == 0)							m_value = I3G_SHADE_FLAT;			
	else if( i3String::Compare((char *)pszValueStr,"Gouraud") == 0)					m_value = I3G_SHADE_GOURAUD;
	else if( i3String::Compare((char *)pszValueStr, "Phong") == 0)					m_value = I3G_SHADE_PHONG;
	else if( i3String::Compare((char *)pszValueStr, "Custom Lighting") == 0)		m_value = I3G_SHADE_CUSTOM;
	else if( i3String::Compare((char *)pszValueStr, "LuxMap") == 0)					m_value = I3G_SHADE_LUXMAP;
	else if( i3String::Compare((char *)pszValueStr, "HSL") == 0)					m_value = I3G_SHADE_HEMISPHERE;
	else if( i3String::Compare((char *)pszValueStr, "GI1") == 0)					m_value = I3G_SHADE_GI1;
}

void CpropertyShadeMode::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_SHADE_FLAT			: strcpy( pszValueStr, "Flat");				break;
	case I3G_SHADE_GOURAUD		: strcpy( pszValueStr, "Gouraud");			break;
	case I3G_SHADE_PHONG		: strcpy( pszValueStr, "Phong");			break;
	case I3G_SHADE_CUSTOM		: strcpy( pszValueStr, "Custom Lighting");	break;
	case I3G_SHADE_LUXMAP		: strcpy( pszValueStr, "LuxMap");			break;
	case I3G_SHADE_HEMISPHERE	: strcpy( pszValueStr, "HSL");				break;
	case I3G_SHADE_GI1			: strcpy( pszValueStr, "GI1");				break;
	}
}

void CpropertyShadeMode::GetValueList(i3StringList * pList)
{
	pList->Add( "Flat");				
	pList->Add( "Gouraud");
	pList->Add( "Phong");
	pList->Add( "Custom Lighting");
	pList->Add( "LuxMap");
	pList->Add( "HSL");
	pList->Add( "GI1");
}

void CpropertyShadeMode::OnChangeList(void)
{
	//미정
}


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_RENDER_MODE Property 
CpropertyRenderMode::CpropertyRenderMode()
{
	m_Style |= I3_LIST; 
}

CpropertyRenderMode::~CpropertyRenderMode()
{

}

void CpropertyRenderMode::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Point") == 0)			m_value = I3G_RENDER_POINT;			
	else if(i3String::Compare((char *)pszValueStr,"Line") == 0)		m_value = I3G_RENDER_LINE;			
	else if(i3String::Compare((char *)pszValueStr,"Polygon") == 0)	m_value = I3G_RENDER_POLYGON;			
}

void CpropertyRenderMode::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_RENDER_POINT			: strcpy( pszValueStr, "Point");		break;
	case I3G_RENDER_LINE			: strcpy( pszValueStr, "Line");			break;
	case I3G_RENDER_POLYGON			: strcpy( pszValueStr, "Polygon");		break;
	}
}

void CpropertyRenderMode::GetValueList(i3StringList * pList)
{
	pList->Add("Point");				
	pList->Add("Line");			
	pList->Add("Polygon");			
}

void CpropertyRenderMode::OnChangeList(void)
{
	//미정
}


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_IMAGE_FORMAT Property 
CpropertyImageFormat::CpropertyImageFormat()
{
	m_Style |= I3_LIST; 
}

CpropertyImageFormat::~CpropertyImageFormat()
{

}

void CpropertyImageFormat::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)			m_value = I3G_IMAGE_FORMAT_NONE			;			
	else if(i3String::Compare((char *)pszValueStr,"I4") == 0)		m_value = I3G_IMAGE_FORMAT_I4			;			
	else if(i3String::Compare((char *)pszValueStr,"I8") == 0)		m_value = I3G_IMAGE_FORMAT_I8			;			
	else if(i3String::Compare((char *)pszValueStr,"L8") == 0)		m_value = i3G_IMAGE_FORMAT_L8			;			
	else if(i3String::Compare((char *)pszValueStr,"A8") == 0)		m_value = i3G_IMAGE_FORMAT_A8			;			
	else if(i3String::Compare((char *)pszValueStr,"RGB565") == 0)	m_value = I3G_IMAGE_FORMAT_RGB_565		;			
	else if(i3String::Compare((char *)pszValueStr,"XRGB1555") == 0)	m_value = I3G_IMAGE_FORMAT_XRGB_1555	;			
	else if(i3String::Compare((char *)pszValueStr,"ARGB1555") == 0)	m_value = I3G_IMAGE_FORMAT_ARGB_1555	;			
	else if(i3String::Compare((char *)pszValueStr,"XRGB4444") == 0)		m_value = I3G_IMAGE_FORMAT_XRGB_4444	;			
	else if(i3String::Compare((char *)pszValueStr,"ARGB4444") == 0)		m_value = I3G_IMAGE_FORMAT_ARGB_4444	;			
	else if(i3String::Compare((char *)pszValueStr,"RGB888") == 0)		m_value = I3G_IMAGE_FORMAT_RGB_888		;			
	else if(i3String::Compare((char *)pszValueStr,"BGR888") == 0)		m_value = I3G_IMAGE_FORMAT_BGR_888		;			


	else if(i3String::Compare((char *)pszValueStr,"RGBX8888") == 0)		m_value = I3G_IMAGE_FORMAT_RGBX_8888	;			
	else if(i3String::Compare((char *)pszValueStr,"BGRX8888") == 0)		m_value = I3G_IMAGE_FORMAT_BGRX_8888	;			
	else if(i3String::Compare((char *)pszValueStr,"RGBA8888") == 0)		m_value = I3G_IMAGE_FORMAT_RGBA_8888	;			
	else if(i3String::Compare((char *)pszValueStr,"BGRA8888") == 0)		m_value = I3G_IMAGE_FORMAT_BGRA_8888	;			
	else if(i3String::Compare((char *)pszValueStr,"VU88") == 0)			m_value = I3G_IMAGE_FORMAT_VU_88		;			
	else if(i3String::Compare((char *)pszValueStr,"QWVU8888") == 0)		m_value = I3G_IMAGE_FORMAT_QWVU_8888	;			
	else if(i3String::Compare((char *)pszValueStr,"DXT1X") == 0)		m_value = I3G_IMAGE_FORMAT_DXT1_X		;			
	else if(i3String::Compare((char *)pszValueStr,"DXT1") == 0)			m_value = I3G_IMAGE_FORMAT_DXT1			;			
	else if(i3String::Compare((char *)pszValueStr,"DXT2") == 0)			m_value = I3G_IMAGE_FORMAT_DXT2			;			
	else if(i3String::Compare((char *)pszValueStr,"DXT3") == 0)			m_value = I3G_IMAGE_FORMAT_DXT3			;			
	else if(i3String::Compare((char *)pszValueStr,"DXT4") == 0)			m_value = I3G_IMAGE_FORMAT_DXT4			;			
	else if(i3String::Compare((char *)pszValueStr,"DXT5") == 0)			m_value = I3G_IMAGE_FORMAT_DXT5			;			
	else if(i3String::Compare((char *)pszValueStr,"FORCEDWORD") == 0)	m_value = I3G_IMAGE_FORMAT_FORCE_DWORD	;

}

void CpropertyImageFormat::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_IMAGE_FORMAT_NONE					: strcpy( pszValueStr, "None");			break;
	case I3G_IMAGE_FORMAT_I4					: strcpy( pszValueStr, "I4");			break;
	case I3G_IMAGE_FORMAT_I8					: strcpy( pszValueStr, "I8");			break;
	case i3G_IMAGE_FORMAT_L8					: strcpy( pszValueStr, "L8");			break;
	case i3G_IMAGE_FORMAT_A8					: strcpy( pszValueStr, "A8");			break;
	case I3G_IMAGE_FORMAT_RGB_565				: strcpy( pszValueStr, "RGB565");		break;
	case I3G_IMAGE_FORMAT_XRGB_1555				: strcpy( pszValueStr, "XRGB1555");		break;
	case I3G_IMAGE_FORMAT_ARGB_1555				: strcpy( pszValueStr, "ARGB1555");		break;
	case I3G_IMAGE_FORMAT_XRGB_4444				: strcpy( pszValueStr, "XRGB4444");		break;
	case I3G_IMAGE_FORMAT_ARGB_4444				: strcpy( pszValueStr, "ARGB4444");		break;
	case I3G_IMAGE_FORMAT_RGB_888				: strcpy( pszValueStr, "RGB888");		break;
	case I3G_IMAGE_FORMAT_BGR_888				: strcpy( pszValueStr, "BGR888");		break;

	case I3G_IMAGE_FORMAT_RGBX_8888					: strcpy( pszValueStr, "RGBX8888");		break;
	case I3G_IMAGE_FORMAT_BGRX_8888					: strcpy( pszValueStr, "BGRX8888");		break;
	case I3G_IMAGE_FORMAT_RGBA_8888					: strcpy( pszValueStr, "RGBA8888");		break;
	case I3G_IMAGE_FORMAT_BGRA_8888					: strcpy( pszValueStr, "BGRA8888");		break;
	case I3G_IMAGE_FORMAT_VU_88						: strcpy( pszValueStr, "VU88");		break;
	case I3G_IMAGE_FORMAT_QWVU_8888					: strcpy( pszValueStr, "QWVU8888");		break;
	case I3G_IMAGE_FORMAT_DXT1_X					: strcpy( pszValueStr, "DXT1X");		break;
	case I3G_IMAGE_FORMAT_DXT1						: strcpy( pszValueStr, "DXT1");		break;
	case I3G_IMAGE_FORMAT_DXT2						: strcpy( pszValueStr, "DXT2");		break;
	case I3G_IMAGE_FORMAT_DXT3						: strcpy( pszValueStr, "DXT3");		break;
	case I3G_IMAGE_FORMAT_DXT4						: strcpy( pszValueStr, "DXT4");		break;
	case I3G_IMAGE_FORMAT_DXT5						: strcpy( pszValueStr, "DXT5");		break;

	case I3G_IMAGE_FORMAT_ABGR_16F					: strcpy( pszValueStr, "ABGR_16F");		break;
	case I3G_IMAGE_FORMAT_ABGR_32F					: strcpy( pszValueStr, "ABGR_32F");		break;
	case I3G_IMAGE_FORMAT_ABGR_16					: strcpy( pszValueStr, "ABGR_16");		break;
	case I3G_IMAGE_FORMAT_R_32F						: strcpy( pszValueStr, "R_32F");		break;
	case I3G_IMAGE_FORMAT_GR_32F					: strcpy( pszValueStr, "GR_32F");		break;

	case I3G_IMAGE_FORMAT_FORCE_DWORD				: strcpy( pszValueStr, "FORCEDWORD");		break;

	}
}

void CpropertyImageFormat::GetValueList(i3StringList * pList)
{
	pList->Add("None");					
	pList->Add("I4");				
	pList->Add("I8");				
	pList->Add("L8");					
	pList->Add("A8");				
	pList->Add("RGB565");			
	pList->Add("XRGB1555");				
	pList->Add("ARGB1555");			
	pList->Add("XRGB4444");			
	pList->Add("ARGB4444");				
	pList->Add("RGB888");			
	pList->Add("BGR888");			

	pList->Add("RGBX8888");			
	pList->Add("BGRX8888");			
	pList->Add("RGBA8888");			
	pList->Add("BGRA8888");			
	pList->Add("VU88");			
	pList->Add("QWVU8888");			
	pList->Add("DXT1X");			
	pList->Add("DXT1");			
	pList->Add("DXT2");			
	pList->Add("DXT3");			
	pList->Add("DXT4");			
	pList->Add("DXT5");			

	pList->Add("ABGR_16F");		
	pList->Add("ABGR_32F");		
	pList->Add("ABGR_16");		
	pList->Add("R_32F");		
	pList->Add("GR_32F");		

	pList->Add("FORCEDWORD");			

}

void CpropertyImageFormat::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// CpropertyImage Property  
CpropertyImage::CpropertyImage()
{
	m_Style = I3_BIG_DIALOG;
	m_pTexture = NULL;
}

CpropertyImage::~CpropertyImage()
{
}

void CpropertyImage::Get( char * pszValueStr)
{
	strcpy(pszValueStr,"Click This line"); 
}

void CpropertyImage::Set( const char * pszValueStr)
{

}


void CpropertyImage::PopUpDialog(void)
{
	CImageDlg ImageDlg; 

	//Set Texture Image;
	ImageDlg.SetTexture(m_pTexture);

	//원본과 다른이미지를 저장하기로 되어있다면 그이미지의 포인터가 넘어오고
	//같은경우나 취소된 경우는 null값이 넘어온다.

	//Open Dialog Box
	if(IDOK == ImageDlg.DoModal())
	{
		if(ImageDlg.m_bChanged)
		{	
			m_pTexture = ImageDlg.GetImage();
			m_pTexture->AddRef();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_PRIMITIVE Property 
CpropertyPrimitive::CpropertyPrimitive()
{
	m_Style |= I3_LIST; 
}

CpropertyPrimitive::~CpropertyPrimitive()
{

}

void CpropertyPrimitive::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"None") == 0)				m_value = I3G_PRIM_NONE;			
	else if(i3String::Compare((char *)pszValueStr,"PointList") == 0)	m_value = I3G_PRIM_POINTLIST;
	else if(i3String::Compare((char *)pszValueStr,"LineList") == 0)		m_value = I3G_PRIM_LINELIST;			
	else if(i3String::Compare((char *)pszValueStr,"LineStrip") == 0)	m_value = I3G_PRIM_LINESTRIP;
	else if(i3String::Compare((char *)pszValueStr,"TriList") == 0)		m_value = I3G_PRIM_TRILIST;			
	else if(i3String::Compare((char *)pszValueStr,"TriStrip") == 0)		m_value = I3G_PRIM_TRISTRIP;
	else if(i3String::Compare((char *)pszValueStr,"TriFan") == 0)		m_value = I3G_PRIM_TRIFAN;
}

void CpropertyPrimitive::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_PRIM_NONE					: strcpy( pszValueStr, "None");			break;
	case I3G_PRIM_POINTLIST				: strcpy( pszValueStr, "PointList");	break;
	case I3G_PRIM_LINELIST				: strcpy( pszValueStr, "LineList");		break;
	case I3G_PRIM_LINESTRIP				: strcpy( pszValueStr, "LineStrip");	break;
	case I3G_PRIM_TRILIST				: strcpy( pszValueStr, "TriList");		break;
	case I3G_PRIM_TRISTRIP				: strcpy( pszValueStr, "TriStrip");		break;
	case I3G_PRIM_TRIFAN				: strcpy( pszValueStr, "TriFan");		break;
	}
}

void CpropertyPrimitive::GetValueList(i3StringList * pList)
{
	pList->Add("None");				
	pList->Add("PointList");			
	pList->Add("LineList");			
	pList->Add("LineStrip");				
	pList->Add("TriList");			
	pList->Add("TriStrip");			
	pList->Add("TriFan");			
}

void CpropertyPrimitive::OnChangeList(void)
{
	//미정
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// CpropertyImage Property  
CpropertyGeometry::CpropertyGeometry()
{
	m_Style = I3_BIG_DIALOG; 
}

CpropertyGeometry::~CpropertyGeometry()
{

}
void CpropertyGeometry::Get( char * pszValueStr)
{
	strcpy(pszValueStr,"Click This line");
}

void CpropertyGeometry::Set( const char * pszValueStr)
{

}


void CpropertyGeometry::PopUpDialog(void)
{
	CGeometryDlg GeometryDlg; 

	GeometryDlg.m_pGeomAttr = m_pGeometry;
	//Open Dialog Box
	if(IDOK == GeometryDlg.DoModal())
	{
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/////// VEC4D Property  
CpropertyVec4d::CpropertyVec4d()
{
	m_Style |= I3_EDIT;
}

CpropertyVec4d::~CpropertyVec4d() 
{

}

void CpropertyVec4d::Get( char * pszValueStr)
{
	sprintf( pszValueStr, "( %f, %f, %f, %f )", getX(&m_value), getY(&m_value), getZ(&m_value), getW(&m_value));
}

void CpropertyVec4d::Set( const char * pszValueStr)
{
	i3LineLexer Lexer;
	char szToken[128];
	char sep;
	int NumCount = 0;

	Lexer.SetMasterString( (char *)pszValueStr);

	while( Lexer.GetNextToken( (char *)szToken, &sep, (char *)" ,)}]|"))
	{
		if( i3String::IsDigitString( szToken))
		{
			REAL32 val = (REAL32) atof( szToken);

			switch( NumCount)
			{
				case 0 :		setX( &m_value, val);	break;
				case 1 :		setY( &m_value, val);	break;
				case 2 :		setZ( &m_value, val);	break;
				case 3 :		setW( &m_value, val);	break;
				default :					return;
			}

			NumCount++;
		}
	}
}




///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_COORD_SOURCE Property 
CpropertyTextureCS::CpropertyTextureCS()
{
	m_Style |= I3_LIST; 
}

CpropertyTextureCS::~CpropertyTextureCS()
{

}

void CpropertyTextureCS::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Vertex") == 0)						m_value = I3G_TEXTURE_COORD_SOURCE_VERTEX;
	else if(i3String::Compare((char *)pszValueStr,"CameraSpacePosition") == 0)		m_value = I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION;
	else if(i3String::Compare((char *)pszValueStr,"CameraSpaceNormal") == 0)		m_value = I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL;
	else if(i3String::Compare((char *)pszValueStr,"CameraSpaceReflection") == 0)	m_value = I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION;
}

void CpropertyTextureCS::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_TEXTURE_COORD_SOURCE_VERTEX					: strcpy( pszValueStr, "Vertex");					break; 
	case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		: strcpy( pszValueStr, "CameraSpacePosition");		break; 
	case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		: strcpy( pszValueStr, "CameraSpaceNormal");		break; 
	case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	: strcpy( pszValueStr, "CameraSpaceReflection");	break; 
	}
}

void CpropertyTextureCS::GetValueList(i3StringList * pList)
{
	pList->Add("Vertex");
	pList->Add("CameraSpacePosition");
	pList->Add("CameraSpaceNormal");
	pList->Add("CameraSpaceReflection");
}

void CpropertyTextureCS::OnChangeList(void)
{
	//미정
}


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_FUNCTION Property 
CpropertyTextureFunc::CpropertyTextureFunc()
{
	m_Style |= I3_LIST; 
}

CpropertyTextureFunc::~CpropertyTextureFunc()
{

}

void CpropertyTextureFunc::Set(const char * pszValueStr)
{
	if(i3String::Compare((char *)pszValueStr,"Replace") == 0)					m_value = I3G_TEXTURE_FUNC_REPLACE;			
	else if(i3String::Compare((char *)pszValueStr,"Modulate") == 0)				m_value = I3G_TEXTURE_FUNC_MODULATE;			
	else if(i3String::Compare((char *)pszValueStr,"Add") == 0)					m_value = I3G_TEXTURE_FUNC_ADD;			
	else if(i3String::Compare((char *)pszValueStr,"SpecularHighlight") == 0)	m_value = I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT;			
}

void CpropertyTextureFunc::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3G_TEXTURE_FUNC_REPLACE			: strcpy( pszValueStr, "Replace");break; 
	case I3G_TEXTURE_FUNC_MODULATE			: strcpy( pszValueStr, "Modulate");break; 
	case I3G_TEXTURE_FUNC_ADD				: strcpy( pszValueStr, "Add");break; 
	case I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT: strcpy( pszValueStr, "SpecularHighlight");break; 
	}
}

void CpropertyTextureFunc::GetValueList(i3StringList * pList)
{
	pList->Add("Replace");
	pList->Add("Modulate");
	pList->Add("Add");
	pList->Add("SpecularHighlight");
}

void CpropertyTextureFunc::OnChangeList(void)
{
	//미정
}

//20053.01.05 raja
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// for  CpropertyStencilOP
CpropertyStencilOP::CpropertyStencilOP()
{
	m_Style |= I3_LIST;
}
CpropertyStencilOP::~CpropertyStencilOP()
{

}
void CpropertyStencilOP::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case	I3G_STENCIL_OP_KEEP			:	strcpy(pszValueStr,"Keep");			break;
	case	I3G_STENCIL_OP_ZERO			:	strcpy(pszValueStr,"Zero");			break;
	case	I3G_STENCIL_OP_REPLACE		:	strcpy(pszValueStr,"Replace");		break;
	case	I3G_STENCIL_OP_INC_CLAMP	:	strcpy(pszValueStr,"INC_Clamp");	break;
	case	I3G_STENCIL_OP_DEC_CLAMP	:	strcpy(pszValueStr,"DEC_Clamp");	break;
	case	I3G_STENCIL_OP_INVERT		:	strcpy(pszValueStr,"Invert");		break;
	case	I3G_STENCIL_OP_INC			:	strcpy(pszValueStr,"INC");			break;
	case	I3G_STENCIL_OP_DEC			:	strcpy(pszValueStr,"DEC");			break;
	}
}
void CpropertyStencilOP::Set(const char * pszValueStr)
{
	if(i3String::Compare(pszValueStr,"Keep") == 0)			m_value = I3G_STENCIL_OP_KEEP;
	else if(i3String::Compare(pszValueStr,"Zero") == 0)		m_value = I3G_STENCIL_OP_ZERO;
	else if(i3String::Compare(pszValueStr,"Relplace") == 0)	m_value = I3G_STENCIL_OP_REPLACE;
	else if(i3String::Compare(pszValueStr,"INC_Clamp") == 0)m_value = I3G_STENCIL_OP_INC_CLAMP;
	else if(i3String::Compare(pszValueStr,"DEC_Clamp") == 0)m_value = I3G_STENCIL_OP_DEC_CLAMP;
	else if(i3String::Compare(pszValueStr,"Invert") == 0)	m_value = I3G_STENCIL_OP_INVERT;
	else if(i3String::Compare(pszValueStr,"INC") == 0)		m_value = I3G_STENCIL_OP_INC;
	else if(i3String::Compare(pszValueStr,"DEC") == 0)		m_value = I3G_STENCIL_OP_DEC;
}

void CpropertyStencilOP::GetValueList(i3StringList * pList)
{
	pList->Add("Keep");
	pList->Add("Zero");
	pList->Add("Replace");
	pList->Add("INC_Clamp");
	pList->Add("DEC_Clamp");
	pList->Add("Invert");
	pList->Add("INC");
	pList->Add("DEC");
}
void CpropertyStencilOP::OnChangeList(void)
{
	//
}



///////////////////////////////////////////////////////////////////////////////////////////
//FOR I3_P_SET_NODEFLAG
CPropertySetNodeFlag::CPropertySetNodeFlag()
{
	m_Style |= I3_DIALOG;
}
CPropertySetNodeFlag::~CPropertySetNodeFlag()
{

}
void CPropertySetNodeFlag::Get( char * pszValueStr)
{
	char	cbuf[12];
	wsprintf(cbuf,"0x%lX",m_value);
	strcpy(pszValueStr,cbuf); 
}
void CPropertySetNodeFlag::Set( const char * pszValueStr)
{

}
void CPropertySetNodeFlag::PopUpDialog(void)
{
	CSetNodeFlagDlg	FlagDlg;
	FlagDlg.m_nFlag = m_value;

	if(IDOK == FlagDlg.DoModal())
	{
		m_value = FlagDlg.m_nFlag;
	}
}

/////////////////////////////////////////////////////////////////////////////////
///FOR I3_P_BILLBOARDINFO - space
CPropertyBillboardSpace::CPropertyBillboardSpace()
{
	m_Style |= I3_LIST;
}
CPropertyBillboardSpace::~CPropertyBillboardSpace()
{

}
void CPropertyBillboardSpace::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3SG_BILLBOARD_SPACE_WORLD:	strcpy(pszValueStr,"World");	break;
	case I3SG_BILLBOARD_SPACE_SCREEN:	strcpy(pszValueStr,"Screen");	break;
	}
}
void CPropertyBillboardSpace::Set(const char * pszValueStr)
{
	if(i3String::Compare(pszValueStr,"World") == 0)	m_value = I3SG_BILLBOARD_SPACE_WORLD;
	else if(i3String::Compare(pszValueStr,"Screen") == 0)	m_value = I3SG_BILLBOARD_SPACE_SCREEN;
}

void CPropertyBillboardSpace::GetValueList(i3StringList * plist)
{
	plist->Add("World");
	plist->Add("Screen");
}
void CPropertyBillboardSpace::OnChangeList(void)
{

}
/*
//////////////////////////////////////////////////////////////////////////////////////
///FOR I3_P_BILLBOARDINFO - Editmask
CPropertyBillboard_EditMask::CPropertyBillboard_EditMask()
{
	m_Style |= I3_LIST;
}
CPropertyBillboard_EditMask::~CPropertyBillboard_EditMask()
{

}
void CPropertyBillboard_EditMask::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case I3SG_BILLBOARD_EDIT_POSITION:	strcpy(pszValueStr,"Position");	break;
	case I3SG_BILLBOARD_EDIT_UV:		strcpy(pszValueStr,"UV");		break;
	case I3SG_BILLBOARD_EDIT_COLOR:		strcpy(pszValueStr,"Color");	break;
	case I3SG_BILLBOARD_EDIT_AXIS:		strcpy(pszValueStr,"Axis");		break;
	case I3SG_BILLBOARD_EDIT_ENABLE:	strcpy(pszValueStr,"Enable");	break;
	}
}

void CPropertyBillboard_EditMask::Set(const char * pszValueStr)
{
	if(i3String::Compare(pszValueStr,"Position") == 0)	m_value = I3SG_BILLBOARD_EDIT_POSITION;
	else if(i3String::Compare(pszValueStr,"UV") == 0)	m_value = I3SG_BILLBOARD_EDIT_UV;
	else if(i3String::Compare(pszValueStr,"Color") == 0 ) m_value = I3SG_BILLBOARD_EDIT_COLOR;
	else if(i3String::Compare(pszValueStr,"Axis" ) == 0 ) m_value = I3SG_BILLBOARD_EDIT_AXIS;
	else if(i3String::Compare(pszValueStr,"Enable") == 0 ) m_value = I3SG_BILLBOARD_EDIT_ENABLE;
}

void CPropertyBillboard_EditMask::GetValueList(i3StringList * plist)
{
	plist->Add("Position");
	plist->Add("UV");
	plist->Add("Color");
	plist->Add("Axis");
	plist->Add("Enable");
}
void CPropertyBillboard_EditMask::OnChangeList(void)
{

}
//end raja
//*/




///////////////////////////////////////////////////////////////////////////////////////////
/////// CpropertyCollider Property  
CpropertyColliderStyle::CpropertyColliderStyle()
{
	m_Style = I3_DIALOG;
}

CpropertyColliderStyle::~CpropertyColliderStyle()
{
}

void CpropertyColliderStyle::Get( char * pszValueStr)
{
	sprintf(pszValueStr,"%d", m_pColliderSet->GetStyle());	
}

void CpropertyColliderStyle::Set( const char * pszValueStr)
{
	
}

void CpropertyColliderStyle::PopUpDialog(void)
{
	cColliderStyle StyleDlg; 
	StyleDlg.SetColliderSet(m_pColliderSet); 
	if(IDOK == StyleDlg.DoModal())
	{
		m_pColliderSet->SetStyle(StyleDlg.GetStyle()); 
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
/////// Terrain Property  
CpropertyTerrainType::CpropertyTerrainType()
{
	m_Style |= I3_LIST;
}
CpropertyTerrainType::~CpropertyTerrainType()
{

}
void CpropertyTerrainType::Get(char * pszValueStr)
{
	switch(m_value)
	{
	case	I3_TERRAIN_CONCRETE			:	strcpy(pszValueStr,"Concreate");	break;
	case	I3_TERRAIN_STEEL			:	strcpy(pszValueStr,"Steel");		break;
	case	I3_TERRAIN_GROUND			:	strcpy(pszValueStr,"Ground");		break;
	case	I3_TERRAIN_WOOD				:	strcpy(pszValueStr,"Wood");			break;
	case	I3_TERRAIN_SNOW				:	strcpy(pszValueStr,"Snow");			break;
	case	I3_TERRAIN_WATER_DEEP		:	strcpy(pszValueStr,"Water_Deep");	break;
	case	I3_TERRAIN_WATER_SHALLOW	:	strcpy(pszValueStr,"Water_Shallow");break;
	case	I3_TERRAIN_WET_CLAY			:	strcpy(pszValueStr,"WetClay");		break;
	case	I3_TERRAIN_GRASS			:	strcpy(pszValueStr,"Grass");		break;
	case	I3_TERRAIN_MARBLE			:	strcpy(pszValueStr,"Marble");		break;
	}
}
void CpropertyTerrainType::Set(const char * pszValueStr)
{
	if(i3String::Compare(pszValueStr,"Concreate") == 0)			m_value = I3_TERRAIN_CONCRETE	;	
	else if(i3String::Compare(pszValueStr,"Steel") == 0)		m_value = I3_TERRAIN_STEEL		;
	else if(i3String::Compare(pszValueStr,"Ground") == 0)		m_value = I3_TERRAIN_GROUND		;
	else if(i3String::Compare(pszValueStr,"Wood") == 0)			m_value = I3_TERRAIN_WOOD		;	
	else if(i3String::Compare(pszValueStr,"Snow") == 0)			m_value = I3_TERRAIN_SNOW		;	
	else if(i3String::Compare(pszValueStr,"Water_Deep") == 0)	m_value = I3_TERRAIN_WATER_DEEP	;
	else if(i3String::Compare(pszValueStr,"Water_Shallow") == 0)m_value = I3_TERRAIN_WATER_SHALLOW;
	else if(i3String::Compare(pszValueStr,"WetClay") == 0)		m_value = I3_TERRAIN_WET_CLAY	;	
	else if(i3String::Compare(pszValueStr,"Grass") == 0)		m_value = I3_TERRAIN_GRASS		;
	else if(i3String::Compare(pszValueStr,"Marble") == 0)		m_value = I3_TERRAIN_MARBLE		;
}

void CpropertyTerrainType::GetValueList(i3StringList * pList)
{
	pList->Add("Concreate");
	pList->Add("Steel");
	pList->Add("Ground");
	pList->Add("Wood");
	pList->Add("Snow");
	pList->Add("Water_Deep");
	pList->Add("Water_Shallow");
	pList->Add("WetClay");
	pList->Add("Grass");
	pList->Add("Marble");
}
void CpropertyTerrainType::OnChangeList(void)
{
	//
}



