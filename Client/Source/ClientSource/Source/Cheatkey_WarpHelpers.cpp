#include "pch.h"
#include "Cheatkey_WarpHelpers.h"
#include "BattleSlotContext.h"
#include "Cheatkey_Misc.h"
#include <sstream>
#include <functional>

namespace Cheatkey
{
	namespace WarpHelpers
	{
		//----------------------------------------------
		// 이동 캐릭터 찾기
		//----------------------------------------------
		Parser_SearchMovementPlayer::Parser_SearchMovementPlayer()
		{
			_Regist("slot", &Parser_SearchMovementPlayer::_SearchByNumOfSlot);
			_Regist("nick", &Parser_SearchMovementPlayer::_SearchByNick);
		}

		Parser_SearchMovementPlayer::~Parser_SearchMovementPlayer()
		{
			i3::cu::for_each_delete_clear(m_elems);
		}

		bool Parser_SearchMovementPlayer::operator()(const i3::string& param, i3::vector<INT32>& players)
		{
			i3::vector<i3::string> tokens;
			Tokenize(param, tokens, ", '\"[]<>{}()", "");
			if (tokens.empty()) return false;

			const i3::string& key = tokens[0];

			i3::vector<i3::string> params;
			for (size_t i=1; i<tokens.size(); i++)
			{
				if (tokens[i] == "") continue;
				params.push_back(tokens[i]);
			}

			struct fn_same : binary_function<Element*, i3::string, bool>
			{
				bool operator()(const Element* lhs, const i3::string& rhs) const
				{
					return (lhs->key == rhs) ? true : false;
				}
			};
			i3::vector<Element*>::iterator it = 
				std::find_if(m_elems.begin(), m_elems.end(), std::bind2nd(fn_same(), key));

			if (it != m_elems.end())
			{
				mf f = (*it)->mfPtr;
				return (this->*f)(params, players);
			}

			return false;
		}

		void Parser_SearchMovementPlayer::_Regist(const i3::string& key, const mf f)
		{
			Element* elem = new Element;

			i3::string k(key); i3::to_upper(k); TrimEmptySpace(k);
			elem->key = k;

			elem->mfPtr = f;

			m_elems.push_back(elem);
		}

		bool Parser_SearchMovementPlayer::_SearchByNumOfSlot(const i3::vector<i3::string>& tokens, i3::vector<INT32>& players)
		{
			INT32 numOfSlot = -1;

			for (size_t i=0; i<tokens.size(); i++)
			{
				numOfSlot = ::atoi(tokens[i].c_str());
				
				if (numOfSlot < 0 || 16 < numOfSlot)
					numOfSlot = BattleSlotContext::i()->getMySlotIdx();

				CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(numOfSlot);
				if (player)
					players.push_back(numOfSlot);
			}

			if (players.empty()) return false;

			i3::vector<INT32>::iterator pos = std::unique(players.begin(), players.end());
			if (pos)
				players.erase(pos, players.end());

			return true;
		}

		bool Parser_SearchMovementPlayer::_SearchByNick(const i3::vector<i3::string>& tokens, i3::vector<INT32>& players)
		{
			for (size_t i=0; i<tokens.size(); i++)
			{
				i3::wstring nick; nick.assign(tokens[i].begin(), tokens[i].end());

				for (size_t slot=0; slot<SLOT_MAX_COUNT; slot++)
				{
					i3::wstring search(BattleSlotContext::i()->getNickForSlot(slot));

					if (nick == search)
					{
						CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(slot);
						if (player)
							players.push_back(slot);

						continue;
					}
				}
			}

			if (players.empty()) return false;

			i3::vector<INT32>::iterator pos = std::unique(players.begin(), players.end());
			if (pos)
				players.erase(pos, players.end());

			return true;
		}

		//----------------------------------------------
		// 이동 목적지 파싱
		//----------------------------------------------
		Parser_SearchMovementDestination::Parser_SearchMovementDestination()
		{
			_Regist("slot", &Parser_SearchMovementDestination::_SearchByNumOfSlot);
			_Regist("nick", &Parser_SearchMovementDestination::_SearchByNick);
			_Regist("pos", &Parser_SearchMovementDestination::_SearchByPos);
			_Regist("set", &Parser_SearchMovementDestination::_SearchByLocationName);
		}

		Parser_SearchMovementDestination::~Parser_SearchMovementDestination()
		{
			i3::cu::for_each_delete_clear(m_elems);
		}

		bool Parser_SearchMovementDestination::operator()(const i3::string& param, VEC3D& pos)
		{
			i3::vector<i3::string> tokens; 
			Tokenize(param, tokens, ", '\"[]<>{}()", "");
			if (tokens.empty()) return false;

			const i3::string& key = tokens[0];

			i3::vector<i3::string> params;
			for (size_t i=1; i<tokens.size(); i++)
			{
				if (tokens[i] == "") continue;
				params.push_back(tokens[i]);
			}

			struct fn_same : std::binary_function<Element*, i3::string, bool>
			{
				bool operator()(const Element* lhs, const i3::string& rhs) const
				{
					return (lhs->key == rhs) ? true : false;
				}
			};
			i3::vector<Element*>::iterator it = 
				std::find_if(m_elems.begin(), m_elems.end(), std::bind2nd(fn_same(), key));

			if (it != m_elems.end())
			{
				mf f = (*it)->mfPtr;
				return (this->*f)(params, pos);
			}

			return false;
		}

		void Parser_SearchMovementDestination::_Regist(const i3::string& key, const mf f)
		{
			Element* elem = new Element;

			i3::string k(key); i3::to_upper(k); TrimEmptySpace(k);
			elem->key = k;

			elem->mfPtr = f;

			m_elems.push_back(elem);
		}

		bool Parser_SearchMovementDestination::_SearchByNumOfSlot(const i3::vector<i3::string>& tokens, VEC3D& pos)
		{
			INT32 numOfSlot = ::atoi(tokens[0].c_str());

			if (numOfSlot < 0 || 16 < numOfSlot)
				numOfSlot = BattleSlotContext::i()->getMySlotIdx();

			CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(numOfSlot);
			if (player == nullptr) return false;

			i3Vector::Copy(&pos, player->GetPos());

			return true;
		}

		bool Parser_SearchMovementDestination::_SearchByNick(const i3::vector<i3::string>& tokens, VEC3D& pos)
		{
			for (INT32 i=0; i<SLOT_MAX_COUNT; i++)
			{
				i3::wstring nick; nick.assign(tokens[0].begin(), tokens[0].end());
				i3::wstring search(BattleSlotContext::i()->getNickForSlot(i));

				if (nick == search)
				{
					CGameCharaBase* player = g_pCharaManager->getCharaByNetIdx(i);
					if (player == nullptr) return false;

					i3Vector::Copy(&pos, player->GetPos());
					return true;
				}
			}

			return false;
		}

		bool Parser_SearchMovementDestination::_SearchByPos(const i3::vector<i3::string>& tokens, VEC3D& pos)
		{
			if (tokens.size() != 3) return false;

			pos.x = static_cast<REAL32>(atof(tokens[0].c_str()));
			pos.y = static_cast<REAL32>(atof(tokens[1].c_str()));
			pos.z = static_cast<REAL32>(atof(tokens[2].c_str()));

			return true;
		}

		bool Parser_SearchMovementDestination::_SearchByLocationName(const i3::vector<i3::string>& tokens, VEC3D& pos)
		{
			WarpKey k;
			k.locationName = tokens[0].c_str();
			k.stageID = g_pFramework->GetCurrentStageIndex();

			if (WarpStorageInst().GetLocation(k, pos))
				return true;

			return false;
		}

		//----------------------------------------------
		// 위치 저장 기능 : XML
		//----------------------------------------------
		WarpXmlFile::WarpXmlFile()
		{
			WIN32_FIND_DATA fd;
			HANDLE h = ::FindFirstFile("Cheatkey_WarpLocations.xml", &fd);
			if (h == INVALID_HANDLE_VALUE)
			{
				HANDLE f = ::CreateFile(TEXT("Cheatkey_WarpLocations.xml"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				CloseHandle(f);
			}
			FindClose(h);

			m_doc.LoadFile("Cheatkey_WarpLocations.xml");
		}

		WarpXmlFile::~WarpXmlFile()
		{
			m_doc.Clear();
		}

		bool WarpXmlFile::Load(WarpStorage& memory)
		{
			TiXmlNode* stages = m_doc.FirstChild("Stages");
			if (stages == nullptr) return false;

			for (TiXmlElement* stageElem = stages->FirstChildElement("Stage");
				stageElem != nullptr; stageElem = stageElem->NextSiblingElement())
			{
				WarpKey k;

				stageElem->Attribute("Index", &k.stageID);

				for (TiXmlElement* infoElem = stageElem->FirstChildElement("Info");
					infoElem != nullptr; infoElem = infoElem->NextSiblingElement())
				{
					VEC3D pos;

					k.locationName = infoElem->Attribute("LocationName");
					i3::to_upper(k.locationName);

					std::string strpos = infoElem->Attribute("Position");
					std::istringstream isspos(strpos);
					isspos >> pos.x >> pos.y >> pos.z;

					memory.AddLocation(k, pos);
				}
			}

			return true;
		}

		void WarpXmlFile::Save(WarpStorage& memory)
		{
			if (memory.empty()) return;

			// 실제 데이터 저장 전 파일 클리어.
			m_doc.Clear();
			m_doc.SaveFile();

			// 정렬.
			i3::vector<WarpKey> keys;
			memory.FillupAllKeysTo(keys);

			struct fn
			{
				bool operator()(const WarpKey& lhs, const WarpKey& rhs) const
				{
					if (lhs.stageID < rhs.stageID) return true;
					else if (lhs.stageID == rhs.stageID)
					{
						if (lhs.locationName < rhs.locationName) return true;
					}

					return false;
				}
			};
			i3::sort(keys.begin(), keys.end(), fn());

			// 저장.
			TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "euc-kr", "");
			m_doc.LinkEndChild(decl);

			TiXmlElement* stages = new TiXmlElement("Stages");
			m_doc.LinkEndChild(stages);

			INT32 currStageID = -1;
			TiXmlElement* stage = nullptr;
			for (size_t i=0; i<keys.size(); i++)
			{
				const WarpKey& k = keys[i];
				VEC3D pos; memory.GetLocation(k, pos);

				if (currStageID != k.stageID)
				{
					stage = new TiXmlElement("Stage");
					stage->SetAttribute("Index", k.stageID);
					stages->LinkEndChild(stage);

					currStageID = k.stageID;
				}

				I3ASSERT_RETURN(stage);

				TiXmlElement* info = new TiXmlElement("Info");

				info = (TiXmlElement*)stage->LinkEndChild(info);

				if( info != nullptr)
				{
					info->SetAttribute("LocationName", k.locationName.c_str());

					std::ostringstream ossLoc;
					ossLoc << pos.x << " " << pos.y << " " << pos.z;
					info->SetAttribute("Position", ossLoc.str().c_str());
				}
			}

			m_doc.SaveFile();
			keys.clear();
		}

		//----------------------------------------------
		// 워프 저장 기능 : 메모리
		//----------------------------------------------
		WarpStorage& WarpStorageInst()
		{
			static WarpStorage s;
			return s;
		}

		WarpStorage::WarpStorage()
		{
			WarpXmlFile f;
			f.Load(*this);
		}

		WarpStorage::~WarpStorage()
		{
			WarpXmlFile f;
			f.Save(*this);

			m_pool.clear();
		}
		void WarpStorage::AddLocation(const WarpKey& k, const VEC3D& pos)
		{
			//m_pool[k] = pos; // 무조건 저장.

			LookupTable::iterator it = m_pool.find(k);
			if (it != m_pool.end())
			{
				VEC3D& val = it->second;
				val.x = pos.x;
				val.y = pos.y;
				val.z = pos.z;
			}
			else
				m_pool.insert(LookupTable::value_type(k, pos));
		}

		bool WarpStorage::GetLocation(const WarpKey& k, VEC3D& outPos)
		{
			LookupTable::iterator it = m_pool.find(k);

			if (it != m_pool.end())
			{
				VEC3D& pos = it->second;
				i3Vector::Copy(&outPos, &pos);
				return true;
			}

			return false;
		}

		bool WarpStorage::DelLocation(const WarpKey& k)
		{
			LookupTable::iterator it = m_pool.find(k);

			if (it != m_pool.end())
			{
				m_pool.erase(it);
				return true;
			}

			return false;
		}

		bool WarpStorage::empty() const { return m_pool.empty(); }

		void WarpStorage::FillupAllKeysTo(i3::vector<WarpKey>& keys)
		{
			LookupTable::iterator it;
			for (it = m_pool.begin(); it != m_pool.end(); it++)
			{
				keys.push_back(it->first);
			}
		}
	}
}