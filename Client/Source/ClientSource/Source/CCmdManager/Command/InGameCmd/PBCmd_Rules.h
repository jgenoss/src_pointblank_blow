#pragma once

#include "Cheatkey_ICommand.h"

namespace Cheatkey
{
	class KillCount : public ICommand
	{
	public:
		explicit KillCount(ILimitPred* p) : ICommand(p) , m_killCount(0) {}
		virtual ~KillCount() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		INT32 m_killCount;
	};

	class RoundTimeCtrl : public ICommand
	{
	public:
		explicit RoundTimeCtrl(ILimitPred* p) : ICommand(p) {}
		virtual ~RoundTimeCtrl() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;
	};

	class TS_IncrPlayTime : public ICommand
	{
	public:
		explicit TS_IncrPlayTime(ILimitPred* p) : ICommand(p), m_minute(0) {}
		virtual ~TS_IncrPlayTime() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

	private:
		INT32 m_minute;
	};

	class SoloPlay : public ICommand
	{
	public:
		explicit SoloPlay(ILimitPred* p) : ICommand(p) {}
		virtual ~SoloPlay() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class HackDetectOff : public ICommand
	{
	public:
		explicit HackDetectOff(ILimitPred* p) : ICommand(p) {}
		virtual ~HackDetectOff() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class ChangeMissionModeWinCount : public ICommand
	{
	public:
		explicit ChangeMissionModeWinCount(ILimitPred* p) : ICommand(p), m_count_R(0), m_count_B(0) {}
		virtual ~ChangeMissionModeWinCount() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		UINT8 m_count_R, m_count_B;
	};

	class ChangeBattleTimeUnlimit : public ICommand
	{
	public:
		explicit ChangeBattleTimeUnlimit(ILimitPred* p) : ICommand(p) {}
		virtual ~ChangeBattleTimeUnlimit() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class PauseBattle : public ICommand
	{
	public:
		explicit PauseBattle(ILimitPred* p) : ICommand(p), m_sec(0) {}
		virtual ~PauseBattle() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		UINT32 m_sec;
	};

	class SetKillCount : public ICommand
	{
	public:
		explicit SetKillCount(ILimitPred* p) : ICommand(p), m_redKillCnt(0), m_blueKillCnt(0) {}
		virtual ~SetKillCount() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		UINT16 m_redKillCnt, m_blueKillCnt;
	};

	class ClanWarMatchingPoint : public ICommand
	{
	public:
		explicit ClanWarMatchingPoint(ILimitPred* p) : ICommand(p) {}
		virtual ~ClanWarMatchingPoint() {}

		static ICommand* Creator();

	private:
		virtual bool _Execute() override;
	};

	class ClanWarMatchResult : public ICommand
	{
	public:
		explicit ClanWarMatchResult(ILimitPred* p) : ICommand(p), m_eWinTeam(TEAM_RED) {}
		virtual ~ClanWarMatchResult() {}

		static ICommand* Creator();

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override;
		virtual bool _Execute() override;

		TEAM_TYPE	m_eWinTeam;
	};

}