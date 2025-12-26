export interface NotchTable {
  // Struktur ggf. anpassen, falls Details bekannt
  [key: string]: any;
}

export interface TSWControl {
  controllerName: string;
  type: string;
  hardwareType: string;
  lastSentValue: number;
  notches: NotchTable;
  // ggf. weitere Felder je nach Control-Typ
}
