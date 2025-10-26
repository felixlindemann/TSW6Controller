@echo off
:: ===============================================================
::  Disable-TSW6-API-Access.cmd
::
::  (c) 2025 Felix Lindemann. All rights reserved.
::  Nutzung auf eigenes Risiko.
::  Dieses Skript wird ohne Garantie bereitgestellt („as is“).
:: ===============================================================

echo.
echo ===============================================================
echo  TSW6 External API Cleanup (CMD-Starter)
echo ===============================================================
echo.

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "Start-Process PowerShell -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File \"%~dp0Disable-TSW6-API-Access.ps1\"' -Verb RunAs"

pause
