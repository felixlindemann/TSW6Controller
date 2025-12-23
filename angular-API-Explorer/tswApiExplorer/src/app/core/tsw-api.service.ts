import { Injectable } from '@angular/core';
import { HttpClient, HttpParams } from '@angular/common/http';
import { map, Observable } from 'rxjs';
import { TswGetResponse, TswListResponse, TswNode, TswRootNode, UiNode } from './tsw-api.models';


@Injectable({ providedIn: 'root' })
export class TswApiService {
  // Using /api to avoid CORS via Angular dev proxy
  private readonly baseUrl = 'http://10.10.78.96:31270';

  constructor(private readonly http: HttpClient) { }

  listRaw(nodePath: string | null): Observable<TswListResponse> {
    const cleaned = this.stripRootPrefix((nodePath ?? '').trim().replace(/^\/+|\/+$/g, ''));
    const url = cleaned.length > 0
      ? `${this.baseUrl}/list/${encodeURIComponent(cleaned).replace(/%2F/g, '/')}`
      : `${this.baseUrl}/list`;

    return this.http.get<TswListResponse>(url);
  }

  /**
   * Returns normalized nodes for the UI and strips "Root/" everywhere.
   */
  list(nodePath: string | null): Observable<{ response: TswListResponse; nodes: UiNode[] }> {
    const current = this.stripRootPrefix((nodePath ?? '').trim().replace(/^\/+|\/+$/g, ''));

    return this.listRaw(nodePath).pipe(
      map((res) => {
        // Also strip Root/ from response NodePath for UI consistency
        const response: TswListResponse = {
          ...res,
          NodePath: this.stripRootPrefix(res.NodePath ?? ''),
        };

        const rawNodes = response.Nodes ?? [];
        const normalized = rawNodes.map(n => this.toUiNode(n, current));

        return { response, nodes: normalized };
      })
    );
  }

  get(nodePath: string, endpointName: string): Observable<TswGetResponse> {
    const cleanedPath = this.stripRootPrefix((nodePath ?? '').trim().replace(/^\/+|\/+$/g, ''));
    const cleanedEndpoint = (endpointName ?? '').trim();

    if (!cleanedPath) throw new Error('TswApiService.get(): nodePath is empty.');
    if (!cleanedEndpoint) throw new Error('TswApiService.get(): endpointName is empty.');

    const encodedPath = encodeURIComponent(cleanedPath).replace(/%2F/g, '/');
    const encodedEndpoint = encodeURIComponent(cleanedEndpoint);

    return this.http.get<TswGetResponse>(`${this.baseUrl}/get/${encodedPath}.${encodedEndpoint}`);
  }

  private toUiNode(node: TswNode, currentPath: string): UiNode {
    // Root-level node shape has NodePath + NodeName
    if (this.isRootNode(node)) {
      const stripped = this.stripRootPrefix(node.NodePath);
      return {
        name: node.NodeName,
        path: stripped, // <-- Root/ removed
        collapsedChildren: node.CollapsedChildren,
        hasPreviewChildren: Array.isArray(node.Nodes) && node.Nodes.length > 0
      };
    }

    // Child-level node shape has only Name (relative)
    const childName = node.Name;
    const path = currentPath ? `${currentPath}/${childName}` : childName;
    return { name: childName, path };
  }

  private isRootNode(node: TswNode): node is TswRootNode {
    return (node as any)?.NodePath !== undefined && (node as any)?.NodeName !== undefined;
  }

  private stripRootPrefix(path: string): string {
    const p = (path ?? '').trim().replace(/^\/+|\/+$/g, '');
    if (p === 'Root') return '';
    if (p.startsWith('Root/')) return p.substring('Root/'.length);
    return p;
  }

  private valueToQueryString(value: unknown): string {
    if (value === null) return 'null';
    if (value === undefined) return '';
    if (typeof value === 'string') return value;
    if (typeof value === 'number' || typeof value === 'boolean') return String(value);

    // objects/arrays: send as JSON string
    try {
      return JSON.stringify(value);
    } catch {
      return String(value);
    }
  }

  set(nodePath: string, endpointName: string, value: unknown) {
    const cleanedPath = this.stripRootPrefix((nodePath ?? '').trim().replace(/^\/+|\/+$/g, ''));
    const cleanedEndpoint = (endpointName ?? '').trim();

    if (!cleanedPath) throw new Error('TswApiService.set(): nodePath is empty.');
    if (!cleanedEndpoint) throw new Error('TswApiService.set(): endpointName is empty.');

    const encodedPath = encodeURIComponent(cleanedPath).replace(/%2F/g, '/');
    const encodedEndpoint = encodeURIComponent(cleanedEndpoint);

    const url = `${this.baseUrl}/set/${encodedPath}.${encodedEndpoint}`;

    // API expects ?Value=...
    const params = new HttpParams().set('Value', this.valueToQueryString(value));

    return this.http.patch<unknown>(url, null, { params });
  }


}
