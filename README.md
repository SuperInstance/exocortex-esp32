# Exocortex ESP32 Sensor Node

An Arduino/PlatformIO sketch for ESP32 that reads analog sensors and communicates with an [Exocortex](https://github.com/SuperInstance/exocortex) server.

## Features

- **Sense**: Reads temperature (GPIO36) and soil moisture (GPIO34), POSTs to `/tap/sense` every 30s
- **Recall**: GETs `/tap/recall?q=<query>` to retrieve stored memories
- **Predict**: GETs `/tap/predict?sensor=<name>&reading=<value>` for predictions
- Plain-text protocol (no JSON parsing needed on the microcontroller)

## Wiring

Defaults use ADC1 pins on a typical ESP32 dev board. Change `TEMP_PIN` and
`MOIST_PIN` in `src/main.cpp` if your board differs.

```
ESP32          Sensor
─────          ──────
GPIO36  ←────  TMP36 Vout (middle pin)
GPIO34  ←────  Capacitive Soil Moisture (AOUT)
3V3     ────→  Both sensor VCC
GND     ────→  Both sensor GND

TMP36 pinout (flat side facing you):
  Left: VCC (3.3V)
  Middle: Vout → GPIO36
  Right: GND

Capacitive Soil Moisture:
  VCC → 3.3V
  GND → GND
  AOUT → GPIO34
```

## Configuration

### Option 1: Edit source
Change the defines at the top of `src/main.cpp`:
```cpp
#define WIFI_SSID "YourNetwork"
#define WIFI_PASS "YourPassword"
#define CORTEX_URL "http://192.168.1.100:9000"
```

### Option 2: PlatformIO build flags (recommended)
Edit `platformio.ini`:
```ini
build_flags =
  -DWIFI_SSID=\"YourNetwork\"
  -DWIFI_PASS=\"YourPassword\"
  -DCORTEX_URL=\"http://your-server:9000\"
```

## Build & Flash

### PlatformIO CLI
```bash
pio run -t upload
pio device monitor
```

### PlatformIO IDE (VS Code)
1. Open this folder
2. Edit `platformio.ini` with your WiFi credentials
3. Click Upload → Serial Monitor

## API Protocol

### POST /tap/sense
**Body** (text/plain): `t:28.5 h:62 z:3`
- `t` = temperature (°C)
- `h` = humidity/moisture (%)
- `z` = zone ID

### GET /tap/recall?q=text
**Response** (text/plain): matching memory fragments

### GET /tap/predict?sensor=x&reading=1.0
**Response** (text/plain): predicted value

## Continuous Integration

CI runs on every push/PR to `main`/`master`:

- `clang-format --dry-run --Werror src/main.cpp`
- `pio run`

The previous workflow was a no-op placeholder that always passed; it now
actually builds the firmware.

## Size

> ⚠️ The previous README claimed "under 20KB compiled"; that was incorrect.
> The ESP32 Arduino framework + WiFi/HTTPClient produce a much larger image.

Observed build footprint (release build, `esp32dev`):

- Flash: ~800 KB
- RAM: ~46 KB

The application sketch itself is small; most of the footprint is the
ESP32 Arduino core, WiFi, and HTTPClient libraries. No external JSON
libraries are required.
