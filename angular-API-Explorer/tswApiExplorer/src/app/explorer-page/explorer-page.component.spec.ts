import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ExplorerPageComponent } from './explorer-page.component';

describe('ExplorerPageComponent', () => {
  let component: ExplorerPageComponent;
  let fixture: ComponentFixture<ExplorerPageComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ExplorerPageComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ExplorerPageComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
