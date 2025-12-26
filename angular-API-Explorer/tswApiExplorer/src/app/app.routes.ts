import { Routes, UrlMatchResult, UrlSegment } from '@angular/router'; 
import { ExplorerPageComponent } from './explorer-page/explorer-page.component';
import { WifiConfigEditorComponent } from './wifi-config-editor/wifi-config-editor.component';
import { ServerConfigEditorComponent } from './server-config-editor/server-config-editor.component'; 
import { DnsConfigEditorComponent } from './device-config-editor';
import { LandingPageComponent } from './landing-page/landing-page.component';

function listMatcher(segments: UrlSegment[]): UrlMatchResult | null {
  // Match:
  //  - /list
  //  - /list/anything/with/slashes
  if (segments.length === 0) return null;
  if (segments[0].path !== 'list') return null;

  const rest = segments.slice(1).map(s => s.path).join('/');
  return {
    consumed: segments,
    posParams: {
      nodePath: new UrlSegment(rest, {})
    }
  };
}

export const routes: Routes = [
  { path: '', pathMatch: 'full', component: LandingPageComponent },
  { path: 'home', component: LandingPageComponent },

  // WifiConfig Editor
  { path: 'wifi-config', component: WifiConfigEditorComponent },

  // ServerConfig Editor
  { path: 'server-config', component: ServerConfigEditorComponent },

  // DNSConfig Editor
  { path: 'dns-config', component: DnsConfigEditorComponent },

  // /list (no nodePath)
  { path: 'list', component: ExplorerPageComponent },

  // /list/** (capture everything after list into param "nodePath")
  { matcher: listMatcher, component: ExplorerPageComponent }
];
