#pragma once

namespace rsc
{
	class i3pack;

	class IPackCommand
	{
	public:
		virtual ~IPackCommand() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) {}
	};

	class PackCommand_AddFileToPack : public IPackCommand
	{
	public:
		virtual ~PackCommand_AddFileToPack() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) override;

		static IPackCommand* Creator();
	};

	class PackCommand_RemoveFileFromPack : public IPackCommand
	{
	public:
		virtual ~PackCommand_RemoveFileFromPack() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) override;

		static IPackCommand* Creator();
	};

	class PackCommand_SavePack : public IPackCommand
	{
	public:
		virtual ~PackCommand_SavePack() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) override;

		static IPackCommand* Creator();
	};

	class PackCommand_EncryptFileInPack : public IPackCommand
	{
	public:
		virtual ~PackCommand_EncryptFileInPack() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) override;

		static IPackCommand* Creator();
	};

	class PackCommand_SetChecksumToPack : public IPackCommand
	{
	public:
		virtual ~PackCommand_SetChecksumToPack() {}

		virtual void operator()(std::shared_ptr<i3pack> i3PackPtr, const std::vector<std::string>& tokenList) override;

		static IPackCommand* Creator();
	};
}