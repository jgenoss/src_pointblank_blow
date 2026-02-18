#if !defined( __TIMESTAMP_H)
#define __TIMESTAMP_H

#include "i3Transform.h"
#include "i3SpriteAttr.h"

class I3_EXPORT_SCENE i3TimeStamp : public i3Transform
{
	I3_EXPORT_CLASS_DEFINE( i3TimeStamp, i3Transform);
protected:
	REAL32			m_TimeStamp = 0.0f;

	i3Node *		m_pPrev = nullptr;
	i3Node *		m_pNext = nullptr;

	i3ElementBase *	m_pRefObject = nullptr;

public:
	virtual ~i3TimeStamp(void);

	REAL32			getTimeStamp(void)			{ return m_TimeStamp; }
	void			setTimeStamp( REAL32 tm)	{ m_TimeStamp = tm; }

	i3Node *		getPrev(void)				{ return m_pPrev; }
	void			setPrev( i3Node * pNext)	{ m_pPrev = pNext; }

	i3Node *		getNext(void)				{ return m_pNext; }
	void			setNext( i3Node * pNext)	{ m_pNext = pNext; }

	i3ElementBase *	getRefObject(void)			{ return m_pRefObject; }
	void			setRefObject( i3ElementBase * pObj)
	{
		I3_REF_CHANGE( m_pRefObject, pObj);
	}
};

#endif
