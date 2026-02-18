#ifndef __VOTERECORDER__
#define	__VOTERECORDER__

struct TYPE_VOTE_RECORD
{
	TUID	m_i64CallUID;			// 신고자 UID
	TUID	m_i64TargetUID;			// 피신고자 UID
	UINT32	m_ui32RTComplete;		// 투표 종료 시간
};

// 투표기록 관련 클래스를 분리하였습니다.	2011.06.01 안국정
// TYPE_VOTE_RECORD 배열은 링버퍼로 관리됩니다. 만약, 링버퍼가 가득차면 제일 오래된 데이터를 강제로 덮어씁니다.
class CVoteRecorder
{
public:
	CVoteRecorder();
	BOOL	Check( TUID i64CallUID, TUID i64TargetUID );
	void	Record( TUID i64CallUID, TUID i64TargetUID );	// 신고자, 피신고자, 신고시간 기록
	void	Clean();										// 시간이 오래지나 더 이상 참고할 필요없는 기록을 지웁니다.

private:
	TYPE_VOTE_RECORD	m_aRecord[MAX_SIZE_VOTE_RECORD];		// 투표 기록 데이터
	INT32	m_i32BeginPtr;				// 가장 오래된 레코드 데이터를 가리키는 포인터
	INT32	m_i32EndPtr;				// 가장 최신의 레코드 데이터를 가리키는 포인터 겸 다음 자료가 들어갈곳의 위치(BeginPtr과 EndPtr이 같다면 자료가 없거나 가득찬 상태)
	INT32	m_i32Count;					// 들어있는 자료수
};

#endif