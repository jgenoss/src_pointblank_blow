#ifndef _NoteBox_h
#define _NoteBox_h

class CNoteBox: public i3ElementBase
{
	I3_CLASS_DEFINE(CNoteBox);
public:
	CNoteBox();
	virtual ~CNoteBox();

	void Init(void);
	BOOL Add(INT32 id);
	BOOL CheckReaded(INT32 id);
	BOOL Delete(INT32 id);

	INT32 GetCount(void) const;	

private:	
	INT32 m_noteId[MAX_NORMAL_NOTE_COUNT];
	INT32 m_count;
};

#endif
