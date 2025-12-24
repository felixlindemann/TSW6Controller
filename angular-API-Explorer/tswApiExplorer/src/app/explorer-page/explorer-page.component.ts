import { CommonModule } from '@angular/common';
import { Component, computed, inject, signal, OnInit, DestroyRef } from '@angular/core';
import { takeUntilDestroyed } from '@angular/core/rxjs-interop';
import { FormsModule } from '@angular/forms';
import { ActivatedRoute, Router, RouterModule, NavigationEnd } from '@angular/router';
import { CdkDragDrop, DragDropModule, moveItemInArray } from '@angular/cdk/drag-drop';
import { forkJoin, of } from 'rxjs';
import { catchError, filter, finalize, map } from 'rxjs/operators';

import { TswApiService } from '../core/tsw-api.service';
import type { TswEndpoint, TswGetResponse, TswListResponse } from '../core/tsw-api.models';

type UiNode = {
  displayName: string;
  nextPath: string; // normalized, WITHOUT "Root/"
};

type Breadcrumb = {
  label: string;
  segments: string[]; // segments after /list
};

type NotchRow = {
  label: string;
  tswValue: number | null;
  rangeMin: number;
  rangeMax: number;
  locked: boolean;
  selected: boolean;
};

type InputMeta = {
  min: number;
  max: number;
  notchCount: number;
};

@Component({
  selector: 'app-explorer-page',
  standalone: true,
  imports: [CommonModule, FormsModule, RouterModule, DragDropModule],
  templateUrl: './explorer-page.component.html',
  styleUrl: './explorer-page.component.scss',
})
export class ExplorerPageComponent implements OnInit {
  private readonly api = inject(TswApiService);
  private readonly router = inject(Router);
  private readonly route = inject(ActivatedRoute);
  private readonly destroyRef = inject(DestroyRef);

  // UI state
  readonly loading = signal(false);
  readonly error = signal<string | null>(null);

  readonly listResponse = signal<TswListResponse | null>(null);

  readonly nodes = signal<UiNode[]>([]);
  readonly endpoints = signal<TswEndpoint[]>([]);

  readonly nodeFilter = signal('');
  readonly endpointFilter = signal('');

  readonly selectedEndpoint = signal<TswEndpoint | null>(null);
  readonly endpointResponse = signal<TswGetResponse | null>(null);

  // Write UI
  readonly writeValue = signal<number | null>(null);
  readonly inputMeta = signal<InputMeta | null>(null);

  // NotchTable editor UI
  readonly notchRows = signal<NotchRow[]>([]);
  readonly customNotchCount = signal<number>(5);
  readonly notchJsonPreview = computed(() => this.buildNotchTableJsonPreview());
  readonly controllerForNotchTable = computed(() => this.buildControllerForNotchTable());
  readonly notchValidationErrors = computed(() => this.validateNotchTable());

  // Path handling
  readonly currentSegments = signal<string[]>([]); // segments AFTER /list
  readonly currentNodePath = computed(() => this.currentSegments().join('/')); // WITHOUT Root/
  readonly isRoot = computed(() => this.currentSegments().length === 0);

  // Breadcrumbs (clickable)
  readonly breadcrumbs = computed<Breadcrumb[]>(() => {
    const segs = this.currentSegments();
    const crumbs: Breadcrumb[] = [{ label: 'Root', segments: [] }];

    let acc: string[] = [];
    for (const s of segs) {
      acc = [...acc, s];
      crumbs.push({ label: s, segments: acc });
    }
    return crumbs;
  });

  // Filtered lists
  readonly filteredNodes = computed(() => {
    const q = this.nodeFilter().trim().toLowerCase();
    const items = this.nodes();
    if (!q) return items;
    return items.filter(n => n.displayName.toLowerCase().includes(q));
  });

  readonly filteredEndpoints = computed(() => {
    const q = this.endpointFilter().trim().toLowerCase();
    const items = this.endpoints();
    if (!q) return items;
    return items.filter(e => e.Name.toLowerCase().includes(q));
  });

  ngOnInit() {
    // React to URL changes (including browser back/forward and programmatic navigation)
    this.router.events
      .pipe(
        filter((event): event is NavigationEnd => event instanceof NavigationEnd),
        takeUntilDestroyed(this.destroyRef)
      )
      .subscribe(event => {
        this.handleUrlChange(event.urlAfterRedirects || event.url);
      });

    // Initial load
    this.handleUrlChange(this.router.url);
  }

  private handleUrlChange(url: string) {
    const segs = this.extractSegmentsAfterList(url);

    // Only reload if the path actually changed
    const currentPath = this.currentSegments().join('/');
    const newPath = segs.join('/');
    if (currentPath === newPath && this.listResponse() !== null) {
      return; // No change, skip reload
    }

    this.currentSegments.set(segs);
    this.loadList();
  }

  // ----------------------------
  // Navigation
  // ----------------------------

  navigateBreadcrumb(segments: string[]) {
    this.router.navigate(['/list', ...segments]);
  }

  navigateToNode(nextPath: string) {
    const segs = nextPath ? nextPath.split('/').filter(Boolean) : [];
    this.router.navigate(['/list', ...segs]);
  }

  // ----------------------------
  // API Loading
  // ----------------------------

  private loadList() {
    this.error.set(null);
    this.loading.set(true);

    const path = this.currentNodePath(); // '' for root
    this.api
      .list(path ? path : null)
      .pipe(
        map(resp => this.normalizeListResponse(resp)),
        catchError(err => {
          this.error.set(this.formatError(err));
          this.listResponse.set(null);
          this.nodes.set([]);
          this.endpoints.set([]);
          this.selectedEndpoint.set(null);
          this.endpointResponse.set(null);
          this.inputMeta.set(null);
          this.notchRows.set([]);
          return of(null);
        }),
        finalize(() => this.loading.set(false))
      )
      .subscribe(resp => {
        if (!resp) return;

        this.listResponse.set(resp);
        this.nodes.set(this.extractUiNodes(resp));
        this.endpoints.set(resp.Endpoints ?? []);

        // If selected endpoint is not in list anymore, clear
        const sel = this.selectedEndpoint();
        if (sel && !(resp.Endpoints ?? []).some(e => e.Name === sel.Name)) {
          this.selectedEndpoint.set(null);
          this.endpointResponse.set(null);
          this.inputMeta.set(null);
          this.notchRows.set([]);
        }
      });
  }

  readEndpoint(endpoint: TswEndpoint) {
    this.selectedEndpoint.set(endpoint);
    this.endpointResponse.set(null);
    this.error.set(null);

    // Load the endpoint value
    this.refreshSelectedEndpointValue(true);

    // Maybe load meta for InputValue slider
    this.maybeLoadInputMeta(endpoint);
  }

  refreshSelectedEndpointValue(setWriteValueFromGet: boolean) {
    const sel = this.selectedEndpoint();
    if (!sel) return;

    const nodePath = this.currentNodePath();
    this.api
      .getEndpoint(nodePath, sel.Name)
      .pipe(
        catchError(err => {
          this.error.set(this.formatError(err));
          return of(null);
        })
      )
      .subscribe(resp => {
        if (!resp) return;
        this.endpointResponse.set(resp);

        const numeric = this.extractReturnValueNumber(resp);
        if (setWriteValueFromGet && sel.Writable && typeof numeric === 'number') {
          this.writeValue.set(numeric);
        }
      });
  }

  // Set tswValue for a notch row to the endpoint
  setTswValueForRow(index: number) {
    const sel = this.selectedEndpoint();
    if (!sel || !sel.Writable) return;

    const rows = this.notchRows();
    const row = rows[index];
    if (!row || row.tswValue === null) return;

    const nodePath = this.currentNodePath();
    this.error.set(null);

    this.api
      .setEndpoint(nodePath, sel.Name, row.tswValue)
      .pipe(
        catchError(err => {
          this.error.set(this.formatError(err));
          return of(null);
        })
      )
      .subscribe(ok => {
        if (!ok) return;
        // Update slider/writeValue to match
        this.writeValue.set(row.tswValue);
        // Refresh to confirm
        this.refreshSelectedEndpointValue(false);
      });
  }

  writeSelectedEndpoint() {
    const sel = this.selectedEndpoint();
    if (!sel || !sel.Writable) return;

    const value = this.writeValue();
    if (value === null || Number.isNaN(value)) return;

    const nodePath = this.currentNodePath();
    this.error.set(null);
    this.loading.set(true);

    this.api
      .setEndpoint(nodePath, sel.Name, value)
      .pipe(
        catchError(err => {
          this.error.set(this.formatError(err));
          return of(null);
        }),
        finalize(() => this.loading.set(false))
      )
      .subscribe(ok => {
        if (!ok) return;
        // After successful SET -> refresh with GET
        this.refreshSelectedEndpointValue(false);
      });
  }

  // ----------------------------
  // InputValue Meta + Slider
  // ----------------------------

  private maybeLoadInputMeta(endpoint: TswEndpoint) {
    this.inputMeta.set(null);
    this.notchRows.set([]);

    if (!endpoint.Writable) return;

    const endpointName = endpoint.Name || '';
    const looksLikeInputValue = endpointName === 'InputValue' || endpointName.endsWith('.InputValue') || endpointName.includes('InputValue');
    if (!looksLikeInputValue) return;

    const names = (this.endpoints() ?? []).map(e => e.Name);
    const hasMin = names.includes('Function.GetMinimumInputValue');
    const hasMax = names.includes('Function.GetMaximumInputValue');
    const hasNotches = names.includes('Function.GetNotchCount');

    if (!hasMin || !hasMax || !hasNotches) return;

    const nodePath = this.currentNodePath();

    forkJoin({
      min: this.api.getEndpoint(nodePath, 'Function.GetMinimumInputValue').pipe(map(r => this.extractReturnValueNumber(r))),
      max: this.api.getEndpoint(nodePath, 'Function.GetMaximumInputValue').pipe(map(r => this.extractReturnValueNumber(r))),
      notchCount: this.api.getEndpoint(nodePath, 'Function.GetNotchCount').pipe(map(r => this.extractReturnValueNumber(r))),
    })
      .pipe(
        catchError(err => {
          this.error.set(this.formatError(err));
          return of({ min: null, max: null, notchCount: null });
        })
      )
      .subscribe(meta => {
        const min = typeof meta.min === 'number' ? meta.min : null;
        const max = typeof meta.max === 'number' ? meta.max : null;
        const notchCount = typeof meta.notchCount === 'number' ? Math.round(meta.notchCount) : null;

        if (min === null || max === null || notchCount === null || notchCount <= 0) return;

        this.inputMeta.set({ min, max, notchCount });

        // Init NotchTable rows (use notchCount)
        this.buildDefaultNotchRows(notchCount);
      });
  }

  sliderStep(): number | 'any' {
    const meta = this.inputMeta();
    if (!meta) return 'any';
    if (meta.notchCount <= 1) return 'any';
    const denom = meta.notchCount - 1;
    const step = (meta.max - meta.min) / denom;
    return step > 0 ? step : 'any';
  }

  // ----------------------------
  // NotchTable Editor
  // ----------------------------

  private buildDefaultNotchRows(notchCount: number) {
    // Reasonable default: ranges distributed over 0..100
    const rows: NotchRow[] = [];
    if (notchCount <= 1) {
      rows.push({ label: 'Notch 0', tswValue: null, rangeMin: 0, rangeMax: 100, locked: false, selected: false });
      this.notchRows.set(rows);
      return;
    }

    for (let i = 0; i < notchCount; i++) {
      const start = Math.round((i * 100) / (notchCount - 1));
      const end =
        i === notchCount - 1
          ? 100
          : Math.round(((i + 1) * 100) / (notchCount - 1)) - 1;

      rows.push({
        label: `Notch ${i}`,
        tswValue: null,
        rangeMin: Math.max(0, Math.min(100, start)),
        rangeMax: Math.max(0, Math.min(100, end)),
        locked: false,
        selected: false,
      });
    }

    this.notchRows.set(rows);
  }

  autoDistributeRanges() {
    const meta = this.inputMeta();
    if (!meta) return;
    this.buildDefaultNotchRows(meta.notchCount);
  }

  createNotchesByCustomCount() {
    const count = this.customNotchCount();
    if (count <= 0) return;
    this.buildDefaultNotchRows(count);
  }

  setFirstTswValueToMin() {
    const meta = this.inputMeta();
    if (!meta) return;
    const rows = this.notchRows();
    if (rows.length === 0) return;
    this.updateNotchRowTswValue(0, meta.min);
  }

  setLastTswValueToMax() {
    const meta = this.inputMeta();
    if (!meta) return;
    const rows = this.notchRows();
    if (rows.length === 0) return;
    this.updateNotchRowTswValue(rows.length - 1, meta.max);
  }

  autoFillRangeMin() {
    const rows = this.notchRows();
    if (rows.length === 0) return;

    const updated = [...rows];
    let lastRangeMax = -1; // So first row gets rangeMin = 0

    for (let i = 0; i < rows.length; i++) {
      if (updated[i].locked) {
        // Skip locked row, but use its rangeMax for next calculation
        lastRangeMax = updated[i].rangeMax;
        continue;
      }
      updated[i] = { ...updated[i], rangeMin: lastRangeMax + 1 };
      lastRangeMax = updated[i].rangeMax;
    }

    this.notchRows.set(updated);
  }

  autoFillRangeMax() {
    const rows = this.notchRows();
    if (rows.length === 0) return;

    const updated = [...rows];
    let nextRangeMin = 101; // So last row gets rangeMax = 100

    for (let i = rows.length - 1; i >= 0; i--) {
      if (updated[i].locked) {
        // Skip locked row, but use its rangeMin for next calculation
        nextRangeMin = updated[i].rangeMin;
        continue;
      }
      updated[i] = { ...updated[i], rangeMax: nextRangeMin - 1 };
      nextRangeMin = updated[i].rangeMin;
    }

    this.notchRows.set(updated);
  }

  addNotchRow() {
    const rows = [...this.notchRows()];
    rows.push({ label: `Custom ${rows.length}`, tswValue: null, rangeMin: 0, rangeMax: 100, locked: false, selected: false });
    this.notchRows.set(rows);
  }

  removeNotchRow(index: number) {
    const rows = [...this.notchRows()];
    rows.splice(index, 1);
    this.notchRows.set(rows);
  }

  moveNotchRowUp(index: number) {
    if (index <= 0) return;
    const rows = [...this.notchRows()];
    [rows[index - 1], rows[index]] = [rows[index], rows[index - 1]];
    this.notchRows.set(rows);
  }

  moveNotchRowDown(index: number) {
    const rows = [...this.notchRows()];
    if (index >= rows.length - 1) return;
    [rows[index], rows[index + 1]] = [rows[index + 1], rows[index]];
    this.notchRows.set(rows);
  }

  dropNotchRow(event: CdkDragDrop<NotchRow[]>) {
    const rows = [...this.notchRows()];
    moveItemInArray(rows, event.previousIndex, event.currentIndex);
    this.notchRows.set(rows);
  }

  updateNotchRowLabel(index: number, value: string) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, label: value } : r
    );
    this.notchRows.set(rows);
  }

  updateNotchRowTswValue(index: number, value: number | null) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, tswValue: value } : r
    );
    this.notchRows.set(rows);
  }

  updateNotchRowRangeMin(index: number, value: number) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, rangeMin: value } : r
    );
    this.notchRows.set(rows);
  }

  updateNotchRowRangeMax(index: number, value: number) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, rangeMax: value } : r
    );
    this.notchRows.set(rows);
  }

  updateNotchRowLocked(index: number, value: boolean) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, locked: value } : r
    );
    this.notchRows.set(rows);
  }

  updateNotchRowSelected(index: number, value: boolean) {
    const rows = this.notchRows().map((r, idx) =>
      idx === index ? { ...r, selected: value } : r
    );
    this.notchRows.set(rows);
  }

  // Selection helpers
  hasSelectedRows(): boolean {
    return this.notchRows().some(r => r.selected);
  }

  selectedRowCount(): number {
    return this.notchRows().filter(r => r.selected).length;
  }

  allRowsSelected(): boolean {
    const rows = this.notchRows();
    return rows.length > 0 && rows.every(r => r.selected);
  }

  someRowsSelected(): boolean {
    const rows = this.notchRows();
    const selectedCount = rows.filter(r => r.selected).length;
    return selectedCount > 0 && selectedCount < rows.length;
  }

  toggleSelectAll(event: Event) {
    const checked = (event.target as HTMLInputElement).checked;
    const rows = this.notchRows().map(r => ({ ...r, selected: checked }));
    this.notchRows.set(rows);
  }

  clearSelection() {
    const rows = this.notchRows().map(r => ({ ...r, selected: false }));
    this.notchRows.set(rows);
  }

  // Distribute values evenly for selected rows, respecting locked rows as interval boundaries
  distributeSelectedTswValues() {
    const rows = this.notchRows();
    const selectedIndices = rows
      .map((r, i) => (r.selected ? i : -1))
      .filter(i => i >= 0);

    if (selectedIndices.length < 2) return;

    // Split into intervals by locked rows
    const intervals = this.splitIntoIntervals(selectedIndices, rows, 'tswValue');
    
    const updated = [...rows];
    for (const interval of intervals) {
      if (interval.editableIndices.length === 0) continue;
      
      const startValue = interval.startValue;
      const endValue = interval.endValue;
      const totalCount = interval.allIndices.length;
      
      if (totalCount < 2) continue;
      
      const step = (endValue - startValue) / (totalCount - 1);
      
      interval.allIndices.forEach((rowIndex, i) => {
        if (!rows[rowIndex].locked) {
          const value = startValue + step * i;
          updated[rowIndex] = { ...updated[rowIndex], tswValue: Math.round(value * 1000) / 1000 };
        }
      });
    }

    this.notchRows.set(updated);
  }

  distributeSelectedRangeMin() {
    const rows = this.notchRows();
    const selectedIndices = rows
      .map((r, i) => (r.selected ? i : -1))
      .filter(i => i >= 0);

    if (selectedIndices.length < 2) return;

    // Split into intervals by locked rows
    const intervals = this.splitIntoIntervals(selectedIndices, rows, 'rangeMin');
    
    const updated = [...rows];
    for (const interval of intervals) {
      if (interval.editableIndices.length === 0) continue;
      
      const startValue = interval.startValue;
      const endValue = interval.endValue;
      const totalCount = interval.allIndices.length;
      
      if (totalCount < 2) continue;
      
      const step = (endValue - startValue) / (totalCount - 1);
      
      interval.allIndices.forEach((rowIndex, i) => {
        if (!rows[rowIndex].locked) {
          const value = startValue + step * i;
          updated[rowIndex] = { ...updated[rowIndex], rangeMin: Math.round(value) };
        }
      });
    }

    this.notchRows.set(updated);
  }

  distributeSelectedRangeMax() {
    const rows = this.notchRows();
    const selectedIndices = rows
      .map((r, i) => (r.selected ? i : -1))
      .filter(i => i >= 0);

    if (selectedIndices.length < 2) return;

    // Split into intervals by locked rows
    const intervals = this.splitIntoIntervals(selectedIndices, rows, 'rangeMax');
    
    const updated = [...rows];
    for (const interval of intervals) {
      if (interval.editableIndices.length === 0) continue;
      
      const startValue = interval.startValue;
      const endValue = interval.endValue;
      const totalCount = interval.allIndices.length;
      
      if (totalCount < 2) continue;
      
      const step = (endValue - startValue) / (totalCount - 1);
      
      interval.allIndices.forEach((rowIndex, i) => {
        if (!rows[rowIndex].locked) {
          const value = startValue + step * i;
          updated[rowIndex] = { ...updated[rowIndex], rangeMax: Math.round(value) };
        }
      });
    }

    this.notchRows.set(updated);
  }

  private splitIntoIntervals(
    selectedIndices: number[],
    rows: NotchRow[],
    field: 'tswValue' | 'rangeMin' | 'rangeMax'
  ): { allIndices: number[]; editableIndices: number[]; startValue: number; endValue: number }[] {
    const intervals: { allIndices: number[]; editableIndices: number[]; startValue: number; endValue: number }[] = [];
    
    // Find locked rows within selection
    const lockedIndicesInSelection = selectedIndices.filter(i => rows[i].locked);
    
    if (lockedIndicesInSelection.length === 0) {
      // No locked rows - single interval from first to last
      const firstIdx = selectedIndices[0];
      const lastIdx = selectedIndices[selectedIndices.length - 1];
      const startValue = this.getFieldValue(rows[firstIdx], field);
      const endValue = this.getFieldValue(rows[lastIdx], field);
      
      intervals.push({
        allIndices: selectedIndices,
        editableIndices: selectedIndices.filter(i => !rows[i].locked),
        startValue,
        endValue,
      });
    } else {
      // Split by locked rows
      let currentStart = 0;
      
      for (const lockedIdx of lockedIndicesInSelection) {
        const lockedPosition = selectedIndices.indexOf(lockedIdx);
        
        if (lockedPosition > currentStart) {
          // Interval from currentStart to locked row (inclusive)
          const intervalIndices = selectedIndices.slice(currentStart, lockedPosition + 1);
          const firstIdx = intervalIndices[0];
          const lastIdx = intervalIndices[intervalIndices.length - 1];
          
          intervals.push({
            allIndices: intervalIndices,
            editableIndices: intervalIndices.filter(i => !rows[i].locked),
            startValue: this.getFieldValue(rows[firstIdx], field),
            endValue: this.getFieldValue(rows[lastIdx], field),
          });
        }
        
        currentStart = lockedPosition;
      }
      
      // Last interval from last locked to end
      if (currentStart < selectedIndices.length - 1) {
        const intervalIndices = selectedIndices.slice(currentStart);
        const firstIdx = intervalIndices[0];
        const lastIdx = intervalIndices[intervalIndices.length - 1];
        
        intervals.push({
          allIndices: intervalIndices,
          editableIndices: intervalIndices.filter(i => !rows[i].locked),
          startValue: this.getFieldValue(rows[firstIdx], field),
          endValue: this.getFieldValue(rows[lastIdx], field),
        });
      }
    }
    
    return intervals;
  }

  private getFieldValue(row: NotchRow, field: 'tswValue' | 'rangeMin' | 'rangeMax'): number {
    const value = row[field];
    return typeof value === 'number' ? value : 0;
  }

  trackNotchRow(index: number, row: NotchRow): number {
    return index;
  }

  // Row "Get": reads current endpoint value and stores it as tswValue for that row
  captureTswValueForRow(index: number) {
    const sel = this.selectedEndpoint();
    if (!sel) return;

    const nodePath = this.currentNodePath();
    this.api
      .getEndpoint(nodePath, sel.Name)
      .pipe(
        catchError(err => {
          this.error.set(this.formatError(err));
          return of(null);
        })
      )
      .subscribe(resp => {
        if (!resp) return;
        this.endpointResponse.set(resp);

        const numeric = this.extractReturnValueNumber(resp);
        if (typeof numeric !== 'number') return;

        // update row
        const rows = [...this.notchRows()];
        const row = rows[index];
        if (!row) return;
        rows[index] = { ...row, tswValue: numeric };
        this.notchRows.set(rows);

        // also jump slider/value to that position
        if (sel.Writable) {
          this.writeValue.set(numeric);
        }
      });
  }

  copyNotchJson() {
    const txt = JSON.stringify(this.buildNotchTableJsonPreview(), null, 2);
    navigator.clipboard?.writeText(txt).catch(() => undefined);
  }

  exportNotchJson() {
    const json = this.buildNotchTableJsonPreview();
    const txt = JSON.stringify(json, null, 2);
    const blob = new Blob([txt], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    
    // Generate filename from controller path
    const controller = this.controllerForNotchTable();
    const filename = controller
      ? controller.replace(/^\/set\//, '').replace(/\//g, '_').replace(/\./g, '_') + '.json'
      : 'notch-table.json';
    
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }

  importNotchJson(event: Event) {
    const input = event.target as HTMLInputElement;
    const file = input.files?.[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = () => {
      try {
        const content = reader.result as string;
        const json = JSON.parse(content);

        // Expected format: { controller: string, positions: [{ label, tsw, range: [min, max] }] }
        const positions = json?.positions;
        if (!Array.isArray(positions)) {
          this.error.set('Invalid JSON format: "positions" array not found');
          return;
        }

        const rows: NotchRow[] = positions.map((p: any, i: number) => ({
          label: typeof p.label === 'string' ? p.label : `Notch ${i}`,
          tswValue: typeof p.tsw === 'number' ? p.tsw : null,
          rangeMin: Array.isArray(p.range) && typeof p.range[0] === 'number' ? p.range[0] : 0,
          rangeMax: Array.isArray(p.range) && typeof p.range[1] === 'number' ? p.range[1] : 100,
          locked: false,
          selected: false,
        }));

        this.notchRows.set(rows);
        this.error.set(null);
      } catch (e) {
        this.error.set('Failed to parse JSON file');
      }

      // Reset input so the same file can be selected again
      input.value = '';
    };

    reader.onerror = () => {
      this.error.set('Failed to read file');
      input.value = '';
    };

    reader.readAsText(file);
  }

  // Controller string = SET endpoint without "?Value="
  private buildControllerForNotchTable(): string {
    const sel = this.selectedEndpoint();
    if (!sel) return '';
    const nodePath = this.currentNodePath();
    if (!nodePath) return '';
    return `/set/${nodePath}.${sel.Name}`;
  }

  private buildNotchTableJsonPreview(): unknown {
    const controller = this.controllerForNotchTable();
    const rows = this.notchRows();

    return {
      controller,
      positions: rows.map(r => ({
        label: r.label,
        tsw: r.tswValue ?? 0,
        range: [r.rangeMin, r.rangeMax],
      })),
    };
  }

  private validateNotchTable(): { row?: number; message: string; type: 'error' | 'warning' }[] {
    const errors: { row?: number; message: string; type: 'error' | 'warning' }[] = [];
    const rows = this.notchRows();
    const meta = this.inputMeta();

    if (rows.length === 0) return errors;

    // Rule 6: First row rangeMin = 0
    if (rows[0].rangeMin !== 0) {
      errors.push({ row: 0, message: `Zeile 1: rangeMin sollte 0 sein (ist ${rows[0].rangeMin})`, type: 'error' });
    }

    // Rule 7: Last row rangeMax = 100
    const lastIdx = rows.length - 1;
    if (rows[lastIdx].rangeMax !== 100) {
      errors.push({ row: lastIdx, message: `Zeile ${lastIdx + 1}: rangeMax sollte 100 sein (ist ${rows[lastIdx].rangeMax})`, type: 'error' });
    }

    // Rule 2: First tswValue >= meta.min
    if (meta && rows[0].tswValue !== null && rows[0].tswValue < meta.min) {
      errors.push({ row: 0, message: `Zeile 1: tswValue (${rows[0].tswValue}) < Minimum (${meta.min})`, type: 'error' });
    }

    // Rule 3: Last tswValue <= meta.max
    if (meta && rows[lastIdx].tswValue !== null && rows[lastIdx].tswValue > meta.max) {
      errors.push({ row: lastIdx, message: `Zeile ${lastIdx + 1}: tswValue (${rows[lastIdx].tswValue}) > Maximum (${meta.max})`, type: 'error' });
    }

    for (let i = 0; i < rows.length; i++) {
      const row = rows[i];
      const rowNum = i + 1;

      // Rule: tswValue must not be null and must be numeric
      if (row.tswValue === null || row.tswValue === undefined) {
        errors.push({ row: i, message: `Zeile ${rowNum}: tswValue darf nicht leer sein`, type: 'error' });
      } else if (typeof row.tswValue !== 'number' || !Number.isFinite(row.tswValue)) {
        errors.push({ row: i, message: `Zeile ${rowNum}: tswValue muss eine gültige Zahl sein`, type: 'error' });
      }

      // Rule: rangeMin must not be null and must be numeric
      if (row.rangeMin === null || row.rangeMin === undefined) {
        errors.push({ row: i, message: `Zeile ${rowNum}: rangeMin darf nicht leer sein`, type: 'error' });
      } else if (typeof row.rangeMin !== 'number' || !Number.isFinite(row.rangeMin)) {
        errors.push({ row: i, message: `Zeile ${rowNum}: rangeMin muss eine gültige Zahl sein`, type: 'error' });
      }

      // Rule: rangeMax must not be null and must be numeric
      if (row.rangeMax === null || row.rangeMax === undefined) {
        errors.push({ row: i, message: `Zeile ${rowNum}: rangeMax darf nicht leer sein`, type: 'error' });
      } else if (typeof row.rangeMax !== 'number' || !Number.isFinite(row.rangeMax)) {
        errors.push({ row: i, message: `Zeile ${rowNum}: rangeMax muss eine gültige Zahl sein`, type: 'error' });
      }

      // Rule 5: Same row rangeMax > rangeMin
      if (row.rangeMax <= row.rangeMin) {
        errors.push({ row: i, message: `Zeile ${rowNum}: rangeMax (${row.rangeMax}) muss größer sein als rangeMin (${row.rangeMin})`, type: 'error' });
      }

      if (i > 0) {
        const prevRow = rows[i - 1];

        // Rule 1a: tswValue ascending
        if (row.tswValue !== null && prevRow.tswValue !== null && row.tswValue <= prevRow.tswValue) {
          errors.push({ row: i, message: `Zeile ${rowNum}: tswValue (${row.tswValue}) muss größer sein als Zeile ${i} (${prevRow.tswValue})`, type: 'error' });
        }

        // Rule 1b: rangeMin ascending
        if (row.rangeMin <= prevRow.rangeMin) {
          errors.push({ row: i, message: `Zeile ${rowNum}: rangeMin (${row.rangeMin}) muss größer sein als Zeile ${i} (${prevRow.rangeMin})`, type: 'error' });
        }

        // Rule 1c: rangeMax ascending
        if (row.rangeMax <= prevRow.rangeMax) {
          errors.push({ row: i, message: `Zeile ${rowNum}: rangeMax (${row.rangeMax}) muss größer sein als Zeile ${i} (${prevRow.rangeMax})`, type: 'error' });
        }

        // Rule 4: No overlapping intervals
        if (row.rangeMin <= prevRow.rangeMax) {
          errors.push({ row: i, message: `Zeile ${rowNum}: rangeMin (${row.rangeMin}) muss größer sein als rangeMax von Zeile ${i} (${prevRow.rangeMax})`, type: 'error' });
        }
      }
    }

    return errors;
  }

  rowHasError(rowIndex: number): boolean {
    return this.notchValidationErrors().some((e: { row?: number; message: string; type: 'error' | 'warning' }) => e.row === rowIndex);
  }

  // ----------------------------
  // Helpers (normalize TSW differences)
  // ----------------------------

  private normalizeListResponse(resp: TswListResponse): TswListResponse {
    // Ensure NodePath in response is normalized (remove "Root/" prefix if present)
    const nodePath = (resp.NodePath ?? '').toString();
    const normalized = this.stripRootPrefix(nodePath);
    return { ...resp, NodePath: normalized };
  }

  private extractUiNodes(resp: TswListResponse): UiNode[] {
    const rawNodes: any[] = (resp as any).Nodes ?? [];

    // Root-level uses NodeName/NodePath; other levels use { Name }
    const ui: UiNode[] = rawNodes
      .map(n => {
        const nodeName: string | undefined =
          typeof n?.NodeName === 'string' ? n.NodeName :
          typeof n?.Name === 'string' ? n.Name :
          undefined;

        const nodePath: string | undefined =
          typeof n?.NodePath === 'string' ? n.NodePath : undefined;

        if (!nodeName) return null;

        // If NodePath exists (root-style), derive nextPath from it
        // Example: "Root/CurrentDrivableActor" -> "CurrentDrivableActor"
        // For deeper: "Root/CurrentDrivableActor/BrakePipe_B" -> "CurrentDrivableActor/BrakePipe_B"
        let nextPath: string;
        if (nodePath) {
          nextPath = this.stripRootPrefix(nodePath);
        } else {
          // non-root list response: Node is just a name, so append to current path
          const base = this.currentNodePath();
          nextPath = base ? `${base}/${nodeName}` : nodeName;
        }

        return { displayName: nodeName, nextPath };
      })
      .filter(Boolean) as UiNode[];

    return ui;
  }

  private extractSegmentsAfterList(url: string): string[] {
    // url starts with "/list" or something else
    const clean = url.split('?')[0].split('#')[0];
    const parts = clean.split('/').filter(Boolean);

    const listIndex = parts.indexOf('list');
    if (listIndex === -1) return [];

    const after = parts.slice(listIndex + 1).map(s => decodeURIComponent(s));

    // Remove "Root" if someone manually entered it
    if (after.length > 0 && after[0] === 'Root') {
      return after.slice(1);
    }
    return after;
  }

  private stripRootPrefix(path: string): string {
    if (!path) return '';
    if (path === 'Root') return '';
    if (path.startsWith('Root/')) return path.slice('Root/'.length);
    return path;
  }

  private extractReturnValueNumber(resp: TswGetResponse | null): number | null {
    if (!resp) return null;

    // common shapes:
    // { Values: { ReturnValue: 1 } }
    // { Values: { InputValue: 0 } }
    // { Value: 1 }
    // { Values: { Something: 1 } }  (we try ReturnValue first, then first numeric value)
    const anyResp: any = resp as any;

    const rv = anyResp?.Values?.ReturnValue;
    if (typeof rv === 'number') return rv;

    const v = anyResp?.Value;
    if (typeof v === 'number') return v;

    // Some endpoints might return numeric strings
    if (typeof rv === 'string') {
      const num = Number(rv);
      if (Number.isFinite(num)) return num;
    }
    if (typeof v === 'string') {
      const num = Number(v);
      if (Number.isFinite(num)) return num;
    }

    // Fallback: check all values in Values object for the first numeric value
    const values = anyResp?.Values;
    if (values && typeof values === 'object') {
      for (const key of Object.keys(values)) {
        const val = values[key];
        if (typeof val === 'number') return val;
        if (typeof val === 'string') {
          const num = Number(val);
          if (Number.isFinite(num)) return num;
        }
      }
    }

    return null;
  }

  private formatError(err: any): string {
    if (!err) return 'Unknown error';
    if (typeof err === 'string') return err;
    if (err?.message) return err.message;
    try {
      return JSON.stringify(err);
    } catch {
      return 'Request failed';
    }
  }
}
