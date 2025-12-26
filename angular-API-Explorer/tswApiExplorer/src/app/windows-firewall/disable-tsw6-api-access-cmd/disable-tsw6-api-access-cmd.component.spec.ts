import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DisableTsw6ApiAccessCmdComponent } from './disable-tsw6-api-access-cmd.component';

describe('DisableTsw6ApiAccessCmdComponent', () => {
  let component: DisableTsw6ApiAccessCmdComponent;
  let fixture: ComponentFixture<DisableTsw6ApiAccessCmdComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [DisableTsw6ApiAccessCmdComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DisableTsw6ApiAccessCmdComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
