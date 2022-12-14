float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
Texture2D gTexture;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float2 texCoord : TEXCOORD;

};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 screenPos : TEXCOORD1;
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

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	float4 pos = float4(input.pos, 1.f);
	pos = (mul(pos, gWorldViewProj));

	output.pos = pos;
	// Set the TexCoord
	output.texCoord = input.texCoord;
	output.screenPos = output.pos;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
	float2 ssUV = (float2)0;
	ssUV.x = input.screenPos.x / input.screenPos.w / 2.0f + 0.5f;
	ssUV.y = -input.screenPos.y / input.screenPos.w / 2.0f + 0.5f;

	float3 sample = gTexture.Sample(samLinear, ssUV);
	return float4(sample, 1.f);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
	pass P0
	{
		// Set states...
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

technique11 TransparencyTech
{
	pass P0
	{
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}