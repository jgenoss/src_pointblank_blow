#include "i3CommonType.h"
#include "i3PackFileFindMgr.h"

#include "i3PackVolume.h"

#include "i3Base/string/system/is_filedate_less.h"
#include "i3Base/string/ext/make_unix_path.h"

i3PackFileFindMgr::~i3PackFileFindMgr()
{
	i3PackFileFindMgr::RemoveAll();
}

const i3PackFileFindEntity*	i3PackFileFindMgr::FindEntity(const i3::literal_range& strResourceFilePath) const    // strResourceFilePath는 일단 상대경로로 간주합니다.. i3PackFile 찾는건 상대경로가 원래 조건이었습니다.
{
	i3::stack_string strRscFile = strResourceFilePath;
	i3::make_unix_path(strRscFile);
	i3::rc_string strKey;
	i3::to_lower_copy(strKey, strRscFile);

	typedef i3::unordered_map<i3::rc_string, i3PackFileFindEntity*> map_type;
	map_type::const_iterator it =	m_FindMap.find(strKey);

	if (it == m_FindMap.end())
		return NULL;
	return it->second;
}

namespace
{
	void MakeDirectoryPath(i3PackNode* pNode, i3::stack_string& outPath)
	{
		i3PackNode* pParent = static_cast<i3PackNode*>(pNode->getParent());
		if (pParent) // 부모가 없는 루트노드의 경우는 아무것도 넣지 않도록 한다..
		{
			MakeDirectoryPath(pParent, outPath);

			if (pNode->hasName())
			{
				outPath += pNode->GetNameString();
				outPath += '/';			// unixPath만을 인정..
			}
		}
	}

	void PrintErrorMessage_RedundantResourceFile(const i3::stack_string& strFullPath, const i3PackFileFindEntity* existed_entity, const i3PackFileFindEntity* input_entity, bool isInputNewer)
	{
		const i3::rc_string& existed_volName = existed_entity->pPackVolume->getVolumeFilename();
		const i3::rc_string& input_volName = input_entity->pPackVolume->getVolumeFilename();
		const i3::rc_string& newer_volName = (isInputNewer) ? input_volName : existed_volName;
		
		I3LOG_TYPE logType = i3PackFileFindMgr::i()->GetErrorMessageType();

		I3PRINTLOG(logType,   "Redundant ResourceFile Exists in i3Pack : rscFilePath = \"%s\", existed_volPath = \"%s\", reading_volPath = \"%s\" \r\n"
							  "%s is newer and shall be selected for finding file.", strFullPath, existed_volName, input_volName, newer_volName);
	}
	
	void MakeEntity_ForResource(i3PackVolume* pPackVolume, i3PackNode* pNode, I3_PACK_FILE_REF* pPackFileRef, const i3::stack_string& strDirPathSlash, i3::unordered_map<i3::rc_string, i3PackFileFindEntity*>& outMap)
	{
		i3::stack_string fullPath = strDirPathSlash;
		fullPath += pPackFileRef->m_szName;						// 디렉토리에 파일명 결합..
																// 소문자화 처리..
		i3::rc_string strKey;

		i3::to_lower_copy(strKey, fullPath);

		typedef i3::unordered_map<i3::rc_string, i3PackFileFindEntity*> map_type;
		map_type::insert_commit_data  commit_data;
		typedef i3::pair<map_type::iterator, bool> pair_type;

		pair_type pair_res = outMap.insert_check(strKey, commit_data);

/*		if (pair_res.second == false)  // 기존에 있는 데이터로 에러메세지 처리를 해주어야 합니다.... 그러나, 두 데이터 중 최신버전을 유지하는 기존 구현도 맞추어 주도록 한다...
		{
			map_type::iterator it = pair_res.first;
			i3PackFileFindEntity entity;
			entity.pPackVolume = pPackVolume;
			entity.pPackNode = pNode;
			entity.pPackFileRef = pPackFileRef;

			i3PackFileFindEntity* pExistedEntity = it->second;
			const i3::rc_string& lhsPackFileName = pExistedEntity->pPackVolume->getVolumeFilename();
			const i3::rc_string& rhsPackFileName = pPackVolume->getVolumeFilename();
			bool isNewer = i3::is_modified_filedate_less(lhsPackFileName, rhsPackFileName);

			PrintErrorMessage_RedundantResourceFile(fullPath, pExistedEntity, &entity, isNewer);

			if (isNewer)		// 
			{
				pExistedEntity->pPackVolume = pPackVolume;
				pExistedEntity->pPackNode = pNode;
				pExistedEntity->pPackFileRef = pPackFileRef;				// 삽입은 필요없고 값 변경만 해주면 될것이다..
			}
		}
		else   // 새 값 넣어주어야 합니당.
		{*/
			i3PackFileFindEntity* pNewEntity = new i3PackFileFindEntity;
			pNewEntity->pPackVolume = pPackVolume;
			pNewEntity->pPackNode = pNode;
			pNewEntity->pPackFileRef = pPackFileRef;
			outMap.insert_commit( i3::make_pair(strKey, pNewEntity), commit_data);
//		}

	}


	void BuildMapRec(i3PackVolume* pPackVolume, i3PackNode* pNode, i3::unordered_map<i3::rc_string, i3PackFileFindEntity*>& outMap)
	{
	

		const INT32 numFile = pNode->getFileCount();

		if (numFile > 0)		// 오옷..맵 등록 조건...읍으면 말고...
		{
			i3::stack_string strDirPathSlash;
			MakeDirectoryPath(pNode, strDirPathSlash);					// 이 함수의 말미는 /로 끝난다..

			for (INT32 i = 0; i < numFile; ++i)
			{
				I3_PACK_FILE_REF* pPackFileRef = pNode->getFile(i);
				MakeEntity_ForResource(pPackVolume, pNode, pPackFileRef, strDirPathSlash, outMap);
			}
			
		}

		const INT32 numChild = pNode->getChildCount();
		for (INT32 i = 0; i < numChild; ++i)
		{
			i3PackNode* pPackNode = static_cast<i3PackNode*>(pNode->getChild(i));
			BuildMapRec(pPackVolume, pPackNode, outMap);
		}
	}
}

void	i3PackFileFindMgr::AddPackVolume(i3PackVolume* pPackVolume)
{
	i3PackNode* pRoot =	pPackVolume->getRoot();

	BuildMapRec(pPackVolume, pRoot, m_FindMap);				 // 1줄로 간단해보이지만, 내부함수구현들이 나름 길다..
}

void	i3PackFileFindMgr::RemoveAll()
{
	i3::cu::for_each_delete_clear(m_FindMap);				// 알아서 세컨드를 잘 지워주는 유틸이다..
}



