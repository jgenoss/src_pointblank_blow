#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class Warp : public ICommand
	{
	public:
		explicit Warp(ILimitPred* p) : ICommand(p) {}
		virtual ~Warp() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		i3::vector<INT32> m_players;
		VEC3D m_destination;
	};

	class Warp_SaveLocation : public ICommand
	{
	public:
		explicit Warp_SaveLocation(ILimitPred* p) : ICommand(p) {}
		virtual ~Warp_SaveLocation() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		VEC3D m_pos;
		i3::string m_locationName;
	};

	class Warp_DelLocation : public ICommand
	{
	public:
		explicit Warp_DelLocation(ILimitPred* p) : ICommand(p) {}
		virtual ~Warp_DelLocation() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		i3::string m_locationName;
	};

	class WarpAll : public ICommand
	{
	public:
		explicit WarpAll(ILimitPred* p) : ICommand(p) {}
		virtual ~WarpAll() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		VEC3D m_destination;
	};

	class Warp_ShowLocations : public ICommand
	{
	public:
		explicit Warp_ShowLocations(ILimitPred* p) : ICommand(p) {}
		virtual ~Warp_ShowLocations() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class Warp_HideLocations : public ICommand
	{
	public:
		explicit Warp_HideLocations(ILimitPred* p) : ICommand(p) {}
		virtual ~Warp_HideLocations() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};
}