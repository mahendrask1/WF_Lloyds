"""Build tag metadata from project CSV files."""

from __future__ import annotations

import csv
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional


@dataclass
class TagInfo:
    tag: str
    address: int
    fc: str
    access: str
    category: str
    description: str
    semantics: str = ""
    signal_group: str = ""
    highlight: bool = False


HIGHLIGHT_TAGS = {
    "I024", "I025", "I026", "I027",
    "M008", "M010", "M011", "M012",
    "M019", "M042", "M051", "M066",
}

# Panel / VFD I-O notes for diagnostic table (from plc.c default wiring)
IO_DESCRIPTIONS: Dict[str, str] = {
    "I003": "Panel DI — remote start/stop command input",
    "I004": "Panel DI — WF drive healthy feedback",
    "I005": "Panel DI — downstream interlock",
    "I024": "WF VFD DI — default PLC copies to M008 (remote/local)",
    "I025": "WF VFD DI — default PLC copies to M010 (emergency)",
    "I026": "WF VFD DI — Prakesh PLC copies to M008 (remote/local)",
    "I027": "WF VFD DI — default PLC local start → M011",
    "I028": "WF VFD DI — extended inverter input",
    "I029": "WF VFD DI — extended inverter input",
    "I032": "PF VFD DI — prefeeder inverter inputs",
    "O000": "DO status — output 0 (WF start/stop LED path)",
    "O001": "DO status — output 1",
    "O002": "DO status — output 2 (general fault)",
    "O003": "DO status — output 3 (panel healthy)",
    "O005": "DO status — output 5 (WF run / M066 path)",
}


def _signal_group(tag: str) -> str:
    n = _tag_num(tag)
    if tag.startswith("I"):
        if n <= 23:
            return "Panel DI"
        if n <= 31:
            return "WF VFD DI"
        return "PF / ext DI"
    if tag.startswith("O"):
        return "DO status"
    if tag.startswith("M"):
        if n <= 39:
            return "Marker coil (RW)"
        return "Marker status (RO)"
    return "Other"


def _tool_dir() -> Path:
    """App root — PyInstaller bundle dir when frozen, else this script's folder."""
    if getattr(sys, "frozen", False):
        return Path(sys._MEIPASS)
    return Path(__file__).resolve().parent


def _repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def _find_csv(name: str) -> Path:
    local = _tool_dir() / "data" / name
    if local.exists():
        return local
    if not getattr(sys, "frozen", False):
        repo = _repo_root() / name
        if repo.exists():
            return repo
    raise FileNotFoundError(f"Missing tag map CSV: {name}")


def _load_functional_descriptions() -> Dict[str, tuple[str, str]]:
    path = _find_csv("modbus_bit_functional_description.csv")
    out: Dict[str, tuple[str, str]] = {}
    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            tag = row["Tag"].strip()
            desc = row.get("Functional_Description", "").strip()
            sem = row.get("Value_Semantics", "").strip()
            if ".." in tag:
                continue
            out[tag] = (desc, sem)
    return out


def _tag_num(tag: str) -> int:
    m = re.search(r"(\d+)$", tag)
    return int(m.group(1)) if m else 0


def load_all_tags() -> List[TagInfo]:
    path = _find_csv("modbus_bits_expanded.csv")
    functional = _load_functional_descriptions()

    tags: List[TagInfo] = []
    with path.open(newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            tag = row["Tag"].strip()
            if tag.startswith("RESERVED"):
                continue
            addr = int(row["Bit_Address"])
            fc = row["Function_Code"].strip()
            access = row["Access"].strip()
            typ = row["Type"].strip()
            notes = row.get("Notes", "").strip()

            desc, sem = functional.get(tag, ("", ""))
            if tag in IO_DESCRIPTIONS:
                desc = IO_DESCRIPTIONS[tag]
            elif not desc:
                desc = notes
            if not sem:
                sem = notes if tag not in IO_DESCRIPTIONS else "1=ON/active; 0=OFF/inactive"

            if tag.startswith("M") and _tag_num(tag) <= 39:
                category = "writable_markers"
            elif tag.startswith("M"):
                category = "status_markers"
            elif tag.startswith("I"):
                category = "digital_inputs"
            elif tag.startswith("O"):
                category = "digital_outputs"
            else:
                category = "other"

            tags.append(
                TagInfo(
                    tag=tag,
                    address=addr,
                    fc=fc,
                    access=access,
                    category=category,
                    description=desc,
                    semantics=sem,
                    signal_group=_signal_group(tag),
                    highlight=tag in HIGHLIGHT_TAGS,
                )
            )

    tags.sort(key=lambda t: (t.category, t.address))
    return tags


def tags_by_category(tags: List[TagInfo]) -> Dict[str, List[TagInfo]]:
    out: Dict[str, List[TagInfo]] = {}
    for t in tags:
        out.setdefault(t.category, []).append(t)
    return out


def panel_di_tags(tags: List[TagInfo]) -> List[TagInfo]:
    """Panel digital inputs I000–I023 (FC02 addr 150–173)."""
    return [t for t in tags if t.tag.startswith("I") and _tag_num(t.tag) <= 23]


def vfd_di_tags(tags: List[TagInfo]) -> List[TagInfo]:
    """WF / PF VFD digital inputs I024–I039 (FC02 addr 174–189)."""
    return [t for t in tags if t.tag.startswith("I") and _tag_num(t.tag) >= 24]
