@FOR /R %1 %%1 IN (.svn) DO RMDIR /S /Q "%%1"
@PAUSE 

