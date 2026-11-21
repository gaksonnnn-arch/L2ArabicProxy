@echo off
REM ============================================================================
REM Mohamed Tarek - L2ArabicProxy Build Script
REM ============================================================================

REM 1. Initialize Visual C++ 32-bit (x86) environment
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"

REM 2. Compile d3d9_proxy.cpp with MSVC
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.52.37025\bin\Hostx86\x86\cl.exe" /LD /O2 /EHsc /std:c++17 /utf-8 d3d9_proxy.cpp /link /OUT:d3d9.dll /DEF:d3d9.def user32.lib gdi32.lib gdiplus.lib

REM 3. Check compilation result
if %errorlevel%==0 (
    echo ========================================================
    echo BUILD SUCCESS - Build completed by Mohamed Tarek
    echo ========================================================

    powershell -NoProfile -Command "$ErrorActionPreference = 'Stop'; try { $games = Get-ChildItem -LiteralPath 'E:\Games' -Directory | Where-Object { $_.Name -like 'Lineage II*Samurai Crow*' }; if ($games.Count -ne 1) { throw 'Could not find Samurai Crow game directory on E:\Games' }; $target = Join-Path $games[0].FullName 'Samurai Crow\system'; if (-not (Test-Path -LiteralPath (Join-Path $target 'NWindow.dll'))) { throw ('NWindow.dll missing in target: ' + $target) }; $stamp = Get-Date -Format 'yyyyMMdd-HHmmss'; foreach ($name in @('d3d9.dll','l2arabic.ini','Cairo.ttf','Cairo-Bold.ttf')) { $source = Join-Path (Get-Location) $name; $destination = Join-Path $target $name; if (Test-Path -LiteralPath $destination) { Copy-Item -LiteralPath $destination -Destination ($destination + '.pre-build-' + $stamp + '.bak') }; Copy-Item -LiteralPath $source -Destination $destination -Force }; Write-Host ('Deployed successfully to: ' + $target); exit 0 } catch { Write-Error $_; exit 1 }"
    if errorlevel 1 exit /b 1

    del /Q d3d9_proxy.obj d3d9_proxy.lib d3d9_proxy.exp >nul 2>&1

    echo ========================================================
    echo FILES DEPLOYED TO GAME SYSTEM FOLDER!
    echo ========================================================
) else (
    echo ========================================================
    echo BUILD FAILED - Check compilation errors above
    echo ========================================================
)