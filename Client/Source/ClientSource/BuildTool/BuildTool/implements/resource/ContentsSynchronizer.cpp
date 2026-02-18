#include "stdafx.h"
#include "ContentsSynchronizer.h"
#include "ContentsCrawlerFactory.h"
#include "FileCrawlerFactory.h"
#include "FileSync_Factory.h"
#include "PackSync_Factory.h"
#include "MediaFileComparator.h"
#include "NativeFileCompare.h"
#include <boost/filesystem.hpp>
#include "Util/PathTokenizer.h"
#include "SyncShaders.h"
#include <boost/format.hpp>

namespace rsc
{
	IContentsSynchronizer::IContentsSynchronizer()
		: m_progressCallback(std::make_shared<SyncProgressCallback_Null>())
	{}

	IContentsSynchronizer::~IContentsSynchronizer() 
	{}


	// ------------------------------------------------------------------------------
	// 일반 리소스 복사/패킹 동기화.
	// ------------------------------------------------------------------------------
	ContentsSynchronizer_CopyPack::ContentsSynchronizer_CopyPack(ContentsDataPtr contentsData, CountryDataPtr countryData)
		: m_contentsData(contentsData), m_countryData(countryData)
	{}

	ContentsSynchronizer_CopyPack::~ContentsSynchronizer_CopyPack()
	{}

	void ContentsSynchronizer_CopyPack::operator()() const
	{
		m_progressCallback->OnStart();

		m_progressCallback->OnProgress(0.1f);

		// 관련 리소스 파일 모으기.
		auto key = std::make_pair(m_countryData->GetVersion(), m_contentsData->strCategoryName);
		
		auto crawler = FileCrawlerFactory::i()->Create(
			key, m_contentsData, m_countryData->GetMediaWorkdir(), m_countryData->GetCountryName());
		
		std::vector<PathPtr> fileRelPathList;
		crawler->operator()(fileRelPathList);

		// 미디어 데이터 검증(존재 유무).
		Path fileAbsPath;

		for (auto it = fileRelPathList.begin(); it != fileRelPathList.end();)
		{
			fileAbsPath = m_countryData->GetMediaWorkdir() / *(it->get());
			if (!fs::exists(fileAbsPath))
			{
				// 관련 데이터가 없을 수도 있다.
				// 예를 들어 i3s에서 참조하는 i3i 파일 경로가 잘 못
				// 입력되었다면 파일 찾기에 실패할 수 있다.
				I3TRACE("[Sync Media] : 오류 [%s]", fileAbsPath.string().c_str());

				// 없는 파일은 목록에서 삭제한 상태로 동기화 실행한다.
				it = fileRelPathList.erase(it);
				continue;
			}

			++it;
		}

		// 파일명 대소문자 구분.
		const std::wstring& wstrMediaWorkdir = m_countryData->GetMediaWorkdir().wstring();
		for (auto &fileRelPath : fileRelPathList)
		{
			ClassifyCapitalAndSmallLetter(fileRelPath, wstrMediaWorkdir);
		}

		// 파일 동기화 : Media -> QA
		m_progressCallback->OnProgress(0.7f);

		std::string ext;
		for (auto it = fileRelPathList.begin(); it != fileRelPathList.end();)
		{
			const Path& fileRelPath = *(it->get());

			ext = fileRelPath.extension().string();		
			i3::to_upper(ext);

			auto syncQA = FileSync_Factory::i()->RequestObj(ext, fileRelPath);
			
			if (!syncQA->operator()(*(it->get()), m_countryData->GetMediaWorkdir(), m_countryData->GetQAWorkdir()))
			{
				it = fileRelPathList.erase(it);
				continue;
			}

			++it;
		}
		
		// 파일 동기화 : QA -> Live.
		m_progressCallback->OnProgress(0.95f);

		auto syncLive = PackSync_Factory::i()->RequestObj(std::make_pair(m_countryData->GetVersion(), m_contentsData->strCategoryName));

		syncLive->operator()(fileRelPathList, m_countryData->GetQAWorkdir(), m_countryData->GetLiveWorkdir());

		m_progressCallback->OnEnd(true);
	}


	// --------------------------------------------
	//  특정 파일들 동기화.
	// --------------------------------------------
	FileSynchronizer_CopyPack::FileSynchronizer_CopyPack(ContentsDataPtr contentsData, CountryDataPtr countryData, const std::vector<PathPtr>& fileRelPathList)
		: m_contentsData(contentsData), m_countryData(countryData)
	{
		m_fileRelPathList.assign(fileRelPathList.begin(), fileRelPathList.end());
	}

	FileSynchronizer_CopyPack::~FileSynchronizer_CopyPack()
	{}

	void FileSynchronizer_CopyPack::operator()() const
	{
		m_progressCallback->OnStart();

		m_progressCallback->OnProgress(0.1f);

		std::vector<PathPtr> fileRelPathList;
		fileRelPathList.assign(m_fileRelPathList.begin(), m_fileRelPathList.end());

		// 미디어 데이터 검증(존재 유무).
		Path fileAbsPath;

		for (auto it = fileRelPathList.begin(); it != fileRelPathList.end();)
		{
			fileAbsPath = m_countryData->GetMediaWorkdir() / *(it->get());
			if (!fs::exists(fileAbsPath))
			{
				// 관련 데이터가 없을 수도 있다.
				// 예를 들어 i3s에서 참조하는 i3i 파일 경로가 잘 못
				// 입력되었다면 파일 찾기에 실패할 수 있다.
				I3TRACE("[Sync Media] : 오류 [%s]", fileAbsPath.string().c_str());

				// 없는 파일은 목록에서 삭제한 상태로 동기화 실행한다.
				it = fileRelPathList.erase(it);
				continue;
			}

			++it;
		}

		// 파일명 대소문자 구분.
		const std::wstring& wstrMediaWorkdir = m_countryData->GetMediaWorkdir().wstring();
		for (auto &fileRelPath : fileRelPathList)
		{
			ClassifyCapitalAndSmallLetter(fileRelPath, wstrMediaWorkdir);
		}

		// 파일 동기화 : Media -> QA
		m_progressCallback->OnProgress(0.7f);

		std::string ext;
		for (auto it = fileRelPathList.begin(); it != fileRelPathList.end();)
		{
			const Path& fileRelPath = *(it->get());

			ext = fileRelPath.extension().string();
			i3::to_upper(ext);

			auto syncQA = FileSync_Factory::i()->RequestObj(ext, fileRelPath);

			if (!syncQA->operator()(*(it->get()), m_countryData->GetMediaWorkdir(), m_countryData->GetQAWorkdir()))
			{
				it = fileRelPathList.erase(it);
				continue;
			}

			++it;
		}

		// 파일 동기화 : QA -> Live.
		m_progressCallback->OnProgress(0.95f);

		PackSyncPtr syncLive;
		if (!m_contentsData->strCategoryName.empty())
		{
			syncLive = PackSync_Factory::i()->RequestObj(std::make_pair(m_countryData->GetVersion(), m_contentsData->strCategoryName));
			syncLive->operator()(fileRelPathList, m_countryData->GetQAWorkdir(), m_countryData->GetLiveWorkdir());
		}
		else
		{
			// 카테고리를 모르므로 등록된 모든 패킹 객체를 대상으로 수행.
			std::vector<std::string> categoryNames;
			PackSync_Factory::i()->GetCategoryNamesByVer(m_countryData->GetVersion(), categoryNames);

			for (const auto& categoryName : categoryNames)
			{
				syncLive = PackSync_Factory::i()->RequestObj(std::make_pair(m_countryData->GetVersion(), categoryName));
				syncLive->operator()(fileRelPathList, m_countryData->GetQAWorkdir(), m_countryData->GetLiveWorkdir());
			}
		}

		m_progressCallback->OnEnd(true);
	}


	// --------------------------------------------
	//  셰이더 동기화.
	// --------------------------------------------
	ShaderSynchronizer::ShaderSynchronizer(const Path& HLSLFolderAbsPath, const Path& i3EngineTargetAbsPath)
		: m_HLSLFolderAbsPath(HLSLFolderAbsPath), m_i3EngineTargetAbsPath(i3EngineTargetAbsPath)
	{}

	ShaderSynchronizer::~ShaderSynchronizer() 
	{}

	void ShaderSynchronizer::operator()() const
	{
		m_progressCallback->OnStart();
		m_progressCallback->OnProgress(0.5f);

		if (SyncShaders_NonFormatted(m_HLSLFolderAbsPath, m_i3EngineTargetAbsPath))
		{
			const char* fmt = R"(shader memory code has been exported to below source files.
%s/include/i3Gfx/i3ShaderMemoryCode.h
%s/src/i3Gfx/i3ShaderMemoryCode.cpp)";

			const std::string strMessage = boost::io::str(boost::format(fmt) % m_i3EngineTargetAbsPath % m_i3EngineTargetAbsPath);
			
			std::wstring wstrMessage;
			i3::mb_to_utf16(strMessage, wstrMessage);

			m_progressCallback->OnEnd(true);
			::MessageBox(NULL, wstrMessage.c_str(), L"Shader Sync Succeeded", MB_OK);
			return;
		}
		
		::MessageBox(NULL, L"shader sync failed. please check paths of this version by version setting dialog.", L"Shader Sync Failed", MB_OK);
		m_progressCallback->OnEnd(true);
	}
}