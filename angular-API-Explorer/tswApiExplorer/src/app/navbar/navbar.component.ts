
import { Component } from '@angular/core';
import { RouterModule } from '@angular/router';
import { CommonModule } from '@angular/common';
import { ArduinoApiService } from '../core/arduino-api.service';

@Component({
  selector: 'app-navbar',
  imports: [RouterModule, CommonModule],
  standalone: true,
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.scss',
  providers: [ArduinoApiService]
})
export class NavbarComponent {
  showModal = false;
  showError = false;
  isConfigDropdownOpen = false;
  private pollInterval: any;
  private pollTimeout: any;

  constructor(private arduinoApi: ArduinoApiService) {}

  onRebootClick() {
    this.showModal = true;
    this.showError = false;
    this.arduinoApi.reboot().subscribe({
      next: () => this.startPolling(),
      error: () => this.startPolling()
    });
  }

  isConfigDropdownActive(): boolean {
    // Aktiv, wenn einer der drei Routen aktiv ist
    return [
      '/wifi-config',
      '/server-config',
      '/dns-config'
    ].some(path => window.location.pathname.endsWith(path));
  }

  private startPolling() {
    let elapsed = 0;
    this.pollInterval = setInterval(() => {
      elapsed += 500;
      this.arduinoApi.getStatus().subscribe({
        next: () => {
          this.closeModal();
        },
        error: () => {
          if (elapsed >= 15000) {
            this.showErrorModal();
          }
        }
      });
    }, 500);
    this.pollTimeout = setTimeout(() => {
      this.showErrorModal();
    }, 15000);
  }

  private closeModal() {
    this.showModal = false;
    this.showError = false;
    clearInterval(this.pollInterval);
    clearTimeout(this.pollTimeout);
  }

  private showErrorModal() {
    this.showError = true;
    clearInterval(this.pollInterval);
    clearTimeout(this.pollTimeout);
  }
}
