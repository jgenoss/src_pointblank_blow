#include "pch.h"
#include "AnnounceConfig.h"

I3_CLASS_INSTANCE(CAnnounceConfig);//, i3ElementBase);

CAnnounceConfig::CAnnounceConfig()
{
	m_announceCount = 0;
	i3mem::FillZero(m_announceInfo, sizeof(m_announceInfo));
}

CAnnounceConfig::~CAnnounceConfig()
{
	// do nothing
}

bool CAnnounceConfig::Create(void)
{
	// 파일 열기
	TiXmlDocument doc("Gui/Image/announce.xml");

	if (!doc.LoadFile())
	{
		return false;
	}
	

	// 공지 요소 찾기
	TiXmlElement* pAnnounce = doc.FirstChildElement("ANNOUNCE");

	if (pAnnounce == nullptr)
	{
		return false;
	}


	// 이미지 요소 찾기
	TiXmlElement* pImage = pAnnounce->FirstChildElement("IMAGE");

	while(pImage != nullptr && m_announceCount < MAX_ANNOUNCE_COUNT)
	{
		// 모든 속성이 필요하다
		if (nullptr != pImage->Attribute("name") && nullptr != pImage->Attribute("path") && nullptr != pImage->Attribute("scroll"))
		{

			// 값 설정
			AnnounceInfo info;

			if (pImage->Attribute("URL"))
				i3::string_ncopy_nullpad(info._url, pImage->Attribute("URL"), MAX_STRING_COUNT - 1);
			else
				i3::string_ncopy_nullpad(info._url, "", MAX_STRING_COUNT - 1);

			i3::string_ncopy_nullpad(info._name, pImage->Attribute("name"), MAX_STRING_COUNT - 1);
			i3::string_ncopy_nullpad(info._path, pImage->Attribute("path"), MAX_PATH - 1);
			pImage->Attribute("scroll", &info._scroll);

			i3mem::Copy(&m_announceInfo[m_announceCount], &info, sizeof(m_announceInfo[m_announceCount]));
			m_announceCount++;
		}

		pImage = pImage->NextSiblingElement("IMAGE");
	}

	return true;
}

INT32 CAnnounceConfig::GetCount(void) const
{
	return m_announceCount;
}

const char* CAnnounceConfig::GetName(INT32 idx) const
{
	I3ASSERT(idx < m_announceCount);
	return m_announceInfo[idx]._name;
}

const char* CAnnounceConfig::GetURL(INT32 idx) const
{
	I3ASSERT(idx < m_announceCount);
	return m_announceInfo[idx]._url;
}

const char* CAnnounceConfig::GetPath(INT32 idx) const
{
	I3ASSERT(idx < m_announceCount);
	return m_announceInfo[idx]._path;
}

bool CAnnounceConfig::IsScroll(INT32 idx) const
{
	I3ASSERT(idx < m_announceCount);
	return m_announceInfo[idx]._scroll != 0;
}
