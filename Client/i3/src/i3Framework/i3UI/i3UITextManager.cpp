#include "i3FrameworkPCH.h"
#include "i3UITextManager.h"
#include "i3Base/itl/vector_util.h"

//I3_CLASS_INSTANCE( i3UITextManager, i3GfxResource);
I3_CLASS_INSTANCE( i3UITextManager);

i3UITextManager *	g_pTextManager = nullptr;

#define		PAGE_UNIT		512

i3UITextManager::i3UITextManager(void)
{
	g_pTextManager = this;
}

i3UITextManager::~i3UITextManager(void)
{
	I3_SAFE_RELEASE( m_pUniscribe);
	I3_SAFE_RELEASE( m_pFontManager);

	RemoveAllText();
	FreeAllLetterPage();
	g_pTextManager = nullptr;
}

bool i3UITextManager::Create( bool bLegacy)
{
	m_pFontManager = i3UIFontManager::new_object();
	m_pFontManager->Create( bLegacy);

	m_pUniscribe = i3UIUniscribe::new_object();
	m_pUniscribe->Init();
	
	return true;
}

void i3UITextManager::_AddLetterPage(void)
{
	i3UILetter * pPage;

	pPage = (i3UILetter *) i3MemAlloc( sizeof(i3UILetter) * PAGE_UNIT);

	m_LetterPageList.push_back( pPage);

	// 추가로 할당된 영역을 Free-List에 추가해준다.
	INT32 i;

	for( i = 0; i < PAGE_UNIT; i++)
	{
		pPage[i].setPrev( nullptr);
		pPage[i].setNext( m_pFreeHead);
		
		pPage[i].reset();

		if( m_pFreeHead != nullptr)
			m_pFreeHead->setPrev( & pPage[i]);

		m_pFreeHead = & pPage[i];
	}
}

void i3UITextManager::FreeAllLetterPage(void)
{
	i3UILetter * pPage;

	for(size_t i = 0; i < m_LetterPageList.size(); i++)
	{
		pPage = m_LetterPageList[i];

		i3MemFree( pPage);
	}

	m_LetterPageList.clear();
	m_pFreeHead = nullptr;
}

i3UILetter *	i3UITextManager::AllocLetter(void)
{
	if( m_pFreeHead == nullptr)
	{
		_AddLetterPage();
	}

	// Double-Linked-List인 Free-List의 첫 Node를 떼어내어 사용한다.
	I3ASSERT( m_pFreeHead != nullptr);

	i3UILetter * pLetter = m_pFreeHead;

	m_pFreeHead = m_pFreeHead->getNext();

	pLetter->setNext( nullptr);

	if( m_pFreeHead != nullptr)
		m_pFreeHead->setPrev( nullptr);

	return pLetter;
}

void i3UITextManager::FreeLetter( i3UILetter * pLetter)
{
	pLetter->setNext( m_pFreeHead);

	if( m_pFreeHead != nullptr)
		m_pFreeHead->setPrev( pLetter);

	m_pFreeHead = pLetter;
}

void i3UITextManager::FreeLetterList( i3UILetter * pLetter)
{
	i3UILetter * pTail = pLetter;

	// pLetter로 시작하는 Sub List의 끝을 찾아, Free Head에 연결
	while( pTail->getNext() != nullptr)
		pTail = pTail->getNext();

	pTail->setNext( m_pFreeHead);

	if( m_pFreeHead != nullptr)
		m_pFreeHead->setPrev( pTail);

	m_pFreeHead = pLetter;
}

void i3UITextManager::AddText( i3UIText * pText)
{
	I3_MUST_ADDREF( pText);
	m_TextList.push_back( pText);
}

void i3UITextManager::RemoveAllText(void)
{
	INT32 i;
	i3UIText * pText;

	for( i = 0; i < getTextCount(); i++)
	{
		pText = getText( i);

		I3_MUST_RELEASE( pText);
	}

	m_TextList.clear();
}

i3UIText *	i3UITextManager::CreateText( INT32 maxLength)
{
	i3UIText * pText;

	pText = i3UIText::new_object_ref();

	AddText( pText);

	return pText;
}

void i3UITextManager::FreeText( i3UIText * pText)
{
	I3ASSERT( pText != nullptr);

	I3_MUST_RELEASE( pText);

	i3::vu::remove(m_TextList, pText);
}

void i3UITextManager::ReparseAllText(void)
{
	
	INT32 i;

	for( i = 0; i < getTextCount(); i++)
	{
		i3UIText * pText = getText( i);
		pText->RedrawGlyphs();
	}
	
}

bool i3UITextManager::Destroy( bool bLostDevice)
{
	if( m_pFontManager != nullptr)
		m_pFontManager->OnDestroy( bLostDevice);

	return true;
}

bool i3UITextManager::Revive( i3RenderContext * pCtx)
{
	if( m_pFontManager != nullptr)
		m_pFontManager->OnRevive( pCtx);

	ReparseAllText();

	return true;
}
