// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColour[3];
    float4 diffuseColour[3];
	float4 position[3];

};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir[3];
    float lightIntensity[3];
    float4 color[3];
	float4 endColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	//default amount of colour
	color = ambientColour;

	for (int i = 0; i < 3; i++)
	{
		// Invert the light direction for calculations.
		lightDir[i] = normalize(input.position3D - position[i]);

		// Calculate the amount of light on this pixel.
		lightIntensity[i] = saturate(dot(input.normal, -lightDir[i]));

		if (lightIntensity[i] > 0.0f)
		{
			color[i] += (diffuseColour[i] * lightIntensity[i]);
			color[i] = saturate(color[i]);
		}
	}


    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	endColor = saturate(color[0] + color[1] + color[2]) * textureColor;
	
    return endColor;
}

