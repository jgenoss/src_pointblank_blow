@FOR /R %1 %%1 IN (Resource) DO RMDIR /S /Q "%%1"
@PAUSE 
