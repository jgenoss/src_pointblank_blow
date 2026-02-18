#pragma once

class Ci3ImageContext
{
public:
	Ci3ImageContext(void);
	~Ci3ImageContext(void);
private:
	i3Texture			*m_pTexture;

	char				m_cFileName[256];	//파일명
	char				m_cExt[8];			//파일종류
	
	I3G_IMAGE_FORMAT	m_PersistFormat;	//이미지 저장포맷
	I3G_IMAGE_FORMAT	m_Format;			//이미지 포맷

	UINT32				m_nImageWidth;		//이미지 크기 WIDTH
	UINT32				m_nImageHeight;		//이미지 크기 HEIGHT

	UINT32				m_nLevelCount;		//퍼픽셀 비트
	UINT32				m_nBitPerPixel;		//퍼픽셀 비트
	UINT32				m_nStride;			//Stride
	UINT32				m_nPitch;			//Pitch
	UINT32				m_nDataSize[MAX_MIPMAP_LEVEL+1];		//DataSize
	POINT				m_posMouse;			//
																//0-7각 레벨별 데이터사이즈 , 8: 전체데이터사이즈
public:
	void				SetImageName( const char * szFileName);

	void				SetContext(i3Texture * pTexture);

	char				*GetPersistFormatStr( char * szText );
	char				*GetFormatStr(char * pszValueStr);
	char				*GetFileName()	{return m_cFileName;}
	char				*GetFileExt()	{return m_cExt;		}

	UINT32				GetFormat()		{return m_Format;}
	UINT32				GetWidth()		{return m_nImageWidth;}
	UINT32				GetHeight()		{return m_nImageHeight;}
	UINT32				GetLevelCount()	{return m_nLevelCount;}
	UINT32				GetBitPerPixel(){return m_nBitPerPixel;}
	UINT32				GetStride()		{return m_nStride;}
	UINT32				GetPitch()		{return m_nPitch;}
	UINT32				GetDataSize(INT32	nIndex)	{return m_nDataSize[nIndex];}

	BOOL				HasAlpha();
};
