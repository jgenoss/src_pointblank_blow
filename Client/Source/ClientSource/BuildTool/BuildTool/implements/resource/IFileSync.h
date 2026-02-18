#pragma once

namespace rsc
{
	struct FileSyncCallback
	{
		virtual void OnSucceeded(const Path& fileRelPath, 
								 const Path& srcWorkdir, 
								 const Path& dstWorkdir) 
		{}

		virtual void OnFailed(const Path& fileRelPath, 
							  const Path& srcWorkdir, 
							  const Path& dstWorkdir, 
							  const wchar_t* errMessage) 
		{}

		virtual ~FileSyncCallback() {}
	};

	using FileSyncCallbackPtr = std::shared_ptr<FileSyncCallback>;

	class IFileSync
	{
	public:
		IFileSync();
		virtual ~IFileSync() = 0;

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const = 0;

		void				SetCallback(FileSyncCallbackPtr callback)	{ m_callback = callback; }
		FileSyncCallback*	GetCallback() const							{ return m_callback.get(); }

	private:
		FileSyncCallbackPtr m_callback;
	};
}