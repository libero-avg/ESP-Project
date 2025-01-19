@echo off
:loop
timeout /t 5 >nul
nslookup esp32.local >nul 2>&1
if %errorlevel% equ 0 (
    start http://esp32.local/
    exit
)
goto loop