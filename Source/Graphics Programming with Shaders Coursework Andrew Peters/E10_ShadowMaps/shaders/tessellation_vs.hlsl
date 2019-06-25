// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(cb1)
{
	float time;
	float padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;


	float height = 10.0f;

	input.position.w = 1.0f;

	//offset the values absed on the colour of the texture
	float4 textureColor = (0.0f, 0.0f, 0.0f, 1.0f);
	textureColor = shaderTexture.SampleLevel(SampleType, input.tex, 0);
	input.position.y += height* textureColor.x;


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = input.position;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = input.normal;

	//// Normalize the normal vector.
	//output.normal = normalize(output.normal);

    // Pass the input color into the hull shader.
    //output.colour = float4(1.0, 0.0, 0.0, 1.0);
    
    return output;
}
