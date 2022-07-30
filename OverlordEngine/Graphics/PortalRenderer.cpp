#include "stdafx.h"
#include "PortalRenderer.h"


ID3D11Buffer* PortalRenderer::m_pDefaultVertexBufferP = {};

PortalRenderer::~PortalRenderer()
{
	if(m_pPortal1RenderTarget)
		SafeDelete(m_pPortal1RenderTarget)
	if(m_pPortal2RenderTarget)
		SafeDelete(m_pPortal2RenderTarget)
	//SafeDelete(m_pShadowMapGenerator)

	SafeRelease(m_pDefaultInputLayout);
	SafeRelease(m_pDefaultVertexBufferP);
}

void PortalRenderer::Initialize()
{
	//TODO_W8(L"Implement Initialize")
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)
	//m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<PostPortalMaterial>();
	m_pShadowMapGenerator = ContentManager::Load<ID3DX11Effect>(L"Effects/PortalRenderer.fx");

	//m_GeneratorTechniqueContext = m_pShadowMapGenerator->GetTechniqueContext(0);

		const VertexPosTex vertices[m_VertexCount]
		{
			{{-1.f,-1.f,0.f},{0.f,1.f}},
			{{-1.f,1.f,0.f},{0.f,0.f}},
			{{1.f,-1.f,0.f},{1.f,1.f}},
			{{1.f,1.f,0.f},{1.f,0.f}}
		};
	
		//Create VertexBuffer
		D3D11_BUFFER_DESC buffDesc{};
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(VertexPosTex) * m_VertexCount;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;
	
		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = &vertices[0];
	
		//Create Buffer
		HANDLE_ERROR(m_GameContext.d3dContext.pDevice->CreateBuffer(&buffDesc, &initData, &m_pDefaultVertexBufferP));

		//Create Default InputLayout if nullptr
		if (!m_pDefaultInputLayout)
		{
			//Assuming the InputLayout for all PostProcessing Effects is the same...
			EffectHelper::BuildInputLayout(m_GameContext.d3dContext.pDevice, m_pShadowMapGenerator->GetTechniqueByIndex(0), &m_pDefaultInputLayout);
		}
	//}
}

void PortalRenderer::UpdateMeshFilter(const SceneContext& /*sceneContext*/, MeshFilter* /*pMeshFilter*/) const
{
	//TODO_W8(L"Implement UpdateMeshFilter")
	//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
	//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material

	//ShadowGeneratorType meshType{};
	//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	//auto techniqueContext = m_GeneratorTechniqueContexts[int(meshType)];
	//auto techniqueContext = m_GeneratorTechniqueContext;

	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
	//pMeshFilter->BuildVertexBuffer(sceneContext, techniqueContext.inputLayoutID, techniqueContext.inputLayoutSize, techniqueContext.pInputLayoutDescriptions);
}

void PortalRenderer::Begin(const SceneContext& /*sceneContext*/, CameraComponent* /*pCamera*/, bool /*rt1*/)
{
	const auto pSourceSRV = m_GameContext.pGame->GetRenderTarget()->GetColorShaderResourceView();
	m_pShadowMapGenerator->GetVariableByName("gTexture")->AsShaderResource()->SetResource(pSourceSRV);
}

void PortalRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* /*pMeshFilter*/, const XMFLOAT4X4& /*meshWorld*/, const std::vector<XMFLOAT4X4>& /*meshBones*/)
{
	//TODO_W8(L"Implement DrawMesh")
	//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)

	
	auto techniqueContext = m_GeneratorTechniqueContext;

	m_GameContext.pGame->SetRenderTarget(m_pPortal1RenderTarget);
	//		- Easily achieved by calling OverlordGame::SetRenderTarget (m_GameContext has a reference to OverlordGame)
	//2. Clear the destination RT with a Purple color
	//		- Using purple will make debugging easier, when the screen is purple you'll know something is wrong with your post-processing effects
	m_pPortal1RenderTarget->Clear(XMFLOAT4{ Colors::Red });

	//3. Set The Pipeline!
	//		- Set Inputlayout > m_pDefaultInputLayout (The inputlayout for all post-processing effects should 'normally' be the same POSITION/TEXCOORD)
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(m_pDefaultInputLayout);

	//		- Set PrimitiveTopology (check the VertexBuffer for the correct topology)
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//		- Set VertexBuffer > m_pDefaultVertexBuffer (Represents a full screen quad, already defined in clipping space)
	const UINT offset = 0;
	const UINT stride = sizeof(VertexPosTex);
	//const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(techniqueContext.inputLayoutID, mesh.id);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pDefaultVertexBufferP, &stride, &offset);


	//		- Iterate the technique passes (same as usual)
		//			- apply the pass
		//			- DRAW! (use the m_VertexCount constant for the number of vertices)
	auto tech = m_pShadowMapGenerator->GetTechniqueByIndex(0);
	D3DX11_TECHNIQUE_DESC techDesc{};

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_VertexCount, 0);
	}

	//Release Source SRV from pipeline
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
}

void PortalRenderer::End(const SceneContext&) const
{
	//TODO_W8(L"Implement End")
		//This function is called at the end of the Shadow-pass, all shadow-casting meshes should be drawn to the ShadowMap at this point.
		//Now we can reset the Main Game Rendertarget back to the original RenderTarget, this also Unbinds the ShadowMapRenderTarget as RTV from the Pipeline, and can safely use it as a ShaderResourceView after this point.

		//1. Reset the Main Game RenderTarget back to default (OverlordGame::SetRenderTarget)
	m_GameContext.pGame->SetRenderTarget(nullptr);
	//		- Have a look inside the function, there is a easy way to do this...
}

ID3D11ShaderResourceView* PortalRenderer::GetShadowMap() const
{
	return m_pPortal1RenderTarget->GetColorShaderResourceView();
}

void PortalRenderer::SetRenderTargets(RenderTarget* pPortal1RT, RenderTarget* pPortal2RT)
{
	m_pPortal1RenderTarget = pPortal1RT;
	m_pPortal2RenderTarget = pPortal2RT;
}

void PortalRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pPortal1RenderTarget->HasColorSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pPortal1RenderTarget->GetColorShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
