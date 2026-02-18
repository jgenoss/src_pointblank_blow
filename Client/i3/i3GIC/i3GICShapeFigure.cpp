#include "stdafx.h"
#include "i3GICShapeFigure.h"
#include "i3GICDiagramContext.h"
#include "i3GICProperty.h"
#include "i3GICColorProperty.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeFigure);

i3GICShapeFigure::i3GICShapeFigure()
{
	setBorderColor( RGB( 0, 0, 0));
	setFillColor( ::GetSysColor(COLOR_3DFACE));
}

i3GICShapeFigure::~i3GICShapeFigure()
{
	_DeleteGDIFillObject();
	_DeleteGDIBorderObject();
}

void i3GICShapeFigure::_SelectPenAndBrush( i3GICDiagramContext * pCtx)
{
	if( m_hpenBorder != nullptr)
	{
		if( isStyle( I3GIC_STYLE_NOBORDERLINE))
			pCtx->selectObject( ::GetStockObject( NULL_PEN));
		else
			pCtx->selectObject( m_hpenBorder);

		if( isStyle( I3GIC_STYLE_NOFILL))
			pCtx->selectObject( ::GetStockObject( NULL_BRUSH));
		else
			pCtx->selectObject( m_hbrFill);
	}
}

void i3GICShapeFigure::_CreateGDIFillObject( void)
{
	I3ASSERT( m_hbrFill == nullptr);

	m_hbrFill = ::CreateSolidBrush( m_FillColor);
	I3ASSERT( m_hbrFill != nullptr);
}

void i3GICShapeFigure::_DeleteGDIFillObject( void)
{
	if( m_hbrFill != nullptr)
	{
		::DeleteObject( m_hbrFill);
		m_hbrFill = nullptr;
	}
}

void i3GICShapeFigure::_CreateGDIBorderObject( void)
{
	I3ASSERT( m_hpenBorder == nullptr);

	m_hpenBorder = ::CreatePen( m_BorderStyle, m_uBorderSize, m_BorderColor);
	I3ASSERT( m_hpenBorder != nullptr);
}

void i3GICShapeFigure::_DeleteGDIBorderObject( void)
{
	if( m_hpenBorder != nullptr)
	{
		::DeleteObject( m_hpenBorder);
		m_hpenBorder = nullptr;
	}
}

/*virtual*/ void i3GICShapeFigure::OnBindDiagram( bool bBind)
{
	i3GICShape::OnBindDiagram( bBind);

	if( bBind)
	{
		_CreateGDIFillObject();
		_CreateGDIBorderObject();
	}
	else
	{
		_DeleteGDIFillObject();
		_DeleteGDIBorderObject();
	}
}

/*virtual*/ void i3GICShapeFigure::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShape::CopyTo( pDest, method);

	i3GICShapeFigure * pDestShape = (i3GICShapeFigure*) pDest;

	pDestShape->setBorderStyle( getBorderStyle());
	pDestShape->setBorderColor( getBorderColor());
	pDestShape->setBorderSize( getBorderSize());
	pDestShape->setFillColor( getFillColor());
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct GIC_SHAPEFIGURE
	{
		UINT8		m_ID[4] = { 'G', 'S', 'F', '1' };
		UINT32		m_BorderStyle = 0;
		UINT32		m_BorderColor = 0;
		UINT32		m_uBorderSize = 0;
		UINT32		m_FillColor = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

/*virtual*/ UINT32 i3GICShapeFigure::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::GIC_SHAPEFIGURE info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShape::OnSave( pResFile);
	I3_CHKIO( Rc);
	//Result += Rc;
	
	info.m_BorderStyle = (UINT32) m_BorderStyle;
	info.m_BorderColor = (UINT32) m_BorderColor;
	info.m_uBorderSize = m_uBorderSize;
	info.m_FillColor = m_FillColor;

	Rc = pStream->Write( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3GICShapeFigure::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::GIC_SHAPEFIGURE info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShape::OnLoad( pResFile);
	I3_CHKIO( Rc);
	//Result += Rc;

	UINT8 Ver[4];
	UINT32 cur = pStream->GetPosition();

	Rc = pStream->Read( Ver, sizeof( Ver));
	I3_CHKIO( Rc);

	pStream->SetPosition( cur);

	if( Ver[0] == 'G' && Ver[1] == 'S' && Ver[2] == 'F' && Ver[3] == '1')
	{	// New
		Rc = pStream->Read( &info, sizeof( info));
		I3_CHKIO( Rc);
		Result += Rc;

		m_BorderStyle = (GIC_SHAPE_BORDER_STYLE) info.m_BorderStyle;
		m_BorderColor = (COLORREF) info.m_BorderColor;
		setBorderSize( info.m_uBorderSize);
		setFillColor( info.m_FillColor);
	}
	else
	{	// Old
		setFillColor( (COLORREF) getDepth());
		setBorderSize( 1);
		m_BorderColor = 0;
	}

	return Result;
}

/*virtual*/ bool i3GICShapeFigure::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShape::OnSaveXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  _T("Figure data Save fail.") );
		return false;
	}

	pXML->addAttr( _T("BorderStyle"), (UINT32) getBorderStyle());
	pXML->addAttr( _T("BorderColor"), (UINT32) getBorderColor());
	pXML->addAttr( _T("BorderSize"), getBorderSize());
	pXML->addAttr( _T( "FillColor"), (UINT32) getFillColor());

	return true;
}

/*virtual*/ bool i3GICShapeFigure::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShape::OnLoadXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  _T( "Figure data load fail."));
		return false;
	}

	UINT32 uTemp;

	pXML->getAttr( _T("BorderStyle"), &uTemp);
	setBorderStyle( (GIC_SHAPE_BORDER_STYLE) uTemp);

	pXML->getAttr( _T("BorderColor"), &uTemp);
	setBorderColor( (COLORREF) uTemp);

	pXML->getAttr( _T("BorderSize"), &uTemp);
	setBorderSize( uTemp);

	pXML->getAttr( _T("FillColor"), &uTemp);
	setFillColor( (COLORREF) uTemp);

	return true;
}


/*virtual*/ void i3GICShapeFigure::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShape::OnProperty( pCtrl);

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Figure"));

	// Fill bk flag
	COleVariant varFillRect( (short)(!isStyle( I3GIC_STYLE_NOFILL)), VT_BOOL );
	i3GICProperty * pPropFill = new i3GICProperty( _T( "Fill Bk"), varFillRect);
	pPropFill->setShape( this);
	pPropFill->setUpdateType( GIC_PROPERTY_UPDATE_FILL);
	pProperty->AddSubItem( pPropFill);

	// Color
	i3GICColorProperty * pPropFillColor = new i3GICColorProperty( _T("Fill Color"), getFillColor() );
	pPropFillColor->EnableOtherButton( _T("Other"));
	pPropFillColor->EnableAutomaticButton( _T("Default"), ::GetSysColor(COLOR_3DFACE));
	pPropFillColor->setShape( this);
	pProperty->AddSubItem( pPropFillColor);

	// Border Style
	COleVariant varStyle( _T("Solid"));
	i3GICProperty * pPropBorderStyle = new i3GICProperty( _T( "Border Style"), varStyle, _T( "테두리 선의 종류입니다."));
	pPropBorderStyle->setShape( this);
	pPropBorderStyle->setUpdateType( GIC_PROPERTY_UPDATE_BORDERSTYLE);
	pPropBorderStyle->AddOption( _T("Solid"));
	pPropBorderStyle->AddOption( _T("Dash"));
	pPropBorderStyle->AddOption( _T("Dot"));
	pPropBorderStyle->AddOption( _T("DashDot"));
	pPropBorderStyle->AddOption( _T("DashDotDot"));

	varStyle.SetString( pPropBorderStyle->GetOption( (int) getBorderStyle()), VT_BSTR);
	pPropBorderStyle->SetValue( varStyle);

	pProperty->AddSubItem( pPropBorderStyle);

	// Border size
	COleVariant varSize( (long)m_uBorderSize, VT_UI4);
	i3GICProperty * pPropBorderSize = new i3GICProperty( _T( "Border Size"), varSize, _T( "테두리의 두께입니다."));
	pPropBorderSize->setShape( this);
	pPropBorderSize->setUpdateType( GIC_PROPERTY_UPDATE_BORDERSIZE);
	pPropBorderSize->EnableSpinControl( TRUE, 0, 10);
	pProperty->AddSubItem( pPropBorderSize);

	//  border Color
	i3GICColorProperty * pPropColor = new i3GICColorProperty( _T("Border Color"), getBorderColor() );
	pPropColor->EnableOtherButton( _T("Other"));
	pPropColor->EnableAutomaticButton( _T("Default"), ::GetSysColor(COLOR_3DFACE));
	pPropColor->setShape( this);
	pPropColor->setUpdateType( GIC_PROPERTY_UPDATE_COLOR_BORDER);
	pProperty->AddSubItem( pPropColor);

	pCtrl->AddProperty( pProperty);
}

#endif	// I3_DEBUG

