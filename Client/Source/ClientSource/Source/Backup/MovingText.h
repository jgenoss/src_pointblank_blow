#ifndef _MovingText_h
#define _MovingText_h

#if 지워질_것들

#define SET_POINT2D(point2d,xPoint,yPoint)			do {(point2d).x = (xPoint); (point2d).y = (yPoint);} while(0)
#define EQUAL_POINT2D(point1,point2)				((point1).x == (point2).x && (point1).y == (point2).y)
#define COPY_POINT2D(pointDest,pointSource)			do {(pointDest).x = (pointSource).x; (pointDest).y = (pointSource).y;} while(0)

struct MovingTextNode
{
	i3TextNodeDX2*	_textNode;
	POINT2D			_startPoint;
	POINT2D			_endPoint;
	char			_text[MAX_STRING_COUNT];
};

class CMovingText: public i3GameObjBase
{
	I3_CLASS_DEFINE(CMovingText, i3GameObjBase);
public:
	CMovingText();
	virtual ~CMovingText();

	virtual BOOL Create(i3Node *pParentNode = NULL,BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void SetMovingPosition(POINT2D startPoint,POINT2D endPoint);	
	void SetMovingText(const char* movingText);

private:
	i3TextNodeDX2*	m_pTextNode[8];
	MovingTextNode	m_movingText;
	POINT2D			m_startPoint;
	POINT2D			m_endPoint;
};

#endif

#endif
