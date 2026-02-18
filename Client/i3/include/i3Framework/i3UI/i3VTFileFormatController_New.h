#pragma once

#include "i3VTFileFormatController.h"

struct i3VTFileFormatData;

class i3VTFileFormatController_New : public i3VTFileFormatController
{
public:
	i3VTFileFormatController_New(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData);

	virtual i3VTFileFormatController*	MoveConvertedController() override;

	virtual bool	IsControllerNewFormat() const { return true; }
	virtual bool	IsControllerGameOnly() const { return true; }

	virtual bool Open(const char * pszPath) override;
	virtual bool Flush(i3FileStream * pFile) override;
	virtual bool Defragment() override;

	virtual bool PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY) override;

	virtual i3VTSubsetController* GetSubsetController() const { return nullptr; }

	virtual bool	LoadRefTextureInfo() override { return false; }
	virtual bool	SaveRefTextureInfo() override { return false; }

private:
	virtual void			SetupMainFileBaseOffset() override {}
	virtual void			CreateAllFileHandle() override;			// 가상함수일 필요가 없다면, 멤버함수일 필요도 없으므로 좀더 판단해볼것..
	virtual I3VT_IOHANDLE*	CreateFileHandle(INT32 idxTexInfo) override;	// 이것 역시 가상함수일 필요가 있는지 조사해야함..
	friend class i3VTFileFormatController_New_Tool;
};

