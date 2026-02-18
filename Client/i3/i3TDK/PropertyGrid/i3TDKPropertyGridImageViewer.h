#pragma once
#include "../i3TDKImageViewCtrl.h"

// Image Property에 있는 버튼을 눌렀을 때 창을 띄워 이미지를 보여주기 위한 클래스
class I3_EXPORT_TDK i3TDKPropertyGridImageViewer : public i3TDKImageViewCtrl
{
	DECLARE_DYNAMIC( i3TDKPropertyGridImageViewer)

private:
	bool		m_bCreated = false;

protected:
	DECLARE_MESSAGE_MAP()

public:
	i3TDKPropertyGridImageViewer() {}
	virtual ~i3TDKPropertyGridImageViewer() {}

	bool			IsCreated( void)		{	return m_bCreated;	}

	HDC				GetMemDC( void)			{	return m_MemDC;	}
	HBITMAP			GetHBitmap( void)		{	return m_hBitmap;	}

	UINT32			GetImageWidth( void);
	UINT32			GetImageHeight( void);
	const char*		GetImageFormatText( void);
	
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnDropFiles(HDROP hDropInfo);

	void			SetShowWindow( bool bShow);
	//afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};