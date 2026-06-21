"""Modbus RTU over serial — Windows COM via ctypes (no pip). Optional pyserial fallback."""

from __future__ import annotations

import struct
import sys
import time
from typing import List, Optional


def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF


def list_serial_ports() -> List[str]:
    ports: List[str] = []
    if sys.platform == "win32":
        try:
            import winreg

            key = winreg.OpenKey(
                winreg.HKEY_LOCAL_MACHINE, r"HARDWARE\DEVICEMAP\SERIALCOMM"
            )
            i = 0
            while True:
                try:
                    _name, value, _typ = winreg.EnumValue(key, i)
                    if value not in ports:
                        ports.append(value)
                    i += 1
                except OSError:
                    break
            winreg.CloseKey(key)
        except OSError:
            pass
        ports.sort(key=lambda p: (len(p), p))
        return ports

    try:
        from serial.tools import list_ports

        return [p.device for p in list_ports.comports()]
    except ImportError:
        return []


class _Win32Serial:
    """Minimal Windows COM port — stdlib ctypes only."""

    GENERIC_READ = 0x80000000
    GENERIC_WRITE = 0x40000000
    OPEN_EXISTING = 3
    FILE_ATTRIBUTE_NORMAL = 0x80
    INVALID_HANDLE_VALUE = -1

    def __init__(
        self,
        port: str,
        baudrate: int = 19200,
        parity: str = "E",
        stopbits: int = 1,
        bytesize: int = 8,
        timeout: float = 1.0,
    ) -> None:
        import ctypes
        from ctypes import wintypes

        self._ctypes = ctypes
        self.timeout = timeout
        self._handle: Optional[int] = None

        kernel32 = ctypes.windll.kernel32
        com_prefix = "\\\\.\\"
        path = port if port.startswith(com_prefix) else com_prefix + port

        handle = kernel32.CreateFileW(
            path,
            self.GENERIC_READ | self.GENERIC_WRITE,
            0,
            None,
            self.OPEN_EXISTING,
            self.FILE_ATTRIBUTE_NORMAL,
            None,
        )
        if handle == self.INVALID_HANDLE_VALUE or handle is None:
            err = kernel32.GetLastError()
            raise IOError(f"Cannot open {port} (Win32 error {err})")

        self._handle = handle

        class DCB(ctypes.Structure):
            _fields_ = [
                ("DCBlength", wintypes.DWORD),
                ("BaudRate", wintypes.DWORD),
                ("fBinary", wintypes.DWORD, 1),
                ("fParity", wintypes.DWORD, 1),
                ("fOutxCtsFlow", wintypes.DWORD, 1),
                ("fOutxDsrFlow", wintypes.DWORD, 1),
                ("fDtrControl", wintypes.DWORD, 2),
                ("fDsrSensitivity", wintypes.DWORD, 1),
                ("fTXContinueOnXoff", wintypes.DWORD, 1),
                ("fOutX", wintypes.DWORD, 1),
                ("fInX", wintypes.DWORD, 1),
                ("fErrorChar", wintypes.DWORD, 1),
                ("fNull", wintypes.DWORD, 1),
                ("fRtsControl", wintypes.DWORD, 2),
                ("fAbortOnError", wintypes.DWORD, 1),
                ("fDummy2", wintypes.DWORD, 17),
                ("wReserved", wintypes.WORD),
                ("XonLim", wintypes.WORD),
                ("XoffLim", wintypes.WORD),
                ("ByteSize", wintypes.BYTE),
                ("Parity", wintypes.BYTE),
                ("StopBits", wintypes.BYTE),
                ("XonChar", ctypes.c_char),
                ("XoffChar", ctypes.c_char),
                ("ErrorChar", ctypes.c_char),
                ("EofChar", ctypes.c_char),
                ("EvtChar", ctypes.c_char),
                ("wReserved1", wintypes.WORD),
            ]

        dcb = DCB()
        dcb.DCBlength = ctypes.sizeof(DCB)
        if not kernel32.GetCommState(handle, ctypes.byref(dcb)):
            self.close()
            raise IOError(f"GetCommState failed for {port}")

        dcb.BaudRate = baudrate
        dcb.ByteSize = bytesize
        dcb.fBinary = 1
        dcb.fParity = 1 if parity in ("E", "O") else 0
        dcb.Parity = {"N": 0, "E": 2, "O": 1}.get(parity.upper(), 2)
        dcb.StopBits = 0 if stopbits == 1 else 2
        dcb.fOutxCtsFlow = 0
        dcb.fOutxDsrFlow = 0
        dcb.fDtrControl = 1
        dcb.fRtsControl = 1

        if not kernel32.SetCommState(handle, ctypes.byref(dcb)):
            self.close()
            raise IOError(f"SetCommState failed for {port}")

        class COMMTIMEOUTS(ctypes.Structure):
            _fields_ = [
                ("ReadIntervalTimeout", wintypes.DWORD),
                ("ReadTotalTimeoutMultiplier", wintypes.DWORD),
                ("ReadTotalTimeoutConstant", wintypes.DWORD),
                ("WriteTotalTimeoutMultiplier", wintypes.DWORD),
                ("WriteTotalTimeoutConstant", wintypes.DWORD),
            ]

        ms = max(int(timeout * 1000), 100)
        timeouts = COMMTIMEOUTS(50, 0, ms, 0, ms)
        kernel32.SetCommTimeouts(handle, ctypes.byref(timeouts))
        kernel32.PurgeComm(handle, 0x0000000F)

    @property
    def is_open(self) -> bool:
        return self._handle is not None

    def close(self) -> None:
        if self._handle is not None:
            self._ctypes.windll.kernel32.CloseHandle(self._handle)
            self._handle = None

    def reset_input_buffer(self) -> None:
        if self._handle is not None:
            self._ctypes.windll.kernel32.PurgeComm(self._handle, 0x00000008)

    def reset_output_buffer(self) -> None:
        if self._handle is not None:
            self._ctypes.windll.kernel32.PurgeComm(self._handle, 0x00000004)

    def write(self, data: bytes) -> None:
        if self._handle is None:
            raise IOError("Port closed")
        kernel32 = self._ctypes.windll.kernel32
        written = self._ctypes.c_ulong(0)
        ok = kernel32.WriteFile(
            self._handle,
            data,
            len(data),
            self._ctypes.byref(written),
            None,
        )
        if not ok:
            raise IOError("WriteFile failed")

    def flush(self) -> None:
        pass

    def read(self, size: int = 1) -> bytes:
        if self._handle is None:
            raise IOError("Port closed")
        kernel32 = self._ctypes.windll.kernel32
        buf = self._ctypes.create_string_buffer(size)
        read = self._ctypes.c_ulong(0)
        ok = kernel32.ReadFile(
            self._handle,
            buf,
            size,
            self._ctypes.byref(read),
            None,
        )
        if not ok:
            return b""
        return buf.raw[: read.value]

    @property
    def in_waiting(self) -> int:
        if self._handle is None:
            return 0
        ctypes = self._ctypes
        kernel32 = ctypes.windll.kernel32
        errors = ctypes.c_ulong(0)

        class COMSTAT(ctypes.Structure):
            _fields_ = [
                ("fCtsHold", ctypes.c_ulong, 1),
                ("fDsrHold", ctypes.c_ulong, 1),
                ("fRlsdHold", ctypes.c_ulong, 1),
                ("fXoffHold", ctypes.c_ulong, 1),
                ("fXoffSent", ctypes.c_ulong, 1),
                ("fEof", ctypes.c_ulong, 1),
                ("fTxim", ctypes.c_ulong, 1),
                ("fReserved", ctypes.c_ulong, 9),
                ("cbInQue", ctypes.c_ulong),
                ("cbOutQue", ctypes.c_ulong),
            ]

        stat = COMSTAT()
        kernel32.ClearCommError(self._handle, ctypes.byref(errors), ctypes.byref(stat))
        return int(stat.cbInQue)


def _open_serial(port, baudrate, parity, stopbits, bytesize, timeout):
    if sys.platform == "win32":
        return _Win32Serial(port, baudrate, parity, stopbits, bytesize, timeout)
    try:
        import serial

        return serial.Serial(
            port=port,
            baudrate=baudrate,
            parity=parity,
            stopbits=stopbits,
            bytesize=bytesize,
            timeout=timeout,
        )
    except ImportError as exc:
        raise ImportError("pyserial required on non-Windows platforms") from exc


class ModbusRtuClient:
    def __init__(
        self,
        port: str,
        baudrate: int = 19200,
        parity: str = "E",
        stopbits: int = 1,
        bytesize: int = 8,
        slave_id: int = 1,
        timeout: float = 1.0,
    ) -> None:
        self.slave_id = slave_id
        self.timeout = timeout
        self._ser = _open_serial(port, baudrate, parity, stopbits, bytesize, timeout)
        self._ser.reset_input_buffer()
        self._ser.reset_output_buffer()

    def close(self) -> None:
        if self._ser and self._ser.is_open:
            self._ser.close()

    @property
    def is_open(self) -> bool:
        return self._ser.is_open

    def _exchange(self, pdu: bytes) -> bytes:
        frame = bytes([self.slave_id]) + pdu
        crc = crc16_modbus(frame)
        frame += struct.pack("<H", crc)
        self._ser.reset_input_buffer()
        self._ser.write(frame)
        self._ser.flush()
        time.sleep(0.05)

        resp = bytearray()
        deadline = time.time() + self.timeout
        while time.time() < deadline:
            waiting = getattr(self._ser, "in_waiting", 0) or 0
            chunk = self._ser.read(waiting if waiting else 1)
            if chunk:
                resp.extend(chunk)
                if len(resp) >= 3 and resp[1] in (0x01, 0x02):
                    need = 3 + resp[2] + 2
                    if len(resp) >= need:
                        break
                if len(resp) >= 8 and resp[1] in (0x05, 0x06):
                    break
                if len(resp) >= 5 and resp[1] & 0x80:
                    break
            else:
                time.sleep(0.01)

        if len(resp) < 5:
            raise TimeoutError("No Modbus response (timeout)")

        if resp[0] != self.slave_id:
            raise IOError(f"Unexpected slave id {resp[0]} (expected {self.slave_id})")

        if resp[1] & 0x80:
            raise IOError(f"Modbus exception 0x{resp[2]:02X}")

        payload = bytes(resp[:-2])
        recv_crc = struct.unpack("<H", resp[-2:])[0]
        if crc16_modbus(payload) != recv_crc:
            raise IOError("CRC error on Modbus response")

        return bytes(resp)

    def read_bits(self, function_code: int, start: int, count: int) -> List[bool]:
        pdu = struct.pack(">BHH", function_code, start, count)
        resp = self._exchange(pdu)
        byte_count = resp[2]
        bits: List[bool] = []
        for i in range(count):
            byte_index = i // 8
            bit_index = i % 8
            if byte_index >= byte_count:
                bits.append(False)
                continue
            bits.append(bool(resp[3 + byte_index] & (1 << bit_index)))
        return bits

    def read_coils(self, start: int, count: int) -> List[bool]:
        return self.read_bits(0x01, start, count)

    def read_discrete_inputs(self, start: int, count: int) -> List[bool]:
        return self.read_bits(0x02, start, count)

    def write_coil(self, address: int, value: bool) -> None:
        pdu = struct.pack(">BHH", 0x05, address, 0xFF00 if value else 0x0000)
        self._exchange(pdu)
