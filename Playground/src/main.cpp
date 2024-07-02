#include "playground.hpp"

int main()
{
	std::wstring pipelineCode = LR"(
//Allow inclusion of predefined code
#include <myInclude>
//Allow inclusion of code contained inside user provided file
#include "pipeline/myCustomInclude.lum"

//Definition of an input variable, from the user to the VertexPass
Input -> VertexPass : type variableNameA;
//Definition of a data passed by the vertex pass to the fragment pass
VertexPass -> FragmentPass : type variableNameB;

//Definition of attribute block, who will contain information shared by every triangle of a single call
AttributeBlock pushConstants
{
	type variableNameC;
};

//Definition of a constant block, who will contain information shared by every triangle of every call.
ConstantBlock constants
{
	type variableNameD;
};

//Description of Sampler2D.
//Texture is suppose to be concidered as a "struct" with the following method :
// - int myTexture.size.width -> represent the width in pixel of the texture
// - int myTexture.size.height -> represent the height in pixel of the texture
// - vec2 myTexture.convertUVToPixel(ivec2 pixelCoord) -> convert a coordinate as pixels into a UV coordinate
// - ivec2 myTexture.convertUVToPixel(vec2 uvCoord) -> convert a coordinate as UVs into a pixel coordinate
// - Color myTexture.pixel(vec2 coord) -> request a pixel at coordinate "coord" expressed between 0 and 1
// - Color myTexture.fetch(ivec2 coord) -> request a pixel at coordinate "coord" express between [0 / 0] and [width / height]
Texture myTexture;

//Creation of a new function, names [functionName], returning a [returnType], and taking as input a [type]
returnType functionName(type inputVariableNameA)
{
	returnType result;

	return (result);
}

//Definition of the vertex pass, who will be responsible to fill the value "pixelPosition" and pass the value desired to the fragment pass
VertexPass()
{
	vec4 finalPixelPosition = vec4(0, 0, 0, 1);

	//Definition of a value with a constructor like in glsl
	variableNameB = type(10);

	//Add the content of the debug to the debug log of the pipeline, readable using the "debugLog" method of the Pipeline object
	//This line may be writen inside the shader a large number of time. 
	debug("Variable B = " + variableNameB + " elements");

	if (variableNameB == 5)
	{
		//raiseException is a build-in function that will raise an error, who will throw an exception inside your program with the desired error message
		//The exception raised will be a spk::PipelineException, and will contain the line and function who raised the exception in addition of the string passed as parameter to raiseException
		raiseException("Custom pipeline failed with variable B == " + variableNameB + "!");
	}

	//pixelPosition is a key word of Lumina, who will describe the pixel on screen to edit, express between -1 and 1 on both axis.
	//Its type is vec4
	pixelPosition = finalPixelPosition;
}

FragmentPass()
{
	vec4 finalPixelColor = vec4(0, 1, 0, 1);

	//pixelColor is a key word of Lumina, who will describe the pixel color on screen to edit, express between 0 and 1 on both axis.
	//Its type is vec4
	pixelColor = finalPixelColor;
}
)";

	return (0);
}