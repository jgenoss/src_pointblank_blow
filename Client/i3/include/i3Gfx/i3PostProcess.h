#if !defined(__I3_POSTPROCESS_H)
#define __I3_POSTPROCESS_H

#include "i3GfxResource.h"
#include "i3RenderTarget.h"

class i3PostProcessManager;

class I3_EXPORT_GFX i3PostProcess : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcess, i3GfxResource);

protected:
	bool		m_bIsEnable = false;
	bool		m_bIsLast = false;

public:
	virtual void			Create(i3PostProcessManager * pManager) {}
	virtual	void			ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput) {}

	//결과물이 새로운 렌더 타겟으로 나오는지 리턴해준다.
	//하위 Post Process에 따라 설정해주어야 할 값.
	virtual bool			IsResultOut()						{ return false; }

	virtual void			SetEnable(bool isEnable)			{ m_bIsEnable = isEnable; }
	virtual bool			IsEnable()							{ return m_bIsEnable; }

	virtual void			SetLast(bool isLast)				{ m_bIsLast = isLast; }
	virtual bool			IsLast()							{ return m_bIsLast; }

	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

};


#endif