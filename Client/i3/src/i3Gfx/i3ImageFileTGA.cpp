#include "i3GfxType.h"

#include "i3ImageFile.h"
#include "i3ImageFileTGA.h"
#include "i3ImageUtil.h"
#include "i3Texture.h"
#include "i3GfxUtil.h"

// 16 bit targa files are stored as r g b r g b ...
// 24 bit targa files are stored as b g r b g r ...
// and  32 bit targa files are stored as b g r a b g r a ....
//
// #define SWAP_RED_AND_BLU(p) { INT8 _tmp; _tmp = (p)->red; (p)->red = (p)->blu; (p)->blu = _tmp;}
//
#define SWAP_RED_AND_BLU(p)

bool i3Tga::ReadTgaFileHeader(
   i3Stream* sp,
   TGAHEADER* pHeader
)
{
	UINT32 Rc;

	Rc = sp->Read( pHeader, sizeof(TGAHEADER));

	if(Rc == STREAM_ERR)
	{
		i3Log( "i3Tga::ReadTgaFileHeader()", "Could not read TGA file header information.");
		goto Error;
	}

	return true;
Error:
	return false;
}

i3Clut *	i3Tga::ReadCLUT( TGAFILE * tgaFile, i3Stream * sp)
{
	i3Clut * pClut;
	char * pBuf;
	UINT32 Rc, Stride, Pitch;
	bool Rv;

	if( tgaFile->header.idLength > 0)
	{
		sp->SetPosition( tgaFile->header.idLength, STREAM_CURRENT);
	}

	pClut = i3Clut::new_object();
	I3ASSERT( pClut);

	Rv = pClut->Create( tgaFile->header.colorMapIndex + tgaFile->header.colorMapLength, 
				tgaFile->header.colorMapEntrySize, 8);
	if( Rv == false)
	{
		i3Log( "i3Tga::ReadCLUT()", "Could not create CLUT object.");
		I3_MUST_RELEASE( pClut);
		return nullptr;
	}

	Stride = tgaFile->header.colorMapEntrySize >> 3;
	Pitch = pClut->GetColorCount() * Stride;

	pBuf = (char *) i3MemAlloc( pClut->GetColorCount() * Stride);
	I3ASSERT( pBuf);

	Rc = sp->Read( pBuf, Pitch);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Tga::Read4BitTgaData()", "Could not read CLUT data.");
		I3_MUST_RELEASE( pClut);
		i3MemFree( pBuf);
		return nullptr;
	}

	switch( tgaFile->header.colorMapEntrySize)
	{
		case 16 :
			i3Image_Convert( I3G_IMAGE_FORMAT_ARGB_1555, pBuf, pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), 1, nullptr);
			break;

		case 24 :
			i3Image_Convert( I3G_IMAGE_FORMAT_BGR_888, pBuf, pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), 1, nullptr);
			break;

		case 32 :
			i3Image_Convert( I3G_IMAGE_FORMAT_BGRA_8888, pBuf, pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), 1, nullptr);
			break;
	}


	i3MemFree( pBuf);

	return pClut;
}

bool i3Tga::WriteCLUT( TGAFILE * tgaFile, i3Stream * sp, i3Clut * pClut)
{
	UINT32 Rc, Stride, Pitch;
	char * pBuf;

	Stride = tgaFile->header.colorMapEntrySize >> 3;
	Pitch = pClut->GetColorCount() * Stride;

	pBuf = (char *) i3MemAlloc( Pitch);

	switch( tgaFile->header.colorMapEntrySize)
	{
		case 16 :
			i3Image_Convert( I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_ARGB_1555, pBuf, 1, nullptr);
			break;

		case 24 :
			i3Image_Convert( I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_BGR_888, pBuf, 1, nullptr);
			break;

		case 32 :
			i3Image_Convert( I3G_IMAGE_FORMAT_RGBA_8888, pClut->GetData(), pClut->GetColorCount(), 1,	1, nullptr,
								I3G_IMAGE_FORMAT_BGRA_8888, pBuf, 1, nullptr);
			break;
	}

	Rc = sp->Write( pBuf, Pitch);

	i3MemFree( pBuf );

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Tga::WriteCLUT()", "Could not read CLUT data.");
		return false;
	}

	return true;
}

bool i3Tga::Read4BitTgaData( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex)
{
	int i;
	unsigned char *image;
	unsigned char *imageEnd;
	unsigned char *tempBuffer;
	UINT32 Rc;
	int imageSize = (tgaFile->header.width * tgaFile->header.height) >> 1;
	int lineSize = tgaFile->header.width >> 1;

	image = tgaFile->image.data;
	imageEnd = image + imageSize;

	// Read CLUT Data
	{
		i3Clut * pClut;

		pClut = ReadCLUT( tgaFile, sp);
		if( pClut == nullptr)
		{
			return false;
		}

		pTex->SetClut( 0, pClut);

		I3_MUST_RELEASE( pClut);
	}

	if( tgaFile->header.imageType != RUNLENGTH)
	{
		for (i = 0; i < tgaFile->header.height; i++)
		{
			Rc = sp->Read(image, lineSize); 
			if( Rc == STREAM_ERR )
			{
				i3Log( "i3Tga::Read4BitTgaData()", "Could not read pixel data.");
				return false;
			}
			image += (tgaFile->header.width) >> 1;
		}
	}
	else
	{
		unsigned char count;
		char pixel;

		while( image < imageEnd)
		{
			if( sp->Read( &count, 1 ) == STREAM_ERR)
				return false;

			if( count & 0x80)
			{
				// low 7 bits is repeat count - 1;
				count = (count & 0x7f);

				Rc =  sp->Read( &pixel, sizeof(pixel) );
				if( Rc == STREAM_ERR )
				{
					return false;
				}
				do
				{
					*image = pixel;
					image++;
				} while (count--);
			}
			else
			{
				// low 7 bits is nr of uncompressed pixels to follow
				count++;

				// if( sp->Read(image, sizeof(RGBTripple15), count) != (size_t)count)
				if( sp->Read( image, count ) == STREAM_ERR )
				{
					return false;
				}
				image+=count;
			}
		}
	}

	if( (tgaFile->header.imageDescriptor & 0x20) == 0)
	{
		unsigned char * top, * bottom;

		// image is bottom up, we flip it
		top = tgaFile->image.data;
		bottom = top + lineSize * (tgaFile->header.height - 1);

		if( (int) tgaFile->TempBufferSize < lineSize)
		{
			I3MEM_SAFE_FREE_POINTER(tgaFile->TempBuffer);
			tgaFile->TempBufferSize = 0;
		}

		if( tgaFile->TempBuffer == nullptr)
		{
//			INT32 idx; 
//			idx = (INT32)i3MemAlloc( lineSize);
			tgaFile->TempBuffer = i3MemAlloc( lineSize );

			if( tgaFile->TempBuffer == nullptr)
			{
				return false;
			}
			tgaFile->TempBufferSize = lineSize;
		}

		tempBuffer = (unsigned char *)tgaFile->TempBuffer;
		for (i = 0; i < tgaFile->header.height / 2; i++)
		{
			memcpy( tempBuffer, top, lineSize);
			memcpy( top, bottom, lineSize);
			memcpy( bottom, tempBuffer, lineSize);
			top += lineSize;
			bottom -= lineSize;
		}
		// and set the flag to say we're top down
		tgaFile->header.imageDescriptor |= 0x20;
	}
	return true;
}

bool i3Tga::Read8BitTgaData( TGAFILE* tgaFile, i3Stream* sp, i3Texture * pTex)
{
	int i;
	unsigned char *image;
	unsigned char *imageEnd;
	unsigned char *tempBuffer;
	UINT32 Rc;
	int imageSize = (tgaFile->header.width * tgaFile->header.height);
	int lineSize = tgaFile->header.width;

	// Read CLUT Data
	{
		i3Clut * pClut;

		pClut = ReadCLUT( tgaFile, sp);
		if( pClut == nullptr)
		{
			return false;
		}

		pTex->SetClut( 0, pClut);

		I3_MUST_RELEASE( pClut);
	}

	image = tgaFile->image.data;
	imageEnd = image + imageSize;

	if( tgaFile->header.imageType != RUNLENGTH)
	{
		for (i = 0; i < tgaFile->header.height; i++)
		{
			Rc = sp->Read(image, lineSize); 
			if( Rc == STREAM_ERR )
			{
				i3Log( "i3Tga::Read4BitTgaData()", "Could not read pixel data.");
				return false;
			}
			image += lineSize;
		}
	}
	else
	{
		unsigned char count;
		char pixel;

		while( image < imageEnd)
		{
			if( sp->Read( &count, 1 ) == STREAM_ERR)
				return false;

			if( count & 0x80)
			{
				// low 7 bits is repeat count - 1;
				count = (count & 0x7f);

				Rc =  sp->Read( &pixel, sizeof(pixel) );
				if( Rc == STREAM_ERR )
				{
					return false;
				}
				do
				{
					*image = pixel;
					image++;
				} while (count--);
			}
			else
			{
				// low 7 bits is nr of uncompressed pixels to follow
				count++;

				// if( sp->Read(image, sizeof(RGBTripple15), count) != (size_t)count)
				if( sp->Read( image, count ) == STREAM_ERR )
				{
					return false;
				}
				image+=count;
			}
		}
	}

	if( (tgaFile->header.imageDescriptor & 0x20) == 0)
	{
		unsigned char * top, * bottom;

		// image is bottom up, we flip it
		top = tgaFile->image.data;
		bottom = top + lineSize * (tgaFile->header.height - 1);

		if( (int) tgaFile->TempBufferSize < lineSize)
		{
			I3MEM_SAFE_FREE_POINTER(tgaFile->TempBuffer);
			tgaFile->TempBufferSize = 0;
		}

		if( tgaFile->TempBuffer == nullptr)
		{
//			INT32 idx; 
//			idx = i3MemAlloc( lineSize);
			tgaFile->TempBuffer = i3MemAlloc( lineSize);

			if( tgaFile->TempBuffer == nullptr)
			{
				return false;
			}
			tgaFile->TempBufferSize = lineSize;
		}

		tempBuffer = (unsigned char *)tgaFile->TempBuffer;
		for (i = 0; i < tgaFile->header.height / 2; i++)
		{
			memcpy( tempBuffer, top, lineSize);
			memcpy( top, bottom, lineSize);
			memcpy( bottom, tempBuffer, lineSize);
			top += lineSize;
			bottom -= lineSize;
		}
		// and set the flag to say we're top down
		tgaFile->header.imageDescriptor |= 0x20;
	}

	return true;
}

bool i3Tga::Read16BitTgaData( TGAFILE* tgaFile, i3Stream* sp, i3Texture * /*pTex*/)
{
	int i;
	RGBTripple15 *image;
	RGBTripple15 *imageEnd;
	RGBTripple15 *tempBuffer;
	int imageSize = tgaFile->header.width * tgaFile->header.height;

	image = tgaFile->image.bits16;
	imageEnd = image + imageSize;

	if( tgaFile->header.imageType != RUNLENGTH)
	{
		for (i = 0; i < tgaFile->header.height; i++)
		{
			if( sp->Read(image, sizeof(RGBTripple15) * tgaFile->header.width) == STREAM_ERR )
			{
				return false;
			}
			image += tgaFile->header.width;
		}
	}
	else
	{
		unsigned char count;
		RGBTripple15 pixel;
		while (image < imageEnd)
		{
			// count = sp->Fgetc();
			if( sp->Read( &count, 1 ) == STREAM_ERR)
				return false;

			if( count & 0x80)
			{
				// low 7 bits is repeat count - 1;
				count = (count & 0x7f);
				// if( sp->Read(&pixel, sizeof(RGBTripple15), 1) != 1)
				if( sp->Read( &pixel, sizeof(RGBTripple15) ) == STREAM_ERR )
				{
					return false;
				}
				do
				{
					*image = pixel;
					image++;
				} while (count--);
			}
			else
			{
				// low 7 bits is nr of uncompressed pixels to follow
				count++;
				// if( sp->Read(image, sizeof(RGBTripple15), count) != (size_t)count)
				if( sp->Read( image, sizeof(RGBTripple15) * count ) == STREAM_ERR )
				{
					return false;
				}
				image+=count;
			}
		}
	}

	if( (tgaFile->header.imageDescriptor & 0x20) == 0)
	{
		RGBTripple15* top;
		RGBTripple15* bottom;

		// image is bottom up, we flip it
		top = tgaFile->image.bits16;
		bottom = top + tgaFile->header.width * (tgaFile->header.height - 1);
		if(tgaFile->TempBufferSize < tgaFile->header.width * sizeof(RGBTripple15))
		{
			I3MEM_SAFE_FREE_POINTER(tgaFile->TempBuffer);
			tgaFile->TempBufferSize = 0;
			tgaFile->TempBuffer = 0;
		}

		if( tgaFile->TempBuffer == nullptr)
		{
			tgaFile->TempBuffer = i3MemAlloc( tgaFile->header.width * sizeof(RGBTripple15)); 
			
			if(tgaFile->TempBuffer == nullptr)
			{
				return false;
			}
			tgaFile->TempBufferSize = tgaFile->header.width * sizeof(RGBTripple15);
		}
		tempBuffer = (RGBTripple15 *)tgaFile->TempBuffer;
		for (i = 0; i < tgaFile->header.height/2; i++)
		{
			memcpy (tempBuffer, top, tgaFile->header.width * sizeof(RGBTripple15));
			memcpy (top, bottom, tgaFile->header.width * sizeof(RGBTripple15));
			memcpy (bottom, tempBuffer, tgaFile->header.width * sizeof(RGBTripple15));
			top += tgaFile->header.width;
			bottom -= tgaFile->header.width;
		}
		// and set the flag to say we're top down
		tgaFile->header.imageDescriptor |= 0x20;
	}
	return true;
}

bool i3Tga::Read24BitTgaData( TGAFILE *tgaFile, i3Stream* sp, i3Texture * /*pTex*/)
{
	int i;
	RGBTripple *image;
	RGBTripple *imageEnd;
	int imageSize = tgaFile->header.width * tgaFile->header.height;

	image = tgaFile->image.bits24;
	imageEnd = image + imageSize;

	if( tgaFile->header.imageType != RUNLENGTH)
	{
		if( sp->Read(image, sizeof(RGBTripple) * imageSize) == STREAM_ERR )
		{
			return false;
		}

		/*
		for (i = 0; i < imageSize; i++)
		{
			SWAP_RED_AND_BLU(image + i);
		}
		*/
	}
	else 
	{
		unsigned char count;
		RGBTripple pixel;
		while (image < imageEnd)
		{
			if( sp->Read( &count, 1 ) == STREAM_ERR)
				return false;

			if( count & 0x80)
			{
				// low 7 bits is repeat count - 1;
				count = (count & 0x7f);
				if( sp->Read(&pixel, sizeof(RGBTripple) ) == STREAM_ERR ) return false;
				//SWAP_RED_AND_BLU(&pixel);
				do {
					*image++ = pixel;
				} while (count--);
			}
			else
			{
				// low 7 bits is nr of uncompressed pixels to follow
				count++;
				if( sp->Read(image, sizeof(RGBTripple) * count) == STREAM_ERR ) return false;

				/*
				for (i = 0; i < count; i++)
				{
					SWAP_RED_AND_BLU(image + i);
				}
				*/
				image += count;
			}
		}
	}

	if( (tgaFile->header.imageDescriptor & 0x20) == 0)
	{
		RGBTripple* top;
		RGBTripple* bottom;
		RGBTripple *tempBuffer;

		// image is bottom up, we flip it
		top = tgaFile->image.bits24;
		bottom = top + tgaFile->header.width * (tgaFile->header.height - 1);
		if( tgaFile->TempBufferSize < tgaFile->header.width * sizeof(RGBTripple)) 
		{
			I3MEM_SAFE_FREE_POINTER(tgaFile->TempBuffer);
			tgaFile->TempBufferSize = 0;
			tgaFile->TempBuffer = 0;
		}

		if( tgaFile->TempBuffer == nullptr) 
		{
			tgaFile->TempBuffer = i3MemAlloc( tgaFile->header.width * sizeof(RGBTripple)); 
			
			if(tgaFile->TempBuffer == nullptr) 
			{
				return false;
			}
			tgaFile->TempBufferSize = tgaFile->header.width * sizeof(RGBTripple);
		}
		tempBuffer = (RGBTripple *)tgaFile->TempBuffer;
		for (i = 0; i < tgaFile->header.height/2; i++) {
			memcpy (tempBuffer, top, tgaFile->header.width * sizeof(RGBTripple));
			memcpy (top, bottom, tgaFile->header.width * sizeof(RGBTripple));
			memcpy (bottom, tempBuffer, tgaFile->header.width * sizeof(RGBTripple));
			top += tgaFile->header.width;
			bottom -= tgaFile->header.width;
		}

		// and set the flag to say we're top down
		tgaFile->header.imageDescriptor |= 0x20;
	}
	return true;
}

bool i3Tga::Read32BitTgaData( TGAFILE *tgaFile, i3Stream* sp, i3Texture * /*pTex*/)
{
	int i;
	RGBQuad *image;
	RGBQuad *imageEnd;
	int imageSize = tgaFile->header.width * tgaFile->header.height;

	image = tgaFile->image.bits32;
	imageEnd = image + imageSize;

	if( tgaFile->header.imageType != RUNLENGTH)
	{	// Uncompressed
		if( sp->Read(image, sizeof(RGBQuad) * imageSize) == STREAM_ERR ) {
			return false;
		}
		for (i = 0; i < imageSize; i++) {
			SWAP_RED_AND_BLU(image + i);
		}
	}
	else
	{	// RLE Compressed
		unsigned char count;
		RGBQuad pixel;
		while (image < imageEnd)
		{
			// count = sp->Fgetc();
			if( sp->Read( &count, 1 ) == STREAM_ERR)
				return false;

			if( count & 0x80)
			{
				// low 7 bits is repeat count - 1;
				count = (count & 0x7f);
				if( sp->Read(&pixel, sizeof(RGBQuad) ) == STREAM_ERR )
				{
					return false;
				}
				SWAP_RED_AND_BLU(&pixel);
				do
				{
					*image = pixel;
					image++;
				} while (count--);
			}
			else
			{
				// low 7 bits is nr of uncompressed pixels to follow
				count++;
				do
				{
					if( sp->Read( image, sizeof(RGBQuad) ) == STREAM_ERR )
					{
						return false;
					}
					SWAP_RED_AND_BLU(image);
					image++;
				} while (--count);
			}
		}
	}

	if( (tgaFile->header.imageDescriptor & 0x20) == 0)
	{
		RGBQuad *top;
		RGBQuad *bottom;
		RGBQuad *tempBuffer;

		// image is bottom up, we flip it
		top = tgaFile->image.bits32;
		bottom = top + tgaFile->header.width * (tgaFile->header.height - 1);
		if( tgaFile->TempBuffer && tgaFile->TempBufferSize < tgaFile->header.width * sizeof(RGBQuad))
		{
			i3MemFree (tgaFile->TempBuffer);
			tgaFile->TempBufferSize = 0;
			tgaFile->TempBuffer = 0;
		}
		if( tgaFile->TempBuffer == 0)
		{
//			INT32 idx;
//			idx = i3MemAlloc(tgaFile->header.width * sizeof(RGBQuad));
			tgaFile->TempBuffer = i3MemAlloc(tgaFile->header.width * sizeof(RGBQuad));
			
			if(tgaFile->TempBuffer  == 0)
			{
				return false;
			}
			tgaFile->TempBufferSize = tgaFile->header.width * sizeof(RGBQuad);
		}
		tempBuffer = (RGBQuad *)tgaFile->TempBuffer;
		for (i = 0; i < tgaFile->header.height/2; i++)
		{
			memcpy (tempBuffer, top, tgaFile->header.width * sizeof(RGBQuad));
			memcpy (top, bottom, tgaFile->header.width * sizeof(RGBQuad));
			memcpy (bottom, tempBuffer, tgaFile->header.width * sizeof(RGBQuad));
			top += tgaFile->header.width;
			bottom -= tgaFile->header.width;
		}

		// and set the flag to say we're top down
		tgaFile->header.imageDescriptor |= 0x20;
	}
	return true;
}

void i3Tga::FreeTgaFile (
   TGAFILE *tgafile
)
{
   if (tgafile != 0) {
	   I3MEM_SAFE_FREE_POINTER(tgafile->TempBuffer);
   }
}

bool i3ImageFile::LoadTGA( i3Stream * pStream, i3Texture * pTexture)
{
	char * pBuf;
	TGAFILE tgaFile;
	I3G_IMAGE_FORMAT PixelFormat;
	bool Rv;

	I3ASSERT( pTexture != nullptr);

	pTexture->setRestorePoint( pStream);

	// Header 읽기
	if( i3Tga::ReadTgaFileHeader( pStream, &tgaFile.header ) != true )
	{
		goto Error;
	}

	// Image
	switch( tgaFile.header.bits )
	{
		case 4 :	PixelFormat = I3G_IMAGE_FORMAT_I4;	break;
		case 8 :	PixelFormat = I3G_IMAGE_FORMAT_I8;	break;
		case 16 :	PixelFormat = I3G_IMAGE_FORMAT_XRGB_1555;	break;
		case 24 :	PixelFormat = I3G_IMAGE_FORMAT_BGR_888;		break;
		case 32 :	PixelFormat = I3G_IMAGE_FORMAT_BGRA_8888;	break;
		default :
			I3TRACE1( "i3ImageFile::LoadTGA() - Invalid Pixel Bitscount : %d\n", tgaFile.header.bits);
			goto Error;
	}

	if( pTexture->Create( tgaFile.header.width, tgaFile.header.height, 1, PixelFormat, 0) == false)
	{
		goto Error;
	}

	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{

		UINT32 ImageSz;

		ImageSz = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetPersistFormat());

		tgaFile.allocatedSize = ImageSz;

		pBuf = (char *) i3MemAlloc( ImageSz);

	}
	else
	{
		pBuf = pTexture->Lock( 0, 0);
		tgaFile.allocatedSize = pTexture->GetDataSize( 0);
	}

	tgaFile.image.data = (unsigned char *) pBuf;

	switch( tgaFile.header.bits )
	{
	case 4:		Rv = i3Tga::Read4BitTgaData( &tgaFile, pStream, pTexture);	break;
	case 8:		Rv = i3Tga::Read8BitTgaData( &tgaFile, pStream, pTexture);	break;
	case 16:	Rv = i3Tga::Read16BitTgaData( &tgaFile, pStream, pTexture);	break;
	case 24:	Rv = i3Tga::Read24BitTgaData( &tgaFile, pStream, pTexture);	break;
	case 32:	Rv = i3Tga::Read32BitTgaData( &tgaFile, pStream, pTexture);	break;
	default:
		{
			i3Log( "i3ImageFile::LoadTGA()", "Invalid Bitscount : %d", tgaFile.header.bits);
			goto Error;
		}
	}

	i3Tga::FreeTgaFile( &tgaFile);

	if( pTexture->GetPersistFormat() != pTexture->GetFormat())
	{
		pTexture->SetPersistData( 0, (INT8 *) pBuf);

		pTexture->MakeRuntimeForm();
	}
	else
	{
		pTexture->Unlock( 0);

		// Persist Form과 동일한 경우엔 Persist data를 보유하지 않는다.
		//pTexture->MakePersistForm(); 
	}

	return true;

Error:

	I3_MUST_RELEASE( pTexture);				// <- 문제성의 Code!

	i3Log( "i3ImageFile::LoadTGA()", "Could not create Image object.");
	return false;
}

bool i3ImageFile::SaveTGA( i3Stream * pStream, i3Texture * pTexture)
{
	char * pBuf;
	TGAFILE tgaFile;
	i3Clut * pClut;
	int AlphaBits = 0;
	UINT32 Rc;

	switch( pTexture->GetPersistFormat() & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB))
	{
		case I3G_IMAGE_FORMAT_I4 :
		case I3G_IMAGE_FORMAT_I8 :
			AlphaBits= 0;
			break;

		case I3G_IMAGE_FORMAT_RGB_565 :
			AlphaBits = 0;
			break;

		case I3G_IMAGE_FORMAT_XRGB_1555 :
		case I3G_IMAGE_FORMAT_ARGB_1555 :
			AlphaBits = 1;
			break;

		case I3G_IMAGE_FORMAT_XRGB_4444 :
		case I3G_IMAGE_FORMAT_ARGB_4444 :
			AlphaBits = 4;
			break;

		case I3G_IMAGE_FORMAT_BGR_888 :
			AlphaBits = 0;
			break;

		case I3G_IMAGE_FORMAT_BGRX_8888 :
			AlphaBits = 0;
			break;

		case I3G_IMAGE_FORMAT_BGRA_8888 :
			AlphaBits = 8;
			break;

		default :
			i3Log( "i3IamgeFile::SaveTGA()", "Invalid Texture Format : %08X\n", pTexture->GetFormat());
			break;
	}

	// Header 설정
	{
		tgaFile.header.idLength = 0;

		pClut = pTexture->GetClut(0);

		if( pTexture->GetClut(0) == nullptr)
		{
			tgaFile.header.colorMapType = 0;
			tgaFile.header.imageType = 2;
			tgaFile.header.colorMapIndex = 0;
			tgaFile.header.colorMapLength = 0;
			tgaFile.header.colorMapEntrySize = 0;
		}
		else
		{
			tgaFile.header.colorMapType = 1;
			tgaFile.header.imageType = 1;
			tgaFile.header.colorMapIndex = 0;
			tgaFile.header.colorMapLength = (INT16) pClut->GetColorCount();
			tgaFile.header.colorMapEntrySize = 32;
		}

		tgaFile.header.xOrigin = 0;
		tgaFile.header.yOrigin = 0;
		tgaFile.header.width = (INT16) pTexture->GetWidth();
		tgaFile.header.height = (INT16) pTexture->GetHeight();
		tgaFile.header.bits = (INT8) i3Gfx::GetImageFormatBitCount( pTexture->GetPersistFormat());
		tgaFile.header.imageDescriptor = (INT8) 0x20;	//AlphaBits | 0x20;

		tgaFile.image.data = nullptr;
		tgaFile.allocatedSize = 0;
	}

	Rc = pStream->Write( &tgaFile.header, sizeof(TGAHEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ImageFile::SaveTGA()", "Could not write header data.");
		goto Error;
	}

	// Clut
	if( pClut != nullptr)
	{
		i3Tga::WriteCLUT( &tgaFile, pStream, pClut);
	}

	// Image
	{
		UINT32 ImageSz;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//재천씨가 만들어 놓았던 코드 입니다.
		//if( pTexture->GetPersistFormat() != pTexture->GetFormat())
		//{
		//	pBuf = (char *) pTexture->GetPersistData( 0);
		//	ImageSz = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetPersistFormat());
		//}
		//else
		//{
		//	pBuf = pTexture->Lock( 0, 0);
		//	ImageSz = pTexture->GetDataSize();
		//}

		char * pBufSource = nullptr;
		I3G_IMAGE_FORMAT  tgaFormat ;

		switch(tgaFile.header.bits)
		{
		case 4 :	tgaFormat = I3G_IMAGE_FORMAT_I4;		break;
		case 8 :	tgaFormat = I3G_IMAGE_FORMAT_I8;		break;
		case 16 :	tgaFormat = I3G_IMAGE_FORMAT_XRGB_1555;	break;
		case 24 :	tgaFormat = I3G_IMAGE_FORMAT_BGR_888;	break;
		case 32 :	tgaFormat = I3G_IMAGE_FORMAT_BGRA_8888;	break;
		default :
			I3TRACE1( "i3ImageFile::SaveTGA() - Invalid Pixel Bitscount : %d\n", tgaFile.header.bits);
			tgaFormat = I3G_IMAGE_FORMAT_BGRA_8888;	
			break;			
		} 
		ImageSz = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), pTexture->GetHeight(), 1, tgaFormat);

		pBufSource = (char *)i3MemAlloc( ImageSz);

		pBuf = pBufSource; 
		i3Image_Convert( pTexture->GetFormat(), pTexture->Lock( 0, 0), pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetClut(0),
			tgaFormat, pBuf, 1, nullptr);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if((pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM) != 0)
		{
			char * pTempBuf = (char *) i3MemAlloc( ImageSz);

			INT32 nBytesPerLine = i3Gfx::CalcRasterBufferSize( pTexture->GetWidth(), 1, 1, pTexture->GetPersistFormat());
			I3_PLATFORM PersistPlatform = I3_PLATFORM_UNKNOWN;

			switch( pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM)
			{
				case I3G_IMAGE_FORMAT_MASK_PSP :	PersistPlatform = I3_PLATFORM_PSP;	break;
				default :	
					I3TRACE( "i3ImageFileTGA::SaveTGA() - Unsupported swizzled image format : %x\n",
						pTexture->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM);
					break;
			}

			i3ImageConvertSwizzledToLinear( pTempBuf, pBuf, nBytesPerLine, pTexture->GetHeight(), 
				PersistPlatform);

			Rc = pStream->Write( pTempBuf, ImageSz);

			i3MemFree( pTempBuf );
		}
		else
		{
			Rc = pStream->Write( pBuf, ImageSz);
		}

		i3MemFree( pBufSource);

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ImageFile::SaveTGA()", "Could not write image data.");
			goto Error;
		}
	}

	i3Tga::FreeTgaFile( &tgaFile);

	if( pTexture->GetPersistFormat() == pTexture->GetFormat())
		pTexture->Unlock( 0);

	return true;

Error:

	i3Log( "i3ImageFile::LoadTGA()", "Could not create Image object.");
	return false;
}
