# ===============================================================
#  Enable-TSW6-API-Access.ps1
#
#  (c) 2025 Felix Lindemann. All rights reserved.
#  Dieses Skript wird ohne Garantie bereitgestellt („as is“).
#  Nutzung, Veränderung und Weitergabe erfolgen auf eigenes Risiko.
#  Es besteht keinerlei Anspruch auf Funktionsfähigkeit oder Support.
# ===============================================================

$Port = 31270
$RuleName = "TSW6 External API"
$basePath = Join-Path $env:USERPROFILE "Documents\My Games\TrainSimWorld6\Saved\Config"
$enginePath = Join-Path $basePath "WindowsNoEditor\engine.ini"
$keyFile = Join-Path $basePath "CommAPIKey.txt"
$initialKey = "r2SpKhypgdoIfJQkgbCdKnXV2mKbrAwAqug3A3K/UA8="

Write-Host ""
Write-Host "==============================================================="
Write-Host "TSW6 External API Firewall & Portproxy Setup"
Write-Host "==============================================================="

# Entferne alte Firewallregel
$existingRule = Get-NetFirewallRule -DisplayName $RuleName -ErrorAction SilentlyContinue
if ($existingRule) {
    Write-Host "Alte Firewall-Regel '$RuleName' wird entfernt..."
    Remove-NetFirewallRule -DisplayName $RuleName
}

Write-Host ""
Write-Host "Bitte wähle eine Option:"
Write-Host "1) Gesamtes Subnetz freigeben (alle Geräte im privaten Netzwerk)"
Write-Host "2) Nur eine bestimmte IP-Adresse freigeben (z. B. dein Controller)"
Write-Host ""

$choice = Read-Host "Deine Auswahl (1 oder 2)"
$bindValue = ""

switch ($choice) {
    "1" {
        New-NetFirewallRule -DisplayName $RuleName -Direction Inbound -Action Allow -Protocol TCP -LocalPort $Port -Profile Any -Enabled True
        $bindValue = "0.0.0.0"
        Write-Host "✅ Firewall-Regel für gesamtes Subnetz erstellt."
    }
    "2" {
        $remoteIP = Read-Host "Bitte die IP-Adresse des Controllers eingeben (z. B. 192.168.1.42)"
        if ([string]::IsNullOrWhiteSpace($remoteIP)) {
            Write-Host "❌ Keine IP-Adresse angegeben – Abbruch."
            exit
        }
        New-NetFirewallRule -DisplayName $RuleName -Direction Inbound -Action Allow -Protocol TCP -LocalPort $Port -RemoteAddress $remoteIP -Profile Any -Enabled True
        $bindValue = $remoteIP
        Write-Host "✅ Firewall-Regel für $remoteIP erstellt."
    }
    Default {
        Write-Host "❌ Ungültige Eingabe – keine Regel erstellt."
        exit
    }
}

# Engine.ini anpassen
Write-Host ""
Write-Host "---------------------------------------------------------------"
Write-Host "Möchtest du die engine.ini automatisch anpassen?"
Write-Host "Datei: $enginePath"
Write-Host "---------------------------------------------------------------"
$engineChoice = Read-Host "Anpassen? (J/N)"

if ($engineChoice -match '^[JjYy]') {
    if (-not (Test-Path $enginePath)) {
        Write-Host "engine.ini wird neu erstellt..."
        New-Item -Path $enginePath -ItemType File -Force | Out-Null
    }
    $sectionHeader = "[HTTPServer.Listeners]"
    $bindLine = "bind=$bindValue"
    $content = Get-Content $enginePath -ErrorAction SilentlyContinue | Where-Object { $_ -notmatch '^\[HTTPServer\.Listeners\]' -and $_ -notmatch '^bind=' }
    Add-Content -Path $enginePath -Value "`r`n$sectionHeader`r`n$bindLine`r`n"
    Write-Host "✅ engine.ini angepasst: $bindLine"
}

# CommAPIKey anlegen
if (-not (Test-Path $keyFile)) {
    Set-Content -Path $keyFile -Value $initialKey
    Write-Host "✅ CommAPIKey.txt erstellt."
}

# Portproxy hinzufügen
Write-Host ""
Write-Host "---------------------------------------------------------------"
Write-Host "Richte Portproxy ein, um externe Zugriffe zu erlauben..."
Write-Host "---------------------------------------------------------------"
netsh interface portproxy add v4tov4 listenport=$Port listenaddress=0.0.0.0 connectport=$Port connectaddress=127.0.0.1
Write-Host "✅ Portproxy erstellt: 0.0.0.0:$Port → 127.0.0.1:$Port"

# Explorer öffnen
if (Test-Path $basePath) {
    Start-Process explorer.exe $basePath
}

Write-Host ""
Write-Host "==============================================================="
Write-Host "Setup abgeschlossen. Prüfe mit:"
Write-Host "netsh interface portproxy show all"
Write-Host "==============================================================="
