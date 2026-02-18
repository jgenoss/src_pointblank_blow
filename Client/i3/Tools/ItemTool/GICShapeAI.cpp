#include "stdafx.h"
#include "GICShapeAI.h"

#include "AIGraphView.h"
#include "i3Base/i3StringListBuffer.h"
#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE( GICShapeAI); //, i3GICShapeRect);

static HFONT		s_hFont = NULL;

#define		NORMAL_COLOR			RGB( 255, 255, 128)
#define		TEMPLATE_COLOR			RGB( 192, 192, 255)
#define		ERROR_COLOR				RGB( 255, 128, 128)

GICShapeAI::GICShapeAI(void)
{
	m_pAIState = NULL;

	if( s_hFont == NULL)
	{
		s_hFont = i3TDK::CreateDefFont( 32, "Tahoma");
	}

	m_hbrBlendFill = NULL;
	m_blendFillColor = RGB( 255, 170, 80);
	
	setFillColor( NORMAL_COLOR);
	m_bTemplateAI = false;

	m_bError = false;
}

GICShapeAI::~GICShapeAI(void)
{
	I3_SAFE_RELEASE( m_pAIState);

	_DeleteGDIBlendFillObject();
}

void GICShapeAI::setAIState( i3AIState * pState)
{
	I3_REF_CHANGE( m_pAIState, pState);
}

void GICShapeAI::_CreateGDIBlendFillObject()
{
	I3ASSERT( m_hbrBlendFill == NULL);

	m_hbrBlendFill = ::CreateSolidBrush( m_blendFillColor);
	I3ASSERT( m_hbrBlendFill != NULL);
}

void GICShapeAI::_DeleteGDIBlendFillObject()
{
	if( m_hbrBlendFill != NULL)
	{
		::DeleteObject( m_hbrBlendFill);
		m_hbrBlendFill = NULL;
	}
}

void GICShapeAI::OnDetach( i3GICDiagram * pDiagram)
{

}

bool	GICShapeAI::OnPreSize( REAL32 cx, REAL32 cy)
{
	return i3GICShapeRect::OnPreSize( cx, cy);
}

void GICShapeAI::setErrorState( bool bState)
{
	m_bError = bState;

	if( m_bError)
	{
		setFillColor( ERROR_COLOR);
	}
	else
	{
		m_bTemplateAI = !(m_pAIState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE);
	}
}

void	GICShapeAI::OnDraw( i3GICDiagramContext * pCtx)
{
	CRect rt;

	CAIGraphView * pView = (CAIGraphView*) pCtx->getView();
	if( pView == NULL)
		return;

//	i3GICDeviceContext * pGDC = pView->getGDC();
	//pGDC->setDC( pView->getDrawDC());
	//pGDC->reset( getX( pCtx->getStackTop()), getY( pCtx->getStackTop()));

	//if( pGDC == NULL)
	//	return;

	if( (m_hpenBorder != NULL) &&
		(m_hbrFill != NULL) &&
		(m_hbrBlendFill != NULL) )
	{

		bool bTemplate = false;
		
		if( m_pAIState != NULL)
			bTemplate =  (m_pAIState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) != 0;

//		pGDC->SelectObject( m_hpenBorder);
//		pGDC->SelectObject( s_hFont);

		INT32 _x1, _y1, _x2, _y2;

		_x1 = (INT32) getPosX();
		_y1 = (INT32) getPosY();
		_x2 = (INT32) (getPosX() + getWidth());
		_y2 = (INT32) (getPosY() + getHeight());

		if( m_pAIState != NULL && m_pAIState->getBlendTime() > 0.0f)
		{
			//pGDC->SelectObject( m_hbrBlendFill);

			_x2 = _x1 + (INT32)( m_pAIState->getBlendTime() * 100.0f);			// / 0.01

//			pGDC->Rectangle( _x1, _y1, _x2 + 2, _y2);

			_x1 = _x2;
			_x2 = (INT32) (getPosX() + getWidth());
		}

		if( m_pAIState != NULL)
		{
			if( m_bError == false)
			{
				if( bTemplate != m_bTemplateAI)
				{
					m_bTemplateAI = bTemplate;

					if( m_bTemplateAI == false)
						setFillColor( NORMAL_COLOR);
					else
						setFillColor( TEMPLATE_COLOR);
				}	
			}
		}

// 		pGDC->SelectObject( m_hbrFill);
// 		pGDC->Rectangle( _x1, _y1, _x2, _y2);
// 
// 		pGDC->SetBkMode( TRANSPARENT);
// 
// 		pGDC->MoveTo( _x1, _y1 + 40);
// 		pGDC->LineTo( _x2, _y1 + 40);

		// Name
		rt.SetRect( _x1 + 3, _y1 + 3, _x2 - 3, _y1 + 39);

		if((m_pAIState != NULL) && ( m_pAIState->hasName()))
		{
			//pGDC->DrawText( m_pAIState->GetName(), &rt, DT_END_ELLIPSIS | DT_CENTER | DT_SINGLELINE);
		}
		else
		{
			//pGDC->DrawText( "<NONAMED>", &rt, DT_END_ELLIPSIS | DT_CENTER | DT_SINGLELINE);
		}

		// Animation
		rt.SetRect( _x1 + 3, _y1 + 41, _x2 - 3, _y2 - 3);
		
		if( m_pAIState != NULL)
		{
			char conv[256] = "(null)";

			if( bTemplate == false)
			{
				i3Animation * pAnim = (i3Animation *) m_pAIState->getAnim();

				if( m_pAIState->getAnim() != NULL)
				{
				//	i3String::SplitFileName( pAnim->GetName(), conv, FALSE);
					i3::extract_filetitle(pAnim->GetName(), conv);
				}
			}
			else
			{
			//	i3String::SplitFileName( m_pAIState->getAnimName(), conv, FALSE);
				i3::extract_filetitle(m_pAIState->getAnimName(), conv);
			}

			//pGDC->DrawText( conv, &rt, DT_END_ELLIPSIS | DT_CENTER | DT_SINGLELINE);
		}

		i3GICShape::OnDraw( pCtx);
	}
}

void	GICShapeAI::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pAIState != NULL)
	{
		m_pAIState->OnBuildObjectList( List);
	}

	i3GICShapeRect::OnBuildObjectList( List);
}

#pragma pack( push, 8)

typedef struct _tagPersistGICShapeAI
{
	UINT8			m_ID[4];
	OBJREF64		m_pAIState;
	UINT32			m_SrcTextSize;
	UINT32			m_CommentTextSize;
	UINT32			pad[30];
} GIC_PERSIST_SHAPE_AI;

UINT32	GICShapeAI::OnSave( i3ResourceFile * pResFile)
{
	GIC_PERSIST_SHAPE_AI data;
	UINT32 Rc, Result;

	Result = i3GICShapeRect::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "GAI1", 4);
	data.m_pAIState = (OBJREF64) pResFile->GetObjectPersistID( m_pAIState);
	data.m_SrcTextSize = GetStringListBufferSize(m_SrcText);	//	m_SrcText.GetBufferSize();
	data.m_CommentTextSize = GetStringListBufferSize(m_CommentText); //  m_CommentText.GetBufferSize();

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Src Text
	if( data.m_SrcTextSize > 0)
	{
		char * pBuf = new char[ data.m_SrcTextSize + 1];
		I3ASSERT( pBuf != NULL);

	//	m_SrcText.CopyToBuffer( pBuf, data.m_SrcTextSize + 1);
		CopyFromStringListToBuffer(m_SrcText,  pBuf, data.m_SrcTextSize + 1);

		Rc = pResFile->Write( pBuf, data.m_SrcTextSize);
		I3_CHKIO( Rc);

		delete pBuf;
	}

	// Comment Text
	if( data.m_CommentTextSize > 0)
	{
		char * pBuf = new char[ data.m_CommentTextSize + 1];
		I3ASSERT( pBuf != NULL);

		//m_CommentText.CopyToBuffer( pBuf, data.m_CommentTextSize + 1);
		CopyFromStringListToBuffer(m_CommentText, pBuf, data.m_CommentTextSize + 1);

		Rc = pResFile->Write( pBuf, data.m_CommentTextSize);
		I3_CHKIO( Rc);

		delete pBuf;
	}

	return Result;
}

UINT32	GICShapeAI::OnLoad( i3ResourceFile * pResFile)
{
	GIC_PERSIST_SHAPE_AI data;
	UINT32 Rc, Result;

	Result = i3GICShapeRect::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pAIState != 0)
	{
		i3AIState * pState = (i3AIState *) pResFile->FindObjectByID( (OBJREF) data.m_pAIState);

		setAIState( pState);
	}

	// Src Text
	if( data.m_SrcTextSize > 0)
	{
		char * pBuf = new char[ data.m_SrcTextSize + 1];
		I3ASSERT( pBuf != NULL);

		Rc = pResFile->Read( pBuf, data.m_SrcTextSize);
		I3_CHKIO( Rc);

	//	m_SrcText.CopyFromBuffer( pBuf);
		CopyFromBufferToStringList(m_SrcText, pBuf);

		delete pBuf;
	}

	// Comment Text
	if( data.m_CommentTextSize > 0)
	{
		char * pBuf = new char[ data.m_CommentTextSize + 1];
		I3ASSERT( pBuf != NULL);

		Rc = pResFile->Read( pBuf, data.m_CommentTextSize);
		I3_CHKIO( Rc);

	//	m_CommentText.CopyFromBuffer( pBuf);
		CopyFromBufferToStringList(m_CommentText, pBuf);

		delete pBuf;
	}

	return Result;
}

/*virtual*/ void GICShapeAI::OnBindDiagram( bool bBind)
{
	i3GICShapeRect::OnBindDiagram( bBind);

	if( bBind)	_CreateGDIBlendFillObject();
	else		_DeleteGDIBlendFillObject();
}

