#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectJumpHold.h"

#include "GameCharaContexts.h"

I3_CLASS_INSTANCE(CGameObjectJumpHold);

CGameObjectJumpHold::CGameObjectJumpHold()
{
	m_rJumpPower		= 0.0f;
	m_rJumpDig			= 0.0f;
	m_rJumpHeight		= 0.0f;
	m_pAnim				= nullptr;
	m_rJumpAnimSpeed	= 1.0f;
	m_rManaulClampTime	= 1.0f;
}

CGameObjectJumpHold::~CGameObjectJumpHold()
{
	I3_SAFE_RELEASE( m_pAnim );
}

typedef struct __tagPersist_ObjJumpHold
{
	UINT32			nVer;
	REAL32			rJumpPower;
	REAL32			rJumpDig;
	REAL32			rJumpHeight;
	REAL32			rJumpAnimSpeed;
	UINT32			pad[4];
} PERSIST_OBJ_JUMP_HOLD;

UINT32 CGameObjectJumpHold::OnQueryGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{
	(*ppData) = i3MemAlloc(sizeof(PERSIST_OBJ_JUMP_HOLD));

	i3mem::FillZero(*ppData, sizeof(PERSIST_OBJ_JUMP_HOLD));

	PERSIST_OBJ_JUMP_HOLD * pData = (PERSIST_OBJ_JUMP_HOLD*)(*ppData);

	pData->nVer = 101;
	pData->rJumpPower		= m_rJumpPower;
	pData->rJumpDig			= m_rJumpDig;
	pData->rJumpHeight		= m_rJumpHeight;
	pData->rJumpAnimSpeed	= m_rJumpAnimSpeed;
	
	return sizeof(PERSIST_OBJ_JUMP_HOLD);
}

UINT32 CGameObjectJumpHold::OnLoadGameData(i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	UINT32 Rc;
	i3Stream * pStream = pInfo->getStream();

	PERSIST_OBJ_JUMP_HOLD info;

	Rc = pStream->Read(&info, sizeof(PERSIST_OBJ_JUMP_HOLD));
	I3_CHKIO(Rc);

	m_rJumpPower		= info.rJumpPower;
	m_rJumpDig			= info.rJumpDig;
	m_rJumpHeight		= info.rJumpHeight;
	m_rJumpAnimSpeed	= info.rJumpAnimSpeed;

	return Rc;
}

UINT32 CGameObjectJumpHold::OnSave(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	result = i3Object::OnSave(pResFile);
	I3_CHKIO(result);

	PERSIST_OBJ_JUMP_HOLD info;
	info.rJumpPower		= m_rJumpPower;
	info.rJumpDig		= m_rJumpDig;
	info.rJumpHeight	= m_rJumpHeight;
	info.rJumpAnimSpeed = m_rJumpAnimSpeed;

	rc = pResFile->Write(&info, sizeof(info));
	I3_CHKIO(rc);
	result += rc;

	return result;
}

UINT32 CGameObjectJumpHold::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	PERSIST_OBJ_JUMP_HOLD info;

	result = i3Object::OnLoad(pResFile);
	I3_CHKIO(result);

	rc = pResFile->Read(&info, sizeof(info));
	I3_CHKIO(rc);
	result += rc;

	m_rJumpPower		= info.rJumpPower;
	m_rJumpDig			= info.rJumpDig;
	m_rJumpHeight		= info.rJumpHeight;
	m_rJumpAnimSpeed	= info.rJumpAnimSpeed;

	return result;
}

#if defined(I3_DEBUG)//XML
bool	CGameObjectJumpHold::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = new i3XMLElement("JUMPHOLD");
	pXML->LinkEndChild(pChild);

	pChild->addAttr("JUMP_POWER"		, m_rJumpPower);
	pChild->addAttr("JUMP_DIG"			, m_rJumpDig);
	pChild->addAttr("JUMP_HEIGHT"		, m_rJumpHeight);
	pChild->addAttr("JUMP_ANIM_SPEED"	, m_rJumpAnimSpeed);
	
	return true;
}

bool CGameObjectJumpHold::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *)pXML->FirstChild("JUMPHOLD");
	if (pChild == nullptr)
	{
		I3ERRLOG(i3Error::_INVALIDFORMAT);
		return false;
	}

	pChild->getAttr("JUMP_POWER"		, &m_rJumpPower);
	pChild->getAttr("JUMP_DIG"			, &m_rJumpDig);
	pChild->getAttr("JUMP_HEIGHT"		, &m_rJumpHeight);
	pChild->getAttr("JUMP_ANIM_SPEED"	, &m_rJumpAnimSpeed);

	return true;
}
#endif

void CGameObjectJumpHold::Create(i3Framework * pFramework, i3GameResObject * pRes)
{
	CGameControledObject::Create( pFramework, pRes );

	i3::string animpath = m_pObjRes->GetName();
	i3::string key		= ".i3Obj";

	size_t found = animpath.find(key);
	animpath.replace(found,key.length(), ".i3a");

	if (animpath.size() != 0)
	{
		i3Animation * pAnimation = i3Animation::LoadFromFile(animpath.c_str());
		if (pAnimation == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", animpath);
		}

		I3_REF_CHANGE(m_pAnim, pAnimation);
		I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
	}
}

void CGameObjectJumpHold::InitObject(void)
{
	REAL32 key_frame = (REAL32)(m_pAnim->GetTrack(0)->GetKeyframeCount() - 1);
	m_rManaulClampTime = 0.0f;

	if (key_frame > 0.0f) m_rManaulClampTime = m_rJumpDig / key_frame;
	else m_rManaulClampTime = 0.0f;

	if (m_rManaulClampTime >= 0.9f) m_rManaulClampTime = 0.9f;

	getSceneObject()->PlayAnim(m_pAnim, I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, m_rManaulClampTime, I3_ANIM_PLAY_MODE_MANUAL_CLAMP, I3_ANIM_PLAY_MODE_STOP);
}

void CGameObjectJumpHold::ResetObject()
{
}

void CGameObjectJumpHold::EnableObject()
{
}

void CGameObjectJumpHold::ProcessObject(REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
}

BOOL CGameObjectJumpHold::JumpToChara(const NxShape* pShape, CGameCharaBase * pChara)
{
	I3ASSERT(pShape);
	I3ASSERT(pChara != nullptr);

	const char* pChar	= nullptr;
	NxActor* pActor		= nullptr;
	pActor = &(pShape->getActor());

	I3ASSERT(pActor);
	pChar = pActor->getName();

	if (pChar == nullptr)	return false;

	VEC3D vDir;
	MATRIX* mat = GetMatrix();

	vDir = *i3Matrix::GetAt(mat);
	i3Vector::Normalize(&vDir, &vDir);
	vDir *= -1.0f;

	pChara->getActionContext()->_Play_ForceJump(&vDir, m_rJumpPower, m_rJumpHeight);
	
	return true;
}

void CGameObjectJumpHold::PlayJumpAnim()
{
	getSceneObject()->PlayAnim(m_pAnim, I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, m_rJumpAnimSpeed, m_rManaulClampTime, I3_ANIM_PLAY_MODE_MANUAL_CLAMP, I3_ANIM_PLAY_MODE_STOP);
}
