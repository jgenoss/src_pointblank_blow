#pragma once

#include "ContentsDefines.h"
#include "Settings.h"
#include "SyncProgressCallback.h"

namespace rsc
{
	enum SYNCHRONIZER_TYPE
	{
		SYNCHRONIZER_TYPE_CONTENTS,
		SYNCHRONIZER_TYPE_FILES,
		SYNCHRONIZER_TYPE_SHADER,
	};

	struct ContentsSyncData
	{
		SYNCHRONIZER_TYPE		type;
		ContentsDataPtr			contents;
		CountryDataPtr			country;
		std::vector<PathPtr>	fileRelPathList;
	};
	using ContentsSyncDataPtr = std::shared_ptr < ContentsSyncData >;

	class IFileCrawler;

	class IContentsSynchronizer
	{
	public:
		IContentsSynchronizer();
		virtual ~IContentsSynchronizer();

		virtual void operator()() const = 0;

		void SetProgressCallback(SyncProgressCallbackPtr cb) { m_progressCallback = cb; }

	protected:
		SyncProgressCallbackPtr m_progressCallback;
	};
	using ContentsSynchronizerPtr = std::shared_ptr<IContentsSynchronizer>;

	class Synchronizer_Null : public IContentsSynchronizer
	{
	public:
		virtual ~Synchronizer_Null() {}
		virtual void operator()() const {}
	};

	class ContentsSynchronizer_CopyPack : public IContentsSynchronizer
	{
	public:
		ContentsSynchronizer_CopyPack(ContentsDataPtr contentsData, CountryDataPtr countryData);
		virtual ~ContentsSynchronizer_CopyPack();

		virtual void operator()() const;

	private:
		ContentsDataPtr m_contentsData;
		CountryDataPtr m_countryData;
	};

	class FileSynchronizer_CopyPack : public IContentsSynchronizer
	{
	public:
		FileSynchronizer_CopyPack(ContentsDataPtr contentsData, CountryDataPtr countryData, const std::vector<PathPtr>& fileRelPathList);
		virtual ~FileSynchronizer_CopyPack();

		virtual void operator()() const;

	private:
		ContentsDataPtr m_contentsData;
		CountryDataPtr m_countryData;
		std::vector<PathPtr> m_fileRelPathList;
	};

	class ShaderSynchronizer : public IContentsSynchronizer
	{
	public:
		ShaderSynchronizer(const Path& HLSLFolderAbsPath, const Path& i3EngineTargetAbsPath);
		virtual ~ShaderSynchronizer();

		virtual void operator()() const;

	private:
		const Path m_HLSLFolderAbsPath;
		const Path m_i3EngineTargetAbsPath;
	};
}