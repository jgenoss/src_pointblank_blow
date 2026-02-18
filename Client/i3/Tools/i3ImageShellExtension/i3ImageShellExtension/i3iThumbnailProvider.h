#pragma once

#include "i3ImageDefine.h"
#include <windows.h>
#include <thumbcache.h>     // For IThumbnailProvider
#include <wincodec.h>       // Windows Imaging Codecs

#pragma comment(lib, "windowscodecs.lib")

#define THUMBNAIL_SIZE 256

enum SUPPORT_FILE_TYPE
{
	FILE_TYPE_NONE,
	FILE_TYPE_I3I,
	FILE_TYPE_DDS,
	FILE_TYPE_MAX,
};

class i3iThumbnailProvider :
	public IInitializeWithStream,
	public IThumbnailProvider
{
public:
	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	// IInitializeWithStream
	IFACEMETHODIMP Initialize(IStream *pStream, DWORD grfMode);

	// IThumbnailProvider
	IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

	i3iThumbnailProvider();

	HRESULT LoadI3IFile(HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha);
	HRESULT LoadI3I_Cube(I3I_FILE_HEADER3 * pInfo, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha);
	HRESULT LoadI3I_Normal(I3I_FILE_HEADER3 * pInfo, HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha);
	HRESULT CopyPixelDataToBuffer(I3I_FILE_HEADER3 * pInfo, BYTE * pDest);

	HRESULT LoadDDSFile(HBITMAP * phbmp, WTS_ALPHATYPE * pdwAlpha);

protected:
	~i3iThumbnailProvider();

	void	SetCubeMapBuffer(BYTE * pSrc, BYTE * pDest, INT32 width, INT32 height, I3G_IMAGE_FORMAT format);
	SUPPORT_FILE_TYPE GetFileTypeByName(const wchar_t * target);

private:
	// Reference count of component.
	long m_cRef;

	// Provided during initialization.
	IStream *m_pStream;
};