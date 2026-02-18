#include "pch.h"
#include "GameCameraManager.h"
#include "GameCamera_LocationChange.h"

#include "../GameCharaBase.h"
#include <sstream>

using namespace CameraLocationChange;

MoveLocation::MoveLocation(const Key& key, const Val& val) 
	: m_key(key), m_val(val)
{
}

void MoveLocation::SetValue(const Val& val)
{
	m_val = val;
}

const Val& MoveLocation::GetValue() const
{
	return m_val;
}

void MoveLocation::operator()()
{
	if (!i3::same_of<CGameCamera_FLY*>(g_pCamera))
	{
		CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FLY);
	}

	CGameCamera_FLY* flyCam = static_cast<CGameCamera_FLY*>(g_pCamera);
	flyCam->SetCameraDir(*(i3Matrix::GetAt(&m_val.mat)));
	flyCam->SetCameraUp(*(i3Matrix::GetUp(&m_val.mat)));
	flyCam->SetCameraRight(*(i3Matrix::GetRight(&m_val.mat)));
	flyCam->SetCameraPos(*(i3Matrix::GetPos(&m_val.mat)));
	flyCam->SetMouseDelta(m_val.mouseDelta);
	flyCam->SetRotateY(m_val.rotY);
	flyCam->SetPreRotateY(m_val.prerotY);
}



Storage& CameraLocationChange::TheStorage()
{
	static Storage storage;
	return storage;
}

Storage::Storage() 
{
	XmlFile f;
	f.Load();
}

Storage::~Storage() 
{
	// 종료 시점에서 정보를 xml파일에 저장한다.
	XmlFile f;
	f.Save();

#ifdef I3_DEBUG
	UINT32 count = 0;
	count = i3::for_each(m_pool.begin(), m_pool.end(), PoolDeleter());
	I3TRACE("CamFunctionFactory Delete Count : %d\n", count);
#else
	i3::for_each(m_pool.begin(), m_pool.end(), PoolDeleter());
#endif
}

void Storage::PoolDeleter::operator()(i3::pair<Key, MoveLocation*> i)
{
	MoveLocation* p = i.second;
	if (p)
	{
		count++;
		delete p; 
	}
}

MoveLocation* Storage::Add(const Key& key, const Val& val)
{
	MoveLocation* f = Find(key);

	if (f)
	{
		static_cast<MoveLocation*>(f)->SetValue(val);
	}
	else
	{
		f = new MoveLocation(key, val);
		m_pool.insert(KeyMap::value_type(key, f));
	}

	return f;
}

MoveLocation* Storage::Find(const Key& key)
{
	KeyMap::const_iterator it = m_pool.find(key);
	return (it != m_pool.end()) ? it->second : 0;
}

Storage::iterator Storage::begin()
{
	return m_pool.begin();
}

Storage::iterator Storage::end()
{
	return m_pool.end();
}

bool Storage::empty() const
{
	return m_pool.empty();
}


/*
	Storage(RAM) - Xml(DISK) 연동.

	Key, Value가 저장되는 데이터임.
		- Key : Stage Index attribute + InputKey
		- Value : matrix, mouse delta, rotation Y 

	Storage가 처음으로 호출되는 시점에 Xml로부터 내용을 읽어들여서 
	CameraLocationChange::Storage에 Key/Value를 저장하고 사용.

    Storage가 소멸하는 시점에 저장된 카메라 정보를 Xml 파일에 기록함.

	Xml 형식은 아래와 같음.

	<Stages>
		<Stage Index="0">
			<Info InputKey="0" Matrix="0.457099 0 -0.889416 0 0 1 0 0 0.889416 0 0.457099 0 3.22296 4.96532 27.6325 1 " MouseDelta="0 0 0" RotY="0.000000" PreRotY="-62.000000"/>
			<Info InputKey="1" Matrix="0.157121 0 -0.987579 0 -0.137444 0.990268 -0.021867 0 0.977968 0.139173 0.155591 0 8.11541 5.30751 24.5723 1 " MouseDelta="0 0 0" RotY="-8.000000" PreRotY="112.000000"/>
			<Info InputKey="2" Matrix="0.506035 0 0.862513 0 0.398264 0.887011 -0.233661 0 -0.765058 0.461748 0.448858 0 -8.68939 7.31104 28.4464 1 " MouseDelta="0 0 0" RotY="-27.500000" PreRotY="64.000000"/>
		</Stage>
		<Stage Index="12">
			<Info InputKey="1" Matrix="0.622268 0 -0.782804 0 -0.0204914 0.999657 -0.0162891 0 0.782536 0.0261769 0.622055 0 -4.18864 4.68123 23.5793 1 " MouseDelta="-6016.96 -3 0" RotY="-1.500000" PreRotY="-112.000000"/>
			<Info InputKey="4" Matrix="0.884987 0 -0.465616 0 0 1 0 0 0.465616 0 0.884987 0 -11.2799 4.61509 23.6606 1 " MouseDelta="-6064.5 0 0" RotY="0.000000" PreRotY="-62.000000"/>
		</Stage>
	</Stages>
*/

XmlFile::XmlFile()
{
	WIN32_FIND_DATA findData;
	HANDLE find = ::FindFirstFile("CameraLocationChange.xml", &findData);
	if (find == INVALID_HANDLE_VALUE)
	{
		HANDLE file = ::CreateFile(TEXT("CameraLocationChange.xml"), GENERIC_READ | GENERIC_WRITE, 0, NULL, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(file);
	}
	FindClose(find);

	m_doc.LoadFile("CameraLocationChange.xml");
}

XmlFile::~XmlFile()
{
	m_doc.Clear();
}

void XmlFile::Load()
{
	TiXmlNode* stages = m_doc.FirstChild("Stages");
	if (stages == 0) return;

	for (TiXmlElement* stageElem = stages->FirstChildElement("Stage");
		 stageElem != nullptr; stageElem = stageElem->NextSiblingElement())
	{
		Key k;

		int index = 0;
		stageElem->Attribute("Index", &index);
		k.stage = static_cast<UINT32>(index);

		for (TiXmlElement* infoElem = stageElem->FirstChildElement("Info");
			infoElem != nullptr; infoElem = infoElem->NextSiblingElement())
		{
			Val v;

			int inputKey = 0;
			infoElem->Attribute("InputKey", &inputKey);
			k.inputKey = static_cast<I3I_KEY>(inputKey);

			std::string szMatrix = infoElem->Attribute("Matrix");
			std::istringstream matrix(szMatrix);
			matrix	>> v.mat.m[0][0] >> v.mat.m[0][1] >> v.mat.m[0][2] >> v.mat.m[0][3]
					>> v.mat.m[1][0] >> v.mat.m[1][1] >> v.mat.m[1][2] >> v.mat.m[1][3]
					>> v.mat.m[2][0] >> v.mat.m[2][1] >> v.mat.m[2][2] >> v.mat.m[2][3]
					>> v.mat.m[3][0] >> v.mat.m[3][1] >> v.mat.m[3][2] >> v.mat.m[3][3];


			std::string  szMouseDelta = infoElem->Attribute("MouseDelta");
			std::istringstream mouseDelta(szMouseDelta);
			mouseDelta >> v.mouseDelta.x >> v.mouseDelta.y;

			double rotY = 0.0;
			infoElem->Attribute("RotY", &rotY);
			v.rotY = static_cast<REAL32>(rotY);

			double prerotY = 0.0;
			infoElem->Attribute("PreRotY", &prerotY);
			v.prerotY = static_cast<REAL32>(prerotY);

			// storage에 저장
			TheStorage().Add(k, v);
		}
	}
}

void XmlFile::Save()
{
	Storage& s = TheStorage();
	if (s.empty()) return;

	// 모두 삭제하고 Storage 내용을 전부 기록한다.
	m_doc.Clear();
	m_doc.SaveFile();

	// 해쉬테이블의 내용을 벡터에 담아서 정렬시킴. 스테이지 및 명령키 순서에 따름.
	i3::vector<Key> keyList;

	Storage::iterator it;
	for (it = s.begin(); it != s.end(); it++)
	{
		keyList.push_back(it->first);
	}

	struct fn
	{
		bool operator()(const Key& lhs, const Key& rhs) const
		{
			if (lhs.stage < rhs.stage) return true;
			else if (lhs.stage == rhs.stage)
			{
				if (lhs.inputKey < rhs.inputKey) return true;
			}

			return false;
		}
	};
	i3::sort(keyList.begin(), keyList.end(), fn());

	// 저장.

	// declaration
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "euc-kr", "");
	m_doc.LinkEndChild(decl);

	// 실제 정보들.
	TiXmlElement* stages = new TiXmlElement("Stages");
	m_doc.LinkEndChild(stages);

	struct CreateStage
	{
		void operator()(TiXmlElement*& out_stage, INT32& out_curStageIdx, TiXmlElement* stageList, const Key& k) const
		{
			out_stage = new TiXmlElement("Stage");
			out_stage->SetAttribute("Index", static_cast<INT32>(k.stage));
			stageList->LinkEndChild(out_stage);
			out_curStageIdx = k.stage;
		}
	};
	
	struct FillStageInfoOfStage
	{
		void operator()(TiXmlElement* stage, const Key& k, const Val& v) const
		{
			TiXmlElement* info = new TiXmlElement("Info");
			stage->LinkEndChild(info);

			info->SetAttribute("InputKey", k.inputKey);

			std::ostringstream ossMatrix;
			ossMatrix << v.mat.m[0][0] << " " << v.mat.m[0][1] << " " << v.mat.m[0][2] << " " << v.mat.m[0][3] << " "
				<< v.mat.m[1][0] << " " << v.mat.m[1][1] << " " << v.mat.m[1][2] << " " << v.mat.m[1][3] << " "
				<< v.mat.m[2][0] << " " << v.mat.m[2][1] << " " << v.mat.m[2][2] << " " << v.mat.m[2][3] << " "
				<< v.mat.m[3][0] << " " << v.mat.m[3][1] << " " << v.mat.m[3][2] << " " << v.mat.m[3][3] << " ";
			std::string matrix = ossMatrix.str();
			info->SetAttribute("Matrix", matrix.c_str());

			std::ostringstream ossMouseDelta;
			ossMouseDelta << v.mouseDelta.x << " " << v.mouseDelta.y << " " << v.mouseDelta.z;
			std::string mouseDelta = ossMouseDelta.str();
			info->SetAttribute("MouseDelta", mouseDelta.c_str());

			info->SetAttribute("RotY", v.rotY);
			info->SetAttribute("PreRotY", v.prerotY);
		}
	};
		
	if (!keyList.empty())
	{
		CreateStage			 fnCreateStage;
		FillStageInfoOfStage fnFillStageInfo;

		INT32 curStage = -1;
		TiXmlElement* stage = 0;

		{
			const Key& k = keyList[0];
			const Val& v = s.Find(k)->GetValue();

			fnCreateStage(stage, curStage, stages, k);
			fnFillStageInfo(stage, k, v);
		}

		for (size_t i = 1; i < keyList.size(); i++)
		{
			const Key& k = keyList[i];
			const Val& v = s.Find(k)->GetValue();

			if (static_cast<UINT32>(curStage) != k.stage) // 새로 Stage 노드를 만들어야 하는 경우
			{
				fnCreateStage(stage, curStage, stages, k);
			}

			fnFillStageInfo(stage, k, v);
		}
	}

	m_doc.SaveFile();
	keyList.clear();
}