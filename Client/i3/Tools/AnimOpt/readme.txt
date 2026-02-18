이름: AnimOpt
제작자: 백성민
배포일: 2008.08.08

----------------------------------------------------------------------------------------
실행 파일 : AnimOpt.exe
필수 파일 : AnimOptWork.ini
생성되는 파일 : Opt된 *.i3a 과 *.i3AnimPack

인자에 지정된 폴더(서브 포함)의 모든 애니메이션 파일(i3a)을 Opt하고 Pack파일을 생성하는 툴입니다.
인자 추가시 Opt나 Pack 생성을 스킵할 수 있습니다.

----------------------------------------------------------------------------------------
실행형식 : AnimOpt.exe [인자1] [인자2] [인자3]

[인자] 애니메이션이 있는 폴더. 상대 경로입니다. (안에 모든 서브 폴더도 포함됩니다.)
[인자] -NoOpt : Opt를 하지 않습니다.
[인자] -NoPack : Pack파일을 생성하지 않습니다.

예1: F:\Project\PointBlank\Media\Weapon\K-2폴더에 존재하는 모든 i3a파일을 Opt하고 Pack파일을 생성한다.
    AnimOpt F:\Project\PointBlank\Media\Weapon\K-2

예2: F:\Project\PointBlank\Media\Weapon\K-2폴더에 존재하는 모든 i3a파일에 대한 Pack파일만 생성한다.
    AnimOpt -NoOpt F:\Project\PointBlank\Media\Weapon\K-2

예2: F:\Project\PointBlank\Media\Weapon\K-2폴더에 존재하는 모든 i3a파일을 Opt만 한다.
    AnimOpt -NoPack F:\Project\PointBlank\Media\Weapon\K-2

----------------------------------------------------------------------------------------
반드시 Working folder가 설정된 AnimOptWork.ini파일이 실행파일 같은 폴더내에 존재해야 합니다.
만약 파일이 없다면 메모장 등으로 아스키 텍스트 형식으로 ini 파일을 생성해 주세요.
Working folder는 절대 경로여야 합니다. 

Working folder 설정 예: F:\Project\PointBlank\Media

----------------------------------------------------------------------------------------
note1. 실행파일의 인자 순서는 상관없습니다.
note2. 애니메이션 폴더는 절대 경로가 아닌 상대 경로여야 합니다. (절대 경로시 에러가 납니다) 
note3. 실행후 콘솔 창이 사라지면 작업후 정상 종료 된것이며, 에러 메시지와 함꼐 콘솔 창이 
남아있다면 정상 실행에 실패한 것입니다.
