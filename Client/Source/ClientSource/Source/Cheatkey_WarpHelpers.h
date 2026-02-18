#pragma once

#include "i3Base/itl/unordered_map.h"

namespace Cheatkey
{
	namespace WarpHelpers
	{
		class Parser_SearchMovementPlayer
		{
		public:
			Parser_SearchMovementPlayer();
			~Parser_SearchMovementPlayer();

			bool operator()(const i3::string& param, i3::vector<INT32>& players);

		private:
			bool _SearchByNumOfSlot(const i3::vector<i3::string>& tokens, i3::vector<INT32>& players);
			bool _SearchByNick(const i3::vector<i3::string>& tokens, i3::vector<INT32>& players);

		private:
			typedef bool (Parser_SearchMovementPlayer::*mf)(const i3::vector<i3::string>&, i3::vector<INT32>&);

			void _Regist(const i3::string& key, const mf f);

			struct Element
			{
				i3::string key; mf mfPtr;
			};
			i3::vector<Element*> m_elems;
		};

		class Parser_SearchMovementDestination
		{
		public:
			Parser_SearchMovementDestination();
			~Parser_SearchMovementDestination();

			bool operator()(const i3::string& param, VEC3D& pos);

		private:
			bool _SearchByNumOfSlot(const i3::vector<i3::string>& tokens, VEC3D& pos);
			bool _SearchByNick(const i3::vector<i3::string>& tokens, VEC3D& pos);
			bool _SearchByPos(const i3::vector<i3::string>& tokens, VEC3D& pos);
			bool _SearchByLocationName(const i3::vector<i3::string>& tokens, VEC3D& pos);

		private:
			typedef bool (Parser_SearchMovementDestination::*mf)(const i3::vector<i3::string>&, VEC3D&);

			void _Regist(const i3::string& key, const mf f);

			struct Element
			{
				i3::string key; mf mfPtr;
			};
			i3::vector<Element*> m_elems;
		};


		struct WarpKey
		{
			std::string locationName;
			INT32 stageID = 0;
		};
	}
	
}

template<>
struct i3::hash<Cheatkey::WarpHelpers::WarpKey>
{
	size_t operator()(const Cheatkey::WarpHelpers::WarpKey& k) const
	{
		return reinterpret_cast<const UINT16&>(k);
	}
};

namespace Cheatkey
{
	namespace WarpHelpers
	{
		class WarpStorage;
		class WarpXmlFile
		{
		public:
			WarpXmlFile();
			~WarpXmlFile();

			bool Load(WarpStorage& memory);
			void Save(WarpStorage& memory);

		private:
			TiXmlDocument m_doc;
		};

		class WarpStorage : i3::noncopyable
		{
		public:
			WarpStorage();
			~WarpStorage();

			void AddLocation(const WarpKey& k, const VEC3D& pos);
			bool GetLocation(const WarpKey& k, VEC3D& outPos);
			bool DelLocation(const WarpKey& k);
			bool empty() const;
			void FillupAllKeysTo(i3::vector<WarpKey>& keys);

		private:
			typedef i3::unordered_map<WarpKey, VEC3D> LookupTable; // 개수가 적으면 비효율적. 하지만 맵별로 10개 이상 저장 가능성 있기 때문에...
			LookupTable m_pool;
		};

		WarpStorage& WarpStorageInst();

		inline bool operator == (const WarpKey& lhs, const WarpKey& rhs)
		{
			if (lhs.stageID == rhs.stageID && lhs.locationName == rhs.locationName) return true;
			return false;
		}	
	}
}