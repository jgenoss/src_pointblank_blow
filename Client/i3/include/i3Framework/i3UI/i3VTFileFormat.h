#pragma once

class i3VirtualTexture;
struct I3VT_IOHANDLE;

class i3VTSubsetController;
class i3VTFileFormatController;

struct i3VTFileFormatData
{
	i3::vector<I3VT_IOHANDLE*>	m_FileHandleList;		// i3VirtualTextureIO보다는 이쪽으로의 이동이 더 좋다..
	i3::rc_string			m_strCurrFullPath;			// 파일이 존재하기만 한다면 일단 보관해둔다..
//	i3::rc_string			m_strNation;				// 국가명..
};

class I3_EXPORT_FRAMEWORK i3VTFileFormat
{
public:
	i3VTFileFormat(i3VirtualTexture* pOwner);
	~i3VTFileFormat();
	
	void	ConvertController();				
	bool	IsControllerNewFormat() const;


	bool			isReadyToWork(void) const { return !m_Data.m_FileHandleList.empty(); }
	INT32			getFileHandleCount() const { return INT32(m_Data.m_FileHandleList.size()); }
	I3VT_IOHANDLE *	getFileHandle(INT32 idx) const;

	
	bool			Create(const char * pszPath, I3G_IMAGE_FORMAT fmt, bool bNewFormat);

	
	bool Open(const char * pszPath);
	bool ReOpen();

	bool Flush(i3FileStream * pFile = nullptr);
	bool Defragment(void);

	// Virtual Texture의 해당 위치에 Texture를 배치한다.
	// - MIPMAP Level 0만이 복사된다.
	// - Format은 Virtual Texture에 맞게 변환된다.
	bool PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY);

	i3VTSubsetController* GetSubsetController() const;
	i3VTFileFormatController* GetFileFormatController() const { return m_pController;  }
	// 호환성 때문에 포함해야한다..
	bool	LoadRefTextureInfo();
	bool	SaveRefTextureInfo();
	
private:

	i3VTFileFormatData		m_Data;
	i3VTFileFormatController*	m_pController;			// 가상디스패치 구현..
														// 래핑된 이유는 상태 수시변경이 있기 때문임..
};														// 널값허용하지 말것..

