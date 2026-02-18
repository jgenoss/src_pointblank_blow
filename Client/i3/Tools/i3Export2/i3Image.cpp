#include "stdafx.h"
#include "i3Image.h"
#include "resource.h"
#include "AboutDlg.h"

i3ImageEXP::i3ImageEXP(void)
{
}

i3ImageEXP::~i3ImageEXP(void)
{
}

int i3ImageEXP::ExtCount()
{
	return 1;
}

const TCHAR * i3ImageEXP::Ext( int n )
{
	return "I3I";
}

const TCHAR * i3ImageEXP::LongDesc( )
{
	return "i3Engine Image File I/O version 1.0";
}

const TCHAR * i3ImageEXP::ShortDesc()
{
	return "I3Engine Image";
}

const TCHAR * i3ImageEXP::AuthorName()
{
	return "Zepetto Corp.";
}

const TCHAR * i3ImageEXP::CopyrightMessage()
{
	return "All rights reserved. 2007";
}

UINT i3ImageEXP::Version()
{
	return 1 * 100;
}

int i3ImageEXP::Capability()
{
	return BMMIO_READER | BMMIO_WRITER | BMMIO_RANDOM_ACCESS | BMMIO_EXTENSION;
}

void i3ImageEXP::ShowAbout( HWND hWnd )
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_DLG_IMAGEABOUT), hWnd, 
				_AboutDlgProc, (LPARAM)NULL);
}

DWORD i3ImageEXP::EvaluateConfigure( )
{
	return 0;
}

BOOL i3ImageEXP::LoadConfigure( void *ptr )
{
	return TRUE;
}

BOOL i3ImageEXP::SaveConfigure( void *ptr )
{
	return TRUE;
}

DWORD i3ImageEXP::ChannelsRequired()
{
	return BMM_CHAN_NONE;
}

DWORD i3ImageEXP::_getFlag( I3G_IMAGE_FORMAT format)
{
	DWORD flag = 0;

	if( format & I3G_IMAGE_FORMAT_MASK_ALPHA)
	{
		flag |= MAP_HAS_ALPHA;
	}

	if( format & I3G_IMAGE_FORMAT_MASK_INDEXED)
	{
		flag |= MAP_PALETTED;
	}

	return flag;
}

int i3ImageEXP::_getType( I3G_IMAGE_FORMAT format)
{
	switch( format)
	{
		case I3G_IMAGE_FORMAT_I8 :			return BMM_PALETTED;

		case I3G_IMAGE_FORMAT_RGB_565 :
		case I3G_IMAGE_FORMAT_BGR_565 :
		case I3G_IMAGE_FORMAT_XRGB_1555 :
		case I3G_IMAGE_FORMAT_ARGB_1555 :
		case I3G_IMAGE_FORMAT_XBGR_1555 :
		case I3G_IMAGE_FORMAT_ABGR_1555 :
		case I3G_IMAGE_FORMAT_XRGB_4444 :
		case I3G_IMAGE_FORMAT_ARGB_4444 :
		case I3G_IMAGE_FORMAT_XBGR_4444 :
		case I3G_IMAGE_FORMAT_ABGR_4444 :	return BMM_TRUE_16;

		case I3G_IMAGE_FORMAT_RGB_888 :
		case I3G_IMAGE_FORMAT_BGR_888 :		return BMM_TRUE_24;
	}

	return BMM_TRUE_32;
}

BMMRES i3ImageEXP::GetImageInfo( BitmapInfo *bi )
{
	i3::pack::IMAGE_FILE_HEADER3		header;
	i3FileStream stream;

	if( stream.Open( bi->Name(), STREAM_READ) == false)
	{
		return BMMRES_FILENOTFOUND;
	}

	i3ImageFile::LoadI3I_Header( &stream, &header, (char *) bi->Name());

	bi->SetFlags( _getFlag( header.m_OldVersionHeader.m_Format));
	bi->SetType( _getType( header.m_OldVersionHeader.m_Format));
	bi->SetWidth( header.m_OldVersionHeader.m_Width);
	bi->SetHeight( header.m_OldVersionHeader.m_Height);
	bi->SetAspect( 1.0f);
	bi->SetGamma( 1.0f);
	bi->SetFirstFrame( 0);
	bi->SetLastFrame( 0);

	stream.Close();

	return BMMRES_SUCCESS;
}

BMMRES i3ImageEXP::GetImageName( BitmapInfo *bi, TCHAR *filename)
{
	return BMMRES_SUCCESS;
}

BitmapStorage * i3ImageEXP::Load( BitmapInfo *bi, Bitmap *map, BMMRES *status )
{
	i3Texture * pTex;
	i3ImageFile file;
	BitmapStorage * pStore;
	int Type;

	pTex = file.Load( bi->Name());
	if( pTex == NULL)
	{
		*status = BMMRES_IOERROR;
		return NULL;
	}

	Type = _getType( pTex->GetFormat());
	
	pStore = BMMCreateStorage( map->Manager(), Type);

	bi->SetFlags( _getFlag( pTex->GetFormat()));
	bi->SetType( _getType( pTex->GetFormat()));
	bi->SetWidth( pTex->GetWidth());
	bi->SetHeight( pTex->GetHeight());
	bi->SetAspect( 1.0f);
	bi->SetGamma( 1.0f);
	bi->SetFirstFrame( 0);
	bi->SetLastFrame( 0);

	if( pStore->Allocate( bi, map->Manager(), BMM_OPEN_R) == 0)
	{
		delete pStore;
		return NULL;
	}

	return pStore;
}

BMMRES i3ImageEXP::OpenOutput(BitmapInfo *bi, Bitmap *map)
{
	return BMMRES_SUCCESS;
}

BMMRES i3ImageEXP::Write(int frame)
{
	return BMMRES_SUCCESS;
}

int i3ImageEXP::Close(int flag)
{
	return TRUE;
}
