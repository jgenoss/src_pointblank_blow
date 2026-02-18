#include "i3FrameworkPCH.h"
#include "i3UILibHdr.h"

#include "i3VirtualTexture.h"
#include "i3UIManager.h"
#include "i3UIFilePathMgr.h"

I3_CLASS_INSTANCE(i3UILibHdr);

void	i3UILibHdr::SetData(UINT32 nVersion, const i3::rc_string& strVTexPath, const i3::rc_string& strScript)
{
	m_nVersion = nVersion;
	m_strVTexPath = strVTexPath;
	m_strScript = strScript;
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹 사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct UILIBHDR_HEAD
	{
		UINT8 ID[4] = { 'U', 'L', 'H', '1' };					// 4
		UINT32 Version = 0;					// 4
		UINT32 VTexPathLength = 0;			// 4
		UINT32 ScriptPathLength = 0;		// 4
	
		UINT32 pad[12] = { 0 };
	}; // size : 64

}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32	i3UILibHdr::OnSave(i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;

	pack::UILIBHDR_HEAD head;

	Result = i3NamedElement::OnSave(pResFile);
	if (Result == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"i3NamedElement::OnSave()\"");
		return Result;
	}

	head.Version = m_nVersion;

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

	{
		m_strVTexPath.clear();
		i3::stack_string strUniquePath;

		if (pVTex != nullptr)
		{
			i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), strUniquePath);
			m_strVTexPath = strUniquePath;
		}

		head.VTexPathLength = strUniquePath.length();
	}

	head.ScriptPathLength = m_strScript.length();
	
	Rc = pResFile->Write(&head, sizeof(head));
	if (Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"pResFile->Write()\"");
		return Rc;
	}
	Result += Rc;

	if (head.VTexPathLength > 0)
	{
		Rc = pResFile->Write(m_strVTexPath.c_str(), head.VTexPathLength);
		if (Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Failed in \"i3NamedElement::OnSave()\"");
			return Rc;
		}
		Result += Rc;
	}

	if (head.ScriptPathLength > 0)
	{
		Rc = pResFile->Write(m_strScript.c_str(), head.ScriptPathLength);

		if (Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Failed in \"i3NamedElement::OnSave()\"");
			return Rc;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3UILibHdr::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;

	pack::UILIBHDR_HEAD head;

	Result = i3NamedElement::OnLoad(pResFile);

	if (Result == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"i3NamedElement::OnLoad()\"");
		return Result;
	}

	Rc = pResFile->Read(&head, sizeof(head));
	if (Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;

	m_nVersion = head.Version;

	{
		i3::stack_string	strRes(head.VTexPathLength, 0);
		Rc = pResFile->Read(&strRes[0], head.VTexPathLength);

		m_strVTexPath = strRes;
	}

	if (Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;

	{
		i3::stack_string	strRes(head.ScriptPathLength, 0);
		Rc = pResFile->Read(&strRes[0], head.ScriptPathLength);

		m_strScript = strRes;
	}

	if (Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;

	return Result;
}

bool	i3UILibHdr::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML(pFile, pXML);

	if (bRv == false)
		return false;

	pXML->addAttr("Version", m_nVersion);

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

	{
		m_strVTexPath.clear();

		i3::stack_string refinedPath;

		if (pVTex != nullptr)
		{
			i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), refinedPath);
			m_strVTexPath = refinedPath;
		}

		pXML->addAttr("VTexture", refinedPath.c_str());
	}

	pXML->addAttr("Script", m_strScript.c_str());

	I3TRACE("UILibHdr Saved to version: %d\n", m_nVersion);

	return true;
}

bool	i3UILibHdr::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->getAttr("Version", &m_nVersion);

	const char * pszScript = pXML->getAttr("Script");

	if (pszScript != nullptr)
	{
		m_strScript = pszScript;
	}

	const char * pszVTexture = pXML->getAttr("VTexture");

	if (pszVTexture != nullptr)
	{
		m_strVTexPath = pszVTexture;
	}

	I3TRACE("UILibHdr Loaded from version: %d\n", m_nVersion);

	return true;

}
