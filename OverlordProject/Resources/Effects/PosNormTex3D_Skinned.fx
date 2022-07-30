float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

float4x4 gBones[100];

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output = (VS_OUTPUT)0;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	//output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	//// Step 2:	rotate the normal: NO TRANSLATION
	////			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	////			thus removing the postion row of the matrix
	//output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	//output.texCoord = input.texCoord;
	//return output;

	float4 transPos = (float4)0;
	float3 transNormal = (float3)0;

	for (int i = 0; i < 4; ++i)
	{
		int index = input.blendIndices[i];
		if (index >= 0)
		{
			float4 boneSpacePos = mul(float4(input.pos, 1.0f), gBones[index]);
			float3 boneSpaceNormal = normalize(mul(input.normal, (float3x3)gBones[index]));

			transPos += boneSpacePos * input.blendWeights[i];
			transNormal += boneSpaceNormal * input.blendWeights[i];
		}

	}

	transPos.w = 1.0f;
	transPos = mul(transPos, gWorldViewProj);
	transNormal = normalize(mul(transPos, (float3x3)gWorld));

	output.pos = transPos;
	output.normal = transNormal;
	output.texCoord = input.texCoord;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

