#if !defined( __I3_GIC_SHAPE_TEXT_RECT_H__)
#define __I3_GIC_SHAPE_TEXT_RECT_H__

#include "i3GICShapeRect.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeTextRect : public i3GICShapeRect
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeTextRect, i3GICShapeRect);

private:
	// persist members
	char		m_szText[260] = { 0 };
	LOGFONTA		m_logFont;
	COLORREF	m_FontColor = RGB(0, 0, 0);
	COLORREF	m_FontBkColor = RGB(255, 255, 255);

	bool		m_bStretchToRect = false;
	bool		m_bBkFill = false;

	//Volatile
	HFONT		m_hFont = nullptr;

	UINT32		m_uSavedStretchH = -MulDiv(10, LOGPIXELSY, 32);

protected:
	void		_CreateGDIFontObject( void);
	void		_DeleteGDIFontObject( void);

	void		_CalcStretchRect( void);

public:
	void		setText( const char * pszStr)	{ i3::string_ncopy_nullpad( m_szText, pszStr, sizeof( m_szText)); }

	LOGFONTA *	getLogFont( void)				{ return &m_logFont; }

	const char * getFont( void)					{ return (const char*) m_logFont.lfFaceName; }
	void		setFont( const char * pszStr)
	{	
		i3::snprintf( m_logFont.lfFaceName, sizeof( m_logFont.lfFaceName), "%s\0", pszStr);

		if( isBindDiagram() )
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}

	UINT32		getFontSize( void)
	{
		return -MulDiv( m_logFont.lfHeight, 32, LOGPIXELSY);
	}

	void		setFontSize( UINT32 size)
	{
		m_logFont.lfHeight = -MulDiv( size, LOGPIXELSY, 32);
		
		if( isBindDiagram() )
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}

	COLORREF	getFontColor( void)					{ return m_FontColor; }
	void		setFontColor( COLORREF color)		{ m_FontColor = color; }

	COLORREF	getFontBkColor( void)				{ return m_FontBkColor; }
	void		setFontBkColor( COLORREF color)		{ m_FontBkColor = color; }

	UINT32		getFontWeight( void)				{ return m_logFont.lfWeight; }

	/** \brief Font Weight
		\param[in] weight = FW_DONTCARE, FW_THIN, FW_EXTRALIGHT, FW_LIGHT, FW_NORMAL, 
							FW_MEDIUM, FW_SEMIBOLD, FW_BOLD, FW_EXTRABOLD , FW_HEAVY
							(default FW_NORMAL)
		*/
	void		setFontWeight( UINT32 weight)
	{
		m_logFont.lfWeight = weight;

		if( isBindDiagram() )
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}

	BOOL		isItalic( void)						{ return (BOOL) m_logFont.lfItalic; }
	void		setItalic( BOOL bEnable)
	{
		m_logFont.lfItalic = (BYTE) bEnable;
		
		if( isBindDiagram() )
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}

	BOOL		isUnderLine( void)					{ return (BOOL) m_logFont.lfUnderline; }
	void		setUnderLine( BOOL bEnable)
	{
		m_logFont.lfUnderline = (BYTE) bEnable;
		
		if( isBindDiagram() )
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}

	bool		isStretchMode( void)				{ return m_bStretchToRect; }
	void		setStretchMode( bool bEnable)
	{
		if( bEnable)
		{
			m_uSavedStretchH = m_logFont.lfHeight;
			_CalcStretchRect(); 
		}
		else
		{
			m_logFont.lfHeight = m_uSavedStretchH;
			if( isBindDiagram() )
			{
				_DeleteGDIFontObject();
				_CreateGDIFontObject();
			}
		}

		m_bStretchToRect = bEnable;
	}

	bool		isBkOpaque( void)					{ return m_bBkFill; }
	void		setBkOpaque( bool bEnable)			{ m_bBkFill = bEnable; }

public:
	i3GICShapeTextRect();
	virtual ~i3GICShapeTextRect();

	virtual void	OnDraw( i3GICDiagramContext * pCtx);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
