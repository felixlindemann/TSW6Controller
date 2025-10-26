# ===============================================================
#  Disable-TSW6-API-Access.ps1
#
#  (c) 2025 Felix Lindemann. All rights reserved.
#  Dieses Skript wird ohne Garantie bereitgestellt („as is“).
#  Nutzung, Veränderung und Weitergabe erfolgen auf eigenes Risiko.
#  Es besteht keinerlei Anspruch auf Funktionsfähigkeit oder Support.
# ===============================================================

$RuleName = "TSW6 External API"
$Port = 31270
$enginePath = Join-Path $env:USERPROFILE "Documents\My Games\TrainSimWorld6\Saved\Config\WindowsNoEditor\engine.ini"

Write-Host "==============================================================="
Write-Host "TSW6 External API Cleanup"
Write-Host "==============================================================="

# Firewallregel entfernen
$existingRule = Get-NetFirewallRule -DisplayName $RuleName -ErrorAction SilentlyContinue
if ($existingRule) {
    Remove-NetFirewallRule -DisplayName $RuleName
    Write-Host "✅ Firewall-Regel entfernt."
}

# Portproxy entfernen
Write-Host "---------------------------------------------------------------"
Write-Host "Entferne Portproxy für Port $Port..."
Write-Host "---------------------------------------------------------------"
netsh interface portproxy delete v4tov4 listenport=$Port listenaddress=0.0.0.0
Write-Host "✅ Portproxy entfernt."

# engine.ini bereinigen
if (Test-Path $enginePath) {
    $content = Get-Content $enginePath
    $newContent = $content | Where-Object { $_ -notmatch '^\[HTTPServer\.Listeners\]' -and $_ -notmatch '^bind=' }
    Set-Content -Path $enginePath -Value $newContent
    Write-Host "✅ engine.ini bereinigt."
}

Write-Host "==============================================================="
Write-Host "Cleanup abgeschlossen."
Write-Host "==============================================================="
