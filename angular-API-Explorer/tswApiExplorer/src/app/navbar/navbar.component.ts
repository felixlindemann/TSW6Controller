
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

  constructor(private arduinoApi: ArduinoApiService) {}

  onRebootClick() {
    this.showModal = true;
    this.arduinoApi.reboot().subscribe({
      next: () => {
        setTimeout(() => {
          this.showModal = false;
        }, 10000);
      },
      error: () => {
        setTimeout(() => {
          this.showModal = false;
        }, 10000);
      }
    });
  }
}
