#if !defined( __I3_SHADER_SOURCE_CODE_MGR_H)
#define __I3_SHADER_SOURCE_CODE_MGR_H

#include "i3Base/smart_ptr/shared_ptr.h"

class IShaderSourceCodeMgr;
struct i3ShaderDXInclude : ID3DXInclude
{
	i3ShaderDXInclude(IShaderSourceCodeMgr* pOwner) : m_pOwner(pOwner) {}
	virtual ~i3ShaderDXInclude() {}

	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Open(D3DXINCLUDE_TYPE type, LPCSTR pszName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
	virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Close(LPCVOID pData);

private:
	IShaderSourceCodeMgr* m_pOwner;
};

class IShaderSourceCodeMgr
{
public:
	IShaderSourceCodeMgr();
	virtual ~IShaderSourceCodeMgr();

	virtual bool			Load(const char* pszPath) = 0;

	virtual i3::string		GetSourceCode(const char* pszName) = 0;

	virtual const char*		GetName() const = 0;

	ID3DXInclude*			GetD3DXInclude() const { return m_pD3DXInclude; }

private:
	ID3DXInclude* m_pD3DXInclude;
};

I3_EXPORT_GFX IShaderSourceCodeMgr* GetShaderSourceCodeMgr();

I3_EXPORT_GFX IShaderSourceCodeMgr* GetShaderSourceCodeMgr_Memory();

I3_EXPORT_GFX IShaderSourceCodeMgr* GetShaderSourceCodeMgr_ResourcePack();

I3_EXPORT_GFX void					SetShaderSourceCodeMgr(IShaderSourceCodeMgr* mgr);

#endif