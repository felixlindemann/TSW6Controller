import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DisableTsw6ApiAccessPsComponent } from './disable-tsw6-api-access-ps.component';

describe('DisableTsw6ApiAccessPsComponent', () => {
  let component: DisableTsw6ApiAccessPsComponent;
  let fixture: ComponentFixture<DisableTsw6ApiAccessPsComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [DisableTsw6ApiAccessPsComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DisableTsw6ApiAccessPsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
