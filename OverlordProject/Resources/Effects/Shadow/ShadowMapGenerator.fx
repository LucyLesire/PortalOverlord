float4x4 gWorld;
float4x4 gLightViewProj;
float4x4 gBones[100];
 
DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS(float3 position:POSITION) :SV_POSITION
{
	//TODO: return the position of the vertex in correct space (hint: seen from the view of the light)
	return mul(float4(position, 1.f), mul(gWorld, gLightViewProj));
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS) : SV_POSITION
{
	//TODO: return the position of the ANIMATED vertex in correct space (hint: seen from the view of the light)

	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	//output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	//// Step 2:	rotate the normal: NO TRANSLATION
	////			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	////			thus removing the postion row of the matrix
	//output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	//output.texCoord = input.texCoord;
	//return output;

	float4 transPos = (float4)0;
	

	float4x4 interPos;

	for (int i = 0; i < 4; ++i)
	{
		int index = BoneIndices[i];
		if (index >= 0)
		{
			interPos += BoneWeights[i] * gBones[index];
		}
	}

	transPos = mul(float4(position, 1.0f), interPos);

	transPos.w = 1.0f;
	transPos = mul(transPos, mul(gWorld, gLightViewProj));

	return transPos;
}
 
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 position:SV_POSITION){}

technique11 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
	    SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}

technique11 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}