#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

namespace PipelineUtils
{
	struct Instruction
	{
		enum class Type
		{
			Include,
			Flow,
			Structure,
			Function,
			Texture,
			AttributeBlock,
			ConstantBlock,
			NamespaceStart,
			NamespaceEnd,
			VertexPass,
			FragmentPass,
			Exception
		};

		Type type;
		std::string content;

		void cleanUp()
		{

		}

		friend std::ostream& operator << (std::ostream& p_os, const Instruction::Type& p_type)
		{
			switch (p_type)
			{
			case Instruction::Type::Include:
				p_os << "Include"; return (p_os);
			case Instruction::Type::Flow:
				p_os << "Flow"; return (p_os);
			case Instruction::Type::Structure:
				p_os << "Structure"; return (p_os);
			case Instruction::Type::Function:
				p_os << "Function"; return (p_os);
			case Instruction::Type::Texture:
				p_os << "Texture"; return (p_os);
			case Instruction::Type::AttributeBlock:
				p_os << "AttributeBlock"; return (p_os);
			case Instruction::Type::ConstantBlock:
				p_os << "ConstantBlock"; return (p_os);
			case Instruction::Type::NamespaceStart:
				p_os << "NamespaceStart"; return (p_os);
			case Instruction::Type::NamespaceEnd:
				p_os << "NamespaceEnd"; return (p_os);
			case Instruction::Type::VertexPass:
				p_os << "VertexPass"; return (p_os);
			case Instruction::Type::FragmentPass:
				p_os << "FragmentPass"; return (p_os);
			case Instruction::Type::Exception:
				p_os << "Exception"; return (p_os);
			default:
				p_os << "Unknow"; return (p_os);
			}
		}

		friend std::ostream& operator << (std::ostream& p_os, const Instruction& p_instruction)
		{
			p_os << "[" << std::setw(15) << p_instruction.type << "] - " << p_instruction.content;

			return (p_os);
		}
	};
};

class Pipeline
{
public:
	Pipeline(const std::string& p_inputCode)
	{
		
	}
};

// In your main function, you can now create a Pipeline object and print its instructions.
int main()
{
	// Example Lumina code as a string (in practice, you would read this from a file)
	std::string luminaCode = R"(
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
)";

	Pipeline pipeline(luminaCode);

	return 0;
}

