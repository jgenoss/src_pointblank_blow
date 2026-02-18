#include "pch.h"
#include "Cheatkey.h"
#include "Cheatkey_LimitPred.h"
#include "Cheatkey_Misc.h"
#include "Cheatkey_NullCommand.h"

#include "CCmdManager/Command/InGameCmd/PBCmd_Warp.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Camera.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_HUD.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Death.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Respawn.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Object.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Helper.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Rules.h"
#include "CCmdManager/Command/PBCmd_DevelopeHelpers.h"
#include "CCmdManager/Command/OutGameCmd/PBCmd_Lobby.h"
#include "CCmdManager/Command/InGameCmd/PBCmd_Character.h"
#include "CCmdManager/Command/OutGameCmd/PBCmd_Medal.h"

#include "i3Base/string/ext/lrtrim.h"
#include <functional>

namespace Cheatkey
{
	template<class T>
	class AddRunnableCommand : public ICommand
	{
	public:
		explicit AddRunnableCommand(ILimitPred* p) : ICommand(p) {}
		virtual ~AddRunnableCommand() {}

		static ICommand* Creator()
		{
			return new AddRunnableCommand(new LPred_PermittedAccountOnly(new LPred_Anywhere));
		}

	private:
		virtual bool _ParseParams(const i3::vector<i3::string>& params) override
		{
			m_params = params;
			return true;
		}

		virtual bool _Execute() override
		{
			Context::i()->AddRunnableCommand(&T::Creator, m_params);
			return true;
		}

	private:
		i3::vector<i3::string> m_params;
	};

	//----------------------------------------------
	// 커맨드 팩토리
	//----------------------------------------------
	class CommandFactory
	{
	public:
		typedef std::tr1::function<ICommand* ()> CreatorPtr;

		CommandFactory()
		{
			_Regist("/WP",				Warp::Creator);							// 서버 디버깅 필요.
			_Regist("/WPSAVE",			Warp_SaveLocation::Creator);
			_Regist("/WPDEL",			Warp_DelLocation::Creator);
			_Regist("/WARPALL",			WarpAll::Creator);						// 서버 디버깅 필요.
			_Regist("/WPSHOW",			Warp_ShowLocations::Creator);
			_Regist("/WPHIDE",			Warp_HideLocations::Creator);
			_Regist("/SPAWNOBJ",		RespawnObj::Creator);
			_Regist("/CAMERA3PFREE",	Toggle3pCamera::Creator);
			_Regist("/FLY",				ToggleFlyCamera::Creator);
			_Regist("/UI",				UIToggle::Creator);
			_Regist("/DIE",				Suicide::Creator);
			_Regist("/SPAWNTIME",		RespawnWaitTimeCtrl::Creator);
			_Regist("/SPAWNPOS",		RespawnPosCtrl::Creator);
			_Regist("/COL",				Collision::Creator);
			_Regist("/DEATHALL",		KillemAll::Creator);
			_Regist("/TS",				TS_IncrPlayTime::Creator);
			_Regist("/SOLO",			SoloPlay::Creator);						// 서버 디버깅 필요.
			_Regist("/HACKOFF",			HackDetectOff::Creator);				// 서버 디버깅 필요.
			_Regist("/OBDMG",			SetObjDamage::Creator);
			_Regist("/POPUP",			Popup::Creator);
			_Regist("/MESSAGEBOX",		MessageBox::Creator);
			_Regist("/TOPVIEW",			TopViewCamera::Creator);
			_Regist("/SLOTINFO",		SlotInfo::Creator);
			_Regist("/TABSHOW",			ShowTabminimapProps::Creator);
			_Regist("/KCOUNT",			KillCount::Creator);
			_Regist("/SWITCHCHC",		SwitchCharacter::Creator);
			_Regist("/FPS",				ShowProfile::Creator);
			_Regist("/MWIN",			ChangeMissionModeWinCount::Creator);	// 서버 디버깅 필요.
			_Regist("/LTIME",			ChangeBattleTimeUnlimit::Creator);		// 서버 디버깅 필요.
			_Regist("/PAUSE",			PauseBattle::Creator);					// 서버 디버깅 필요.
			_Regist("/SKILL",			Skill::Creator);
			_Regist("/ROUNDTIME",		RoundTimeCtrl::Creator);				// 서버 구현 필요.
			_Regist("/WC",				ThrowCollisionBox::Creator);
			_Regist("/MEDALC",			ChangeMedalCount::Creator);
			_Regist("/MEDALS",			ChangeMedalSet::Creator);
			_Regist("/INVINCIBLE",		Invincible::Creator);
			_Regist("/SETKILL",			SetKillCount::Creator);
			_Regist("/CLANWARM",			ClanWarMatchingPoint::Creator);
			_Regist("/CLANWARR",			ClanWarMatchResult::Creator);
			//_Regist("/POS",				AddRunnableCommand<ViewPosition>::Creator);

			_UniqueChecking();
		}

		virtual ~CommandFactory()
		{
			i3::cu::for_each_delete_clear(m_commands);
		}

		CreatorPtr FindCreator(const i3::string& keyword) const 
		{
			i3::string k(keyword);
			i3::to_upper(k);

			for (i3::vector<Elem*>::size_type i=0; i<m_commands.size(); i++)
			{
				if (m_commands[i]->key == k)
					return m_commands[i]->creator;
			}

			return CreatorPtr();
		}

	private:
		void _Regist(const char* key, CreatorPtr creator)
		{
			Elem* elem = new Elem;

			i3::string k(key); i3::to_upper(k); TrimEmptySpace(k);
			elem->key = k;

			elem->creator = creator;

			m_commands.push_back(elem);
		}

		void _UniqueChecking()
		{
			struct fn_duplicate
			{
				bool operator()(const Elem* lhs, const Elem* rhs) const
				{
					return lhs->key == rhs->key; 
				}
			};
			i3::vector<Elem*>::iterator pos = 
				std::unique(m_commands.begin(), m_commands.end(), fn_duplicate());

			if (pos != m_commands.end())
			{
				I3TRACE("cheat key registry : not unique. duplicated %d keys will be deleted. please check below list \n", m_commands.end() - pos);

				for (; pos != m_commands.end(); pos++)
				{
					I3TRACE("%s, ", (*pos)->key);
				}

				m_commands.erase(pos, m_commands.end());
			}
		}

		struct Elem
		{
			i3::string key; CreatorPtr creator;
		};
		i3::vector<Elem*> m_commands;
	}g_cmdFactory;

	//----------------------------------------------
	// 커맨드 생성 인터페이스
	//----------------------------------------------
	i3::shared_ptr<ICommand> CreateCommand(const i3::string& keyword)
	{
		CommandFactory::CreatorPtr f = g_cmdFactory.FindCreator(keyword);
		if (f)
			return i3::shared_ptr<ICommand>(f());

		return i3::shared_ptr<ICommand>(new Null(new LPred_Anywhere));
	}

	i3::shared_ptr<ICommand> CreateCommand(const char* keyword)
	{
		const i3::string strKeyword(keyword);
		return CreateCommand(strKeyword);
	}

	bool ParseCommand(const char* cheatkey, i3::string& outKeyword, i3::vector<i3::string>& outParams)
	{
		i3::string text(cheatkey); 
		i3::to_upper(text);
		i3::lrtrim(text);

		i3::vector<i3::string> tokens;
		Tokenize(text, tokens,  " ", "'\"[]<>{}:;?()");

		if (tokens.empty()) return false;

		outKeyword = tokens[0];

		for (size_t i=1; i<tokens.size(); i++)
			outParams.push_back(tokens[i]);

		return true;
	}


	//----------------------------------------------
	// 컨텍스트
	//----------------------------------------------
	
	Context::~Context()
	{
		CommandMapType::iterator it = m_commands.begin();
		for (; it != m_commands.end(); it++)
		{
			i3::vector<IRunnableCommand*>* cmdList = it->second;
			i3::cu::for_each_delete_clear(*cmdList);
		}

		i3::cu::for_each_delete_clear(m_commands);
	}

	void Context::AddRunnableCommand(CmdCreator key, const i3::vector<i3::string>& params)
	{
		if (key == nullptr) return;

		CommandMapType::iterator it = m_commands.find(key);
		if (it == m_commands.end())
		{
			m_commands.insert(CommandMapType::value_type(key, new i3::vector<IRunnableCommand*>));
			it = m_commands.find(key);
		}

		i3::vector<IRunnableCommand*>* cmdList = it->second;
		if (cmdList)
		{
			const CmdCreator creator = key;
			IRunnableCommand* cmd = creator();
			cmd->ParseParams(params);
			cmdList->push_back(cmd);
		}
	}

	void Context::DelRunnableCommand(CmdCreator key)
	{
		CommandMapType::iterator it = m_commands.find(key);
		if (it != m_commands.end())
		{
			i3::vector<IRunnableCommand*>* cmdList = it->second;
			i3::cu::for_each_delete_clear(*cmdList);
			m_commands.erase(it);
		}
	}

	void Context::OnUpdate(const REAL32 tm)
	{
		CommandMapType::iterator it = m_commands.begin();
		for (; it != m_commands.end(); it++)
		{
			i3::vector<IRunnableCommand*>* cmdList = it->second;
			if (!cmdList) continue;

			i3::vector<IRunnableCommand*>::iterator cmdIter = cmdList->begin();
			for (; cmdIter != cmdList->end();)
			{
				IRunnableCommand* cmd = *cmdIter;
				const bool success = (*cmd)(tm);
				if (!success)
				{
					delete cmd;
					cmdIter = cmdList->erase(cmdIter);
				}
				else
					++cmdIter;
			}
		}
	}
}