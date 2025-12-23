import { CommonModule } from '@angular/common';
import { Component, OnDestroy, OnInit, computed, signal } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { ActivatedRoute, Router, RouterLink } from '@angular/router';
import { Subscription, forkJoin } from 'rxjs';
import { finalize } from 'rxjs/operators';

import { TswApiService } from '../core/tsw-api.service';
import { TswEndpoint, TswGetResponse, TswListResponse, UiNode } from '../core/tsw-api.models';

@Component({
  selector: 'app-explorer-page',
  standalone: true,
  imports: [CommonModule, FormsModule, RouterLink],
  templateUrl: './explorer-page.component.html'
})
export class ExplorerPageComponent implements OnInit, OnDestroy {
  // UI state
  readonly loading = signal(false);
  readonly error = signal<string | null>(null);

  // Raw API results (for showing GET/SET results)
  readonly listResponse = signal<TswListResponse | null>(null);
  readonly selectedEndpoint = signal<TswEndpoint | null>(null);
  readonly endpointResponse = signal<TswGetResponse | null>(null);

  // Normalized nodes for the UI (works for both /list shapes)
  readonly nodes = signal<UiNode[]>([]);

  // Filters
  readonly nodeFilter = signal('');
  readonly endpointFilter = signal('');

  // DTG key input
  readonly commKey = signal(localStorage.getItem('tsw_dtg_comm_key') ?? '');

  // Current node path parsed from URL (/list/<nodePath...>)
  readonly currentNodePath = signal<string>('');

  // Write form state
  readonly writeValue = signal<string>('');
  readonly writing = signal(false);
  readonly writeError = signal<string | null>(null);
  readonly writeResponse = signal<unknown | null>(null);

  // Slider meta (only for InputValue + min/max/notches available)
  readonly sliderEnabled = signal(false);
  readonly sliderMin = signal<number>(0);
  readonly sliderMax = signal<number>(1);
  readonly sliderNotchCount = signal<number>(0);
  readonly sliderStep = signal<number>(0.01);

  private routeSub?: Subscription;

  // Breadcrumbs
  readonly breadcrumbs = computed(() => {
    const path = this.currentNodePath().trim().replace(/^\/+|\/+$/g, '');
    const parts = path.split('/').filter(Boolean);

    const crumbs: { label: string; link: any[] }[] = [{ label: 'list', link: ['/list'] }];
    if (!parts.length) return crumbs;

    let acc: string[] = [];
    for (const p of parts) {
      acc = [...acc, p];
      crumbs.push({ label: p, link: ['/list', ...acc] });
    }
    return crumbs;
  });

  readonly filteredNodes = computed(() => {
    const q = this.nodeFilter().trim().toLowerCase();
    const all = this.nodes();
    if (!q) return all;
    return all.filter(n => n.name.toLowerCase().includes(q));
  });

  readonly filteredEndpoints = computed(() => {
    const r = this.listResponse();
    const q = this.endpointFilter().trim().toLowerCase();
    const eps = r?.Endpoints ?? [];
    if (!q) return eps;
    return eps.filter(e => e.Name.toLowerCase().includes(q));
  });

  constructor(
    private readonly route: ActivatedRoute,
    private readonly router: Router,
    private readonly api: TswApiService
  ) { }

  ngOnInit(): void {
    // IMPORTANT: subscribe to param changes (same component instance, different nodePath)
    this.routeSub = this.route.paramMap.subscribe(pm => {
      const raw = pm.get('nodePath') ?? '';
      const decoded = decodeURIComponent(raw).trim();
      this.currentNodePath.set(decoded);

      this.resetEndpointPanel();
      this.loadList();
    });
  }

  ngOnDestroy(): void {
    this.routeSub?.unsubscribe();
  }

  saveKey(): void {
    localStorage.setItem('tsw_dtg_comm_key', this.commKey().trim());
  }

  clearKey(): void {
    localStorage.removeItem('tsw_dtg_comm_key');
    this.commKey.set('');
  }

  private encodePathForUrl(path: string): string {
    // Encode each segment, keep "/" as separator
    return path
      .split('/')
      .filter(Boolean)
      .map(seg => encodeURIComponent(seg))
      .join('/');
  }

  navigateToPath(nodePath: string): void {
    const cleaned = (nodePath ?? '').trim().replace(/^\/+|\/+$/g, '');
    if (!cleaned) {
      this.router.navigate(['/list']);
      return;
    }
    this.router.navigateByUrl(`/list/${this.encodePathForUrl(cleaned)}`);
  }

  loadList(): void {
    this.loading.set(true);
    this.error.set(null);

    const nodePath = this.currentNodePath().trim();
    const arg = nodePath.length ? nodePath : null;

    this.api.list(arg).pipe(
      finalize(() => this.loading.set(false))
    ).subscribe({
      next: ({ response, nodes }) => {
        this.listResponse.set(response);
        this.nodes.set(nodes);
      },
      error: (err) => {
        this.error.set(this.formatError(err));
        this.listResponse.set(null);
        this.nodes.set([]);
      }
    });
  }

  readEndpoint(ep: TswEndpoint): void {
    const nodePath = this.currentNodePath().trim();

    if (!nodePath) {
      this.error.set('No current node path. Navigate into a node first.');
      return;
    }

    this.loading.set(true);
    this.error.set(null);

    // select + reset write UI each time user chooses an endpoint
    this.selectedEndpoint.set(ep);
    this.endpointResponse.set(null);
    this.writeValue.set('');
    this.writeError.set(null);
    this.writeResponse.set(null);

    // slider defaults off until we know
    this.resetSlider();

    this.api.get(nodePath, ep.Name).pipe(
      finalize(() => this.loading.set(false))
    ).subscribe({
      next: (res: TswGetResponse) => {
        this.endpointResponse.set(res);

        // If it qualifies, fetch slider meta and enable slider
        this.tryEnableInputValueSlider(ep);
      },
      error: (err) => this.error.set(this.formatError(err))
    });
  }

  writeEndpoint(): void {
    const ep = this.selectedEndpoint();
    const nodePath = this.currentNodePath().trim();

    if (!ep) {
      this.writeError.set('No endpoint selected.');
      return;
    }
    if (!ep.Writable) {
      this.writeError.set('Endpoint is read-only.');
      return;
    }
    if (!nodePath) {
      this.writeError.set('No current node path.');
      return;
    }

    this.writing.set(true);
    this.writeError.set(null);
    this.writeResponse.set(null);

    const parsed = this.parseUserValue(this.writeValue());

    this.api.set(nodePath, ep.Name, parsed).subscribe({
      next: (setRes) => {
        // 1) show SET response
        this.writeResponse.set(setRes);

        // 2) then refresh GET of same endpoint
        this.api.get(nodePath, ep.Name).subscribe({
          next: (getRes) => this.endpointResponse.set(getRes),
          error: (err) => this.writeError.set(this.formatError(err)),
          complete: () => this.writing.set(false)
        });
      },
      error: (err) => {
        this.writeError.set(this.formatError(err));
        this.writing.set(false);
      }
    });
  }

onSliderChanged(raw: string): void {
  const v = Number(raw);
  if (!Number.isFinite(v)) {
    this.writeValue.set(raw);
    return;
  }

  const step = this.sliderStep();
  const snapped = this.sliderEnabled() ? this.roundToStep(v, step) : v;
  this.writeValue.set(snapped.toString());
}

refreshSelectedEndpoint(): void {
  const ep = this.selectedEndpoint();
  const nodePath = this.currentNodePath().trim();

  if (!ep) return;
  if (!nodePath) {
    this.error.set('No current node path.');
    return;
  }

  this.loading.set(true);
  this.error.set(null);

  this.api.get(nodePath, ep.Name).pipe(
    finalize(() => this.loading.set(false))
  ).subscribe({
    next: (res: TswGetResponse) => {
      this.endpointResponse.set(res);

      // If slider is enabled, jump slider/value to the current GET value
      const current = this.extractNumber(res);
      if (this.sliderEnabled() && Number.isFinite(current)) {
        const min = this.sliderMin();
        const max = this.sliderMax();
        const step = this.sliderStep();

        const clamped = Math.min(max, Math.max(min, current));
        const snapped = this.roundToStep(clamped, step);

        this.writeValue.set(snapped.toString());
      } else if (Number.isFinite(current)) {
        // If no slider, at least fill the input field with the current value
        this.writeValue.set(String(current));
      }
    },
    error: (err) => this.error.set(this.formatError(err))
  });
}

  private tryEnableInputValueSlider(ep: TswEndpoint): void {
    // Conditions:
    // - endpoint writable
    // - endpoint name contains "InputValue" (covers "Property....InputValue" and similar)
    // - meta endpoints exist in this node's endpoint list
    if (!ep.Writable) return;
    if (!ep.Name.includes('InputValue')) return;

    const eps = this.listResponse()?.Endpoints ?? [];
    const hasMin = eps.some(e => e.Name === 'Function.GetMinimumInputValue');
    const hasMax = eps.some(e => e.Name === 'Function.GetMaximumInputValue');
    const hasNotches = eps.some(e => e.Name === 'Function.GetNotchCount');
    if (!(hasMin && hasMax && hasNotches)) return;

    const nodePath = this.currentNodePath().trim();
    if (!nodePath) return;

    // Fetch meta in parallel
    forkJoin({
      min: this.api.get(nodePath, 'Function.GetMinimumInputValue'),
      max: this.api.get(nodePath, 'Function.GetMaximumInputValue'),
      notches: this.api.get(nodePath, 'Function.GetNotchCount')
    }).subscribe({
      next: ({ min, max, notches }) => {
        const minNum = this.extractNumber(min);
        const maxNum = this.extractNumber(max);
        const notchCount = Math.max(0, Math.floor(this.extractNumber(notches))); 

        if (!Number.isFinite(minNum) || !Number.isFinite(maxNum) || maxNum === minNum) return;

        // Step:
        // - If notchCount >= 2: step = (max-min)/(notchCount-1)
        // - Else: small continuous step
        const step = (notchCount >= 2)
          ? (maxNum - minNum) / (notchCount - 1)
          : (maxNum - minNum) / 100;

        this.sliderMin.set(minNum);
        this.sliderMax.set(maxNum);
        this.sliderNotchCount.set(notchCount);
        this.sliderStep.set(this.safeStep(step));
        this.sliderEnabled.set(true);

        // Initialize writeValue from current GET value if numeric, else min
        const currentVal = this.extractNumber(this.endpointResponse());
        const initial = Number.isFinite(currentVal) ? currentVal : minNum;

        // Clamp into range and round to step
        const clamped = Math.min(maxNum, Math.max(minNum, initial));
        const rounded = this.roundToStep(clamped, this.sliderStep());
this.writeValue.set(rounded.toString());
         
      },
      error: () => {
        // If meta fails, just keep slider off (no noisy error)
        this.resetSlider();
      }
    });
  }

  private resetEndpointPanel(): void {
    this.selectedEndpoint.set(null);
    this.endpointResponse.set(null);
    this.writeValue.set('');
    this.writeError.set(null);
    this.writeResponse.set(null);
    this.resetSlider();
  }

  private resetSlider(): void {
    this.sliderEnabled.set(false);
    this.sliderMin.set(0);
    this.sliderMax.set(1);
    this.sliderNotchCount.set(0);
    this.sliderStep.set(0.01);
  }

  private parseUserValue(input: string): unknown {
    const s = (input ?? '').trim();
    if (s.length === 0) return '';

    // allow raw JSON values: numbers, booleans, objects, arrays, quoted strings, null
    try {
      return JSON.parse(s);
    } catch {
      // fallback: send as plain string
      return s;
    }
  }
private extractNumber(res: unknown): number {
  const r: any = res ?? {};

  // common shapes:
  // { Value: 0.1 }
  // { Values: { ReturnValue: 0.1 } }
  // { Values: { ReturnValue: "0.1" } }
  const v =
    r?.Value ??
    r?.value ??
    r?.ResultValue ??
    r?.Values?.ReturnValue ??
    r?.values?.ReturnValue;

  const num = typeof v === 'number' ? v : Number(v);
  return Number.isFinite(num) ? num : NaN;
}
private safeStep(step: number): number {
  if (!Number.isFinite(step)) return 0.01;
  const abs = Math.abs(step);
  if (abs <= 0) return 0.01;

  // avoid tiny floating artifacts, keep at least 1e-6 precision
  return Math.max(abs, 1e-6);
}

  private roundToStep(value: number, step: number): number {
    if (!Number.isFinite(value) || !Number.isFinite(step) || step <= 0) return value;
    return Math.round(value / step) * step;
  }

  private formatError(err: any): string {
    if (!err) return 'Unknown error';
    if (typeof err === 'string') return err;

    const status = err.status;

    if (status === 403) return '403 Forbidden – DTGCommKey missing/invalid?';
    if (status === 0) return 'Network error (TSW running? HTTP API enabled? Proxy configured?)';

    const serverMsg = err.error?.Message || err.error?.error || err.error?.Result;
    if (serverMsg) return `HTTP ${status ?? '?'}: ${serverMsg}`;

    return `HTTP ${status ?? '?'}: ${err.message ?? 'Request failed'}`;
  }
}
