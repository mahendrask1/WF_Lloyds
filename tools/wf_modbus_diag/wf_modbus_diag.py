#!/usr/bin/env python3
"""
WF Lloyds Modbus Diagnostic Tool — GUI for USB-RS485 to AO module SIO port.

Polls M/I/O bits and allows FC05 writes to M000-M039 coils.
"""

from __future__ import annotations

import tkinter as tk
from tkinter import messagebox, ttk
from typing import Dict, List, Optional

from modbus_rtu import ModbusRtuClient, list_serial_ports
from tag_map import TagInfo, load_all_tags, panel_di_tags, tags_by_category, vfd_di_tags


class WfModbusDiagApp(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("WF Lloyds — Modbus Diagnostic")
        self.geometry("1280x780")
        self.minsize(900, 600)

        self.client: Optional[ModbusRtuClient] = None
        self.connected = False
        self.poll_job: Optional[str] = None
        self.all_tags = load_all_tags()
        self.by_category = tags_by_category(self.all_tags)
        self.panel_di = panel_di_tags(self.all_tags)
        self.vfd_di = vfd_di_tags(self.all_tags)
        n_di = len(self.by_category.get("digital_inputs", []))
        n_do = len(self.by_category.get("digital_outputs", []))
        if n_di == 0 or n_do == 0:
            messagebox.showwarning(
                "Tag map",
                f"Loaded {n_di} DI and {n_do} DO tags.\n"
                "Expected 40 of each — check data/modbus_bits_expanded.csv.",
            )

        self.coil_values: Dict[str, bool] = {}
        self.di_values: Dict[str, bool] = {}
        self.write_vars: Dict[str, tk.BooleanVar] = {}

        self._build_ui()
        self.refresh_ports()
        self.protocol("WM_DELETE_WINDOW", self.on_close)

    # ------------------------------------------------------------------ UI
    def _build_ui(self) -> None:
        top = ttk.Frame(self, padding=8)
        top.pack(fill=tk.X)

        ttk.Label(top, text="COM port:").grid(row=0, column=0, sticky=tk.W, padx=(0, 4))
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(top, textvariable=self.port_var, width=12, state="readonly")
        self.port_combo.grid(row=0, column=1, padx=(0, 8))

        ttk.Button(top, text="Refresh", command=self.refresh_ports).grid(row=0, column=2, padx=(0, 12))

        ttk.Label(top, text="Baud:").grid(row=0, column=3, sticky=tk.W)
        self.baud_var = tk.StringVar(value="19200")
        ttk.Combobox(
            top, textvariable=self.baud_var, width=10, state="readonly",
            values=["9600", "19200", "38400", "57600", "115200"],
        ).grid(row=0, column=4, padx=(4, 12))

        ttk.Label(top, text="Parity:").grid(row=0, column=5, sticky=tk.W)
        self.parity_var = tk.StringVar(value="Even (8E1)")
        ttk.Combobox(
            top, textvariable=self.parity_var, width=12, state="readonly",
            values=["Even (8E1)", "None (8N1)", "Odd (8O1)"],
        ).grid(row=0, column=6, padx=(4, 12))

        ttk.Label(top, text="Slave ID:").grid(row=0, column=7, sticky=tk.W)
        self.slave_var = tk.StringVar(value="1")
        ttk.Entry(top, textvariable=self.slave_var, width=5).grid(row=0, column=8, padx=(4, 12))

        self.connect_btn = ttk.Button(top, text="Connect", command=self.toggle_connect)
        self.connect_btn.grid(row=0, column=9, padx=(0, 8))

        ttk.Label(top, text="Poll (ms):").grid(row=0, column=10, sticky=tk.W)
        self.poll_var = tk.StringVar(value="1000")
        ttk.Combobox(
            top, textvariable=self.poll_var, width=6, state="readonly",
            values=["500", "1000", "2000", "5000"],
        ).grid(row=0, column=11, padx=(4, 8))

        self.auto_poll_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(top, text="Auto poll", variable=self.auto_poll_var).grid(row=0, column=12, padx=4)
        ttk.Button(top, text="Scan now", command=self.scan_once).grid(row=0, column=13, padx=4)

        self.status_var = tk.StringVar(value="Disconnected — connect USB-RS485 to AO module SIO port")
        ttk.Label(self, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W, padding=4).pack(
            fill=tk.X, padx=8, pady=(0, 4)
        )

        hint = ttk.Label(
            self,
            text="Start on the 'VFD DI (24-39)' tab for WF drive inputs (I024–I039, Modbus FC02 addr 174–189). "
                 "Yellow rows = local/remote. Connect to the AO module SIO port to see Live values (0/1).",
            wraplength=1100,
        )
        hint.pack(fill=tk.X, padx=8, pady=(0, 4))

        self.notebook = ttk.Notebook(self)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=8, pady=4)

        self.writable_rows: Dict[str, dict] = {}
        self.readonly_trees: Dict[str, ttk.Treeview] = {}

        self._add_vfd_di_tab()
        self._add_panel_di_tab()
        self._add_combined_io_tab()
        self._add_readonly_tab("digital_outputs", "Output status (O000-O039)", "FC02 read-only — panel DO status")
        self._add_highlights_tab()
        self._add_writable_tab()
        self._add_readonly_tab("status_markers", "Status markers (M040-M099)", "FC02 read-only — marker status bits")
        self.notebook.select(0)

    def _add_writable_tab(self) -> None:
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Writable markers (M000-M039)")

        header = ttk.Frame(frame)
        header.pack(fill=tk.X, padx=4, pady=4)
        ttk.Label(header, text="Set Write checkbox, then click Write on that row (FC05). Live = FC01 readback.",
                  font=("", 9)).pack(side=tk.LEFT)
        ttk.Button(header, text="Write all checked", command=self.write_all_checked).pack(side=tk.RIGHT)

        canvas = tk.Canvas(frame, highlightthickness=0)
        scroll = ttk.Scrollbar(frame, orient=tk.VERTICAL, command=canvas.yview)
        inner = ttk.Frame(canvas)
        inner.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        canvas.create_window((0, 0), window=inner, anchor=tk.NW)
        canvas.configure(yscrollcommand=scroll.set)
        canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)

        cols = ["Tag", "Coil", "Live", "Write", "Description", "Semantics", ""]
        widths = [60, 50, 50, 50, 220, 280, 70]
        for c, w in zip(cols, widths):
            ttk.Label(inner, text=c, font=("", 9, "bold")).grid(row=0, column=cols.index(c), sticky=tk.W, padx=2, pady=2)

        tags = self.by_category.get("writable_markers", [])
        for row_idx, tag in enumerate(tags, start=1):
            bg = "#FFF8DC" if tag.highlight else None
            tag_lbl = tk.Label(inner, text=tag.tag, width=8, anchor=tk.W,
                               font=("", 9, "bold" if tag.highlight else "normal"))
            tag_lbl.grid(row=row_idx, column=0, sticky=tk.W, padx=2, pady=1)
            addr_lbl = tk.Label(inner, text=str(tag.address), width=6, anchor=tk.W)
            addr_lbl.grid(row=row_idx, column=1, sticky=tk.W, padx=2)
            live_lbl = tk.Label(inner, text="—", width=4, anchor=tk.CENTER, relief=tk.GROOVE)
            live_lbl.grid(row=row_idx, column=2, padx=2, pady=1)
            if bg:
                for w in (tag_lbl, addr_lbl, live_lbl):
                    w.configure(bg=bg)

            wvar = tk.BooleanVar(value=False)
            self.write_vars[tag.tag] = wvar
            chk = ttk.Checkbutton(inner, variable=wvar)
            chk.grid(row=row_idx, column=3, padx=2)
            ttk.Label(inner, text=tag.description, wraplength=210).grid(row=row_idx, column=4, sticky=tk.W, padx=2)
            ttk.Label(inner, text=tag.semantics, wraplength=270, foreground="#444").grid(
                row=row_idx, column=5, sticky=tk.W, padx=2
            )
            btn = ttk.Button(inner, text="Write", width=8,
                             command=lambda t=tag: self.write_single_marker(t))
            btn.grid(row=row_idx, column=6, padx=2, pady=1)

            self.writable_rows[tag.tag] = {
                "live_label": live_lbl,
                "tag_info": tag,
            }

        canvas.bind("<MouseWheel>", lambda e: canvas.yview_scroll(int(-1 * (e.delta / 120)), "units"))

    def _add_readonly_tab(self, category: str, title: str, subtitle: str) -> None:
        frame = ttk.Frame(self.notebook)
        short = title.split("(")[0].strip()
        if category == "digital_outputs":
            short = "DO (40)"
        elif category == "status_markers":
            short = "M status"
        self.notebook.add(frame, text=short)

        tags = self.by_category.get(category, [])
        header = ttk.Frame(frame)
        header.pack(fill=tk.X, padx=4, pady=(4, 0))
        ttk.Label(header, text=f"{subtitle}  —  {len(tags)} rows").pack(anchor=tk.W)
        tree = self._make_tree(frame, with_group=True)
        self.readonly_trees[category] = tree

        for tag in tags:
            iid = tag.tag
            tree.insert(
                "", tk.END, iid=iid,
                values=(tag.tag, tag.address, tag.signal_group, "—", tag.description, tag.semantics),
            )
            if tag.highlight:
                tree.tag_configure("hl", background="#FFF8DC")
                tree.item(iid, tags=("hl",))

    def _add_vfd_di_tab(self) -> None:
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="VFD DI (24-39)")
        header = ttk.Frame(frame)
        header.pack(fill=tk.X, padx=4, pady=(4, 0))
        ttk.Label(
            header,
            text=(
                f"WF / PF VFD digital inputs — {len(self.vfd_di)} rows "
                "(FC02 addr 174–189). Filled by firmware Invter_ModCom() from VFD RS485."
            ),
        ).pack(anchor=tk.W)
        tree = self._make_tree(frame, with_group=True, height=max(16, len(self.vfd_di) + 1))
        self.readonly_trees["vfd_di"] = tree
        for tag in self.vfd_di:
            tree.insert(
                "", tk.END, iid=tag.tag,
                values=(tag.tag, tag.address, tag.signal_group, "—", tag.description, tag.semantics),
            )
            if tag.highlight:
                tree.tag_configure("hl", background="#FFF8DC")
                tree.item(tag.tag, tags=("hl",))
        if tree.exists("I024"):
            tree.see("I024")

    def _add_panel_di_tab(self) -> None:
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Panel DI (0-23)")
        header = ttk.Frame(frame)
        header.pack(fill=tk.X, padx=4, pady=(4, 0))
        ttk.Label(
            header,
            text=f"Panel digital inputs — {len(self.panel_di)} rows (FC02 addr 150–173).",
        ).pack(anchor=tk.W)
        tree = self._make_tree(frame, with_group=True)
        self.readonly_trees["panel_di"] = tree
        for tag in self.panel_di:
            tree.insert(
                "", tk.END, iid=tag.tag,
                values=(tag.tag, tag.address, tag.signal_group, "—", tag.description, tag.semantics),
            )
            if tag.highlight:
                tree.tag_configure("hl", background="#FFF8DC")
                tree.item(tag.tag, tags=("hl",))

    def _add_combined_io_tab(self) -> None:
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="DI + DO (80)")
        io_tags = (
            self.by_category.get("digital_inputs", [])
            + self.by_category.get("digital_outputs", [])
        )
        io_tags.sort(key=lambda t: t.address)
        header = ttk.Frame(frame)
        header.pack(fill=tk.X, padx=4, pady=(4, 0))
        ttk.Label(
            header,
            text=f"All digital inputs (I000-I039) and output status (O000-O039) — {len(io_tags)} rows",
        ).pack(anchor=tk.W)
        tree = self._make_tree(frame, with_group=True)
        self.readonly_trees["all_io"] = tree

        for tag in io_tags:
            iid = f"io_{tag.tag}"
            tree.insert(
                "", tk.END, iid=iid,
                values=(tag.tag, tag.address, tag.signal_group, "—", tag.description, tag.semantics),
            )
            if tag.highlight:
                tree.tag_configure("hl", background="#FFF8DC")
                tree.item(iid, tags=("hl",))

    def _add_highlights_tab(self) -> None:
        frame = ttk.Frame(self.notebook)
        self.notebook.add(frame, text="Local/Remote focus")
        ttk.Label(
            frame,
            text="Key markers plus ALL DI/DO for field local/remote diagnosis.",
            padding=6,
        ).pack(anchor=tk.W)
        tree = self._make_tree(frame, with_group=True)
        self.readonly_trees["highlights"] = tree

        focus_tags: List[TagInfo] = []
        for t in self.all_tags:
            if t.highlight:
                focus_tags.append(t)
        for t in self.by_category.get("digital_inputs", []):
            if t not in focus_tags:
                focus_tags.append(t)
        for t in self.by_category.get("digital_outputs", []):
            if t not in focus_tags:
                focus_tags.append(t)
        focus_tags.sort(key=lambda t: (0 if t.highlight else 1, t.address))

        for tag in focus_tags:
            iid = f"hl_{tag.tag}"
            tree.insert(
                "", tk.END, iid=iid,
                values=(tag.tag, tag.address, tag.signal_group, "—", tag.description, tag.semantics),
            )
            if tag.highlight:
                tree.tag_configure("hl", background="#FFF8DC")
                tree.item(iid, tags=("hl",))

    def _make_tree(self, parent: ttk.Frame, with_group: bool = False, height: int = 24) -> ttk.Treeview:
        if with_group:
            cols = ("tag", "addr", "group", "value", "description", "semantics")
            headings = ("Tag", "Modbus addr", "Group", "Live", "Description", "Value meaning")
            widths = (65, 75, 95, 45, 220, 200)
        else:
            cols = ("tag", "addr", "value", "description", "semantics")
            headings = ("Tag", "Modbus addr", "Live", "Description", "Value meaning")
            widths = (70, 80, 60, 280, 320)

        container = ttk.Frame(parent)
        container.pack(fill=tk.BOTH, expand=True, padx=4, pady=4)
        container.rowconfigure(0, weight=1)
        container.columnconfigure(0, weight=1)

        tree = ttk.Treeview(container, columns=cols, show="headings", height=height)
        for col, text, width in zip(cols, headings, widths):
            tree.heading(col, text=text)
            anchor = tk.CENTER if col in ("addr", "value", "group") else tk.W
            tree.column(col, width=width, anchor=anchor, stretch=(col in ("description", "semantics")))

        scroll_y = ttk.Scrollbar(container, orient=tk.VERTICAL, command=tree.yview)
        scroll_x = ttk.Scrollbar(container, orient=tk.HORIZONTAL, command=tree.xview)
        tree.configure(yscrollcommand=scroll_y.set, xscrollcommand=scroll_x.set)

        tree.grid(row=0, column=0, sticky="nsew")
        scroll_y.grid(row=0, column=1, sticky="ns")
        scroll_x.grid(row=1, column=0, sticky="ew")
        return tree

    def _update_tree_value(self, tree: ttk.Treeview, iid: str, val: bool, value_col: int) -> None:
        if not tree.exists(iid):
            return
        vals = list(tree.item(iid, "values"))
        if len(vals) > value_col:
            vals[value_col] = "1" if val else "0"
            tree.item(iid, values=vals)

    # ----------------------------------------------------------- connection
    def refresh_ports(self) -> None:
        ports = list_serial_ports()
        self.port_combo["values"] = ports
        if ports and not self.port_var.get():
            self.port_var.set(ports[0])

    def _parity_char(self) -> str:
        p = self.parity_var.get()
        if p.startswith("None"):
            return "N"
        if p.startswith("Odd"):
            return "O"
        return "E"

    def toggle_connect(self) -> None:
        if self.connected:
            self.disconnect()
        else:
            self.connect()

    def connect(self) -> None:
        port = self.port_var.get().strip()
        if not port:
            messagebox.showerror("Connect", "Select a COM port.")
            return
        try:
            slave = int(self.slave_var.get())
            baud = int(self.baud_var.get())
        except ValueError:
            messagebox.showerror("Connect", "Invalid slave ID or baud rate.")
            return

        try:
            self.client = ModbusRtuClient(
                port=port,
                baudrate=baud,
                parity=self._parity_char(),
                slave_id=slave,
            )
        except Exception as exc:
            messagebox.showerror("Connect failed", str(exc))
            self.client = None
            return

        self.connected = True
        self.connect_btn.configure(text="Disconnect")
        self.status_var.set(f"Connected to {port} @ {baud} 8{self._parity_char()}1, slave {slave}")
        self.scan_once()
        self._schedule_poll()

    def disconnect(self) -> None:
        if self.poll_job:
            self.after_cancel(self.poll_job)
            self.poll_job = None
        if self.client:
            self.client.close()
            self.client = None
        self.connected = False
        self.connect_btn.configure(text="Connect")
        self.status_var.set("Disconnected")

    def on_close(self) -> None:
        self.disconnect()
        self.destroy()

    # -------------------------------------------------------------- polling
    def _schedule_poll(self) -> None:
        if self.poll_job:
            self.after_cancel(self.poll_job)
        if self.connected and self.auto_poll_var.get():
            try:
                ms = int(self.poll_var.get())
            except ValueError:
                ms = 1000
            self.poll_job = self.after(ms, self._poll_tick)

    def _poll_tick(self) -> None:
        if self.connected:
            self.scan_once()
            self._schedule_poll()

    def scan_once(self) -> None:
        if not self.client or not self.connected:
            return
        try:
            coils = self.client.read_coils(0, 40)
            di_m = self.client.read_discrete_inputs(40, 60)
            di_panel = self.client.read_discrete_inputs(150, 24)
            di_vfd = self.client.read_discrete_inputs(174, 16)
            di_i = di_panel + di_vfd
            di_o = self.client.read_discrete_inputs(220, 40)
        except Exception as exc:
            self.status_var.set(f"Scan error: {exc}")
            return

        from datetime import datetime
        vfd_live = sum(1 for t in self.vfd_di if di_i[t.address - 150])
        self.status_var.set(
            f"OK — last scan {datetime.now().strftime('%H:%M:%S')} "
            f"| VFD DI I024–I039: {vfd_live}/{len(self.vfd_di)} high"
        )

        for tag in self.by_category.get("writable_markers", []):
            val = coils[tag.address] if tag.address < len(coils) else False
            self.coil_values[tag.tag] = val
            row = self.writable_rows.get(tag.tag)
            if row:
                lbl: tk.Label = row["live_label"]
                lbl.configure(text="1" if val else "0", fg="#006400" if val else "#888")

        fc02_map: Dict[str, bool] = {}
        for tag in self.by_category.get("status_markers", []):
            idx = tag.address - 40
            fc02_map[tag.tag] = di_m[idx] if 0 <= idx < len(di_m) else False
        for tag in self.by_category.get("digital_inputs", []):
            idx = tag.address - 150
            fc02_map[tag.tag] = di_i[idx] if 0 <= idx < len(di_i) else False
        for tag in self.by_category.get("digital_outputs", []):
            idx = tag.address - 220
            fc02_map[tag.tag] = di_o[idx] if 0 <= idx < len(di_o) else False

        self.di_values = fc02_map

        value_col = 3  # Live column when Group column present

        tree_tags = (
            ("status_markers", self.by_category.get("status_markers", [])),
            ("vfd_di", self.vfd_di),
            ("panel_di", self.panel_di),
            ("digital_outputs", self.by_category.get("digital_outputs", [])),
        )
        for tree_key, tags in tree_tags:
            tree = self.readonly_trees.get(tree_key)
            if not tree:
                continue
            for tag in tags:
                val = fc02_map.get(tag.tag, False)
                self._update_tree_value(tree, tag.tag, val, value_col)

        io_tree = self.readonly_trees.get("all_io")
        if io_tree:
            for tag in (
                self.by_category.get("digital_inputs", [])
                + self.by_category.get("digital_outputs", [])
            ):
                val = fc02_map.get(tag.tag, False)
                self._update_tree_value(io_tree, f"io_{tag.tag}", val, value_col)

        hl_tree = self.readonly_trees.get("highlights")
        if hl_tree:
            for tag in self.all_tags:
                iid = f"hl_{tag.tag}"
                if not hl_tree.exists(iid):
                    continue
                if tag.category == "writable_markers":
                    val = self.coil_values.get(tag.tag, False)
                else:
                    val = fc02_map.get(tag.tag, False)
                self._update_tree_value(hl_tree, iid, val, value_col)

    # ---------------------------------------------------------------- writes
    def write_single_marker(self, tag: TagInfo) -> None:
        if not self.client or not self.connected:
            messagebox.showwarning("Write", "Not connected.")
            return
        value = self.write_vars[tag.tag].get()
        try:
            self.client.write_coil(tag.address, value)
        except Exception as exc:
            messagebox.showerror("Write failed", f"{tag.tag}: {exc}")
            return
        self.status_var.set(f"Wrote {tag.tag} (coil {tag.address}) = {1 if value else 0}")
        self.after(200, self.scan_once)

    def write_all_checked(self) -> None:
        if not self.client or not self.connected:
            messagebox.showwarning("Write", "Not connected.")
            return
        errors: List[str] = []
        for tag in self.by_category.get("writable_markers", []):
            wvar = self.write_vars.get(tag.tag)
            if wvar is None:
                continue
            try:
                self.client.write_coil(tag.address, wvar.get())
            except Exception as exc:
                errors.append(f"{tag.tag}: {exc}")
        if errors:
            messagebox.showerror("Write errors", "\n".join(errors))
        else:
            self.status_var.set("Write all checked — completed")
        self.after(200, self.scan_once)


def main() -> None:
    app = WfModbusDiagApp()
    app.mainloop()


if __name__ == "__main__":
    main()
