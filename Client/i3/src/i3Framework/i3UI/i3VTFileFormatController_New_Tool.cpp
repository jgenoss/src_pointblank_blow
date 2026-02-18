#include "i3FrameworkPCH.h"
#include "i3VTFileFormatController_New_Tool.h"

#include "i3VTFileFormatController_Legacy.h"
#include "i3VTSubsetController.h"
#include "i3VirtualTexture.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/system/traverse_file.h"
#include "i3UIFilePathMgr.h"
#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/string/system/copy_file.h"
#include "i3Base/string/system/is_directory.h"

i3VTFileFormatController_New_Tool::i3VTFileFormatController_New_Tool(i3VirtualTexture* pOwner, i3VTFileFormatData* pFFData)
	: i3VTFileFormatController_New(pOwner, pFFData)
{
	m_pSubsetController = new i3VTSubsetController(pOwner);		// 널값을 허용해선 안된다..
}

i3VTFileFormatController_New_Tool::i3VTFileFormatController_New_Tool(i3VirtualTexture* pOwner,
	i3VTFileFormatData* pFFData, i3VTSubsetController* pSCToMove) : i3VTFileFormatController_New(pOwner, pFFData)
{
	m_pSubsetController = pSCToMove;

	const i3::rc_string& strName = GetVTex()->GetNameString();
	i3::rc_string strNewName;
	i3::change_file_ext_copy(strNewName, strName, "i3VTexList");
	GetVTex()->SetName(strNewName);

}

i3VTFileFormatController*	i3VTFileFormatController_New_Tool::MoveConvertedController()
{
	i3VTFileFormatController* pConverted = new i3VTFileFormatController_Legacy(this->GetVTex(), this->GetVTFileFormatData(), m_pSubsetController);
	m_pSubsetController = nullptr;		// Moved..
	
	delete this;					// delete self
	return pConverted;
}

i3VTFileFormatController_New_Tool::~i3VTFileFormatController_New_Tool()
{
	delete m_pSubsetController;
}

namespace
{
	void CheckAndCopyVTexImageFiles()
	{

		i3::rc_string strCommonVTexDir = i3UIFilePathMgr::i()->Retrieve_CommonUI_VTexDirectory_i3VTexListFormat(true);
		i3::rc_string strLocaleVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);

		if (i3::is_directory(strCommonVTexDir) == false)
			return;
		if (i3::is_directory(strLocaleVTexDir) == false)
			return;
		
		i3::vector<i3::rc_string> vecCommonTexFiles;
		i3::vector<i3::rc_string> vecLocaleTexFiles;
		
		{
			struct FileCB : i3::traverse_file_callback_mb
			{
				i3::vector<i3::rc_string>& vecFile;
				FileCB(i3::vector<i3::rc_string>& vecFile) : vecFile(vecFile) {}
				virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
				{
					vecFile.push_back(fd.cFileName);
				}
				virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path)
				{
					return false;
				}
			};

			i3::literal_range strFilter = "/*.i3VTexImage";

			i3::stack_string strInputPath = strCommonVTexDir;	strInputPath += strFilter;

			FileCB cbCommon(vecCommonTexFiles);
			i3::traverse_file(strInputPath, &cbCommon, false);

			strInputPath = strLocaleVTexDir;	strInputPath += strFilter;

			FileCB cbLocale(vecLocaleTexFiles);
			i3::traverse_file(strInputPath, &cbLocale, false);
		}

		i3::vector<i3::rc_string> vecNeedToCopy;
		typedef i3::vector<i3::rc_string>::iterator iterator;

		for (iterator itCommon = vecCommonTexFiles.begin(); itCommon != vecCommonTexFiles.end(); ++itCommon )
		{
			bool foundSameFile = false;
			for (iterator itLocale = vecLocaleTexFiles.begin(); itLocale != vecLocaleTexFiles.end(); ++itLocale)
			{
				if (i3::generic_is_iequal(*itLocale, *itCommon))		// 동일한 경우 양쪽 다 erase처리..
				{
					vecLocaleTexFiles.erase(itLocale);
					foundSameFile = true;	break;
				}
			}
			
			if (foundSameFile == false)
				vecNeedToCopy.push_back(*itCommon);
		}
		
		const size_t numNeedToCopy = vecNeedToCopy.size();
	
		for (size_t i = 0; i < numNeedToCopy; ++i)
		{
			i3::copy_file(strCommonVTexDir, vecNeedToCopy[i], strLocaleVTexDir);
		}
	}
	
}

bool i3VTFileFormatController_New_Tool::Open(const char * pszPath)	// 툴 호환성문제로 subset/subsetRule을 읽어들이도록 한다.
{
	//
	// i3VTexImage 파일을 복사하는 루틴이 필요함!!
	//
	if (i3UIFilePathMgr::i()->IsLanguageLocaleEnabled())
	{
		CheckAndCopyVTexImageFiles();
	}
	
	bool bResult = i3VTFileFormatController_New::Open(pszPath);

	m_pSubsetController->LoadAllSubsetInfo();			// 서브셋 파일 콘트롤러는 레거시와 신규포맷툴에서만 쓰이게된다..

	return bResult;
}

bool i3VTFileFormatController_New_Tool::Flush(i3FileStream * pFile)		// 툴 호환성문제로 subset/subsetRule를 처리한다.
{
	bool bResult = i3VTFileFormatController_New::Flush(pFile);

	m_pSubsetController->SaveAllSubsetInfo();

	return bResult;
}

bool i3VTFileFormatController_New_Tool::Defragment()
{
	return i3VTFileFormatController_New::Defragment();		// 특별이 차이는 없어보인다..
}

bool i3VTFileFormatController_New_Tool::PlaceTexture(i3Texture * pTex, INT32 * pX, INT32 * pY)
{
	return i3VTFileFormatController_New::PlaceTexture(pTex, pX, pY);	//  특별이 차이는 없어보인다..
}

bool	i3VTFileFormatController_New_Tool::LoadRefTextureInfo()
{
	i3VTFileFormatController_New::LoadRefTextureInfo();
	return true;			// 사실은 아무것도 안읽어도 true처리..
}

bool	i3VTFileFormatController_New_Tool::SaveRefTextureInfo()
{
	// do not impl
	return true;
}

// 이하 3개 함수는 별 차이가 없는것 같지만 그냥 남겨둡니다.

void	i3VTFileFormatController_New_Tool::SetupMainFileBaseOffset()
{
	i3VTFileFormatController_New::SetupMainFileBaseOffset();
}

void	i3VTFileFormatController_New_Tool::CreateAllFileHandle()			// 가상함수일 필요가 없다면, 멤버함수일 필요도 없으므로 좀더 판단해볼것..
{
	i3VTFileFormatController_New::CreateAllFileHandle();
}

I3VT_IOHANDLE*	i3VTFileFormatController_New_Tool::CreateFileHandle(INT32 idxSubset)	// 이것 역시 가상함수일 필요가 있는지 조사해야함..
{
	return i3VTFileFormatController_New::CreateFileHandle(idxSubset);
}
