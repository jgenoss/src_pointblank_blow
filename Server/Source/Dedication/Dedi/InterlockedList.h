#ifndef _INTERLOCKEDLIST_H
#define _INTERLOCKEDLIST_H

#include "../../CommonServerSource/Physics_Protocol.h"

typedef struct _PHYSICS_ITEM {
    SLIST_ENTRY ItemEntry;
	PHYSICS_PACKET_HEADER	header;
	N_PCINFO_HIT_DEDICATED2	hit;
} PHYSICS_ITEM;

class CInterlockedList
{
public:
	UINT16			GetCount() const;
	BOOL			AddTail(PHYSICS_PACKET_HEADER* pHeader, N_PCINFO_HIT_DEDICATED2* pHit);
	PHYSICS_ITEM*	RemoveHead();	// not delete entry
	void			DeleteEntry(SLIST_ENTRY* pListEntry);
	void			RemoveAll();

	CInterlockedList();
	~CInterlockedList();

protected:
    PSLIST_HEADER	m_pListHead;
};

extern CInterlockedList* g_pPhysicsResult; // for hack checker thread

#endif