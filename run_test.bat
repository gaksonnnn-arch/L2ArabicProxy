@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cl /nologo /EHsc /utf-8 /std:c++17 test_reshaper.cpp
if errorlevel 1 exit /b 1
test_reshaper.exe
if errorlevel 1 exit /b 1