import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TswLeverComponent } from './tsw-lever.component';

describe('TswLeverComponent', () => {
  let component: TswLeverComponent;
  let fixture: ComponentFixture<TswLeverComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TswLeverComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TswLeverComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
