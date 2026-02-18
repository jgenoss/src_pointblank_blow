#include "i3FrameworkPCH.h"
#include "i3ResourceFile_i3Chara.h"

#include "i3CharaSGChangeManager.h"
#include "i3Framework.h"
#include "i3GameResChara.h"

I3_CLASS_INSTANCE( i3ResourceFile_i3Chara);

//
// 여기로 들어오는 것은 full_path 혹은 fake I3CHR파일이름이다..
//

UINT32	i3ResourceFile_i3Chara::LoadOrClone( const char * pszPath, bool bCommonRes, bool bInThread)
{
	// 0. 우선 fake I3CHR파일 이름인지 체크...
	m_IsClonedResult = false;

	i3Framework* pFramework = i3Framework::getCurrentFramework();
	i3ResourceManager* resMgr = pFramework->GetResourceManager();

	const i3CharaSGChangeInfo* SGChangeInfo = i3CharaSGChangeManager::i()->FindSGChangeInfo(pszPath);
	
	if (SGChangeInfo == nullptr)		// 없으면 대부분의 경우 기본클래스의 것으로 모든걸 떼운다..
	{
		i3GameRes* ref_res = resMgr->FindResourceByPath(pszPath);	
		// 로딩 중임에도 등록이 그동안 처리되버리는 다소 이상한 케이스를 감안해서..안전하게 검사해본다.
		
		if (ref_res)
		{
			i3GameResChara* resChara = static_cast<i3GameResChara*>(ref_res);
			i3Chara* pChara = resChara->getChara();
			I3ASSERT(pChara != nullptr);
			this->setKeyObject(pChara);
			m_IsClonedResult = true;
			return 0;			// 최소한 STREAM_ERR만 아니면 된다...
		}
		
		return i3ResourceFile::Load(pszPath, bCommonRes);
	}
	
	// 있으면 재활용 처리 ... 재활용에 쓰이는 ref i3CHR파일은 파일타이틀+확장자만으로 구성되어있다..

	i3::rc_string tgt_file_name_only = i3CharaSGChangeManager::GetTgti3CharaFileName(SGChangeInfo);
	i3::rc_string ref_file_name_only = i3CharaSGChangeManager::GetRefi3CharaFileName(SGChangeInfo);

	// 레퍼런스와 타겟이 동일한 이름을 가진 경우 방치하면 그냥 무한재귀가 됨...
	if ( i3::generic_is_iequal(tgt_file_name_only, ref_file_name_only) )
	{
		// 이건 그냥 일반 로딩 처리해버린다.
		i3GameRes* ref_res = resMgr->FindResourceByPath(pszPath);	
		// 로딩 중임에도 등록이 그동안 처리되버리는 다소 이상한 케이스를 감안해서..안전하게 검사해본다.

		if (ref_res)
		{
			i3GameResChara* resChara = static_cast<i3GameResChara*>(ref_res);
			i3Chara* pChara = resChara->getChara();
			I3ASSERT(pChara != nullptr);
			this->setKeyObject(pChara);
			m_IsClonedResult = true;
			return 0;			// 최소한 STREAM_ERR만 아니면 된다...
		}

		return i3ResourceFile::Load(pszPath, bCommonRes);
	}


	const char* full_path_ref_file = resMgr->FindFullPathByOnlyName(ref_file_name_only.c_str());

	i3GameRes* ref_res = nullptr;

	if (full_path_ref_file == nullptr)		// RSC에 전혀 등록 안된 리소스 --> 실패 가능성이 높은 놈..
	{									// pszPath인수로 직접 로딩 시도를 함 해보기는 하자..(sharing이 안된다.)
		I3PRINTLOG(I3LOG_WARN,  "RSC에 등록되지 않은 Resource입니다. %s", ref_file_name_only);
		
		if ( i3FileStream::isFile(pszPath) == false )	// ref파일은 실제 존재안하면 실패한 것임
			return STREAM_ERR;		

		ref_res = resMgr->LoadResource(pszPath, 0, bCommonRes, bInThread, false);

		if (ref_res == nullptr) 
			return STREAM_ERR;				// 직접 로딩도 안될것 같은 진행불가 상황임...)
	}
	else
	{
		ref_res = resMgr->FindResourceByPath(full_path_ref_file);

		if (ref_res == nullptr)		// 아직 로딩완료 안된 상태의 레퍼런스 리소스임..(로딩필요, 혹은 로딩과정중 ?)
		{
			if ( i3FileStream::isFile(full_path_ref_file) == false )	// ref파일은 실제 존재안하면 실패한 것임
				return STREAM_ERR;		

			// 로딩과정중인 놈은 중복로딩 상황도 발생할수 있을 것으로 보이지만...
			// ..이정도 함수로 마무리...
			ref_res = resMgr->LoadBackgroundRes(ref_file_name_only.c_str(), bCommonRes, bInThread);

			if (ref_res == nullptr)
				return STREAM_ERR;
		}
	}


	// 새 캐라에 i3s SceneGraph를 삽입해서 넣어줘야한다..

	i3::rc_string i3s_file_path = i3CharaSGChangeManager::GetTgtI3SFileName(SGChangeInfo);
	
	i3NamedResourceManager* i3sMgr = i3ResourceFile::GetResourceManager( i3SceneGraphInfo::static_meta() );
	I3ASSERT(i3sMgr != nullptr);

	i3SceneGraphInfo* pi3sObj =	static_cast<i3SceneGraphInfo*>(i3sMgr->GetResource( 
		i3s_file_path.c_str(), i3ResourceFile::GetWorkingFolder(), i3s_file_path.c_str()));

	// i3s파일 읽기가 실패했다면??? 그냥 실패로 간주하자..
	
	if (pi3sObj == nullptr)
		return STREAM_ERR;
		
	i3GameResChara* resRefChara = static_cast<i3GameResChara*>(ref_res);
	i3Chara* pRefChara = resRefChara->getChara();
	I3ASSERT(pRefChara != nullptr);

	i3Chara* pNewChara = static_cast<i3Chara*>(  i3GameNode::CreateClone(pRefChara, I3_COPY_INSTANCE) );
	I3ASSERT(pNewChara != nullptr);
	
	pNewChara->setSceneGraphInfo(pi3sObj);
	this->setKeyObject(pNewChara);
	
	// pNewChara는 LoadCharactor 함수 내부에서 I3_SAFE_ADDREF로 처리되므로 여기서는 AddRef()처리를 하지 않도록 한다..

	m_IsClonedResult = true;

	return 0;
}
