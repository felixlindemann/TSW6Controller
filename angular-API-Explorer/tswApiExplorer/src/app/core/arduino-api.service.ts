import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class ArduinoApiService {
  private readonly baseUrl = '/api'; // relative URL für API

  constructor(private http: HttpClient) {}

  // GET /api/status
  getStatus(): Observable<any> {
    return this.http.get(`${this.baseUrl}/status`);
  }

  // GET /api/controls
  getControls(): Observable<any> {
    return this.http.get(`${this.baseUrl}/controls`);
  }

  // GET /api/controls/summary
  getControlsSummary(): Observable<any> {
    return this.http.get(`${this.baseUrl}/controls/summary`);
  }

  // GET /api/controls/values
  getControlValues(): Observable<any> {
    return this.http.get(`${this.baseUrl}/controls/values`);
  }

  // GET /api/controls/{name}
  getControl(name: string): Observable<any> {
    return this.http.get(`${this.baseUrl}/controls/${encodeURIComponent(name)}`);
  }

  // PUT /api/controls/{name}
  updateControl(name: string, data: any): Observable<any> {
    return this.http.put(`${this.baseUrl}/controls/${encodeURIComponent(name)}`, data);
  }

  // POST /api/reboot
  reboot(): Observable<any> {
    return this.http.post(`${this.baseUrl}/reboot`, {});
  }

  // GET /api/config
  getConfig(): Observable<any> {
    return this.http.get(`${this.baseUrl}/config`);
  }

  // PUT /api/config
  updateConfig(data: any): Observable<any> {
    return this.http.put(`${this.baseUrl}/config`, data);
  }
}
