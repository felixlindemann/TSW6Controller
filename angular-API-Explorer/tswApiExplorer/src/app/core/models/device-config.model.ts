export interface WifiConfig {
  ssid: string;
  password: string;
  apModePreferred: boolean;
  apCheckInterval: number;
  heartbeatInterval: number;
}

export interface ServerConfig {
  host: string;
  port: number;
  apiKey: string;
}

export interface DeviceConfig {
  name: string;
  mdnsName: string;
}

export interface DeviceConfigRoot {
  wifi: WifiConfig;
  server: ServerConfig;
  device: DeviceConfig;
}
