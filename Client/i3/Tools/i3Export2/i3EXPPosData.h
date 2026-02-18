#if !defined( I3EXP_CUIPOSDATA_H)
#define I3EXP_CUIPOSDATA_H

class i3Export;

///////////////////////////////////////////////////////////////////////////////
//
// class BookmarkPosData
//
///////////////////////////////////////////////////////////////////////////////
class i3ExportPosData : public CUIPosData
{
protected:
	i3Export *			m_pExport;
	INT32				m_Width;
	INT32				m_Height;

public:
	i3ExportPosData( i3Export * pExport);

	void		SetSize( INT32 w, INT32 h)			
	{ 
		m_Width = w; 
		m_Height = h;
	}

	int	getWidth(void)							{ return m_Width; }
	int getHeight(void)							{ return m_Height; }

	int GetWidth(int sizeType, int orient);
	int GetHeight(int sizeType, int orient);
};

#endif
