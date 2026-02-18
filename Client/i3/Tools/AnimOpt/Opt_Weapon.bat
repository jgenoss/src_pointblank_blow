rem 무기가 추가 또는 변경되면 같이 변경되어야 됩니다.

@echo off

@set WORK_DIR=F:\Project\PointBlank\Media

for %%n in (870MCS,AK-47,C4,C5,DesertEagle,F2000,G36C,Hak,HK-PSG1,K5,K201,K-1,K-2,L115A1,M4A1,M-7Knife,M-9,MK23,MK46,MP5K,MP7,P90_Ext,P90_MC,P99,SG550,SPAS15,Spectre,SSG69,UMP45,SVD-Dragunov,M197) do AnimOpt Weapon\%%n
for %%n in (870MCS,AK-47,C4,C5,DesertEagle,F2000,G36C,Hak,HK-PSG1,K5,K201,K-1,K-2,L115A1,M4A1,M-7Knife,M-9,MK23,MK46,MP5K,MP7,P90_Ext,P90_MC,P99,SG550,SPAS15,Spectre,SSG69,UMP45,SVD-Dragunov,M197) do copy %WORK_DIR%\Weapon\%%n\%%n.i3AnimPack %WORK_DIR%\Weapon\
for %%n in (870MCS,AK-47,C4,C5,DesertEagle,F2000,G36C,Hak,HK-PSG1,K5,K201,K-1,K-2,L115A1,M4A1,M-7Knife,M-9,MK23,MK46,MP5K,MP7,P90_Ext,P90_MC,P99,SG550,SPAS15,Spectre,SSG69,UMP45,SVD-Dragunov,M197) do del %WORK_DIR%\Weapon\%%n\%%n.i3AnimPack

rem M-7의 경우 현재 폴더와 파일이름이 다르므로 예외처리합니다.

copy  %WORK_DIR%\Weapon\M-7Knife.i3AnimPack %WORK_DIR%\Weapon\M-7.i3AnimPack
del %WORK_DIR%\Weapon\M-7Knife.i3AnimPack

echo on