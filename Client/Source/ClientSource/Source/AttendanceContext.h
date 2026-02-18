#if !defined (_ATTENDANCE_CONTEXT_H_)
#define _ATTENDANCE_CONTEXT_H_

// Key : 출석 체크 추가
//----------------------------------------------------------------------//
//								출석체크 관련							//
//----------------------------------------------------------------------//

struct AttendanceData
{
	ATTENDANCE_USER		m_AttendanceUser;
	ATTENDANCE_INFO		m_AttendanceInfo;

	bool IsAttendanceEvent() const
	{
		if( m_AttendanceInfo.m_ui32Version	== 0 )							return false;

		return true;
	}

	bool IsEventTarget() const
	{
		if (m_AttendanceUser.m_ui8EventState == ATTENDANCE_STATE_NONE)		return false;
		
		return true; 
	}

};

// __alignof(AttendanceData)  //

class AttendanceContext : public i3::shared_ginst<AttendanceContext>
{
public:
	AttendanceContext();
	~AttendanceContext();

	// 16.03.14 Attendance type 의 증가로 인해 각각의 Get/Set 함수에 어떤 타입의 이벤트인지 구분하도록 인자를 추가함
public:
	AttendanceData*			SetData(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL); 
	const AttendanceData*	GetData(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;

	const ATTENDANCE_USER *	GetAttendanceUser(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;
	void SetAttendanceState(UINT8 eState, ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL);
	void SetAttendanceSelectItem(UINT8 uiItem, ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL);
	const ATTENDANCE_INFO * GetAttendanceInfo(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_NORMAL) const;

	// 현재 출력중인 출석 이벤트 타입을 반환
	ATTENDANCE_TYPE			GetCurrrentType();
	// 현재 출력중인 출석 이벤트의 모든 프로세스가 완료되어 다음 순서의 출석 이벤트를 출력
	void					SetNextType();

	// 인자값이 없다면 존재하는 모든 출석 이벤트를 검사하여 하나라도 있다면 true 를 반환
	// ATTENDANCE_TYPE_NON 같은게 없기 때문에 부득이하게 MAX 로 default 값을 둡니다
	bool	IsAttendanceEvent(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_MAX) const;
	bool	IsEventTarget(ATTENDANCE_TYPE type = ATTENDANCE_TYPE_MAX) const;

private:
	AttendanceData		m_AttendanceData[ATTENDANCE_TYPE_MAX];

	ATTENDANCE_TYPE		m_ActivateEventType;

};

#endif