#ifndef __I3_IMAGE_FILE_BMP_H
#define __I3_IMAGE_FILE_BMP_H

// Definitions for BMP

#ifndef I3_WINDOWS

#ifndef I3G_G10
	typedef struct PACKED tagBITMAPFILEHEADER { 
	  UINT16    bfType; 
	  UINT32   bfSize; 
	  UINT16    bfReserved1; 
	  UINT16    bfReserved2; 
	  UINT32   bfOffBits; 
	} BITMAPFILEHEADER;

	typedef struct PACKED tagBITMAPINFOHEADER{
	  UINT32  biSize; 
	  INT32   biWidth; 
	  INT32   biHeight; 
	  UINT16   biPlanes; 
	  UINT16   biBitCount; 
	  UINT32  biCompression; 
	  UINT32  biSizeImage; 
	  INT32   biXPelsPerMeter; 
	  INT32   biYPelsPerMeter; 
	  UINT32  biClrUsed; 
	  UINT32  biClrImportant; 
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER; 


	typedef struct PACKED tagRGBQUAD {
	  INT8    rgbBlue; 
	  INT8    rgbGreen; 
	  INT8    rgbRed; 
	  INT8    rgbReserved; 
	} RGBQUAD;

	#define BI_RGB        0L
#endif

#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

#endif // I3_WINDOWS

#endif // __I3_IMAGE_FILE_BMP_H
