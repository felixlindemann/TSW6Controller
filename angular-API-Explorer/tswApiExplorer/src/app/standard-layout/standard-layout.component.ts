import { Component } from '@angular/core';
import { VerticalSliderComponent } from '../vertical-slider/vertical-slider.component';
import { NotchPosition, NotchTable, TSWLever } from '../core/models/tsw-control.model';
import { TswLeverComponent } from '../tsw-lever/tsw-lever.component';

@Component({
  selector: 'app-standard-layout',
  standalone: true,
  imports: [VerticalSliderComponent, TswLeverComponent],
  templateUrl: './standard-layout.component.html',
  styleUrl: './standard-layout.component.scss'
})
export class StandardLayoutComponent {

  /*
    label: string;
  tsw: number;
  range: [number, number];
  */
  pos: NotchPosition[] =[
    { label: '0%', tsw: 0 , range : [0, 10]},
    { label: '25%', tsw: 0.25, range: [11, 30]},
    { label: '50%', tsw: 0.5, range: [31, 60]},
    { label: '75%', tsw: 0.75, range: [61, 90]},
    { label: '100%', tsw: 1, range: [91, 100]} 
  ]
 
notchtable : NotchTable = {
    controller: '/set/analogslider',
    label: 'AFB',
    inverted: true,
    positions: this.pos
}

  sld1: TSWLever = {
    id: 'lever1',
  type: 'TSWLever',
  hardwareType: 'AnalogSlider',
  notches: this.notchtable,
  pin: 12,
  currentPercent: 42,
  currentRaw: 2335,
  inverted: true,
  lastSentValue: 0
  };

  constructor() { }

}
