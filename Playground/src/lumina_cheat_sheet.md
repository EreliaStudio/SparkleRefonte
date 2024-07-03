# Lumina Language Cheat Sheet

## General Overview
Lumina is a wrapper around GLSL behavior, designed to simplify the creation of shader programs. It introduces a structured syntax for defining inputs, outputs, structures, uniform blocks, and shader stages.

## Syntax Elements

### Includes
- Include predefined or user-provided files.
```cpp
#include <predefinedInclude>
#include "shader/userProvidedInclude.lum"
```

### Pipeline Flow
- Define data flow between shader stages.
- Data passed thought flow must remain native data, such as vec2, ivec2, float, int, etc
```cpp
Input -> VertexPass: float variableNameA;
VertexPass -> FragmentPass: vec3 variableNameB;
```

### Structures
- Define custom structures to group variables.
```cpp
struct StructName
{
    float variableA = 1.0;
    vec3 variableB = vec3(0.0, 1.0, 0.0);
    vec4 variableC;
};
```

### Functions
- Define functions with return types and input parameters.
```cpp
returnType functionName(type inputVariableName)
{
    returnType result;
    return result;
}
```
- Example:
```cpp
vec4 myFunction(float inputVariable)
{
    vec4 result = vec4(inputVariable);
    return result;
}
```

### Texture
- Define a texture with methods for manipulation and querying.
```cpp
Texture textureName;
```
- Methods:
    - `int textureName.size.width`
    - `int textureName.size.height`
    - `vec2 textureName.convertUVToPixel(ivec2 pixelCoord)`
    - `ivec2 textureName.convertUVToPixel(vec2 uvCoord)`
    - `Color textureName.pixel(vec2 coord)`
    - `Color textureName.fetch(ivec2 coord)`

### Attribute Blocks
- Define uniform blocks for information shared by every triangle in a single call.
```cpp
AttributeBlock attributeBlockName
{
    int variableName = 5;
};
```

### Constant Blocks
- Define uniform blocks for information shared across all calls.
```cpp
ConstantBlock constantBlockName
{
    float variableName;
};
```

### Namespaces
- Create namespaces to scope functions, structures, constants block, attributes block and texture.
```cpp
namespace NamespaceName
{
    struct NestedStruct
    {
        float value;
    };

    vec4 namespaceFunction(float input)
    {
        vec4 result = vec4(input);
        return result;
    }

    namespace NestedNamespace
    {
        struct InnerStruct
        {
            int innerValue;
        };

        int nestedFunction(int input)
        {
            return input * 2;
        }
    }
}
```
- Usage: 
  - `NamespaceName::NestedStruct`
  - `NamespaceName::namespaceFunction`
  - `NamespaceName::NestedNamespace::InnerStruct`
  - `NamespaceName::NestedNamespace::nestedFunction`

### Shader Stages

#### Vertex Pass
- Define the vertex shader stage.
```cpp
VertexPass()
{
    vec4 finalPixelPosition = vec4(0, 0, 0, 1);
    variableNameB = vec3(10);

    if (variableNameB.x == 5.0)
    {
        raiseException("Custom pipeline failed with variable B == " + variableNameB + "!");
    }

    pixelPosition = finalPixelPosition;
}
```

#### Fragment Pass
- Define the fragment shader stage.
```cpp
FragmentPass()
{
    vec4 finalPixelColor = vec4(0, 1, 0, 1);
    pixelColor = finalPixelColor;
}
```

## Error Handling
- Use `raiseException` to throw exceptions with custom error messages.
- You can customize your error message using values from the shader
```cpp
raiseException("Error message");
raiseException("Error : myValue(" + myValue + ") != myExpectedValue(" + myExpectedValue + ")");
```

## Commenting

Lumina supports both single-line and multi-line comments for documenting your code, explaining complex logic, or temporarily disabling code.

### Single-Line Comments
- Use `//` for single-line comments. Everything following `//` on the same line is considered a comment.

```cpp
// This is a single-line comment
vec3 color = vec3(1.0, 0.0, 0.0); // This comment is on the same line as code
```

### Multi-Line Comments
- Use `/*` to start a multi-line comment and `*/` to end it. This allows you to comment out multiple lines of code or write longer explanations.

```cpp
/*
This is a multi-line comment.
It spans multiple lines.
Useful for detailed explanations or
temporarily disabling large blocks of code.
*/
vec3 color = vec3(1.0, 0.0, 0.0); /* Multi-line comments can also be 
                                    used to comment on parts of code */
```

## Example Code

Here is an example of a complete Lumina pipeline code:

```cpp
// Include predefined and user-provided files
#include <screenConstants>
#include "shader/customInclude.lum"

// Define the pipeline flow
Input -> VertexPass: vec3 vertexPosition;
Input -> VertexPass: vec3 vertexNormal;
Input -> VertexPass: vec2 vertexUV;
VertexPass -> FragmentPass: vec3 fragPosition;
VertexPass -> FragmentPass: vec3 fragNormal;
VertexPass -> FragmentPass: vec2 fragUV;

// Define custom structures
struct Material
{
    vec3 diffuseColor = vec3(1.0, 0.5, 0.3);
    vec3 specularColor = vec3(1.0);
    float shininess = 32.0;
};

// Define a texture
Texture diffuseTexture;

// Define attribute blocks
AttributeBlock modelAttributes
{
    mat4 modelMatrix;
    mat4 normalMatrix;
};

// Define constant blocks
ConstantBlock lightingConstants
{
    vec3 lightPosition = vec3(10.0, 10.0, 10.0);
    vec3 lightColor = vec3(1.0);
    float ambientIntensity = 0.1;
};

// Create namespaces
namespace Lighting
{
    vec3 calculateDiffuse(vec3 normal, vec3 lightDir, vec3 lightColor)
    {
        float diff = max(dot(normal, lightDir), 0.0);
        return diff * lightColor;
    }

    vec3 calculateSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess, vec3 lightColor)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        return spec * lightColor;
    }
}

// Define the vertex shader stage
VertexPass()
{
    vec4 worldPosition = modelAttributes.modelMatrix * vec4(vertexPosition, 1.0);
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(modelAttributes.normalMatrix) * vertexNormal;
    fragUV = vertexUV;

    vec4 clipSpacePosition = screenConstants.canvasMVP * worldPosition;
    pixelPosition = clipSpacePosition;
}

// Define the fragment shader stage
FragmentPass()
{
    // Error handling example
    if (lightingConstants.ambientIntensity < 0.0 || lightingConstants.ambientIntensity > 1.0)
    {
        raiseException("Ambient intensity must be between 0 and 1. Current value: " + lightingConstants.ambientIntensity);
    }

    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightingConstants.lightPosition - fragPosition);
    vec3 viewDir = normalize(-fragPosition);

    vec3 ambient = lightingConstants.ambientIntensity * lightingConstants.lightColor;

    vec3 diffuse = Lighting::calculateDiffuse(normal, lightDir, lightingConstants.lightColor);

    Material material;
    vec3 specular = Lighting::calculateSpecular(normal, lightDir, viewDir, material.shininess, lightingConstants.lightColor);

    vec3 finalColor = ambient + diffuse * material.diffuseColor + specular * material.specularColor;

    vec4 textureColor = diffuseTexture.pixel(fragUV);
    finalColor *= textureColor.rgb;

    if (textureColor.a == 0)
    {
        discard;
    }

    pixelColor = vec4(finalColor, textureColor.a);
}


```

This cheat sheet covers the essential elements of the Lumina language to help you create and manage shaders efficiently. Happy coding!