import { ComponentFixture, TestBed } from '@angular/core/testing';

import { WifiConfigEditorComponent } from './wifi-config-editor.component';

describe('WifiConfigEditorComponent', () => {
  let component: WifiConfigEditorComponent;
  let fixture: ComponentFixture<WifiConfigEditorComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [WifiConfigEditorComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(WifiConfigEditorComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
