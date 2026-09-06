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

Seven **numbered screen buttons** appear under **SELECT SCREEN** (large buttons — not small tabs). Click a button to switch the view below:

1. **VFD inputs** — I024–I039 (start here for drive DI)
2. **Panel inputs** — I000–I023
3. **All I/O** — combined 80 points
4. **Outputs** — O000–O039
5. **Local / Remote** — highlighted field diagnosis
6. **Write** — M000–M039 (FC05 writes)
7. **Status** — M040–M099 (read-only)

The active screen is **blue**; others are grey. Hover a button for a short description.

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

---

## Customer deployment (recommended)

Send the customer **one file**:

`installer\output\WF_Modbus_Diag_Setup.exe`

They double-click it, accept defaults, and get:

- Installed app under **Program Files\WF Lloyds\Modbus Diagnostic**
- Start menu shortcut **WF Lloyds Modbus Diagnostic**
- Optional desktop shortcut (checkbox during install)
- Uninstall via Windows **Apps & features**

**No Python install required** on the customer PC.

### Connection reminder for customers

- USB-RS485 to **AO module SIO** (Modbus), not the VFD RS485 bus
- 19200 8E1, slave ID **1**
- Controller menu: SIO Interface = **Modbus**

---

## Developer: build the installer

On a Windows PC with **Python 3** and **Inno Setup 6** ([download](https://jrsoftware.org/isinfo.php)):

1. Open a command prompt in `tools\wf_modbus_diag`
2. Run **`Build_Installer.bat`**

The script will:

1. Install PyInstaller (via pip)
2. Build `dist\WF_Modbus_Diag\WF_Modbus_Diag.exe` (bundled `data\` CSV tag maps)
3. Compile `installer\output\WF_Modbus_Diag_Setup.exe`

If Inno Setup is not installed, the standalone folder in `dist\WF_Modbus_Diag\` is still produced — zip it for manual deployment.

Manual PyInstaller command (equivalent to the spec file):

```bat
pyinstaller --noconfirm --clean wf_modbus_diag.spec
```

Manual Inno Setup compile:

```bat
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\WF_Modbus_Diag_Setup.iss
```
