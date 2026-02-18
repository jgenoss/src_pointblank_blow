#ifndef _CAMERA_LOCATION_CHANGE_H
#define _CAMERA_LOCATION_CHANGE_H

#include <unordered_map>

namespace CameraLocationChange
{
	struct Key
	{
		UINT32  stage = 0;
		I3I_KEY inputKey = I3I_KEY_UNKNOWN;

		Key() {}
		Key(INT32 s, I3I_KEY k)
		{
			stage = s;
			inputKey = k;
		}
		Key(const Key& src) { Copy(this, &src); }
		void operator=(const Key& src)	{ Copy(this, &src); }

	private:
		void Copy(Key* dst, const Key* src)
		{
			dst->stage = src->stage;
			dst->inputKey = src->inputKey;
		}
	};
}

template<>
struct i3::hash<CameraLocationChange::Key>
{
	size_t operator()(const CameraLocationChange::Key& k) const
	{
		return reinterpret_cast<const UINT16&>(k);
	}
};

namespace CameraLocationChange
{
	inline bool operator == (const CameraLocationChange::Key& lhs, const CameraLocationChange::Key& rhs)
	{
		if (lhs.stage == rhs.stage && lhs.inputKey == rhs.inputKey) return true;

		return false;
	}

	struct Val
	{
		MATRIX	mat;
		VEC3D	mouseDelta;
		REAL32  rotY = 0.0f;
		REAL32  prerotY = 0.0f;

		Val() {}
		Val(MATRIX* mat_, VEC3D* mouseDelta_, REAL32 rotY_, REAL32 prerotY_ )
		{
			i3Matrix::Copy(&mat, mat_);
			i3Vector::Copy(&mouseDelta, mouseDelta_);
			rotY = rotY_;
			prerotY = prerotY_;
		}
		Val(const Val& src) { Copy(this, &src); }
		void operator=(const Val& src)	{ Copy(this, &src); }

	private:
		void Copy(Val* dst, const Val* src)
		{
			i3Matrix::Copy(&dst->mat, &src->mat);
			i3Vector::Copy(&dst->mouseDelta, const_cast<VEC3D*>(&src->mouseDelta));
			rotY = src->rotY;
			prerotY = src->prerotY;
		}
	};

	class MoveLocation : public i3::class_common_pool<MoveLocation>				// 16바이트 정렬 강제되므로, 일단 공용풀에 연결.. (i3Engine의 MATRIX가 16바이트 정렬 강제상태입니다.)
	{
	public:
		MoveLocation(const Key& key, const Val& val);
		virtual ~MoveLocation() {}

		void SetValue(const Val& val);
		const Val& GetValue() const;

		virtual void operator()();

	private:
		Key	m_key;
		Val	m_val;	
	};

	// 저장소 : 첫 호출 시점 ~ 게임종료, 전역으로 생성한다.
	class Storage
	{
		friend Storage& TheStorage();

	public:
		typedef i3::unordered_map<Key, MoveLocation*> KeyMap;
		typedef KeyMap::iterator iterator;

		~Storage();

		MoveLocation* Add(const Key& key, const Val& val);
		MoveLocation* Find(const Key& key);

		iterator begin();
		iterator end();
		bool	 empty() const;

	private:
		Storage();
		Storage(const Storage& rhs);

		KeyMap m_pool;

		struct PoolDeleter
		{
		public:
			PoolDeleter() : count(0) {}
			void operator()(i3::pair<Key, MoveLocation*> i);
			operator UINT32 () { return count; }

		private:
			UINT32 count;
		};
	};
	Storage& TheStorage();

	class XmlFile
	{
	public:
		XmlFile();
		~XmlFile();

		void Load();
		void Save();

	private:
		TiXmlDocument m_doc;
	};
}

#endif
