
import { Component, Input } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TSWLever } from '../core/models/tsw-control.model';
import { TswApiService } from '../core/tsw-api.service';

import { VerticalSliderComponent } from '../vertical-slider/vertical-slider.component';
@Component({
  selector: 'app-tsw-lever',
  standalone: true,
  imports: [CommonModule, VerticalSliderComponent],
  templateUrl: './tsw-lever.component.html',
  styleUrl: './tsw-lever.component.scss'
})
export class TswLeverComponent {


  @Input()
  lever: TSWLever | null = null;

  constructor(private api: TswApiService) {}

  loadLever(controller: string) {
    // Holt den aktuellen Stand des Levers von der API
    if (!this.lever) return;
    this.api.getEndpoint('controls', this.lever.id).subscribe(res => {
      this.lever = res.Value as TSWLever;
    });
  }

  saveLever() {
    if (!this.lever) return;
    // Schreibt die Änderungen zurück an die API
    this.api.setEndpoint('controls', this.lever.id, this.lever).subscribe();
  }
}

