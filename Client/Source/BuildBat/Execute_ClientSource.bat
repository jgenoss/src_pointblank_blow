echo off


rem 이 배치로 클라이언트 솔루션을 열게 하는 이유는
rem I3_ROOT를 재지정해서 하기 위해서입니다.

call define.bat

call %PB_SRC_ROOT%\ClientSource\ClientSource.sln