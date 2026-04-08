# MBIN Binary Format Visualization

## Complete File Layout (Byte-by-Byte)

```
OFFSET (Bytes)  SIZE    TYPE            DESCRIPTION
═══════════════════════════════════════════════════════════════════════════

0               4       char[4]         Magic Number: "MBIN"
4               4       uint32_t        Version: 1

8               4       uint32_t        Input Binding Count (N)

                        ┌─── BINDING SECTION (repeats N times) ───┐
                        │
12+offset       4       uint32_t        Action Name Length (M)
16+offset       M       char[M]         Action Name String
16+M+offset     1       uint8_t         Device Type (0=KB, 1=GP)
17+M+offset     4       SDL_Keycode     Keyboard Key Code (if type=0)
                  or
17+M+offset     4       int             Gamepad Index (if type=1)
21+M+offset     1       uint8_t         Gamepad Button (if type=1)
                        │
                        └──────────────────────────────────────────┘

(Next binding offset calculated based on actual data)

                4       uint32_t        GameObject Count (G)

                        ┌─── GAMEOBJECT SECTION (repeats G times) ┐
                        │
                4       uint32_t        GameObject ID
                4       uint32_t        Name Length (K)
                K       char[K]         Name String
                4       int32_t         Parent ID (-1 = root)
                4       float           World Position X
                4       float           World Position Y
                1       bool            Is Debug Data
                4       uint32_t        Component Count (C)

                        ┌─ COMPONENT SECTION (repeats C times) ──┐
                        │
                4       uint32_t        Component Type Length
                L1      char[L1]        Component Type String
                4       uint32_t        Component Name Length
                L2      char[L2]        Component Name String
                4       uint32_t        Property Count (P)

                        ┌ PROPERTY SECTION (repeats P times) ───┐
                        │
                4       uint32_t        Property Key Length
                K1      char[K1]        Property Key String
                4       uint32_t        Property Value Length
                V1      char[V1]        Property Value String
                        │
                        └───────────────────────────────────────┘
                        │
                        └─────────────────────────────────────────┘
                        │
                        └──────────────────────────────────────────┘
```

## Binary Representation Example

### Minimal Scene (Single Object, No Components)

```
Offset  Hex Data    ASCII   Description
──────────────────────────────────────────────────────────
0x00    4D 42 49 4E         "MBIN"              (Magic)
0x04    01 00 00 00         1                   (Version)
0x08    00 00 00 00         0                   (No bindings)
0x0C    01 00 00 00         1                   (1 GameObject)
0x10    01 00 00 00         1                   (ID)
0x14    04 00 00 00         4                   (Name length)
0x18    43 75 62 65         "Cube"              (Name)
0x1C    FF FF FF FF         -1                  (Parent ID = root)
0x20    00 00 00 00         0.0                 (Pos X)
0x24    00 00 00 00         0.0                 (Pos Y)
0x28    00                  false               (Not debug)
0x29    00 00 00 00         0                   (No components)
```

### Scene with Input Binding and Component

```
Offset  Hex Data    ASCII   Description
──────────────────────────────────────────────────────────
0x00    4D 42 49 4E         "MBIN"              (Magic)
0x04    01 00 00 00         1                   (Version)

(Input Bindings)
0x08    01 00 00 00         1                   (1 Binding)
0x0C    04 00 00 00         4                   (Binding name length)
0x10    4A 75 6D 70         "Jump"              (Binding name)
0x14    00                  0                   (Device = Keyboard)
0x15    20 01 00 00         288 (SDLK_SPACE)    (Key code)

(GameObject)
0x19    01 00 00 00         1                   (1 Object)
0x1D    01 00 00 00         1                   (Object ID)
0x21    04 00 00 00         4                   (Name length)
0x25    50 6C 61 79         "Play"              (Name)
0x29    FF FF FF FF         -1                  (Parent = root)
0x2D    00 00 80 3F         1.0                 (Pos X)
0x31    00 00 40 40         3.0                 (Pos Y)
0x35    01                  true                (Is debug)
0x36    01 00 00 00         1                   (1 Component)

(Component)
0x3A    15 00 00 00         21                  (Type length)
0x3E    54 65 78 74 52...   "TextRenderComponent" (Type)
0x53    04 00 00 00         4                   (Name length)
0x57    54 65 78 74 30      "Text0"             (Component name)
0x5C    02 00 00 00         2                   (2 Properties)

(Property 1)
0x60    04 00 00 00         4                   (Key length)
0x64    74 65 78 74         "text"              (Key)
0x68    05 00 00 00         5                   (Value length)
0x6C    48 65 6C 6C 6F      "Hello"             (Value)

(Property 2)
0x71    05 00 00 00         5                   (Key length)
0x75    63 6F 6C 6F 72      "color"             (Key)
0x7A    0B 00 00 00         11                  (Value length)
0x7E    32 35 35 2C 30...   "255,0,0,255"       (Value = Red)
```

## Memory Layout of Complex Hierarchy

```
Scene: 
  ├─ Player (ID=1, Parent=-1)
  │  ├─ Weapon (ID=2, Parent=1)
  │  └─ Shield (ID=3, Parent=1)
  └─ Enemy (ID=4, Parent=-1)
     └─ Loot (ID=5, Parent=4)

File Order (flat):
[Header][Bindings][Player][Weapon][Shield][Enemy][Loot]
                            ↑       ↑      ↑      ↑    ↑
                          /─┴───────┤      │      │    │
                        Parent ID=1 │      │      │    │
                                    └──────┘      │    │
                                    Parent ID=1   │    │
                                                  └────┘
                              Parent ID=4, Parent ID=4
```

## String Encoding

All strings use **Length-Prefixed UTF-8**:

```
[4-byte uint32_t length][UTF-8 bytes...]

Examples:
"Hello"      → 05 00 00 00  48 65 6C 6C 6F
""           → 00 00 00 00  (empty string)
"Test™"      → 06 00 00 00  54 65 73 74 E2 84 A2 (UTF-8 encoded)
```

## Component Property Examples

### TextRenderComponent
```
Properties Map:
├─ "text" → "Hello World"
├─ "color" → "255,255,255,255"
└─ "fontSize" → "16"
```

### TransformComponent (if added)
```
Properties Map:
├─ "localX" → "0.0"
├─ "localY" → "0.0"
├─ "rotation" → "0.0"
├─ "scaleX" → "1.0"
└─ "scaleY" → "1.0"
```

### TextureRenderComponent
```
Properties Map:
├─ "textureName" → "player.png"
├─ "opacity" → "1.0"
└─ "flipX" → "false"
```

## Alignment and Padding

- **No automatic padding** between fields
- All values are tightly packed
- Strings use length prefix (no null termination)
- Supports any binary data (no endianness concerns for strings)
- Endianness: Little-endian (standard for PC)

## File Size Calculation

```
Base Size = 8 bytes (header)

For each binding:
  + 4 (count) 
  + 4 + strlen (name)
  + 1 (device type)
  + 4 or 5 (device data: keycode=4, gamepad index=4, button=1)

For each GameObject:
  + 4 + 4 (ID)
  + 4 + strlen (name)
  + 4 (parent ID)
  + 4 + 4 (position X, Y)
  + 1 (debug flag)
  + 4 (component count)

For each Component:
  + 4 + strlen (type)
  + 4 + strlen (name)
  + 4 (property count)
  + For each property:
    + 4 + strlen (key)
    + 4 + strlen (value)
```

## Versioning Strategy

Current: Version 1

Future versions can:
- Add new fields at the end of sections
- Add new object types in GameObject section
- Extend component data format
- Add metadata section before bindings

Backwards compatibility maintained by:
- Never changing existing field positions in version 1
- Adding new features in new sections or later versions
- Always validating version before parsing
