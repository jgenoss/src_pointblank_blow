#include <stdio.h>
#include <Shlwapi.h>
#include <Wincrypt.h>   // For CryptStringToBinary.
#include <msxml6.h>
#include <atlbase.h>
#include <atlconv.h>
#include <regex>

#include "i3ImageDefine.h"
#include "i3iThumbnailProvider.h"
#include "Profiler.h"
#include "ImageUtil.h"
#include "ImageUtilCalc.h"
#include "dds.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "msxml6.lib")

extern HINSTANCE g_hInst;
extern long g_cDllRef;

static wchar_t tempstr[0xFFFF];
static wchar_t tempstr2[0xFFFF];

void I3PRINTF(const wchar_t *format, ...)
{
	va_list marker;

	va_start(marker, format);

	vswprintf_s((wchar_t*)tempstr, _countof(tempstr), format, marker);

	va_end(marker);

	swprintf_s(tempstr2, _countof(tempstr2), L"[I3IEXT]%s", tempstr);

	OutputDebugString((wchar_t*)tempstr2);
}


i3iThumbnailProvider::i3iThumbnailProvider() : m_cRef(1), m_pStream(NULL)
{
	InterlockedIncrement(&g_cDllRef);
}


i3iThumbnailProvider::~i3iThumbnailProvider()
{
	InterlockedDecrement(&g_cDllRef);
}


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP i3iThumbnailProvider::QueryInterface(REFIID riid, void **ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(i3iThumbnailProvider, IThumbnailProvider),
		QITABENT(i3iThumbnailProvider, IInitializeWithStream),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) i3iThumbnailProvider::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) i3iThumbnailProvider::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef)
	{
		delete this;
	}

	return cRef;
}

#pragma endregion


#pragma region IInitializeWithStream

// Initializes the thumbnail handler with a stream.
IFACEMETHODIMP i3iThumbnailProvider::Initialize(IStream *pStream, DWORD grfMode)
{
	// A handler instance should be initialized only once in its lifetime. 
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	if (m_pStream == NULL)
	{
		// Take a reference to the stream if it has not been initialized yet.
		hr = pStream->QueryInterface(&m_pStream);
	}

	return hr;
}

#pragma endregion


#pragma region IThumbnailProvider

IFACEMETHODIMP i3iThumbnailProvider::GetThumbnail(UINT cx, HBITMAP *phbmp,
	WTS_ALPHATYPE *pdwAlpha)
{
	HRESULT hr;
	STATSTG stg;

	m_pStream->Stat(&stg, STATFLAG_DEFAULT);

	I3PRINTF(L"%s\n", OLE2CW(stg.pwcsName));

	SUPPORT_FILE_TYPE type = GetFileTypeByName(OLE2CW(stg.pwcsName));

	switch (type)
	{
	case FILE_TYPE_I3I:
		hr = LoadI3IFile(phbmp, pdwAlpha);
		break;
	case FILE_TYPE_DDS:
		hr = LoadDDSFile(phbmp, pdwAlpha);
		break;
	case FILE_TYPE_NONE:
		hr = S_OK;
		break;
	}
	
	RESET_PROFILE();
	REPORT_PROFILE();

	return hr;
}

SUPPORT_FILE_TYPE i3iThumbnailProvider::GetFileTypeByName(const wchar_t * target)
{
	const wchar_t * reg_esp_i3i = L"(\\w+)\\.i3i$";
	const wchar_t * reg_esp_dds = L"(\\w+)\\.dds$";

	std::wregex	regex_i3i(reg_esp_i3i, std::regex_constants::icase);
	std::wregex	regex_dds(reg_esp_dds, std::regex_constants::icase);

	std::wcmatch match;
	std::wcmatch match2;

	if (std::regex_search(target, match, regex_i3i))
	{
		return FILE_TYPE_I3I;
	}
	else if (std::regex_search(target, match2, regex_dds))
	{
		return FILE_TYPE_DDS;
	}
	else
	{
		return FILE_TYPE_NONE;
	}
}

HRESULT i3iThumbnailProvider::LoadDDSFile(HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
{
	HRESULT hr = S_OK;

	DWORD magic;
	DirectX::DDS_HEADER header;
	ULONG cbValue;
	I3G_IMAGE_FORMAT format;

	hr = m_pStream->Read(&magic, sizeof(DWORD), &cbValue);

	if (SUCCEEDED(hr))
	{		
		hr = m_pStream->Read(&header, sizeof(DirectX::DDS_HEADER), &cbValue);

		if (SUCCEEDED(hr))
		{
			if (header.ddspf.flags & DDS_FOURCC)
			{
				//DXT Compressed Format
				switch (header.ddspf.fourCC)
				{
				case MAKEFOURCC('D', 'X', 'T', '1'):
					format = I3G_IMAGE_FORMAT_DXT1;
					break;
				case MAKEFOURCC('D', 'X', 'T', '2'):
					format = I3G_IMAGE_FORMAT_DXT2;
					break;
				case MAKEFOURCC('D', 'X', 'T', '3'):
					format = I3G_IMAGE_FORMAT_DXT3;
					break;
				case MAKEFOURCC('D', 'X', 'T', '4'):
					format = I3G_IMAGE_FORMAT_DXT4;
					break;
				case MAKEFOURCC('D', 'X', 'T', '5'):
					format = I3G_IMAGE_FORMAT_DXT5;
					break;
				}
			}
			else
			{
				//Uncompressed format
				if (header.ddspf.flags & DDS_RGB)
				{
					format = FindImageFormat(header.ddspf.RBitMask, header.ddspf.GBitMask, header.ddspf.BBitMask, header.ddspf.ABitMask);
				}
				else
				{
					hr = S_FALSE;
				}
			}

			BYTE * pBuffer = NULL;
			BYTE * pBitBuffer = NULL;

			UINT32 imageSize = CalcRasterBufferSize(header.width, header.height, 4, format);

			if ((header.caps2 & DDS_CUBEMAP) == 0)
			{
				BITMAPINFO bmi = { sizeof(bmi.bmiHeader) };
				bmi.bmiHeader.biWidth = header.width;
				bmi.bmiHeader.biHeight = -(INT32)header.height;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS,
					reinterpret_cast<void **>(&pBitBuffer), NULL, 0);
				hr = hbmp ? S_OK : E_OUTOFMEMORY;

				pBuffer = (BYTE *)malloc(imageSize * sizeof(BYTE));

				if (header.ddspf.flags & DDS_FOURCC)
				{
					hr = LoadCompressedDDS(m_pStream, pBuffer, header.width, header.height, format);
					i3Image_Convert(format, (char *)pBuffer, header.width, header.height, 1, I3G_IMAGE_FORMAT_RGBA_8888, (char *)pBitBuffer, 1);
				}
				else
				{
					hr = LoadUnCompressedDDS(m_pStream, pBitBuffer, header.width, header.height, format);
				}
				
				if (SUCCEEDED(hr))
				{
					*phbmp = hbmp;
					*pdwAlpha = WTSAT_ARGB;
				}
			}
			else
			{
				BITMAPINFO bmi = { sizeof(bmi.bmiHeader) };
				bmi.bmiHeader.biWidth = header.width * 2;
				bmi.bmiHeader.biHeight = -(INT32)header.height * 3;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS,
					reinterpret_cast<void **>(&pBitBuffer), NULL, 0);
				hr = hbmp ? S_OK : E_OUTOFMEMORY;

				pBuffer = (BYTE *)malloc(imageSize * sizeof(BYTE) * 6);
				//큐브맵 로딩.
				hr = LoadCubeMapDDS(m_pStream, pBuffer, header.width, header.height, imageSize, format);

				UINT32 convertBufferSize = CalcRasterBufferSize(header.width, header.height, 4, I3G_IMAGE_FORMAT_RGBA_8888);
				BYTE * pTempBuffer = (BYTE *)malloc(convertBufferSize * sizeof(BYTE) * 6);

				if (header.ddspf.flags & DDS_FOURCC)
				{
					for (INT32 i = 0; i < 6; i++)
					{
						i3Image_Convert(format, (char *)&pBuffer[imageSize * i], header.width, header.height, 1, I3G_IMAGE_FORMAT_RGBA_8888, (char *)&pTempBuffer[convertBufferSize * i], 1);	
					}
				}

				//복사 하기 전 pTempBuffer에 있는 녀석들을 손봐줘야 한다.
				//1 x 6 블록 형식의 이미지로 되어있기 때문에, 이를 2 x 3 블록 형식의 이미지로 손질해야 함.
				SetCubeMapBuffer(pTempBuffer, pBitBuffer, header.width, header.height, I3G_IMAGE_FORMAT_RGBA_8888);

				if (SUCCEEDED(hr))
				{
					*phbmp = hbmp;
					*pdwAlpha = WTSAT_ARGB;
				}

				free(pTempBuffer);
			}

			if (pBuffer != NULL)
			{
				free(pBuffer);
			}
		}
	}
	
	return hr;
}

void i3iThumbnailProvider::SetCubeMapBuffer(BYTE * pSrc, BYTE * pDest, INT32 width, INT32 height, I3G_IMAGE_FORMAT format)
{
	//읽어들여온 1 x 6 RGBA 32bit 포맷을 2 x 3으로 변환한다.

	INT32 imageSize = CalcRasterBufferSize(width, height, 1, format);
	INT32 imageWidth = CalcRasterBufferSize(width, 1, 1, format);
	INT32 newWidth = CalcRasterBufferSize(width * 2, 1, 4, format);
	INT32 newBlock = CalcRasterBufferSize(width * 2, height, 4, format);

	//Positive X, Negative X
	for (INT32 j = 0; j < height; j++)
	{
		for (INT32 i = 0; i < newWidth; i++)
		{
			if (i < newWidth / 2)
			{
				pDest[i + newWidth * j] = pSrc[i + (imageWidth * j)];
			}
			else
			{
				pDest[i + newWidth * j] = pSrc[imageSize + (i - newWidth / 2) + (imageWidth * j)];
			}
		}
	}

	//Positive Y, Negative Y
	for (INT32 j = 0; j < height; j++)
	{
		for (INT32 i = 0; i < newWidth; i++)
		{
			if (i < newWidth / 2)
			{
				pDest[newBlock + i + newWidth * j] = pSrc[i + (imageWidth * j) + imageSize * 2];
			}
			else
			{
				pDest[newBlock + i + newWidth * j] = pSrc[imageSize * 3 + (i - newWidth / 2) + (imageWidth * j)];
			}
		}
	}

	//Positive Z, Negative Z
	for (INT32 j = 0; j < height; j++)
	{
		for (INT32 i = 0; i < newWidth; i++)
		{
			if (i < newWidth / 2)
			{
				pDest[newBlock * 2 + i + newWidth * j] = pSrc[i + (imageWidth * j) + imageSize * 4];
			}
			else
			{
				pDest[newBlock * 2 + i + newWidth * j] = pSrc[imageSize * 5 + (i - newWidth / 2) + (imageWidth * j)];
			}
		}
	}
}

HRESULT i3iThumbnailProvider::LoadI3IFile(HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
{
	HRESULT hr;

	I3I_FILE_HEADER3 info;
	ULONG cbRead;
	char dummyBuffer[MAX_PATH];

	hr = m_pStream->Read(&info, sizeof(I3I_FILE_HEADER3), &cbRead);

	if (info.m_OldVersionHeader.m_Version[0] >= 4)
	{
		//Version 4 이상은 이름을 가지고 있다.
		//이름 때문에 데이터가 망가지는 것을 막기 위해, 이름을 읽어서 스트림을 이동시킨다.
		if (info.m_NameLength > 0)
		{
			hr = m_pStream->Read(&dummyBuffer, info.m_NameLength, &cbRead);
		}
	}

	if (info.m_TexFlag & I3I_TEXFLAG_CUBE)
	{
		//큐브 텍스처
		//일단 지금은 처리하지 않는다.
	}
	else
	{
		//큐브가 아닌 텍스처
		hr = LoadI3I_Normal(&info, phbmp, pdwAlpha);
	}

	return hr;
}
 
HRESULT i3iThumbnailProvider::LoadI3I_Normal(I3I_FILE_HEADER3 * pInfo, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha)
{
	HRESULT hr;
	*phbmp = NULL;

	UINT16 width = pInfo->m_OldVersionHeader.m_Width;
	UINT16 height = pInfo->m_OldVersionHeader.m_Height;
	I3G_IMAGE_FORMAT format = pInfo->m_OldVersionHeader.m_Format;

	UINT8 * pBits;
	ULONG cbValue;

	__SP(1);

	UINT32 imageSize = CalcRasterBufferSize(width, height, 4, format);

	pBits = (UINT8 *)malloc(imageSize * sizeof(UINT8));

	hr = m_pStream->Read(pBits, (ULONG)imageSize, &cbValue);
	__EP(1);

	if (SUCCEEDED(hr))
	{
		BITMAPINFO bmi = { sizeof(bmi.bmiHeader) };
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		UINT8 * pBitBuffer;

		HBITMAP hbmp = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS,
			reinterpret_cast<void **>(&pBitBuffer), NULL, 0);
		hr = hbmp ? S_OK : E_OUTOFMEMORY;

		//i3Engine image deconder를 사용한 image format 변환

		if (SUCCEEDED(hr))
		{
			i3Image_Convert(format, (char *)pBits, width, height, 1, I3G_IMAGE_FORMAT_RGBA_8888, (char *)pBitBuffer, 1);

			*phbmp = hbmp;
			*pdwAlpha = WTSAT_ARGB;
		}
		else
		{
			DeleteObject(hbmp);
		}
	}

	free(pBits);
	return hr;
}
#pragma endregion