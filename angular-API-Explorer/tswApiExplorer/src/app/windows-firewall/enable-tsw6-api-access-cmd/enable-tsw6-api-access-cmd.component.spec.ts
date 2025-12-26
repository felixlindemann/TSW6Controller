import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EnableTsw6ApiAccessCmdComponent } from './enable-tsw6-api-access-cmd.component';

describe('EnableTsw6ApiAccessCmdComponent', () => {
  let component: EnableTsw6ApiAccessCmdComponent;
  let fixture: ComponentFixture<EnableTsw6ApiAccessCmdComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [EnableTsw6ApiAccessCmdComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(EnableTsw6ApiAccessCmdComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
