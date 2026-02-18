#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_IMAGE_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_IMAGE_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty_String.h"
#include "i3TDKPropertyGridImageViewer.h"

// 이미지의 경로를 표시하고, 1개의 찾아보기 버튼과 1개의 크게보기 버튼을 가진 프로퍼티
class I3_EXPORT_TDK i3TDKPropertyGridProperty_Image : public i3TDKPropertyGridProperty_String
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Image)

private:
	i3Texture						**	m_ppTexture = nullptr;
	i3TDKPropertyGridImageViewer		m_ImageViewer;

	CImageList							m_ImageList;	//Button Image
		
protected:
	virtual BOOL	HasButton() const { return TRUE; }
	virtual void	AdjustButtonRect();
	virtual void	OnClickButton(CPoint point);
	virtual void	OnDrawButton(CDC* pDC, CRect rectButton);

	void			OnButtonLeft( void);
	void			OnButtonRight( void);

	void			LoadImage( void);

	
public:
	i3TDKPropertyGridProperty_Image( PROPERTYINFO* pInfo);
	
	virtual void UpdateFromProperty( void* pData = nullptr);

	HDC				GetViewerDC( void)		{	return m_ImageViewer.GetMemDC();	}
	HBITMAP			GetViewerBitmap( void)	{	return m_ImageViewer.GetHBitmap();	}
	bool			IsViewerCreated( void)	{	return m_ImageViewer.IsCreated();	}

	UINT32			GetImageWidth( void)	{	return m_ImageViewer.GetImageWidth();	}
	UINT32			GetImageHeight( void)	{	return m_ImageViewer.GetImageHeight();	}

	void			CreateViewer( void);

	i3TDKPropertyGridImageViewer*		GetImageViewer( void)		{	return &m_ImageViewer;	}

	//virtual void OnDrawDescription( CDC* pDC, CRect rect);
	//virtual BOOL OnSetCursor() const;
};

#endif

#endif