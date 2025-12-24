import { Injectable } from '@angular/core';
import { HttpClient, HttpParams } from '@angular/common/http';
import { Observable, map, catchError, of } from 'rxjs';
import type { TswGetResponse, TswListResponse } from './tsw-api.models';

@Injectable({ providedIn: 'root' })
export class TswApiService {
  // If you use Angular proxy.conf.json, you can change this to '/api'
  private readonly baseUrl = 'http://10.10.78.96:31270';

  constructor(private readonly http: HttpClient) {}

  /**
   * GET /list or /list/{nodePath}
   * nodePath is expected WITHOUT "Root/" and WITHOUT leading/trailing slashes.
   */
  list(nodePath: string | null): Observable<TswListResponse> {
    const cleaned = this.normalizeNodePath(nodePath);

    const url =
      cleaned.length > 0
        ? `${this.baseUrl}/list/${this.encodePathPreserveSlashes(cleaned)}`
        : `${this.baseUrl}/list`;

    return this.http.get<TswListResponse>(url).pipe(
      map((res) => ({
        ...res,
        NodePath: this.stripRootPrefix(res.NodePath ?? ''),
      }))
    );
  }

  /**
   * GET /get/{nodePath}.{endpoint}
   * nodePath is WITHOUT "Root/"
   */
  getEndpoint(nodePath: string, endpointName: string): Observable<TswGetResponse> {
    const cleanedPath = this.normalizeNodePath(nodePath);
    const cleanedEndpoint = (endpointName ?? '').trim();

    if (!cleanedPath) throw new Error('TswApiService.getEndpoint(): nodePath is empty.');
    if (!cleanedEndpoint) throw new Error('TswApiService.getEndpoint(): endpointName is empty.');

    const url = `${this.baseUrl}/get/${this.encodePathPreserveSlashes(cleanedPath)}.${encodeURIComponent(cleanedEndpoint)}`;
    return this.http.get<TswGetResponse>(url);
  }

  /**
   * PATCH /set/{nodePath}.{endpoint}?Value={value}
   * IMPORTANT: API expects URL param (no body form-data).
   *
   * Returns boolean ok to match your component logic.
   */
  setEndpoint(nodePath: string, endpointName: string, value: unknown): Observable<boolean> {
    const cleanedPath = this.normalizeNodePath(nodePath);
    const cleanedEndpoint = (endpointName ?? '').trim();

    if (!cleanedPath) throw new Error('TswApiService.setEndpoint(): nodePath is empty.');
    if (!cleanedEndpoint) throw new Error('TswApiService.setEndpoint(): endpointName is empty.');

    const url = `${this.baseUrl}/set/${this.encodePathPreserveSlashes(cleanedPath)}.${encodeURIComponent(cleanedEndpoint)}`;

    const params = new HttpParams().set('Value', this.valueToQueryString(value));

    return this.http.patch<unknown>(url, null, { params }).pipe(
      map(() => true),
      catchError(() => of(false))
    );
  }

  // -----------------------------
  // Helpers
  // -----------------------------

  private normalizeNodePath(nodePath: string | null | undefined): string {
    const cleaned = (nodePath ?? '').trim().replace(/^\/+|\/+$/g, '');
    return this.stripRootPrefix(cleaned);
  }

  private stripRootPrefix(path: string): string {
    const p = (path ?? '').trim().replace(/^\/+|\/+$/g, '');
    if (p === 'Root') return '';
    if (p.startsWith('Root/')) return p.substring('Root/'.length);
    return p;
  }

  /**
   * encodeURIComponent would encode "/" to "%2F".
   * We explicitly want to preserve slashes in paths: "A/B" stays "A/B".
   */
  private encodePathPreserveSlashes(path: string): string {
    return encodeURIComponent(path).replace(/%2F/g, '/');
  }

  private valueToQueryString(value: unknown): string {
    if (value === null) return 'null';
    if (value === undefined) return '';
    if (typeof value === 'string') return value;
    if (typeof value === 'number' || typeof value === 'boolean') return String(value);

    try {
      return JSON.stringify(value);
    } catch {
      return String(value);
    }
  }
}
