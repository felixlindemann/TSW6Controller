# ===============================================================
#  Enable-TSW6-API-Access.ps1
#
#  (c) 2025 Felix Lindemann. All rights reserved.
#  Dieses Skript wird ohne Garantie bereitgestellt („as is“).
#  Nutzung, Veränderung und Weitergabe erfolgen auf eigenes Risiko.
#  Es besteht keinerlei Anspruch auf Funktionsfähigkeit oder Support.
# ===============================================================

# Variablen
$Port = 31270
$RuleName = "TSW6 External API"

# Alte Regel ggf. entfernen
$existingRule = Get-NetFirewallRule -DisplayName $RuleName -ErrorAction SilentlyContinue
if ($existingRule) {
    Write-Host "Eine alte Firewall-Regel '$RuleName' wurde gefunden – sie wird entfernt..."
    Remove-NetFirewallRule -DisplayName $RuleName
}

Write-Host "TSW6 External API Firewall Setup"
Write-Host "--------------------------------"
Write-Host "Bitte wähle eine Option:"
Write-Host "1) Gesamtes Subnetz freigeben (alle Geräte im privaten Netzwerk)"
Write-Host "2) Nur eine bestimmte IP-Adresse freigeben (z. B. dein Controller)"
Write-Host ""

$choice = Read-Host "Deine Auswahl (1 oder 2)"

switch ($choice) {
    "1" {
        # Ganze Subnetze freigeben (Private & Domain Profile)
        New-NetFirewallRule `
            -DisplayName $RuleName `
            -Direction Inbound `
            -Action Allow `
            -Protocol TCP `
            -LocalPort $Port `
            -Profile Private,Domain `
            -Enabled True

        Write-Host "Firewall-Regel erstellt: Port $Port für das gesamte Subnetz freigegeben."
    }

    "2" {
        $remoteIP = Read-Host "Bitte die IP-Adresse des Controllers eingeben (z. B. 192.168.1.42)"

        if ([string]::IsNullOrWhiteSpace($remoteIP)) {
            Write-Host "Keine IP angegeben – Abbruch."
            exit
        }

        New-NetFirewallRule `
            -DisplayName $RuleName `
            -Direction Inbound `
            -Action Allow `
            -Protocol TCP `
            -LocalPort $Port `
            -RemoteAddress $remoteIP `
            -Profile Private,Domain `
            -Enabled True

        Write-Host "Firewall-Regel erstellt: Port $Port nur für $remoteIP freigegeben."
    }

    Default {
        Write-Host "Ungültige Eingabe – keine Regel erstellt."
    }
}
