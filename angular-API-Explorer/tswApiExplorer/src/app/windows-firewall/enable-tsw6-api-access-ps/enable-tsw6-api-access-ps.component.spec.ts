import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EnableTsw6ApiAccessPsComponent } from './enable-tsw6-api-access-ps.component';

describe('EnableTsw6ApiAccessPsComponent', () => {
  let component: EnableTsw6ApiAccessPsComponent;
  let fixture: ComponentFixture<EnableTsw6ApiAccessPsComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [EnableTsw6ApiAccessPsComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(EnableTsw6ApiAccessPsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
