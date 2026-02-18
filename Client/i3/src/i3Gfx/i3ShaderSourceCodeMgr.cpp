#include "i3GfxType.h"
#include "i3ShaderSourceCodeMgr.h"
#include "i3Base/itl/algorithm/transform.h"	
#include "i3Base/itl/algorithm/find_if.h"
#include "i3Base/i3PackFile.h"
#include "i3ShaderMemoryCode.h"

namespace
{
	void ToUpper(i3::string& inout)
	{
		i3::transform(inout.begin(), inout.end(), inout.begin(), toupper);
	}
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE i3ShaderDXInclude::Open(D3DXINCLUDE_TYPE type, LPCSTR pszName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
	i3::string code = m_pOwner->GetSourceCode(pszName);

	const size_t size = code.size();
	if (size == 0)
	{
		*ppData = nullptr;
		*pBytes = 0;
		return S_OK;
	}

	BYTE* pData = new BYTE[size];
	if (!pData)
		return E_OUTOFMEMORY;

	i3::copy(code.begin(), code.end(), pData);

	*ppData = pData;
	*pBytes = size;

	return S_OK;
}

COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE i3ShaderDXInclude::Close(LPCVOID pData)
{
	BYTE* pBuff = (BYTE*)pData;
	if (pBuff)
		delete[] pBuff;

	return S_OK;
}


IShaderSourceCodeMgr::IShaderSourceCodeMgr()
{
	m_pD3DXInclude = new i3ShaderDXInclude(this);
}

IShaderSourceCodeMgr::~IShaderSourceCodeMgr()
{
	delete m_pD3DXInclude;
}

namespace
{
	class ShaderSourceCodeMgr_Resource : public IShaderSourceCodeMgr
	{
		struct ShaderSourceCodeData
		{
			i3::string name;
			i3::vector<BYTE> buff;
		};
		typedef i3::shared_ptr<ShaderSourceCodeData> ShaderSourceCodeDataPtr;

	public:
		virtual ~ShaderSourceCodeMgr_Resource() {}

		virtual bool Load(const char* pszPath)
		{
			char szCurDir[MAX_PATH] = { 0, };
			::GetCurrentDirectory(MAX_PATH, szCurDir);

			i3::string i3PackAbsPath(szCurDir);
			i3PackAbsPath += pszPath;

			i3PackVolume* pVol = i3PackVolume::new_object();
			if (!pVol->LoadFromFile(i3PackAbsPath.c_str()))
			{
				pVol->Release();

				char err[MAX_PATH] = { 0 };
				sprintf_s(err, MAX_PATH, "Failed Loading HLSL Pack : %s", pszPath);

				I3PRINTLOG(I3LOG_FATAL, err)
				return false;
			}

			m_dataList.clear();

			i3PackNode* pRootNode = pVol->getRoot();
			for (INT32 i = 0; i < pRootNode->getChildCount(); i++)
			{
				i3PackNode* pNode = (i3PackNode*)pRootNode->getChild(i);
				for (INT32 j = 0; j < pNode->getFileCount(); j++)
				{
					I3_PACK_FILE_REF* pInfo = pNode->getFile(j);
							
					i3PackFile* pPack = i3PackFile::new_object();
					pPack->Open(pVol, pNode, pInfo);

					i3::vector<BYTE> buff;
					buff.resize(pInfo->m_Size);
					pPack->Read(&buff[0], pInfo->m_Size);

					pPack->Release();

					_Register(pInfo->m_szName, buff);
				}
			}

			pVol->Release();

			return true;
		}

		virtual i3::string GetSourceCode(const char* pszName)
		{
			struct fn
			{
				i3::string str;		fn(const char* s) : str(s) { ToUpper(str); }
				bool operator()(const ShaderSourceCodeDataPtr& src) const { return (src->name == str); }
			};

			i3::vector<ShaderSourceCodeDataPtr>::iterator it = i3::find_if(m_dataList.begin(), m_dataList.end(), fn(pszName));

			if (it != m_dataList.end())
			{
				const i3::vector<BYTE>& buff = (*it)->buff;

				return i3::string(buff.begin(), buff.end());
			}

			return i3::string();	// nullptr.
		}

		const char* GetName() const
		{
			return "ResourcePackShader";
		}

	private:
		void _Register(const char* pszName, const i3::vector<BYTE>& buff)
		{
			ShaderSourceCodeDataPtr data = i3::make_shared<ShaderSourceCodeData>();
			
			i3::string name(pszName);
			ToUpper(name);
			data->name = name;

			data->buff.assign(buff.begin(), buff.end());

			m_dataList.push_back(data);
		}

		i3::vector<ShaderSourceCodeDataPtr> m_dataList;
	} g_mgr_resource_pack;

	class ShaderSourceCodeMgr_Memory : public IShaderSourceCodeMgr
	{
	public:
		virtual ~ShaderSourceCodeMgr_Memory() {}

		virtual bool Load(const char* pszPath) { return true; }

		virtual i3::string GetSourceCode(const char* pszName)
		{
			return GetShaderMemoryCode(pszName);
		}

		const char* GetName() const
		{
			return "MemoryShader";
		}
	} g_mgr_memory;

	IShaderSourceCodeMgr* g_curr_mgr = &g_mgr_memory;
}

I3_EXPORT_GFX
IShaderSourceCodeMgr* GetShaderSourceCodeMgr()
{
	return g_curr_mgr;
}

I3_EXPORT_GFX
IShaderSourceCodeMgr* GetShaderSourceCodeMgr_Memory()
{
	return &g_mgr_memory;
}

I3_EXPORT_GFX
IShaderSourceCodeMgr* GetShaderSourceCodeMgr_ResourcePack()
{
	return &g_mgr_resource_pack;
}

I3_EXPORT_GFX
void SetShaderSourceCodeMgr(IShaderSourceCodeMgr* mgr)
{
	if (mgr)
		g_curr_mgr = mgr;
}