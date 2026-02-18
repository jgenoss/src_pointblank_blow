#pragma once

class i3VirtualTexture;
class i3VTSubsetController;
struct I3VT_IOHANDLE;
struct i3VTFileFormatData;

// 기본 클래스..
class i3VTFileFormatController
{
public:
	i3VTFileFormatController( i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData);
	virtual ~i3VTFileFormatController();

	virtual i3VTFileFormatController*	MoveConvertedController() = 0;
	virtual bool						IsControllerNewFormat() const = 0;
	virtual bool						IsControllerGameOnly() const = 0;

	virtual bool Open(const char * pszPath) = 0;
	virtual bool Flush(i3FileStream * pFile) = 0;
	virtual bool Defragment() = 0;
	
	// Virtual Texture의 해당 위치에 Texture를 배치한다.
	// - MIPMAP Level 0만이 복사된다.
	// - Format은 Virtual Texture에 맞게 변환된다.
	virtual bool PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY) = 0;
	virtual i3VTSubsetController* GetSubsetController() const = 0;

	// 호환성 때문에 포함해야한다..
	virtual bool	LoadRefTextureInfo() = 0;
	virtual bool	SaveRefTextureInfo() = 0;

	i3VirtualTexture*		GetVTex() const { return m_Owner;  }			// 항상 존재해야함..
	i3VTFileFormatData*		GetVTFileFormatData() const { return m_pFFData;  }	// 항상 존재해야함..

private:
	void			RemoveAllFileHandle();					// 비가상함수..
	void			ResetAllFileHandleOffset(UINT64 zero_idx_offset);	// ???
	void			TruncateAllFileHandle();

	virtual void	SetupMainFileBaseOffset() = 0;
	virtual void			CreateAllFileHandle() = 0;			// 가상함수일 필요가 없다면, 멤버함수일 필요도 없으므로 좀더 판단해볼것..
	virtual I3VT_IOHANDLE*	CreateFileHandle(INT32 idxSubset) = 0;	// 이것 역시 가상함수일 필요가 있는지 조사해야함..
	
	i3VirtualTexture*		m_Owner;
	i3VTFileFormatData*		m_pFFData;			// 파일포맷전용 데이터..상위클래스가 소유권..널값금지..

	friend class i3VTFileFormat;
	friend class i3VTFileFormatController_Legacy;
	friend class i3VTFileFormatController_New;
	friend class i3VTFileFormatController_New_Tool;
	friend class i3VTSubsetController;
};

