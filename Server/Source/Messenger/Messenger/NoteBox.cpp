#include "pch.h"
#include "NoteBox.h"

I3_CLASS_INSTANCE(CNoteBox, i3ElementBase);

CNoteBox::CNoteBox()
{	
	m_count = 0;
}

CNoteBox::~CNoteBox()
{
	// Do nothing
}

void CNoteBox::Init(void)
{
	m_count = 0;
}

BOOL CNoteBox::Add(INT32 id)
{
	// 쪽지를 기록한다.
	if (m_count < MAX_NORMAL_NOTE_COUNT)
	{
		m_noteId[m_count] = id;
		m_count++;

		return TRUE;
	}
	
	return FALSE;
}

BOOL CNoteBox::CheckReaded(INT32 id)
{
	if (0 < m_count)
	{
		// 해당 쪽지를 찾아 읽은 상태로 바꾼다.
		for(INT32 i = 0; i < m_count; i++)
		{
			if (m_noteId[i] == id)
			{				
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CNoteBox::Delete(INT32 id)
{
	if (0 < m_count)
	{
		// 해당 쪽지를 찾아 삭제한다.
		for(INT32 i = 0; i < m_count; i++)
		{
			if (m_noteId[i] == id)
			{
				INT32 moveCount = m_count - (i + 1);

				// 삭제 - 배열의 뒷요들을 당겨 복사한다.
				memmove(&m_noteId[i], &m_noteId[i + 1], sizeof(INT32) * moveCount);
				m_count--;
				
				return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 CNoteBox::GetCount(void) const
{
	return m_count;
}
