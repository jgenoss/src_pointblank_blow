#pragma once


#include "i3VTFileFormatController_New.h"


class i3VTFileFormatController_New_Tool : public i3VTFileFormatController_New
{
public:
	i3VTFileFormatController_New_Tool(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData);

	i3VTFileFormatController_New_Tool(i3VirtualTexture* pOwner,
		i3VTFileFormatData* pFFData, i3VTSubsetController* pSCToMove);

	virtual ~i3VTFileFormatController_New_Tool();

	virtual i3VTFileFormatController*	MoveConvertedController() override;

	virtual bool	IsControllerNewFormat() const { return true; }
	virtual bool	IsControllerGameOnly() const { return false; }

	virtual bool Open(const char * pszPath) override;		// 툴 호환성문제로 subset/subsetRule를 처리한다.
	virtual bool Flush(i3FileStream * pFile) override;		// 툴 호환성문제로 subset/subsetRule를 처리한다.
	virtual bool Defragment() override;
	
	virtual bool PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY) override;
	virtual i3VTSubsetController* GetSubsetController() const { return m_pSubsetController; }

	// 호환성 때문에 포함해야한다..
	virtual bool	LoadRefTextureInfo() override;
	virtual bool	SaveRefTextureInfo() override;


private:
	virtual void			SetupMainFileBaseOffset() override;
	virtual void			CreateAllFileHandle() override;			// 가상함수일 필요가 없다면, 멤버함수일 필요도 없으므로 좀더 판단해볼것..
	virtual I3VT_IOHANDLE*	CreateFileHandle(INT32 idxTexInfo) override;	// 이것 역시 가상함수일 필요가 있는지 조사해야함..

	i3VTSubsetController*	m_pSubsetController;			// [initialize at constructor]
};

