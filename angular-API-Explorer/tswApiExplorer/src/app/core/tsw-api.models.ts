export interface TswEndpoint {
  Name: string;
  Writable: boolean;
}

// Root-level node (returned by GET /list)
export interface TswRootNode {
  NodePath: string;          // e.g. "Root/Timetable/..."
  NodeName: string;          // e.g. "Timetable" or "6C6D..."
  CollapsedChildren?: number;
  Nodes?: TswRootNode[];     // sometimes nested preview nodes
}

// Child-level node (returned by GET /list/<somePath>)
export interface TswChildNode {
  Name: string;              // e.g. "6C6D..."
}

export type TswNode = TswRootNode | TswChildNode;

export interface TswListResponse {
  Result: string;
  NodePath: string;          // e.g. "Root" or "CurrentDrivableActor"
  NodeName: string;
  Nodes?: TswNode[];
  Endpoints?: TswEndpoint[];
}

export interface TswGetResponse {
  Result: string;
  Value?: unknown;
  Values?: unknown;
  [key: string]: unknown;
}

/**
 * Normalized node used by the UI (always the same shape)
 */
export interface UiNode {
  name: string;              // display name
  path: string;              // absolute or relative path we can navigate to
  collapsedChildren?: number;
  hasPreviewChildren?: boolean;
}
