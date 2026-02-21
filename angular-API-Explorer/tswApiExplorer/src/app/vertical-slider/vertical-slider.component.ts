import { Component, Input, Output, EventEmitter } from '@angular/core';


@Component({
  selector: 'app-vertical-slider',
  standalone: true,
  templateUrl: './vertical-slider.component.html',
  styleUrl: './vertical-slider.component.scss'
})
export class VerticalSliderComponent {
  @Input() min: number = 0;
  @Input() max: number = 100;
  @Input() step: number = 1;
  @Input() value: number = 0;
  @Output() valueChange = new EventEmitter<number>();

  onValueChange(event: Event) {
    const input = event.target as HTMLInputElement;
    this.value = Number(input.value);
    this.valueChange.emit(this.value);
  }
}
