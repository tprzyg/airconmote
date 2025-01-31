export interface LightState {
  led_on: boolean;
}

export interface LightMqttSettings {
  unique_id: string;
  name: string;
  mqtt_path: string;
}

export interface AirConState {
    onoff: boolean;
    mode: string;
    temperature: number;
    flowspeed: number;
    verticalswing: boolean;
    sensor_temp_inside: number;
    sensor_temp_outside: number;
    registers: Array<Array<number>>;
}

export interface AirConMqttSettings {
    unique_id: string;
    name: string;
    mqtt_path: string;
}
