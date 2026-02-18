#pragma once

// 가급적 이 헤더파일은 전역 인클루드하지 말것..

#include "i3VTFileFormatController.h"

class i3VTFileFormatController_Legacy : public i3VTFileFormatController
{
public:
	i3VTFileFormatController_Legacy(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData);
	i3VTFileFormatController_Legacy(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData, i3VTSubsetController* pSCToMove);
	~i3VTFileFormatController_Legacy();

	virtual i3VTFileFormatController*	MoveConvertedController() override;

	virtual bool	IsControllerNewFormat() const { return false;  }
	virtual bool	IsControllerGameOnly() const { return false;  }

	virtual bool Open(const char * pszPath) override;
	virtual bool Flush(i3FileStream * pFile) override;
	virtual bool Defragment() override;

	virtual bool PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY) override;

	virtual i3VTSubsetController* GetSubsetController() const { return m_pSubsetController; }

	virtual bool	LoadRefTextureInfo() override;
	virtual bool	SaveRefTextureInfo() override;

private:
	virtual void			SetupMainFileBaseOffset() override;
	virtual void			CreateAllFileHandle() override;
	virtual I3VT_IOHANDLE*	CreateFileHandle(INT32 idxSubset) override;	// 이것 역시 가상함수일 필요가 있는지 조사해야함..

	i3VTSubsetController*	m_pSubsetController;			// [initialize at constructor]
	UINT64					m_MainFileBaseOffset = 0;		// .i3VTex의 이미지 데이터시작점

};
