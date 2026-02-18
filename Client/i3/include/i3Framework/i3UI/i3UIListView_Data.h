#ifndef __I3UI_LISTVIEW_DATA_H__
#define __I3UI_LISTVIEW_DATA_H__

#include "i3UIFont.h"
#include "i3UIListView_Defines.h"

#define	CELLDATA_SIZE	128

class i3UIListView_Cell;
typedef void (*I3UI_LISTVIEW_CELL_CALLBACK)( i3UIListView_Cell * pCell, void * pData);

struct LVCELL_DATA : i3::class_common_pool<LVCELL_DATA>
{
	i3::vector<i3::string> m_DataList;
	
	bool	m_bColorSet = false;
	COLOR	m_Col = { 255, 255, 255, 255 };
	I3UI_FONTINFO	m_FontInfo;
	I3UI_LISTVIEW_CELL_CALLBACK m_Func = nullptr;

	i3::wstring m_wstrToolTip;

	LVCELL_DATA()
	{
		m_FontInfo.m_strName = "System";
		m_FontInfo.m_Style = 0;
		m_FontInfo.m_Size = 10;
		m_FontInfo.m_Quality = I3UI_FONT_QUALITY_ANTIALIASED;
	}
};



////////////////////////////////////////////////////////////////////////
// 하나의 i3UIListView_Item에 대응하는 data
// 익스포트추가->다시 제거. (12.09.19.수빈)

class LVITEM_DATA	: public i3ElementBase
{
	I3_CLASS_DEFINE( LVITEM_DATA, i3ElementBase );

private:
	INT32			m_nCellCount = 0;		// Cell 개수
	LVCELL_DATA*	m_pCells = nullptr;	// Cell 개수 만큼의 LVCELL_DATA // 반드시 Init()을 콜해줘야 한다..
	INT32			m_nIdx = 0;
	bool			m_bSelected = false;
	bool			m_bEnabled = true;			//default로 true

public:
	virtual ~LVITEM_DATA();

	void		Init( INT32 nCellCount);

	INT32		getCellCount( void)		{	return m_nCellCount;	}
	
	INT32		getIdx( void)			{	return m_nIdx;			}
	INT32		SetIdx( INT32 nIdx)		{	m_nIdx = nIdx;			}

	//CellData 정보를 pData에 복사한다.
	//주위 : pDataOfSize의 크기는 pData의 메모리 버퍼 크기며, 문자열 크기가
	//			128자 일 경우, pData의 메모리 버퍼 크기는 129(널값 포함 버퍼)가 되어야 한다.
	//			즉, 실제 메모리 버퍼보다 -1로 적어야 한다.
	void		GetCellData( INT32 nCellIdx, char* pData, INT32 pDataOfSize);
	const LVCELL_DATA* GetCellData( INT32 nCellIdx) const;
	const i3::string* GetCellData( INT32 nCellIdx, INT32 nTemplateIdx) const;
	
	void		SetCellData( INT32 nCellIdx, const char* pData);
	bool		SetCellData(  INT32 nCellIdx, i3::vector<i3::string>& List);

	COLOR*		getCellColor( INT32 nCellIdx);
	void		setCellColor( INT32 nCellIdx, COLOR* pColor);
	void		restoreCellColor( INT32 nCellIdx);
	
	I3UI_FONTINFO * getFontInfo( INT32 nCellIdx);
	void		setFontInfo( INT32 nCellIdx, I3UI_FONTINFO * pInfo);

	I3UI_LISTVIEW_CELL_CALLBACK getFunc( INT32 nCellIdx);
	void		setFunc( INT32 nCellIdx, I3UI_LISTVIEW_CELL_CALLBACK func);

	void		SetSelected( bool bSel)	{	m_bSelected = bSel;	}
	bool		isSelected( void)		{	return m_bSelected;	}

	bool		isColorSet( INT32 nCellIdx);

	bool		isDisabled( void)			{	return !m_bEnabled;	}
	void		setEnable( bool bEnable)	{	m_bEnabled = bEnable;	}

	const WCHAR16 * getToolTip( INT32 nCellIdx);
	void		setToolTip_utf8( INT32 nCellIdx, const char * pTip);
};

#endif // __I3UI_LISTVIEW_DATA_H__
