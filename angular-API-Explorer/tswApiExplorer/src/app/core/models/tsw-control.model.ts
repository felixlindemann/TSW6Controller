
// Position eines Notches (aus NotchTable)
export interface NotchPosition {
  label: string;
  tsw: number;
  range: [number, number];
}

// NotchTable wie von Arduino geliefert
export interface NotchTable {
  controller: string;
  label: string;
  inverted: boolean;
  positions: NotchPosition[];
}

// Basis-Interface für alle Controls (mit id)
export interface TSWControlBase {
  id: string;
  type: string;
  hardwareType: string;
  lastSentValue: number;
}

// TSWButton
export interface TSWButton extends TSWControlBase {
  type: 'TSWButton';
  hardwareType: 'GPIO';
  notches: NotchTable;
  pin: number;
  isPressed: boolean;
}

// TSWLever
export interface TSWLever extends TSWControlBase {
  type: 'TSWLever';
  hardwareType: 'AnalogSlider';
  notches: NotchTable;
  pin: number;
  currentPercent: number;
  currentRaw: number;
  inverted: boolean;
}

// TSWRotaryKnob
export interface TSWRotaryKnob extends TSWControlBase {
  type: 'TSWRotaryKnob';
  hardwareType: 'RotaryEncoder';
  notches: NotchTable;
  pinA: number;
  pinB: number;
  minValue: number;
  maxValue: number;
  currentTSWValue: number;
}

// TSWMCPButton
export interface TSWMCPButton extends TSWControlBase {
  type: 'TSWMCPButton';
  hardwareType: 'MCP23S17';
  notches: NotchTable;
  buttonIndex: number;
  isPressed: boolean;
}

// TSWGamePadControl
export interface TSWGamePadControl extends TSWControlBase {
  type: 'TSWGamePadControl';
  hardwareType: 'GamepadJoystick';
  axes: {
    x: {
      notches: NotchTable;
    };
    y: {
      notches: NotchTable;
    };
    button: {
      notches: NotchTable;
    };
  };
  xInverted: boolean;
  yInverted: boolean;
}

// Union-Type für alle Controls
export type TSWControl = TSWButton | TSWLever | TSWRotaryKnob | TSWMCPButton | TSWGamePadControl;
