#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_IMAGEGROUP_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_IMAGEGROUP_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty_Group.h"

class i3TDKPropertyGridProperty_Image;
class i3TDKPropertyGridImageViewer;

// 이 프로퍼티 아래에는 1개의 Image Property와 n개의 Empty Property가 들어간다.
class I3_EXPORT_TDK i3TDKPropertyGridProperty_ImageGroup : public i3TDKPropertyGridProperty_Group
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_ImageGroup)

private:
	i3TDKPropertyGridCtrl*				m_pCtrl;		// Window(GridCtrl) Handle [initialize at constructor]
	i3TDKPropertyGridProperty_Image*	m_pImageProp;	// [initialize at constructor]

	INT32								m_RowCount;		// Number of Empty Properties [initialize at constructor]

	INT32								m_nImgh = 0;
	INT32								m_nImgw = 0;
	REAL32								m_fScale = 1.0f;
	

	// 흰색으로 칠해야하는 영역 == 화면에 보이는 Empty Property의 오른쪽 전영역 
	CRect								m_WhiteRect = { 0, 0, 0, 0 };

	// SrcImage를 표시하기 위해 필요한 영역( lefttop 0,0 기준)
	// m_RowCount와 ImageSize에만 dependent
	CRect								m_Rect = { 0, 0, 0, 0 };

	// 가려진 부분을 제외한 영역( 실제로 그려지는 영역)
	// header, description area, scroll bar 등에 의해서 가려질 수 있음
	CRect								m_DestRect = { 0, 0, 0, 0 };

	// 그려질 이미지의 영역 ( This is not always the same as a full size of an image)
	// m_Rect와 m_DestRect에 의해 결정됨
	CRect								m_SrcRect = { 0, 0, 0, 0 };
	
	void			_CalcRect( void);
	
protected:
	void	Init( void);

	// Empty Property의 Name영역에 표시될 이미지 정보를 설정한다. 
	void			SetInfoText( void);
public:
	i3TDKPropertyGridProperty_ImageGroup( PROPERTYINFO* pInfo, i3TDKPropertyGridCtrl* pCtrl, INT32 nRowCount = 5);
	virtual ~i3TDKPropertyGridProperty_ImageGroup();
	
	virtual void	UpdateFromProperty( void* pData = nullptr);

	// ** Rect를 업데이트 해준다 ** ( 중요! 제대로 안되어 있을 경우 이미지가 엉뚱한 영역을 덮어버린다.. ㅜ)
	virtual void	OnPosSizeChanged( CRect);

	// ** Rect에 이미지를 StretchBlt한다. ** ( 깜빡임 현상 좀 어떻게.. )
	virtual void	DrawImage( void);

	//virtual BOOL OnSetCursor() const;
	//virtual void OnDrawName( CDC* pDC, CRect rect);
	//virtual void OnDrawExpandBox( CDC* pDC, CRect rectExpand);
	//virtual void OnClickName( CPoint C);
};

#endif

#endif