import { Component, OnInit } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { ArduinoApiService } from '../core/arduino-api.service';
import { WifiConfig, DeviceConfigRoot } from '../core/models/device-config.model';

@Component({
  selector: 'app-wifi-config-editor',
  templateUrl: './wifi-config-editor.component.html',
  styleUrls: ['./wifi-config-editor.component.scss'],
  standalone: true,
  imports: [CommonModule, FormsModule]
})
export class WifiConfigEditorComponent implements OnInit {
  wifiConfig: WifiConfig | null = null;
  loading = false;
  error: string | null = null;
  saving = false;
  saveSuccess: boolean | null = null;

  constructor(private api: ArduinoApiService) {}

  ngOnInit(): void {
    this.loadConfig();
  }

  loadConfig(): void {
    this.loading = true;
    this.error = null;
    this.saveSuccess = null;
    this.api.getConfig().subscribe({
      next: (cfg: DeviceConfigRoot) => {
        this.wifiConfig = { ...cfg.wifi };
        this.loading = false;
      },
      error: (err: any) => {
        this.error = 'Fehler beim Laden der Konfiguration';
        this.loading = false;
      }
    });
  }

  saveConfig(): void {
    if (!this.wifiConfig) return;
    this.saving = true;
    this.error = null;
    this.saveSuccess = null;
    // Hole aktuelle Config, überschreibe nur wifi
    this.api.getConfig().subscribe({
      next: (cfg: DeviceConfigRoot) => {
        const newConfig: DeviceConfigRoot = { ...cfg, wifi: this.wifiConfig! };
        this.api.updateConfig(newConfig).subscribe({
          next: () => {
            this.saving = false;
            this.saveSuccess = true;
          },
          error: (err: any) => {
            this.saving = false;
            this.saveSuccess = false;
            this.error = 'Fehler beim Speichern der Konfiguration';
          }
        });
      },
      error: (err: any) => {
        this.saving = false;
        this.saveSuccess = false;
        this.error = 'Fehler beim Laden der aktuellen Konfiguration';
      }
    });
  }
}
