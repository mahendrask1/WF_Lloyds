# WF Lloyds Modbus Diagnostic (GUI)

Desktop tool to poll and write Modbus bits on the **AO module SIO port** via USB-RS485.

## Requirements

- Python 3.8+
- USB-RS485 adapter (connected to AO module **SIO / Modbus** terminals, not the VFD bus)

## Install & run

**Easiest:** Double-click **`WF_Modbus_Diag.bat`** or the desktop shortcut **WF Modbus Diag**.

**No pip install needed on Windows** — the tool uses built-in serial support.

**Requires:** Python 3 from [python.org](https://www.python.org/downloads/) with:
- Add python.exe to PATH
- tcl/tk and IDLE (for the GUI)

Disable the fake Windows Store python alias:  
Settings → Apps → Advanced app settings → App execution aliases → turn OFF `python.exe` and `python3.exe`.

## Connection defaults (match firmware)

| Setting | Default |
|---------|---------|
| Baud | 19200 |
| Parity | Even (8E1) |
| Slave ID | 1 |
| Menu | SIO Interface = **Modbus** |

## GUI tabs

1. **Writable markers (M000-M039)** — Live readback, write checkbox, **Write** button per row
2. **Status markers (M040-M099)** — Read-only (FC02)
3. **Digital inputs (I000-I039)** — Read-only; **I024-I029** = VFD inputs
4. **Output status (O000-O039)** — Read-only
5. **Local/Remote focus** — Highlighted diagnostic signals

Yellow rows = key local/remote signals (I024-I027, M008, M011, M051, M066, etc.).

## Local/remote field test

1. Connect, enable **Auto poll** (1 s).
2. Open **Local/Remote focus** tab.
3. Toggle field switches on ACS580 wiring:
   - **I024** should follow local/remote (default PLC → **M008**)
   - **I027** should follow field start (default PLC → **M011**)
4. Watch **M051** (local status) and **M066** (WF run).

If **I024/I027** do not change when switches move, the VFD Modbus DI mapping needs fixing on the drive or in firmware.

## Writes

Uses Modbus **FC05** (Write Single Coil) for M000-M039 only. Use with care on a running machine.
