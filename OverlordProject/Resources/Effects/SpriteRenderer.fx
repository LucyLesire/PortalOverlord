//INCOMPLETE!

float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0;
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
        //Step 3.
        //Do rotation calculations
        //Transform to origin
        //float2 originPos = float2(0, 0);
        //originPos.x = pos.x - offset.x - pivotOffset.x;
        //originPos.y = pos.y - offset.y - pivotOffset.y;

        ////Rotate

        //
        ////Retransform to initial position

        float2 originPos = float2(0, 0);
        originPos.x = pos.x - offset.x - pivotOffset.x;
        originPos.y = pos.y - offset.y - pivotOffset.y;
        //Rotate
        pos.x = (originPos.x * rotCosSin.x) + (originPos.y * -rotCosSin.y);
        pos.y = (originPos.x * rotCosSin.y) + (originPos.y * rotCosSin.x);

        //Retransform to initial position
        pos.x = pos.x + offset.x;
        pos.y = pos.y + offset.y;
    }
    else
    {
        //Step 2.
        //No rotation calculations (no need to do the rotation calculations if there is no rotation applied > redundant operations)
        //Just apply the pivot offset

        pos.x = pos.x - pivotOffset.x;
        pos.y = pos.y - pivotOffset.y;
    }

    //Geometry Vertex Output
    GS_DATA geomData = (GS_DATA)0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
    //Given Data (Vertex Data)
    float3 position = (float3)vertex[0].TransformData; //Extract the position data from the VS_DATA vertex struct
    float2 offset = (float2)vertex[0].TransformData; //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float rotation = vertex[0].TransformData.w; //Extract the rotation data from the VS_DATA vertex struct
    float2 pivot = (float2)vertex[0].TransformData2; //Extract the pivot data from the VS_DATA vertex struct
    float2 scale = float2(vertex[0].TransformData2.z, vertex[0].TransformData2.w); //Extract the scale data from the VS_DATA vertex struct
    float2 texCoord = float2(0, 0); //Initial Texture Coordinate

    //...


    // LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
    // |          / |
    // |       /    |
    // |    /       |
    // | /          |
    // LB----------RB

    float2 rotCosSin = float2(0.f, 0.f);
    if (rotation != 0)
    {
        rotCosSin.x = cos(rotation);
        rotCosSin.y = sin(rotation);
    }

    pivot.x = pivot.x * gTextureSize.x * scale.x;
    pivot.y = pivot.y * gTextureSize.y * scale.y;

    //VERTEX 1 [LT]
    CreateVertex(triStream, position, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivot); //Change the color data too!

    //VERTEX 2 [RT]
    position.x = position.x + (gTextureSize.x * scale.x);
    texCoord.x = 1;
    CreateVertex(triStream, position, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivot); //Change the color data too!

    //VERTEX 3 [LB]
    position.x = position.x - (gTextureSize.x * scale.x);
    position.y = position.y + (gTextureSize.y * scale.y);
    texCoord.x = texCoord.x - 1;
    texCoord.y = texCoord.y + 1;
    CreateVertex(triStream, position, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivot); //Change the color data too!

    //VERTEX 4 [RB]
    position.x = position.x + (gTextureSize.x * scale.x);
    texCoord.x = 1;
    CreateVertex(triStream, position, float4(1, 1, 1, 1), texCoord, rotation, rotCosSin, offset, pivot); //Change the color data too!
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{

    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

// Default Technique
technique11 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        //SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
