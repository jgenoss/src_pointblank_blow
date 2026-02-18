#include "stdafx.h"
#include "i3EXPPosData.h"
#include "i3Export.h"

i3ExportPosData::i3ExportPosData( i3Export * pExport)
{
	m_pExport = pExport;
	m_Width = 640;
	m_Height = 32;
}

int i3ExportPosData::GetWidth( int sizeType, int orient)
{
	return m_Width;
}

int i3ExportPosData::GetHeight( int sizeType, int orient)
{
	return m_Height;
}
