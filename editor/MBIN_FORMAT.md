# MBIN File Format Specification

## Overview
MBIN (Minigin Binary) is a binary serialization format for storing scene data including GameObjects, Components, and Input Bindings.

## File Structure

```
┌─────────────────────────────────────────────────────────────────────┐
│                         MBIN FILE STRUCTURE                          │
└─────────────────────────────────────────────────────────────────────┘

┌──────────┐
│  HEADER  │
├──────────┼────────────────────────────────────────────────────────────┐
│ Magic    │ "MBIN" (4 bytes) - File format identifier                 │
│ Version  │ uint32_t (4 bytes) - Format version (currently 1)         │
└──────────┴────────────────────────────────────────────────────────────┘

┌────────────────────────────────────┐
│   INPUT BINDINGS SECTION           │
├────────────────────────────────────┤
│ Binding Count │ uint32_t           │ Number of input bindings
├────────────────────────────────────┤
│ For each binding (count times):     │
│  ┌─────────────────────────────────┐│
│  │ Action Name        │ string      ││ e.g., "Jump", "Attack"
│  ├─────────────────────────────────┤│
│  │ Device Type        │ uint8_t     ││ 0=Keyboard, 1=Gamepad
│  ├─────────────────────────────────┤│
│  │ If Keyboard:                    ││
│  │  └─ Key Code       │ SDL_Keycode ││ Keyboard key identifier
│  ├─────────────────────────────────┤│
│  │ If Gamepad:                     ││
│  │  ├─ Gamepad Index  │ int        ││ Gamepad/Controller number
│  │  └─ Button Index   │ uint8_t    ││ Button identifier (0=A,1=B,etc)
│  └─────────────────────────────────┘│
└────────────────────────────────────┘

┌────────────────────────────────────┐
│     GAMEOBJECTS SECTION            │
├────────────────────────────────────┤
│ Object Count   │ uint32_t           │ Number of GameObjects
├────────────────────────────────────┤
│ For each object (count times):      │
│  ┌─────────────────────────────────┐│
│  │ ID               │ uint32_t      ││ Unique GameObject ID
│  ├─────────────────────────────────┤│
│  │ Name             │ string        ││ GameObject name
│  ├─────────────────────────────────┤│
│  │ Parent ID        │ int32_t       ││ -1 if no parent, else parent ID
│  ├─────────────────────────────────┤│
│  │ World Position X │ float         ││ X coordinate in world space
│  ├─────────────────────────────────┤│
│  │ World Position Y │ float         ││ Y coordinate in world space
│  ├─────────────────────────────────┤│
│  │ Is Debug Data    │ bool          ││ Debug flag for the object
│  ├─────────────────────────────────┤│
│  │ Component Count  │ uint32_t      ││ Number of attached components
│  ├─────────────────────────────────┤│
│  │ For each component:              ││
│  │  ┌──────────────────────────────┐││
│  │  │ Component Type   │ string     │││ e.g., "TextRenderComponent"
│  │  ├──────────────────────────────┤││
│  │  │ Component Name   │ string     │││ Unique component instance name
│  │  ├──────────────────────────────┤││
│  │  │ Properties Count │ uint32_t   │││ Number of properties
│  │  ├──────────────────────────────┤││
│  │  │ For each property:            │││
│  │  │  ├─ Key          │ string     │││ Property name
│  │  │  └─ Value        │ string     │││ Property value (stored as string)
│  │  └──────────────────────────────┘││
│  └─────────────────────────────────┘│
└────────────────────────────────────┘
```

## Detailed Data Type Specifications

### String Format
```
┌────────────┬─────────────────────────┐
│ Length     │ uint32_t (4 bytes)      │
├────────────┼─────────────────────────┤
│ Data       │ char[] (Length bytes)   │
└────────────┴─────────────────────────┘
```
- Length = 0: Empty string
- Length > 0: Read Length bytes as UTF-8 string

### Input Binding Format

#### Keyboard Binding
```
┌──────────────┬──────────┐
│ Device Type  │ 0 (uint8_t)  │
├──────────────┼──────────┤
│ Key Code     │ SDL_Keycode  │
└──────────────┴──────────┘
```

#### Gamepad Binding
```
┌──────────────┬──────────┐
│ Device Type  │ 1 (uint8_t)  │
├──────────────┼──────────┤
│ Gamepad Index│ int          │
├──────────────┼──────────┤
│ Button Index │ uint8_t      │
└──────────────┴──────────┘

Button Index Mapping:
0 = A
1 = B
2 = X
3 = Y
4 = DPadUp
5 = DPadDown
6 = DPadLeft
7 = DPadRight
8 = Start
9 = Back
10 = LeftShoulder
11 = RightShoulder
```

### GameObject Structure
```
GameObjects form a hierarchy via Parent ID references:
- Root objects have Parent ID = -1
- Child objects reference their parent's ID
- All objects stored in flat list (hierarchy reconstructed via parent references)
```

### Component Storage
```
Components are stored per-GameObject:
- Each component has a type (string) identifying the component class
- Properties are stored as a map of key-value string pairs
- Values can represent different types (stored as serialized strings):
  * Numbers (int, float) -> string representation
  * Text (color codes as "R,G,B,A") -> comma-separated string
  * Filenames -> direct string
```

## Example Data Flow

### Saving a Scene
```
1. User clicks File → Save
2. SDL3 native file picker opens
3. User selects location and confirms
4. For each input binding:
   - Write action name string
   - Write device type + controls
5. For each GameObject in scene:
   - Write ID, name, parent ID
   - Write position (X, Y as floats)
   - Write debug flag
   - For each component:
     - Write type, name
     - Write properties map
6. File written to disk
7. Success notification displayed
```

### Loading a Scene
```
1. User clicks File → Load
2. SDL3 native file picker opens
3. User selects .mbin file and confirms
4. File validation (magic number, version)
5. Read all input bindings into editor
6. Create GameObjects with hierarchy:
   - Root objects created first (parent ID = -1)
   - Child objects linked to parents
7. Add components with properties
8. Scene reconstructed in editor
9. Success notification displayed
```

## Version History

### Version 1 (Current)
- Initial format
- Input bindings (keyboard and gamepad)
- GameObject hierarchy with transforms
- Component system with string properties

## Future Extensions

Potential fields that could be added without breaking compatibility:
- Transform rotation/scale
- Component-specific binary data (not just strings)
- Layer/tag system
- Physics data
- Prefab references
- Animation states
