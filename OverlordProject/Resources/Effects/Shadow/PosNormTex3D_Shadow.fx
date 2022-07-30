float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

float gAmbientColor = 0.5f;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

BlendState DisableBlend
{
	BlendEnable[0] = FALSE;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	//TODO: complete Vertex Shader
	output.pos = mul(float4(input.pos, 1.f), gWorldViewProj);
	output.normal = mul(input.normal, gWorld);
	output.texCoord = input.texCoord;

	output.lPos = mul(float4(input.pos, 1.f), gWorldViewProj_Light);
	//Hint: Don't forget to project our position to light clip space and store it in lPos
	
	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2((float)u * (1.0f / 1280.0f), (float)v * (1.0f / 720.0f));
}

float EvaluateShadowMap(float4 lpos)
{
	//PCF
	float sum = 0.f;
	float x, y;

	//4x4 texel
	for (y = -1.5f; y <= 1.5f; y += 1.0f)
	{
		for (x = -1.5f; x <= 1.5f; x += 1.0f)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
		}
	}

	float shadowValue = sum / 16.f;
	return shadowValue;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuseColor = gDiffuseMap.Sample(samLinear,input.texCoord);

	float shadowValue = 0.f;

	//re-homogenize position after interpolation
	input.lPos.xyz /= input.lPos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (input.lPos.x <= -1.0f || input.lPos.x >= 1.0f ||
		input.lPos.y <= -1.0f || input.lPos.y >= 1.0f ||
		input.lPos.z <= 0.0f || input.lPos.z >= 1.0f)
	{

		return float4(diffuseColor * gAmbientColor);
	}


	//transform clip space coords to texture space coords (-1:1 to 0:1)
	input.lPos.x = input.lPos.x / 2 + 0.5;
	input.lPos.y = input.lPos.y / -2 + 0.5;

	input.lPos.z -= gShadowMapBias;

	//sample shadow map - point sampler
	shadowValue = EvaluateShadowMap(input.lPos);

	if (shadowValue <= input.pos.z)
	{
		return float4(diffuseColor * gAmbientColor);
	}
	
	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;

	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;


	return float4( color_rgb * shadowValue , color_a );
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
		SetBlendState(DisableBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

