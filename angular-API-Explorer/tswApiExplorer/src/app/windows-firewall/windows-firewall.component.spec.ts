import { ComponentFixture, TestBed } from '@angular/core/testing';

import { WindowsFirewallComponent } from './windows-firewall.component';

describe('WindowsFirewallComponent', () => {
  let component: WindowsFirewallComponent;
  let fixture: ComponentFixture<WindowsFirewallComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [WindowsFirewallComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(WindowsFirewallComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
