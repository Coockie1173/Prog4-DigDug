import tkinter as tk
from tkinter import filedialog
import struct

GRID_W = 16
GRID_H = 18
CELL = 32

carved = set()
connections = []
enemies = []

mode = "carve"
selected_connection = None


def cell_from_mouse(event):
    return event.x // CELL, event.y // CELL


def redraw():
    canvas.delete("all")

    for y in range(GRID_H):
        for x in range(GRID_W):
            color = "white"
            if (x, y) in carved:
                color = "lightgreen"

            canvas.create_rectangle(
                x * CELL,
                y * CELL,
                (x + 1) * CELL,
                (y + 1) * CELL,
                fill=color,
                outline="gray",
            )

    for (a, b) in connections:
        x1 = a[0] * CELL + CELL // 2
        y1 = a[1] * CELL + CELL // 2
        x2 = b[0] * CELL + CELL // 2
        y2 = b[1] * CELL + CELL // 2
        canvas.create_line(x1, y1, x2, y2, width=3, fill="blue")

    for typ, pos in enemies:
        x = pos[0] * CELL + CELL // 2
        y = pos[1] * CELL + CELL // 2
        color = "red" if typ == 0 else "orange"
        canvas.create_oval(
            x - 8,
            y - 8,
            x + 8,
            y + 8,
            fill=color,
        )


def click(event):
    global selected_connection

    cell = cell_from_mouse(event)

    if cell[0] >= GRID_W or cell[1] >= GRID_H:
        return

    if mode == "carve":
        if cell in carved:
            carved.remove(cell)
        else:
            carved.add(cell)

    elif mode == "connection":
        if selected_connection is None:
            selected_connection = cell
        else:
            if selected_connection != cell:
                connections.append((selected_connection, cell))
            selected_connection = None

    elif mode == "pooka":
        enemies.append((0, cell))

    elif mode == "fygar":
        enemies.append((1, cell))

    redraw()


def set_mode(m):
    global mode, selected_connection
    mode = m
    selected_connection = None


def save():
    filename = filedialog.asksaveasfilename(
        defaultextension=".tdbin",
        filetypes=[("Terrain Binary", "*.tdbin")],
    )

    if not filename:
        return

    with open(filename, "wb") as f:
        f.write(struct.pack("<H", len(carved)))

        for x, y in carved:
            f.write(struct.pack("<HH", x, y))

        f.write(struct.pack("<H", len(connections)))

        for (a, b) in connections:
            f.write(
                struct.pack(
                    "<HHHH",
                    a[0],
                    a[1],
                    b[0],
                    b[1],
                )
            )

        f.write(struct.pack("<H", len(enemies)))

        for typ, pos in enemies:
            f.write(
                struct.pack(
                    "<BHH",
                    typ,
                    pos[0],
                    pos[1],
                )
            )


root = tk.Tk()
root.title("Terrain .tdbin Editor")

toolbar = tk.Frame(root)
toolbar.pack()

tk.Button(toolbar, text="Carve", command=lambda: set_mode("carve")).pack(side=tk.LEFT)
tk.Button(toolbar, text="Connection", command=lambda: set_mode("connection")).pack(side=tk.LEFT)
tk.Button(toolbar, text="Pooka", command=lambda: set_mode("pooka")).pack(side=tk.LEFT)
tk.Button(toolbar, text="Fygar", command=lambda: set_mode("fygar")).pack(side=tk.LEFT)
tk.Button(toolbar, text="Save", command=save).pack(side=tk.LEFT)

canvas = tk.Canvas(
    root,
    width=GRID_W * CELL,
    height=GRID_H * CELL,
    bg="white",
)
canvas.pack()

canvas.bind("<Button-1>", click)

redraw()

root.mainloop()