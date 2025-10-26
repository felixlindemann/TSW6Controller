# ===============================================================
#  Disable-TSW6-API-Access.ps1
#
#  (c) 2025 Felix Lindemann. All rights reserved.
#  Dieses Skript wird ohne Garantie bereitgestellt („as is“).
#  Nutzung, Veränderung und Weitergabe erfolgen auf eigenes Risiko.
#  Es besteht keinerlei Anspruch auf Funktionsfähigkeit oder Support.
# ===============================================================

# Variablen
$RuleName = "TSW6 External API"

Write-Host "TSW6 External API Firewall Cleanup"
Write-Host "---------------------------------"

$existingRule = Get-NetFirewallRule -DisplayName $RuleName -ErrorAction SilentlyContinue
if ($existingRule) {
    Remove-NetFirewallRule -DisplayName $RuleName
    Write-Host "Firewall-Regel '$RuleName' wurde entfernt."
} else {
    Write-Host "Keine Firewall-Regel '$RuleName' gefunden."
}
