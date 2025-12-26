
import { Component, OnInit } from '@angular/core';
import { NgForm, FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { ArduinoApiService } from '../core/arduino-api.service';
import { DeviceConfig } from '../core/models/device-config.model';

@Component({
	selector: 'app-dns-config-editor',
	standalone: true,
	imports: [CommonModule, FormsModule],
	templateUrl: './dns-config-editor.component.html',
	styleUrl: './dns-config-editor.component.scss',
	providers: [ArduinoApiService]
})
export class DnsConfigEditorComponent implements OnInit {
	deviceConfig: DeviceConfig | null = null;
	loading = false;
	saving = false;
	saveSuccess: boolean | null = null;
	error: string | null = null;

	constructor(private api: ArduinoApiService) {}

	ngOnInit() {
		this.loadConfig();
	}

	loadConfig() {
		this.loading = true;
		this.api.getConfig().subscribe({
			next: (data: any) => {
				this.deviceConfig = data?.device || null;
				this.loading = false;
				this.error = null;
			},
			error: () => {
				this.loading = false;
				this.error = 'Fehler beim Laden der DNS-Konfiguration.';
			}
		});
	}

	saveConfig() {
		if (!this.deviceConfig) return;
		this.saving = true;
		this.api.getConfig().subscribe({
			next: (data: any) => {
				const updated = { ...data, device: this.deviceConfig };
				this.api.updateConfig(updated).subscribe({
					next: () => {
						this.saving = false;
						this.saveSuccess = true;
						setTimeout(() => (this.saveSuccess = null), 2000);
					},
					error: () => {
						this.saving = false;
						this.saveSuccess = false;
					}
				});
			},
			error: () => {
				this.saving = false;
				this.saveSuccess = false;
			}
		});
	}
}
