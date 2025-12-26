export interface DeviceStatus {
  apMode: boolean;
  mode: 'AP' | 'Client';
  ip: string;
  ssid: string;
  rssi: number;
  mdns: string;
  deviceName: string;
  freeHeap: number;
  uptime: number;
}
