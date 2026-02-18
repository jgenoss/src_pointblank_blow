#pragma once

// 여기 함수는 i3RSCGen툴에서는 쓰지 않지만..
// 신규 RSCTool에서 쓸수 있도록 미리 작성해둔다..
// 코드 수정시... PBSVN / SourceNew / ClientSource/RscTool과 i3/Tools/i3RSCGen 
// 양쪽 모두 반영되도록 신경 쓸 것..
//

class RSCNode;
#include "RSCGenDefine.h"

struct RscNodeSet
{
	RSCNode*				m_root;
	RSCNode*				m_category_node[ I3RSC_TYPE_COUNT];	
	i3::unordered_map<i3::fixed_string, RSCNode*>	m_rel_path_map;

	RscNodeSet();
	~RscNodeSet();
};

void	ResetRscNodeSet(RscNodeSet& out);
bool	LoadRSG(const i3::string& full_path, RscNodeSet& out);
bool	SaveRSG(const i3::string& full_path, RSCNode* root);

RSCNode*	FindRscNodeByRelPath(const RscNodeSet& set, const i3::const_char_range& rngRelPath);

// 아래 함수는 old_node == NULL 값을 허용한다.그 경우엔 적당한 parent를 찾아 ADD처리된다.
// new_node == NULL값도 허용한다.. 이경우엔 삭제로 처리한다..

void	ReplaceRSCNode(RscNodeSet& set, RSCNode* old_node, RSCNode* new_node);

// 다음 함수는 확장자를 강제로 RSC로 맞춘다.
bool	ExportIntoRSC(const i3::string& full_path, RSCNode* root);



// RscNode의 이름은 파일의 이름(디렉토리이름제외)이다...

// RscTool에서의 기본 편집 전략...

// RSC 파일의 원소파일 양상 ..    .I3CHR , .I3I , .AnimPack, .WAV 정도로 생각된다.

// RSCTool에서는 편집하고자하는 pef검색창에서 패킹할때.......
// 결과로 나오는 아랫창이 아닌.....검색상단창의 목록에 존재하는 i3PackScript가 아닌..
// 원본/AnimPack 컬럼 정보를 조사해야한다... (이 부분은 각 아이템정보가 쥐고 있음)

// 칼럼정보의 파일포맷을 갖고 어떤 RSG파일과 연계되는지 조사해야한다.. 
// (이부분은 RSCTool에서 처리한다..대체로 하드코딩이 관여될것으로 보인다... 
//  예컨대, 1차적으로는 Weapon.pef/Character.pef 등등카테고리로 항목구분이 될것이다... )

// 0. RSC파일들을 사전에 다 읽도록 ,  기존의 루틴을 수정한뒤 어떤 RSC파일이었는지 정보를
//    담도록 변경... (리팩토링 해야함)
//
// 1. 상대경로나 혹은 파일이름 (현재는 파일이름만 되었음)으로 해시테이블검색을 통해....
//    어느 RSC파일을 수정할지 파악해야함.. (아무래도 기존 목록UI에 표시하는게 좋을 것이다..)

//
// 2. MediaPath의 해당 RSG파일을 읽는다...(이건 필요할때만 1회성으로만 처리해줘야 한다..)
// 3. 검색된 RSCNode객체를 획득하고....m_rel_path_map 사용하면 됨...

// 여기서 2번작업을 다른 작업보다 선행시켜도 상관없다면 그렇게 선행시키되...
// 멀티쓰레드쪽으로 보내버린다..  (동일 RSG파일이 2회로딩 안되도록 중복방지 취합이 중요하다)
// 

// 4. QAPath의 해당 RSG파일을 읽고
// 5. 동일한 RelPath의 RSCNode가 발견되면 교체....  m_rel_path_map 사용하면 됨..
//    그렇지않다면 추가삽입 처리..
// 6. 그리고 QAPath의 해당 RSG파일을 저장 후
// 7. RSC파일 익스포트

// 8. LivePath까지 진행하는 상황인 경우엔 해당RSC파일에 걸려있는 i3PackScript파일을 통한 패킹 갱신까지 해준다.
//    QAPath의 경우는 i3PackScript와는 전혀 무관하게 이미 처리된것으로 간주하면 된다..
//

//
// 9. 주의점... 서브쓰레드에서는 메인 쓰레드에서 쓰던 검색수행을 하면 안된다...
//              3, 5번항목을 제외한 모든 검색처리는 사전에 메인쓰레드에서 수행되었어야한다.
//  

