@echo off
DEL ..\src\images.h
FOR %%X IN (*.hif) DO ..\tools\bin2h %%X
FOR %%X IN (*.h) DO type %%X >> ..\src\images.h
DEL *.h
