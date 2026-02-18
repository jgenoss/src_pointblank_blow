#include "pch.h"
#include "Capture.h"

#if defined( TEST_AUTOMATION)

#include "KeyString.h"

using namespace CaseRun;

I3_CLASS_INSTANCE( CaseRunCapture);

CaseRunCapture::CaseRunCapture(void)
{
	m_pFramework = nullptr;
	m_pOldManager = nullptr;
	m_bCapturing = false;
	m_pEditBox = nullptr;
	m_pMousePickedCtrl = nullptr;
}

CaseRunCapture::~CaseRunCapture(void)
{
	I3_SAFE_RELEASE(m_pMousePickedCtrl);
	I3_SAFE_RELEASE(m_pOldManager);
	I3_SAFE_RELEASE(m_pEditBox);
}

bool CaseRunCapture::Create(CGameFramework * pFramework, Interface * pInterface)
{
	m_pFramework = pFramework;
	m_pInterface = pInterface;

	// i3Framework가 입력을 전달하는데 사용하는 Input Dispatch Manager를 자신으로 대체한다.
	I3_REF_CHANGE(m_pOldManager, m_pFramework->getInputDispatchManager());

	m_pFramework->setInputDispatchManager(this);

	return true;
}

bool CaseRunCapture::Start(void)
{
	CaseRun::StartCapture();

	m_bCapturing = true;

	m_tmStart = i3Timer::GetProfileTime();

	return true;
}

bool CaseRunCapture::End(void)
{
	RefineCapture();

	m_bCapturing = false;

	CaseRun::EndCapture( m_CaptureList);

	RemoveAllNodes();

	return true;
}

REAL32 CaseRunCapture::GetElapsed()
{
	INT64 tm = i3Timer::GetProfileTime();

	double res = i3Timer::GetProfileResolution();

	return (REAL32)((tm - m_tmStart) * res) * 0.001f;
}

void CaseRunCapture::GetFullPath(i3UIControl * pCtrl, std::string & str)
{
	char conv[512];

	conv[0] = 0;			// GetCtrlPath 내부에서 strcat 형태로 작업하기 때문에 반드시 필요.

	pCtrl->TA_GetCtrlPath(conv, _countof(conv) - 1);

	str = conv;
}

void CaseRunCapture::AddNode(NodeBase * pNode)
{
	wchar_t conv[256];

	swprintf_s(conv, _countof(conv) - 1, L"Node_%d", m_CaptureList.size());

	pNode->setName(conv);

	m_CaptureList.push_back(pNode);

	std::wstring info;

	pNode->getInfoString(info);

	I3TRACE(L"# ADD NODE : %s", info.c_str());

	m_pInterface->UpdateText( nullptr, info.c_str());
}

void CaseRunCapture::RemoveNode(INT32 idx)
{
	NodeBase * pNode = m_CaptureList[idx];

	delete pNode;

	m_CaptureList.erase(m_CaptureList.begin() + idx);
}

void CaseRunCapture::RemoveAllNodes()
{
	UINT32 i;

	for (i = 0; i < m_CaptureList.size(); i++)
	{
		NodeBase * pNode = m_CaptureList[i];

		delete pNode;
	}

	m_CaptureList.clear();
	m_KeyDownList.clear();		// KeyDownList의 노드들은 이미 삭제되었기 때문에 별도로 삭제할 필요없다.
}

void CaseRunCapture::CaptureInputAndRelease()
{
	i3UIText * pText = m_pEditBox->getUIText();

	if (pText->getTextLength() > 0)
	{
		Node_Input * pNode = new Node_Input;

		pNode->m_Input = pText->getText();

		AddNode(pNode);
	}

	I3_SAFE_RELEASE(m_pEditBox);
}

Node_KeyDown * CaseRunCapture::IsInKeyDownList(I3I_KEY key)
{
	I3ASSERT( (key >= I3I_KEY_0) && (key < I3I_KEY_MAX));

	const wchar_t * pszKeyName = CaseRun::GetKeyName(key);

	if (pszKeyName == nullptr)
		return nullptr;

	for (UINT32 i = 0; i < m_KeyDownList.size(); i++)
	{
		Node_KeyDown * pNode = m_KeyDownList[i];

		if ( _wcsicmp( pszKeyName, pNode->getKeyName()) == 0)
			return pNode;
	}

	return nullptr;
}

void CaseRunCapture::KeyboardCapture(REAL32 tm)
{
	i3InputDeviceManager * pManager = g_pViewer->GetInputDeviceManager();
	i3InputKeyboard * pKey = pManager->GetKeyboard();
	i3UIManager				*	pUIManager = m_pFramework->getUIManager();
	bool bNormalKey = true;
	i3EventReceiver * pCtrl = pUIManager->GetFocus();

	// Focus가 EditBox Control이라면, 개별 키로 입력을 잡지 않고, 전체 입력 문자열을 Capture한다.
	if (m_pEditBox != nullptr)
	{
		// 이 전까지 EditBox 였다. Focus 이동 -> 입력 종료로 간주.
		if (pCtrl != m_pEditBox)
		{
			CaptureInputAndRelease();
		}
		else
		{
			if (pKey->GetStrokeState(I3I_KEY_ENTER))
			{
				CaptureInputAndRelease();
			}
		}
		bNormalKey = false;
	}

	if (m_pEditBox == nullptr)
	{
		if ((pCtrl != nullptr) && (pCtrl->kind_of(i3UIEditBox::static_meta())))
		{
			// Input Node Capture 시작.
			I3_REF_CHANGE(m_pEditBox, (i3UICaptionControl *)pCtrl);

			bNormalKey = false;
		}
	}

	if (bNormalKey)
	{
		// 일반 Keyboard 입력으로 처리해야 하는  모든 경우.
		for (UINT32 key = I3I_KEY_0; key < I3I_KEY_MAX; key++)
		{
			bool bPress = pKey->GetPressState((I3I_KEY)key) == true;
			Node_KeyDown * pNode = IsInKeyDownList((I3I_KEY)key);

			if (bPress)
			{
				if (pNode == nullptr)
				{
					// 새럽게 눌러졌다.
					pNode = new Node_KeyDown();

					pNode->setKeyName( CaseRun::GetKeyName((I3I_KEY)key));

					m_KeyDownList.push_back(pNode);
					AddNode(pNode);
				}

				pNode->setDuration( pNode->getDuration() + tm);
			}
		}

		for (UINT32 i = 0; i < m_KeyDownList.size(); i++)
		{
			Node_KeyDown * pNode = m_KeyDownList[i];

			I3I_KEY key = GetKey( pNode->getKeyName());

			if (pKey->GetPressState( key) == false)
			{
				// 키가 떨어졌다.

				m_KeyDownList.erase(m_KeyDownList.begin() + i);			// 임시 리스트에서 제거하고...
				i--;
			}
		}
	}
}

void CaseRunCapture::RefineCapture(void)
{
	// 시작과 끝의 Ctrl+Shift+/ 의 조합 제거하기.
	INT32 idx = FindEscapeSeq(0);

	while (idx != -1)
	{
		RemoveNode(idx);
		RemoveNode(idx);
		RemoveNode(idx);

		idx = FindEscapeSeq(idx);
	}
}

bool CaseRunCapture::IsEscSeq(NodeBase * pNode)
{
	if (wcsicmp(pNode->getVerb(), L"KeyDown") != 0)
		return false;

	Node_KeyDown * pKeyDown = (Node_KeyDown *)pNode;

	I3I_KEY key = GetKey( pKeyDown->getKeyName());

	switch ( key)
	{
	case I3I_KEY_LCTRL:
	case I3I_KEY_LSHIFT:
	case I3I_KEY_SLASH:
		return true;
	}

	return false;
}

INT32 CaseRunCapture::FindEscapeSeq(INT32 idxStart)
{
	INT32 idxFind = -1;
	INT32 cnt = 0;

	for (INT32 i = idxStart; i < (INT32)m_CaptureList.size(); i++)
	{
		NodeBase * pNode = m_CaptureList[i];

		bool isESC = IsEscSeq(pNode);

		if (isESC)
		{
			cnt++;

			if (idxFind == -1)
			{
				idxFind = i;
			}

			if (cnt >= 3)
				return idxFind;
		}
		else
		{
			idxFind = -1;
			cnt = 0;
		}
	}

	return -1;
}

void CaseRunCapture::OnInput( REAL32 tm)
{
	if (m_bCapturing)
	{
		KeyboardCapture( tm);
	}
}

bool CaseRunCapture::PreMouseCapture(POINT pt, BUTTON btn, bool bDblClick)
{
	if (m_bCapturing == false)
		return false;

	i3UIManager				*	pUIManager = m_pFramework->getUIManager();

	if (pUIManager != nullptr)
	{
		i3UIControl * pCtrl = pUIManager->getCurrMouseTarget();

		if (pCtrl == nullptr)
		{
			i3EventReceiver * pFocus = m_pFramework->getFocus();

			if ((pFocus != nullptr) && pFocus->kind_of(i3UIControl::static_meta()))
			{
				pCtrl = (i3UIControl *)pFocus;
			}
		}

		if (pCtrl != nullptr)
		{
			if (pCtrl->kind_of(i3UIListBox::static_meta()))
			{
				I3TRACE(L"ListBox\n");
			}
		}

		I3_REF_CHANGE(m_pMousePickedCtrl, pCtrl);
	}
	else
	{
		I3_SAFE_RELEASE(m_pMousePickedCtrl);
	}

	return true;
}

bool CaseRunCapture::PostMouseCapture( BUTTON btn, bool bDblClick)
{
	if (m_bCapturing == false)
		return false;

	if (m_pFramework->IsBattleStage() == false)
	{
		if (m_pMousePickedCtrl != nullptr)
		{
			std::string str;
			bool bCaptureClick = true;

			GetFullPath(m_pMousePickedCtrl, str);
			std::wstring temp(str.begin(), str.end());

			I3TRACE("[CASERUN] Capture : %s\n", str.c_str());

			if (m_pMousePickedCtrl->kind_of(i3UIListBox::static_meta()))
			{
				i3UIListBox * pListBox = (i3UIListBox *)m_pMousePickedCtrl;

				INT32 idx = pListBox->GetCurMouseOn();

				if (idx != -1)
				{
					wchar_t conv[512];

					pListBox->GetItem(idx, conv, _countof(conv) - 1);
					//I3TRACE(L"[CASERUN] Select : %s\n", conv);

					{
						Node_Select * pSel = new Node_Select;

						pSel->m_CtrlPath = temp;
						pSel->m_Item = conv;
						pSel->m_bMustBeSame = true;
						
						AddNode(pSel);
					}

					bCaptureClick = false;
				}
			}
			
			if (bCaptureClick)
			{
				Node_Click * pClick = new Node_Click;

				pClick->set( temp, L"", false, btn, bDblClick);
				
				AddNode(pClick);
				//I3TRACE(L"[CASERUN] Click : %s\n", temp.c_str());
			}

			I3_SAFE_RELEASE(m_pMousePickedCtrl);
		}
	}
	else
	{
	}

	return true;
}

void CaseRunCapture::OnChangeFocus( i3Framework * pFramework, i3EventReceiver * pFrom, i3EventReceiver * pTo)
{
	if (m_pOldManager != nullptr)
		m_pOldManager->OnChangeFocus( pFramework, pFrom, pTo);
}

bool CaseRunCapture::DispatchLButtonDown(UINT32 nFlag, POINT point)
{
	bool rv = true;

	PreMouseCapture( point, BTN_L, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchLButtonDown(nFlag, point);

	PostMouseCapture( BTN_L, false);

	return rv;
}

bool CaseRunCapture::DispatchLButtonUp(UINT32 nFlag, POINT point)
{
	bool rv = true;

	PreMouseCapture(point, BTN_L, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchLButtonUp(nFlag, point);

	PostMouseCapture( BTN_L, false);

	return rv;
}

bool CaseRunCapture::DispatchLButtonDblClk(UINT32 nFlag, POINT point)
{
	bool rv = true;

	PreMouseCapture(point, BTN_L, true);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchLButtonDblClk(nFlag, point);

	PostMouseCapture( BTN_L, true);

	return rv;
}

bool CaseRunCapture::DispatchMButtonDown(UINT32 nFlag, POINT point)
{
	bool rv = true;

	PreMouseCapture(point, BTN_M, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchMButtonDown(nFlag, point);

	PostMouseCapture( BTN_M, false);

	return rv;
}

bool CaseRunCapture::DispatchMButtonUp(UINT32 nFlag, POINT point)
{
	bool rv = true;

	//PreMouseCapture(point, BTN_M, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchMButtonUp(nFlag, point);

	//PostMouseCapture( BTN_M, false);

	return rv;
}

bool CaseRunCapture::DispatchRButtonDown(UINT32 nFlag, POINT point)
{
	bool rv = true;

	PreMouseCapture(point, BTN_R, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchRButtonDown(nFlag, point);

	PostMouseCapture( BTN_R, false);

	return rv;
}

bool CaseRunCapture::DispatchRButtonUp(UINT32 nFlag, POINT point)
{
	bool rv = true;

	//PreMouseCapture(point, BTN_R, false);

	if (m_pOldManager != nullptr)
		rv = m_pOldManager->DispatchRButtonDown(nFlag, point);

	//PostMouseCapture( BTN_R, false);

	return rv;
}

bool CaseRunCapture::DispatchMouseMove(UINT32 nFlag, POINT point)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchMouseMove(nFlag, point);

	return true;
}

bool CaseRunCapture::DispatchMouseWheel(UINT nFlag, short zDelta, POINT point)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchMouseWheel(nFlag, zDelta, point);

	return true;
}

bool CaseRunCapture::DispatchKeyDown(UINT32 nKey)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchKeyDown( nKey);

	return true;
}

bool CaseRunCapture::DispatchKeyUp(UINT32 nKey)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchKeyUp( nKey);

	return true;
}

bool CaseRunCapture::DispatchActivate( bool bActivate)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchActivate( bActivate);

	return true;
}

bool CaseRunCapture::DispatchSetFocus(void)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchSetFocus();

	return true;
}

bool CaseRunCapture::DispatchKillFocus(void)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchKillFocus();

	return true;
}

bool CaseRunCapture::DispatchInput(void)
{
	if (m_pOldManager != nullptr)
		return m_pOldManager->DispatchInput();

	return true;
}

#endif