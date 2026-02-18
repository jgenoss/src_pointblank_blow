#include "i3FrameworkPCH.h"
#include "i3UIListView_Data.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


//#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( LVITEM_DATA);

LVITEM_DATA::~LVITEM_DATA()
{
	I3_SAFE_DELETE_ARRAY( m_pCells);
}

void LVITEM_DATA::Init( INT32 nCellCount)
{
	m_nCellCount = nCellCount;
	m_pCells = new LVCELL_DATA[nCellCount];
	NetworkDump_Malloc( m_pCells, sizeof( LVCELL_DATA) * nCellCount, __FILE__, __LINE__);
}

void LVITEM_DATA::GetCellData( INT32 nCellIdx, char* pData, INT32 pDataOfSize)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		//i3::string_ncopy_nullpad( pData, m_pCells[nCellIdx].m_Data, pDataOfSize);
		const i3::string* str = GetCellData( nCellIdx, 0);
		if( str != nullptr)
		{
			str->copy( pData, pDataOfSize);
		}
	}
}

const LVCELL_DATA* LVITEM_DATA::GetCellData( INT32 nCellIdx) const
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
		return &m_pCells[nCellIdx];

	return nullptr;
}

const i3::string* LVITEM_DATA::GetCellData( INT32 nCellIdx, INT32 nTemplateIdx) const
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		if( nTemplateIdx >= 0 && nTemplateIdx < static_cast<INT32>(m_pCells[nCellIdx].m_DataList.size()))
			return &m_pCells[nCellIdx].m_DataList[nTemplateIdx];
	}

	return nullptr;
}

void LVITEM_DATA::SetCellData( INT32 nCellIdx, const char* pData)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		m_pCells[nCellIdx].m_DataList.clear();
		m_pCells[nCellIdx].m_DataList.push_back( pData);
	}
}

bool LVITEM_DATA::SetCellData( INT32 nCellIdx, i3::vector<i3::string>& List)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		if( m_pCells[nCellIdx].m_DataList != List)
		{
			m_pCells[nCellIdx].m_DataList.clear();
			List.swap( m_pCells[nCellIdx].m_DataList);

			return true;
		}
	}

	return false;
}

COLOR* LVITEM_DATA::getCellColor( INT32 nCellIdx)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
		return &m_pCells[nCellIdx].m_Col;

	return nullptr;
}

void LVITEM_DATA::setCellColor( INT32 nCellIdx, COLOR* pColor)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		m_pCells[nCellIdx].m_bColorSet = true;
		i3Color::Copy( &m_pCells[nCellIdx].m_Col, pColor);
	}
}

void	LVITEM_DATA::restoreCellColor( INT32 nCellIdx)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		m_pCells[nCellIdx].m_bColorSet = false;
	}
}

I3UI_FONTINFO * LVITEM_DATA::getFontInfo( INT32 nCellIdx)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
		return &m_pCells[ nCellIdx].m_FontInfo;
	}

	return nullptr;
}

void LVITEM_DATA::setFontInfo( INT32 nCellIdx, I3UI_FONTINFO * pInfo)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
		i3mem::Copy( &m_pCells[ nCellIdx].m_FontInfo, pInfo, sizeof( I3UI_FONTINFO));
	}
}

I3UI_LISTVIEW_CELL_CALLBACK LVITEM_DATA::getFunc( INT32 nCellIdx)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
		return m_pCells[ nCellIdx].m_Func;
	}

	return nullptr;
}

void LVITEM_DATA::setFunc( INT32 nCellIdx, I3UI_LISTVIEW_CELL_CALLBACK func)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
		m_pCells[ nCellIdx].m_Func = func;
	}
}

bool LVITEM_DATA::isColorSet( INT32 nCellIdx)
{
	if( nCellIdx >= 0 && nCellIdx < m_nCellCount)
	{
		return m_pCells[nCellIdx].m_bColorSet;
	}

	return false;
}

const WCHAR16 * LVITEM_DATA::getToolTip( INT32 nCellIdx)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
		return (const WCHAR16 *) m_pCells[ nCellIdx].m_wstrToolTip.c_str();
	}

	return L"Non Tooltip";
}

void LVITEM_DATA::setToolTip_utf8( INT32 nCellIdx, const char * pTip)
{
	if( nCellIdx > -1 && nCellIdx < m_nCellCount)
	{
//		i3String::UTF8toUCS2( m_pCells[ nCellIdx].m_ToolTip, pTip, sizeof(m_pCells[ nCellIdx].m_ToolTip));
		i3::utf8_to_utf16(pTip, m_pCells[nCellIdx].m_wstrToolTip);
	}
}

//
//I3_CLASS_INSTANCE( LVHEADER_DATA, i3ElementBase );
//
//LVHEADER_DATA::LVHEADER_DATA()
//{
//	::memset( m_Data, 0, sizeof( m_Data ) );
//}
//
//LVHEADER_DATA::~LVHEADER_DATA()
//{
//}
