# EXPOSE_TO_EDITOR Annotation Guide

## Syntax

Place a comment above a member variable to expose it to the editor:

```cpp
// EXPOSE_TO_EDITOR
float speed = 1.0f;

// EXPOSE_TO_EDITOR("Display Name", "Optional tooltip")
int health = 100;
```

## Supported Types

- Primitives: `int`, `float`, `double`, `bool`
- Containers: `std::string`
- Math: `glm::vec2`, `glm::vec3`, `glm::vec4`
- Enums

## Notes

- Annotation must be on the line immediately before the member declaration
- Member variable names with `m_` prefix are automatically cleaned for property names
- Getters and setters are auto-generated for the editor

