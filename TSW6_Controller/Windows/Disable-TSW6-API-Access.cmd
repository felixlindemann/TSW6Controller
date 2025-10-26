@echo off
:: ===============================================================
::  Disable-TSW6-API-Access.cmd
::
::  (c) 2025 Felix Lindemann. All rights reserved.
::  Nutzung auf eigenes Risiko.
:: ===============================================================

echo Starte PowerShell-Skript "Disable-TSW6-API-Access.ps1" als Administrator...

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "Start-Process PowerShell -ArgumentList '-NoProfile -ExecutionPolicy Bypass -File \"%~dp0Disable-TSW6-API-Access.ps1\"' -Verb RunAs"
