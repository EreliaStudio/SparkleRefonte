# Build-In features

## Standard Types
Each of the standard type respect the standard C++ data type for the scalar type, and the Sparkle type for vector and matrix types.
### Scalar Types
- `float`: Single precision floating-point number.
- `int`: Signed integer.
- `uint`: Unsigned integer.
- `bool`: Boolean type.

### Vector Types
- `Vector2`: Vector of 2 single precision floating-point numbers.
- `Vector3`: Vector of 3 single precision floating-point numbers.
- `Vector4`: Vector of 4 single precision floating-point numbers.
- `Vector2Int`: Vector of 2 signed integers.
- `Vector3Int`: Vector of 3 signed integers.
- `Vector4Int`: Vector of 4 signed integers.
- `Vector2UInt`: Vector of 2 unsigned integers.
- `Vector3UInt`: Vector of 3 unsigned integers.
- `Vector4UInt`: Vector of 4 unsigned integers.

### Matrix Types
- `Matrix2x2`: 2x2 matrix of single precision floating-point numbers.
- `Matrix3x3`: 3x3 matrix of single precision floating-point numbers.
- `Matrix4x4`: 4x4 matrix of single precision floating-point numbers.

## Functions

### raiseException
- Use `raiseException` to throw exceptions with custom error messages.
- You can customize your error message using values from the shader
```cpp
raiseException("Error message");
raiseException("Error : myValue(" + myValue + ") != myExpectedValue(" + myExpectedValue + ")");
```

## Structures

### Size
- Define a size, represented by a width and a height
- Attributes:
    - `int mySize.width`
    - `int mySize.height`

### Texture
- Define a texture with methods for manipulation and querying.
- Attributes:
    - `Size textureName.size`
