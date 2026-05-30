#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
CNN 卷积神经网络原理交互演示工具
CNN Convolutional Neural Network Interactive Visualizer

Interactive demos for convolution and pooling operations.
"""

import tkinter as tk
from tkinter import ttk, font
import random
import math
from typing import List, Optional


# ──────────────────────────────────────────────
# Color utilities (matching the HTML dark theme)
# ──────────────────────────────────────────────
THEME = {
    "bg": "#0a0e1a",
    "bg2": "#111827",
    "bg3": "#1a2035",
    "bg4": "#232b42",
    "text": "#e2e8f0",
    "text2": "#94a3b8",
    "text3": "#64748b",
    "accent": "#3b82f6",
    "accent2": "#6366f1",
    "green": "#10b981",
    "orange": "#f59e0b",
    "pink": "#ec4899",
    "cyan": "#06b6d4",
    "accent3": "#8b5cf6",
}


def clamp(v: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, v))


def cell_color(value: float, lo: float = -10, hi: float = 10) -> str:
    """Map a value to a HSL-based color (blue/cyan for low, purple/magenta for high)."""
    t = clamp((value - lo) / (hi - lo), 0, 1)
    hue = 220 - t * 170
    return f"#{_hsl_to_hex(hue, 60, 35)}"


def cell_text_color(value: float, lo: float = -10, hi: float = 10) -> str:
    t = clamp((value - lo) / (hi - lo), 0, 1)
    hue = 220 - t * 170
    return f"#{_hsl_to_hex(hue, 70, 65)}"


def _hsl_to_hex(h: float, s: float, l: float) -> str:
    """Convert HSL to hex string."""
    s /= 100
    l /= 100
    c = (1 - abs(2 * l - 1)) * s
    x = c * (1 - abs((h / 60) % 2 - 1))
    m = l - c / 2

    if h < 60:
        r, g, b = c, x, 0
    elif h < 120:
        r, g, b = x, c, 0
    elif h < 180:
        r, g, b = 0, c, x
    elif h < 240:
        r, g, b = 0, x, c
    elif h < 300:
        r, g, b = x, 0, c
    else:
        r, g, b = c, 0, x

    r = int((r + m) * 255)
    g = int((g + m) * 255)
    b = int((b + m) * 255)
    return f"{r:02x}{g:02x}{b:02x}"


def random_matrix(rows: int, cols: int) -> List[List[int]]:
    return [[random.randint(-9, 9) for _ in range(cols)] for _ in range(rows)]


def get_cell_size(matrix_size: int) -> int:
    if matrix_size <= 5:
        return 40
    if matrix_size <= 7:
        return 36
    if matrix_size <= 9:
        return 30
    if matrix_size <= 11:
        return 26
    return 22


# ──────────────────────────────────────────────
# Editable Matrix Widget
# ──────────────────────────────────────────────
class EditableMatrix(tk.Frame):
    """A grid of editable number cells with color coding."""

    def __init__(self, parent: tk.Widget, rows: int, cols: int, data: Optional[List[List[int]]] = None,
                 editable: bool = True, cell_size: int = 40, readonly: bool = False, **kwargs):
        super().__init__(parent, **kwargs)
        self.rows = rows
        self.cols = cols
        self.cell_size = cell_size
        self.readonly = readonly
        self.cells: List[List[tk.Widget]] = []
        self.entries: List[List[Optional[tk.Entry]]] = []
        self.data = data if data else random_matrix(rows, cols)
        self._build()

    def _build(self):
        for i in range(self.rows):
            row_cells = []
            row_entries = []
            for j in range(self.cols):
                f = tk.Frame(self, width=self.cell_size, height=self.cell_size,
                             bg=cell_color(self.data[i][j]),
                             highlightbackground=THEME["bg4"], highlightthickness=1)
                f.pack_propagate(False)
                f.grid(row=i, column=j, padx=1, pady=1)
                f.bind("<Button-1>", lambda e, r=i, c=j: self._on_click(r, c))

                v = self.data[i][j]
                lbl = tk.Label(f, text=str(v), fg=cell_text_color(v),
                               bg=cell_color(v), font=("Consolas", 10, "bold"))
                lbl.place(relx=0.5, rely=0.5, anchor="center")
                lbl.bind("<Button-1>", lambda e, r=i, c=j: self._on_click(r, c))

                row_cells.append(f)
                row_entries.append(None)
            self.cells.append(row_cells)
            self.entries.append(row_entries)

    def _on_click(self, r: int, c: int):
        if self.readonly:
            return
        self._make_entry(r, c)

    def _make_entry(self, r: int, c: int):
        if self.entries[r][c] is not None:
            return
        f = self.cells[r][c]
        v = self.data[r][c]
        # Clear the frame
        for w in f.winfo_children():
            w.destroy()

        ent = tk.Entry(f, width=4, justify="center",
                       bg=cell_color(v), fg=cell_text_color(v),
                       font=("Consolas", 10, "bold"),
                       relief="flat", bd=0,
                       insertbackground=THEME["text"])
        ent.insert(0, str(v))
        ent.place(relx=0.5, rely=0.5, anchor="center", width=self.cell_size - 6,
                  height=self.cell_size - 6)
        ent.select_range(0, tk.END)
        ent.focus_set()

        def on_confirm(event=None):
            try:
                new_v = int(ent.get())
            except ValueError:
                new_v = 0
            self.data[r][c] = new_v
            self._restore_label(r, c)

        ent.bind("<Return>", on_confirm)
        ent.bind("<FocusOut>", on_confirm)
        self.entries[r][c] = ent

    def _restore_label(self, r: int, c: int):
        f = self.cells[r][c]
        for w in f.winfo_children():
            w.destroy()

        v = self.data[r][c]
        lbl = tk.Label(f, text=str(v), fg=cell_text_color(v),
                       bg=cell_color(v), font=("Consolas", 10, "bold"))
        lbl.place(relx=0.5, rely=0.5, anchor="center")
        lbl.bind("<Button-1>", lambda e, r=r, c=c: self._make_entry(r, c))
        self.entries[r][c] = None

    def update_data(self, new_data: List[List[int]], new_rows: int, new_cols: int):
        """Rebuild the matrix with new data and dimensions."""
        self.data = new_data
        self.rows = new_rows
        self.cols = new_cols
        self.cell_size = get_cell_size(max(new_rows, new_cols))
        for w in self.winfo_children():
            w.destroy()
        self.cells = []
        self.entries = []
        self._build()

    def get_data(self) -> List[List[int]]:
        return self.data

    def highlight_cell(self, r: int, c: int, color: str = None):
        """Set outline on a cell."""
        if 0 <= r < self.rows and 0 <= c < self.cols:
            f = self.cells[r][c]
            if color:
                f.config(highlightbackground=color, highlightthickness=2)
            else:
                f.config(highlightbackground=THEME["bg4"], highlightthickness=1)

    def clear_highlights(self):
        for r in range(self.rows):
            for c in range(self.cols):
                self.cells[r][c].config(highlightbackground=THEME["bg4"], highlightthickness=1)

    def set_cell_text(self, r: int, c: int, text: str, value: float = 0):
        """Set output cell content (read-only)."""
        if 0 <= r < self.rows and 0 <= c < self.cols:
            f = self.cells[r][c]
            for w in f.winfo_children():
                w.destroy()
            lbl = tk.Label(f, text=text, fg=cell_text_color(value, -50, 50),
                           bg=cell_color(value, -50, 50),
                           font=("Consolas", 10, "bold"))
            lbl.place(relx=0.5, rely=0.5, anchor="center")


class ReadonlyMatrix(tk.Frame):
    """A non-editable matrix for displaying output."""

    def __init__(self, parent: tk.Widget, rows: int, cols: int, cell_size: int = 40, **kwargs):
        super().__init__(parent, **kwargs)
        self.rows = rows
        self.cols = cols
        self.cell_size = cell_size
        self.cells: List[List[tk.Frame]] = []
        self.labels: List[List[tk.Label]] = []
        self._build()

    def _build(self):
        for i in range(self.rows):
            row_cells = []
            row_labels = []
            for j in range(self.cols):
                f = tk.Frame(self, width=self.cell_size, height=self.cell_size,
                             bg=THEME["bg4"],
                             highlightbackground=THEME["bg4"], highlightthickness=1)
                f.pack_propagate(False)
                f.grid(row=i, column=j, padx=1, pady=1)
                lbl = tk.Label(f, text="—", fg=THEME["text3"],
                               bg=THEME["bg4"], font=("Consolas", 10))
                lbl.place(relx=0.5, rely=0.5, anchor="center")
                row_cells.append(f)
                row_labels.append(lbl)
            self.cells.append(row_cells)
            self.labels.append(row_labels)

    def rebuild(self, rows: int, cols: int):
        self.rows = rows
        self.cols = cols
        self.cell_size = get_cell_size(max(rows, cols))
        for w in self.winfo_children():
            w.destroy()
        self.cells = []
        self.labels = []
        self._build()

    def set_cell(self, r: int, c: int, text: str, value: float = 0):
        if 0 <= r < self.rows and 0 <= c < self.cols:
            lbl = self.labels[r][c]
            lbl.config(text=text, fg=cell_text_color(value, -50, 50),
                       bg=cell_color(value, -50, 50))

    def highlight_cell(self, r: int, c: int, color: str = None):
        if 0 <= r < self.rows and 0 <= c < self.cols:
            f = self.cells[r][c]
            if color:
                f.config(highlightbackground=color, highlightthickness=2)
            else:
                f.config(highlightbackground=THEME["bg4"], highlightthickness=1)

    def clear_highlights(self):
        for r in range(self.rows):
            for c in range(self.cols):
                self.cells[r][c].config(highlightbackground=THEME["bg4"], highlightthickness=1)


# ──────────────────────────────────────────────
# Convolution Demo Tab
# ──────────────────────────────────────────────
class ConvolutionDemo(tk.Frame):
    def __init__(self, parent: tk.Widget, **kwargs):
        super().__init__(parent, bg=THEME["bg"], **kwargs)
        self.input_size_var = tk.StringVar(value="5")
        self.kernel_size_var = tk.StringVar(value="3")
        self.stride_var = tk.StringVar(value="1")
        self.padding_var = tk.StringVar(value="0")
        self.anim_timer: Optional[str] = None
        self.conv_input_data: Optional[List[List[int]]] = None
        self.conv_kernel_data: Optional[List[List[int]]] = None

        self._build()

    def _build(self):
        # Title
        title_lbl = tk.Label(self, text="🎮 卷积运算交互演示",
                             font=("Segoe UI", 14, "bold"),
                             fg=THEME["text"], bg=THEME["bg"])
        title_lbl.pack(anchor="w", pady=(0, 5))

        desc_lbl = tk.Label(self,
                            text="点击单元格可自定义数值，点击「执行卷积」观察滑动计算过程。",
                            font=("Segoe UI", 9), fg=THEME["text3"], bg=THEME["bg"],
                            wraplength=700, justify="left")
        desc_lbl.pack(anchor="w", pady=(0, 10))

        # Controls
        ctrl_frame = tk.Frame(self, bg=THEME["bg3"], bd=1, relief="solid",
                              highlightbackground=THEME["bg4"])
        ctrl_frame.pack(fill="x", pady=(0, 10))

        inner = tk.Frame(ctrl_frame, bg=THEME["bg3"])
        inner.pack(fill="x", padx=10, pady=8)

        # Row 1 of controls
        row1 = tk.Frame(inner, bg=THEME["bg3"])
        row1.pack(fill="x", pady=2)

        self._add_label_combo(row1, "输入大小:", self.input_size_var,
                              ["3", "4", "5", "6", "7"], self._on_param_change)
        self._add_label_combo(row1, "核大小:", self.kernel_size_var,
                              ["1", "3", "5"], self._on_param_change)
        self._add_label_combo(row1, "步幅:", self.stride_var, ["1", "2"])
        self._add_label_combo(row1, "填充:", self.padding_var, ["0", "1", "2"])

        # Row 2 of controls
        row2 = tk.Frame(inner, bg=THEME["bg3"])
        row2.pack(fill="x", pady=(6, 0))

        run_btn = tk.Button(row2, text="▶ 执行卷积", command=self._run_conv,
                            bg=THEME["accent"], fg="white", bd=0,
                            font=("Segoe UI", 9, "bold"), padx=12, pady=4,
                            cursor="hand2")
        run_btn.pack(side="left", padx=(0, 8))

        reset_btn = tk.Button(row2, text="↻ 重置", command=self._reset,
                              bg=THEME["bg4"], fg=THEME["text"], bd=0,
                              font=("Segoe UI", 9), padx=12, pady=4,
                              cursor="hand2")
        reset_btn.pack(side="left", padx=(0, 8))

        self.step_info = tk.Label(row2, text="", font=("Consolas", 9),
                                  fg=THEME["cyan"], bg=THEME["bg3"])
        self.step_info.pack(side="right")

        # Matrix area
        self.matrix_frame = tk.Frame(self, bg=THEME["bg"])
        self.matrix_frame.pack(fill="both", expand=True)

        self._init_matrices()

    def _add_label_combo(self, parent, label: str, variable: tk.StringVar,
                         values: List[str], command=None):
        f = tk.Frame(parent, bg=THEME["bg3"])
        f.pack(side="left", padx=(0, 12))
        lbl = tk.Label(f, text=label, fg=THEME["text2"], bg=THEME["bg3"],
                       font=("Segoe UI", 9))
        lbl.pack(side="left", padx=(0, 4))
        combo = ttk.Combobox(f, textvariable=variable, values=values, width=4,
                             state="readonly", font=("Segoe UI", 9))
        combo.pack(side="left")
        if command:
            combo.bind("<<ComboboxSelected>>", lambda e: command())

    def _on_param_change(self):
        self._reset()

    def _init_matrices(self):
        input_size = int(self.input_size_var.get())
        ks = int(self.kernel_size_var.get())
        pad = int(self.padding_var.get())
        stride = int(self.stride_var.get())

        padded_size = input_size + 2 * pad
        cell_size = get_cell_size(padded_size)
        kernel_cell_size = get_cell_size(ks)

        self.conv_input_data = random_matrix(input_size, input_size)
        self.conv_kernel_data = random_matrix(ks, ks)

        # Clear
        for w in self.matrix_frame.winfo_children():
            w.destroy()

        # Container for all matrices
        container = tk.Frame(self.matrix_frame, bg=THEME["bg"])
        container.pack(pady=10)

        # Show padded input if padding > 0
        input_label = f"输入矩阵 ({padded_size}×{padded_size}"
        if pad > 0:
            input_label += ", 灰色为 padding"
        input_label += ")"

        # Input matrix
        self._make_matrix_group(container, input_label, "conv_input",
                                self.conv_input_data, input_size, ks, pad, padded_size, cell_size, True)

        # Kernel matrix
        krn_frame = tk.Frame(container, bg=THEME["bg"])
        krn_frame.pack(side="left", padx=15)
        tk.Label(krn_frame, text=f"卷积核 ({ks}×{ks})",
                 font=("Segoe UI", 8, "bold"), fg=THEME["text3"],
                 bg=THEME["bg"]).pack(pady=(0, 4))
        self.kernel_matrix = EditableMatrix(krn_frame, ks, ks, self.conv_kernel_data,
                                            cell_size=kernel_cell_size)

        # Arrow
        arrow_frame = tk.Frame(container, bg=THEME["bg"])
        arrow_frame.pack(side="left", padx=15)
        tk.Label(arrow_frame, text="→", font=("Segoe UI", 28),
                 fg=THEME["accent"], bg=THEME["bg"]).pack(expand=True)

        # Output matrix
        out_size = (padded_size - ks) // stride + 1
        out_cell_size = get_cell_size(out_size)
        out_frame = tk.Frame(container, bg=THEME["bg"])
        out_frame.pack(side="left", padx=15)
        tk.Label(out_frame, text="输出特征图",
                 font=("Segoe UI", 8, "bold"), fg=THEME["text3"],
                 bg=THEME["bg"]).pack(pady=(0, 4))
        self.output_matrix = ReadonlyMatrix(out_frame, out_size, out_size, cell_size=out_cell_size)

        self.step_info.config(text="")

    def _make_matrix_group(self, container, label, name, data, input_size, ks, pad,
                           padded_size, cell_size, editable):
        f = tk.Frame(container, bg=THEME["bg"])
        f.pack(side="left", padx=15)

        tk.Label(f, text=label, font=("Segoe UI", 8, "bold"),
                 fg=THEME["text3"], bg=THEME["bg"]).pack(pady=(0, 4))

        grid_frame = tk.Frame(f, bg=THEME["bg"])
        grid_frame.pack()

        self.input_padded_data = [[0] * padded_size for _ in range(padded_size)]
        for i in range(input_size):
            for j in range(input_size):
                self.input_padded_data[i + pad][j + pad] = data[i][j]

        self.input_cells = []
        for i in range(padded_size):
            row_cells = []
            for j in range(padded_size):
                is_pad = i < pad or i >= pad + input_size or j < pad or j >= pad + input_size
                cf = tk.Frame(grid_frame, width=cell_size, height=cell_size,
                              bg=THEME["bg3"] if is_pad else cell_color(data[i - pad][j - pad] if not is_pad else 0),
                              highlightbackground=THEME["bg4"], highlightthickness=1)
                cf.pack_propagate(False)
                cf.grid(row=i, column=j, padx=1, pady=1)

                if is_pad:
                    lbl = tk.Label(cf, text="0", fg=THEME["text3"],
                                   bg=THEME["bg3"], font=("Consolas", 9))
                    lbl.place(relx=0.5, rely=0.5, anchor="center")
                    row_cells.append(cf)
                else:
                    ri, rj = i - pad, j - pad
                    v = data[ri][rj]
                    lbl = tk.Label(cf, text=str(v), fg=cell_text_color(v),
                                   bg=cell_color(v), font=("Consolas", 9, "bold"),
                                   cursor="hand2")
                    lbl.place(relx=0.5, rely=0.5, anchor="center")

                    def make_click_handler(rr, cc):
                        def handler(event):
                            self._edit_input_cell(grid_frame, rr, cc, padded_size, input_size, pad, cell_size)
                        return handler

                    lbl.bind("<Button-1>", make_click_handler(ri, rj))
                    row_cells.append(cf)
            self.input_cells.append(row_cells)

    def _edit_input_cell(self, grid_frame, r, c, padded_size, input_size, pad, cell_size):
        """Create an entry widget for editing a specific input cell."""
        i, j = r + pad, c + pad
        cf = self.input_cells[i][j]

        for w in cf.winfo_children():
            w.destroy()

        v = self.conv_input_data[r][c]
        ent = tk.Entry(cf, width=3, justify="center",
                       bg=cell_color(v), fg=cell_text_color(v),
                       font=("Consolas", 9, "bold"),
                       relief="flat", bd=0,
                       insertbackground=THEME["text"])
        ent.insert(0, str(v))
        ent.place(relx=0.5, rely=0.5, anchor="center",
                  width=cell_size - 6, height=cell_size - 6)
        ent.select_range(0, tk.END)
        ent.focus_set()

        def on_confirm(event=None):
            try:
                new_v = int(ent.get())
            except ValueError:
                new_v = 0
            self.conv_input_data[r][c] = new_v
            self.input_padded_data[i][j] = new_v
            self._restore_input_label(cf, new_v, r, c)

        ent.bind("<Return>", on_confirm)
        ent.bind("<FocusOut>", on_confirm)

    def _restore_input_label(self, cf, v, r, c):
        for w in cf.winfo_children():
            w.destroy()
        lbl = tk.Label(cf, text=str(v), fg=cell_text_color(v),
                       bg=cell_color(v), font=("Consolas", 9, "bold"),
                       cursor="hand2")
        lbl.place(relx=0.5, rely=0.5, anchor="center")

        def make_handler(rr, cc):
            def handler(event):
                self._edit_input_cell_bounce(cf, rr, cc, v)
            return handler

        lbl.bind("<Button-1>", make_handler(r, c))

    def _edit_input_cell_bounce(self, cf, r, c, v):
        """Helper to re-edit after confirm."""
        for w in cf.winfo_children():
            w.destroy()
        ent = tk.Entry(cf, width=3, justify="center",
                       bg=cell_color(v), fg=cell_text_color(v),
                       font=("Consolas", 9, "bold"),
                       relief="flat", bd=0,
                       insertbackground=THEME["text"])
        ent.insert(0, str(v))
        ent.place(relx=0.5, rely=0.5, anchor="center",
                  width=40 - 6, height=40 - 6)
        ent.select_range(0, tk.END)
        ent.focus_set()

        def on_confirm(event=None):
            try:
                new_v = int(ent.get())
            except ValueError:
                new_v = 0
            self.conv_input_data[r][c] = new_v
            self._restore_input_label(cf, new_v, r, c)

        ent.bind("<Return>", on_confirm)
        ent.bind("<FocusOut>", on_confirm)

    def _run_conv(self):
        if self.anim_timer:
            self.after_cancel(self.anim_timer)
            self.anim_timer = None

        input_size = int(self.input_size_var.get())
        ks = int(self.kernel_size_var.get())
        stride = int(self.stride_var.get())
        pad = int(self.padding_var.get())
        padded_size = input_size + 2 * pad
        out_size = (padded_size - ks) // stride + 1

        # Build padded matrix
        padded = [[0] * padded_size for _ in range(padded_size)]
        for i in range(input_size):
            for j in range(input_size):
                padded[i + pad][j + pad] = self.conv_input_data[i][j]

        # Compute output
        output = []
        for i in range(out_size):
            row = []
            for j in range(out_size):
                s = 0
                for m in range(ks):
                    for n in range(ks):
                        s += padded[i * stride + m][j * stride + n] * self.conv_kernel_data[m][n]
                row.append(s)
            output.append(row)

        # Rebuild output matrix
        out_cell_size = get_cell_size(out_size)
        self.output_matrix.rebuild(out_size, out_size)

        # Animate
        self._anim_step = 0
        self._anim_total = out_size * out_size
        self._anim_output = output
        self._anim_padded_size = padded_size
        self._anim_ks = ks
        self._anim_stride = stride
        self._anim_out_size = out_size

        self._anim_conv_step()

    def _anim_conv_step(self):
        step = self._anim_step
        total = self._anim_total
        if step >= total:
            self.step_info.config(text=f"卷积完成！输出尺寸: {self._anim_out_size}×{self._anim_out_size}")
            self.anim_timer = None
            return

        oi = step // self._anim_out_size
        oj = step % self._anim_out_size
        val = self._anim_output[oi][oj]

        self.output_matrix.set_cell(oi, oj, str(val), val)
        self.output_matrix.highlight_cell(oi, oj, THEME["accent"])
        self.after(200, lambda: self.output_matrix.clear_highlights())

        # Highlight input region
        for r in range(self._anim_padded_size):
            for c in range(self._anim_padded_size):
                if r < len(self.input_cells) and c < len(self.input_cells[r]):
                    self.input_cells[r][c].config(highlightbackground=THEME["bg4"], highlightthickness=1)

        for m in range(self._anim_ks):
            for n in range(self._anim_ks):
                ir = oi * self._anim_stride + m
                ic = oj * self._anim_stride + n
                if ir < len(self.input_cells) and ic < len(self.input_cells[ir]):
                    self.input_cells[ir][ic].config(
                        highlightbackground=THEME["accent"], highlightthickness=2)

        self.step_info.config(text=f"位置 ({oi},{oj}): 计算值 = {val}")

        self._anim_step += 1
        self.anim_timer = self.after(350, self._anim_conv_step)

    def _reset(self):
        if self.anim_timer:
            self.after_cancel(self.anim_timer)
            self.anim_timer = None
        self._init_matrices()


# ──────────────────────────────────────────────
# Pooling Demo Tab
# ──────────────────────────────────────────────
class PoolingDemo(tk.Frame):
    def __init__(self, parent: tk.Widget, **kwargs):
        super().__init__(parent, bg=THEME["bg"], **kwargs)
        self.input_size_var = tk.StringVar(value="4")
        self.pool_type_var = tk.StringVar(value="max")
        self.pool_size_var = tk.StringVar(value="2")
        self.stride_var = tk.StringVar(value="2")
        self.anim_timer: Optional[str] = None
        self.pool_input_data: Optional[List[List[int]]] = None

        self._build()

    def _build(self):
        title_lbl = tk.Label(self, text="🎮 池化运算交互演示",
                             font=("Segoe UI", 14, "bold"),
                             fg=THEME["text"], bg=THEME["bg"])
        title_lbl.pack(anchor="w", pady=(0, 5))

        desc_lbl = tk.Label(self,
                            text="点击单元格可自定义数值，选择池化类型观察计算过程。",
                            font=("Segoe UI", 9), fg=THEME["text3"], bg=THEME["bg"],
                            wraplength=700, justify="left")
        desc_lbl.pack(anchor="w", pady=(0, 10))

        # Controls
        ctrl_frame = tk.Frame(self, bg=THEME["bg3"], bd=1, relief="solid",
                              highlightbackground=THEME["bg4"])
        ctrl_frame.pack(fill="x", pady=(0, 10))

        inner = tk.Frame(ctrl_frame, bg=THEME["bg3"])
        inner.pack(fill="x", padx=10, pady=8)

        row1 = tk.Frame(inner, bg=THEME["bg3"])
        row1.pack(fill="x", pady=2)

        self._add_label_combo(row1, "输入大小:", self.input_size_var,
                              ["3", "4", "5", "6", "8"], self._on_param_change)
        self._add_label_combo(row1, "池化类型:", self.pool_type_var,
                              ["max", "avg"], self._on_param_change)
        self._add_label_combo(row1, "窗口大小:", self.pool_size_var,
                              ["2", "3"], self._on_param_change)
        self._add_label_combo(row1, "步幅:", self.stride_var, ["2", "1"])

        row2 = tk.Frame(inner, bg=THEME["bg3"])
        row2.pack(fill="x", pady=(6, 0))

        run_btn = tk.Button(row2, text="▶ 执行池化", command=self._run_pool,
                            bg=THEME["accent"], fg="white", bd=0,
                            font=("Segoe UI", 9, "bold"), padx=12, pady=4,
                            cursor="hand2")
        run_btn.pack(side="left", padx=(0, 8))

        reset_btn = tk.Button(row2, text="↻ 重置", command=self._reset,
                              bg=THEME["bg4"], fg=THEME["text"], bd=0,
                              font=("Segoe UI", 9), padx=12, pady=4,
                              cursor="hand2")
        reset_btn.pack(side="left", padx=(0, 8))

        self.step_info = tk.Label(row2, text="", font=("Consolas", 9),
                                  fg=THEME["pink"], bg=THEME["bg3"])
        self.step_info.pack(side="right")

        # Matrix area
        self.matrix_frame = tk.Frame(self, bg=THEME["bg"])
        self.matrix_frame.pack(fill="both", expand=True)

        self._init_matrices()

    def _add_label_combo(self, parent, label: str, variable: tk.StringVar,
                         values: List[str], command=None):
        f = tk.Frame(parent, bg=THEME["bg3"])
        f.pack(side="left", padx=(0, 12))
        lbl = tk.Label(f, text=label, fg=THEME["text2"], bg=THEME["bg3"],
                       font=("Segoe UI", 9))
        lbl.pack(side="left", padx=(0, 4))
        combo = ttk.Combobox(f, textvariable=variable, values=values, width=4,
                             state="readonly", font=("Segoe UI", 9))
        combo.pack(side="left")
        if command:
            combo.bind("<<ComboboxSelected>>", lambda e: command())

    def _on_param_change(self):
        self._reset()

    def _init_matrices(self):
        input_size = int(self.input_size_var.get())
        ps = int(self.pool_size_var.get())
        st = int(self.stride_var.get())
        out_size = (input_size - ps) // st + 1

        cell_size = get_cell_size(input_size)
        out_cell_size = get_cell_size(out_size)

        self.pool_input_data = random_matrix(input_size, input_size)

        for w in self.matrix_frame.winfo_children():
            w.destroy()

        container = tk.Frame(self.matrix_frame, bg=THEME["bg"])
        container.pack(pady=10, expand=True)

        # Input matrix
        input_frame = tk.Frame(container, bg=THEME["bg"])
        input_frame.pack(side="left", padx=15)

        tk.Label(input_frame, text=f"输入矩阵 ({input_size}×{input_size})",
                 font=("Segoe UI", 8, "bold"), fg=THEME["text3"],
                 bg=THEME["bg"]).pack(pady=(0, 4))

        self.input_matrix = EditableMatrix(input_frame, input_size, input_size,
                                           self.pool_input_data, cell_size=cell_size)

        # Arrow
        arrow_frame = tk.Frame(container, bg=THEME["bg"])
        arrow_frame.pack(side="left", padx=15)
        tk.Label(arrow_frame, text="→", font=("Segoe UI", 28),
                 fg=THEME["accent"], bg=THEME["bg"]).pack(expand=True)

        # Output matrix
        out_frame = tk.Frame(container, bg=THEME["bg"])
        out_frame.pack(side="left", padx=15)
        pool_type = self.pool_type_var.get()
        label = "Max Pooling 输出" if pool_type == "max" else "Average Pooling 输出"
        tk.Label(out_frame, text=label,
                 font=("Segoe UI", 8, "bold"), fg=THEME["text3"],
                 bg=THEME["bg"]).pack(pady=(0, 4))
        self.output_matrix = ReadonlyMatrix(out_frame, out_size, out_size,
                                            cell_size=out_cell_size)

        self.step_info.config(text="")

    def _run_pool(self):
        if self.anim_timer:
            self.after_cancel(self.anim_timer)
            self.anim_timer = None

        pool_type = self.pool_type_var.get()
        ps = int(self.pool_size_var.get())
        st = int(self.stride_var.get())
        input_size = int(self.input_size_var.get())
        out_size = (input_size - ps) // st + 1

        # Compute output
        output = []
        for i in range(out_size):
            row = []
            for j in range(out_size):
                vals = []
                for m in range(ps):
                    for n in range(ps):
                        vals.append(self.pool_input_data[i * st + m][j * st + n])
                if pool_type == "max":
                    row.append(max(vals))
                else:
                    avg = sum(vals) / len(vals)
                    row.append(round(avg, 1))
            output.append(row)

        out_cell_size = get_cell_size(out_size)
        self.output_matrix.rebuild(out_size, out_size)

        self._anim_step = 0
        self._anim_total = out_size * out_size
        self._anim_output = output
        self._anim_pool_type = pool_type
        self._anim_ps = ps
        self._anim_st = st
        self._anim_out_size = out_size
        self._anim_input_size = input_size

        self._anim_pool_step()

    def _anim_pool_step(self):
        step = self._anim_step
        total = self._anim_total
        if step >= total:
            pool_type = self._anim_pool_type
            self.step_info.config(
                text=f"{'Max' if pool_type == 'max' else 'Average'} Pooling 完成！")
            self.anim_timer = None
            return

        oi = step // self._anim_out_size
        oj = step % self._anim_out_size
        val = self._anim_output[oi][oj]

        self.output_matrix.set_cell(oi, oj, str(val), val)

        # Highlight input window region
        self.input_matrix.clear_highlights()

        # Find max for max pooling
        max_val = -float("inf")
        max_pos = (-1, -1)
        for m in range(self._anim_ps):
            for n in range(self._anim_ps):
                ir = oi * self._anim_st + m
                ic = oj * self._anim_st + n
                self.input_matrix.highlight_cell(ir, ic, THEME["pink"])
                v = self.pool_input_data[ir][ic]
                if self._anim_pool_type == "max" and v > max_val:
                    max_val = v
                    max_pos = (ir, ic)

        if self._anim_pool_type == "max" and max_pos[0] >= 0:
            self.input_matrix.highlight_cell(max_pos[0], max_pos[1], THEME["orange"])

        label = "最大值" if self._anim_pool_type == "max" else "平均值"
        self.step_info.config(text=f"窗口 ({oi},{oj}): {label} = {val}")

        self._anim_step += 1
        self.anim_timer = self.after(450, self._anim_pool_step)

    def _reset(self):
        if self.anim_timer:
            self.after_cancel(self.anim_timer)
            self.anim_timer = None
        self._init_matrices()


# ──────────────────────────────────────────────
# Activation Functions Tab
# ──────────────────────────────────────────────
class ActivationDemo(tk.Frame):
    def __init__(self, parent: tk.Widget, **kwargs):
        super().__init__(parent, bg=THEME["bg"], **kwargs)
        self._build()

    def _build(self):
        title_lbl = tk.Label(self, text="⚡ 激活函数 Activation Functions",
                             font=("Segoe UI", 14, "bold"),
                             fg=THEME["text"], bg=THEME["bg"])
        title_lbl.pack(anchor="w", pady=(0, 15))

        # Create a Canvas for each activation function
        functions = [
            ("ReLU: max(0, x)", self._relu, THEME["accent"]),
            ("Sigmoid: σ(x) = 1/(1+e⁻ˣ)", self._sigmoid, THEME["green"]),
            ("Tanh: tanh(x)", self._tanh, THEME["orange"]),
            ("Leaky ReLU (α=0.01)", self._leaky_relu, THEME["pink"]),
        ]

        for name, fn, color in functions:
            frame = tk.Frame(self, bg=THEME["bg2"], bd=1, relief="solid",
                             highlightbackground=THEME["bg4"])
            frame.pack(fill="x", pady=(0, 10))

            canvas = tk.Canvas(frame, width=700, height=220, bg=THEME["bg2"],
                               highlightthickness=0)
            canvas.pack(padx=10, pady=10)
            self._draw_activation(canvas, fn, color, name)

    def _relu(self, x: float) -> float:
        return max(0, x)

    def _sigmoid(self, x: float) -> float:
        try:
            return 1 / (1 + math.exp(-x))
        except OverflowError:
            return 0 if x < 0 else 1

    def _tanh(self, x: float) -> float:
        return math.tanh(x)

    def _leaky_relu(self, x: float) -> float:
        return x if x > 0 else 0.01 * x

    def _draw_activation(self, canvas: tk.Canvas, fn, color: str, label: str):
        W, H = 700, 220
        pad = 35

        # Grid lines
        canvas.create_line(pad, H // 2, W - pad, H // 2,
                           fill=THEME["text3"], width=1)
        canvas.create_line(W // 2, pad, W // 2, H - pad,
                           fill=THEME["text3"], width=1)

        # Tick labels
        for i in range(-4, 5):
            if i == 0:
                continue
            x = W // 2 + i * (W - 2 * pad) // 8
            canvas.create_text(x, H // 2 + 12, text=str(i),
                               fill=THEME["text3"], font=("Consolas", 8))
            y = H // 2 - i * (H - 2 * pad) // 8
            canvas.create_text(W // 2 - 10, y + 3, text=str(i),
                               fill=THEME["text3"], font=("Consolas", 8))

        canvas.create_text(W // 2 - 10, H // 2 + 12, text="0",
                           fill=THEME["text3"], font=("Consolas", 8))

        # Draw curve
        x_range = 6
        steps = 300
        points = []
        for i in range(steps + 1):
            x_val = -x_range / 2 + (x_range / steps) * i
            y_val = fn(x_val)
            px = pad + ((x_val + x_range / 2) / x_range) * (W - 2 * pad)
            py = H - pad - ((y_val + 3) / 6) * (H - 2 * pad)
            points.extend([px, py])

        canvas.create_line(points, fill=color, width=2.5, smooth=True)

        # Label
        canvas.create_text(pad + 5, pad + 10, text=label, anchor="w",
                           fill=color, font=("Consolas", 10, "bold"))


# ──────────────────────────────────────────────
# CNN Architecture Overview Tab
# ──────────────────────────────────────────────
class ArchitectureOverview(tk.Frame):
    def __init__(self, parent: tk.Widget, **kwargs):
        super().__init__(parent, bg=THEME["bg"], **kwargs)
        self._build()

    def _build(self):
        title_lbl = tk.Label(self, text="🏗️ CNN 网络总体框架",
                             font=("Segoe UI", 14, "bold"),
                             fg=THEME["text"], bg=THEME["bg"])
        title_lbl.pack(anchor="w", pady=(0, 15))

        # Architecture flow diagram using canvas
        canvas = tk.Canvas(self, width=700, height=140, bg=THEME["bg2"],
                           highlightthickness=0, bd=0)
        canvas.pack(pady=10, padx=10, fill="x")

        self._draw_architecture(canvas)

        # Core concepts
        concepts = [
            ("🔍 局部感受野",
             "每个神经元只关注输入的一个局部区域，而非全部像素。这大幅减少了参数数量，并保留了空间结构信息。",
             THEME["accent"]),
            ("🔄 权值共享机制",
             "同一个卷积核在整个输入上滑动，所有位置共享相同的权重参数，使网络具有平移不变性。",
             THEME["green"]),
            ("📊 层次化特征提取",
             "浅层学习边缘、纹理等低级特征，深层学习形状、物体等高级语义特征，逐层抽象。",
             THEME["accent3"]),
        ]

        for title, desc, color in concepts:
            frame = tk.Frame(self, bg=THEME["bg2"], bd=1, relief="solid",
                             highlightbackground=THEME["bg4"])
            frame.pack(fill="x", pady=(0, 8))
            tk.Label(frame, text=title, font=("Segoe UI", 11, "bold"),
                     fg=color, bg=THEME["bg2"]).pack(anchor="w", padx=12, pady=(8, 2))
            tk.Label(frame, text=desc, font=("Segoe UI", 9),
                     fg=THEME["text2"], bg=THEME["bg2"], wraplength=650,
                     justify="left").pack(anchor="w", padx=12, pady=(0, 8))

        # Formula section
        formula_frame = tk.Frame(self, bg=THEME["bg3"], bd=1, relief="solid",
                                 highlightbackground=THEME["bg4"])
        formula_frame.pack(fill="x", pady=(10, 0))

        tk.Label(formula_frame, text="📐 输出尺寸计算公式",
                 font=("Segoe UI", 11, "bold"),
                 fg=THEME["cyan"], bg=THEME["bg3"]).pack(pady=(8, 2))

        tk.Label(formula_frame,
                 text="O = ⌊(I − K + 2P) / S⌋ + 1",
                 font=("Consolas", 14, "bold"),
                 fg=THEME["cyan"], bg=THEME["bg3"]).pack(pady=4)

        tk.Label(formula_frame,
                 text="O = 输出尺寸  I = 输入尺寸  K = 卷积核/池化窗口大小\nP = 填充大小  S = 步幅",
                 font=("Segoe UI", 9),
                 fg=THEME["text3"], bg=THEME["bg3"]).pack(pady=(0, 8))

    def _draw_architecture(self, canvas: tk.Canvas):
        layers = [
            ("Input", THEME["green"]),
            ("Conv", THEME["accent"]),
            ("ReLU", THEME["orange"]),
            ("Pool", THEME["pink"]),
            ("...", THEME["text3"]),
            ("FC", THEME["accent3"]),
            ("Output", THEME["green"]),
        ]

        spacing = 90
        start_x = 30
        y = 60
        r = 18

        for i, (name, color) in enumerate(layers):
            x = start_x + i * spacing
            canvas.create_oval(x - r, y - r, x + r, y + r,
                               fill=THEME["bg3"], outline=color, width=2)
            canvas.create_text(x, y, text=name, fill=color,
                               font=("Segoe UI", 8, "bold"))

            if i < len(layers) - 1:
                nx = start_x + (i + 1) * spacing
                canvas.create_line(x + r, y, nx - r, y,
                                   fill=THEME["accent"], width=2,
                                   arrow="last", arrowshape=(8, 10, 5))


# ──────────────────────────────────────────────
# Main Application
# ──────────────────────────────────────────────
class CNNVisualizerApp:
    def __init__(self, root: tk.Tk):
        self.root = root
        root.title("CNN 卷积神经网络原理交互演示")
        root.geometry("860x780")
        root.configure(bg=THEME["bg"])
        root.minsize(800, 700)

        # Try to set icon
        try:
            root.iconbitmap(default="")
        except Exception:
            pass

        # Style configuration
        self._configure_styles()

        # Main container
        main = tk.Frame(root, bg=THEME["bg"])
        main.pack(fill="both", expand=True, padx=15, pady=10)

        # Notebook (tab control)
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("TNotebook", background=THEME["bg"], borderwidth=0)
        style.configure("TNotebook.Tab",
                        background=THEME["bg3"],
                        foreground=THEME["text2"],
                        padding=[12, 6],
                        font=("Segoe UI", 10, "bold"),
                        borderwidth=0)
        style.map("TNotebook.Tab",
                  background=[("selected", THEME["bg2"]), ("active", THEME["bg4"])],
                  foreground=[("selected", THEME["accent"])])

        notebook = ttk.Notebook(main, style="TNotebook")
        notebook.pack(fill="both", expand=True)

        # Overview tab
        overview = ArchitectureOverview(notebook)
        notebook.add(overview, text="🏗️ 网络框架")

        # Convolution tab
        conv_demo = ConvolutionDemo(notebook)
        notebook.add(conv_demo, text="🔲 卷积层")

        # Pooling tab
        pool_demo = PoolingDemo(notebook)
        notebook.add(pool_demo, text="📊 池化层")

        # Activation tab
        activation = ActivationDemo(notebook)
        notebook.add(activation, text="⚡ 激活函数")

    def _configure_styles(self):
        style = ttk.Style()
        style.configure("TFrame", background=THEME["bg"])
        style.configure("TLabel", background=THEME["bg"], foreground=THEME["text"])
        style.configure("TButton", background=THEME["accent"], foreground="white",
                        borderwidth=0, focusthickness=0)
        style.map("TButton",
                  background=[("active", THEME["accent2"])])

        # Combobox styling
        style.configure("TCombobox",
                        fieldbackground=THEME["bg4"],
                        background=THEME["bg4"],
                        foreground=THEME["text"],
                        selectbackground=THEME["accent"],
                        selectforeground="white",
                        arrowcolor=THEME["text2"],
                        borderwidth=0)
        style.map("TCombobox",
                  fieldbackground=[("readonly", THEME["bg4"])],
                  foreground=[("readonly", THEME["text"])])


def main():
    root = tk.Tk()
    app = CNNVisualizerApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
