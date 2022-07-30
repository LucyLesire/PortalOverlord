#pragma once
class PortalMaterial;
class ShadowMapMaterial;

class PortalRenderer : public Singleton<PortalRenderer>
{
public:
	PortalRenderer(const PortalRenderer& other) = delete;
	PortalRenderer(PortalRenderer&& other) noexcept = delete;
	PortalRenderer& operator=(const PortalRenderer& other) = delete;
	PortalRenderer& operator=(PortalRenderer&& other) noexcept = delete;

	void UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const;

	void Begin(const SceneContext&, CameraComponent* pCamera = nullptr, bool rt1 = true);
	void DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones = {});
	void End(const SceneContext&) const;

	ID3D11ShaderResourceView* GetShadowMap() const;
	const XMFLOAT4X4& GetLightVP() const { return m_LightVP; }

	void SetRenderTargets(RenderTarget* pPortal1RT, RenderTarget* pPortal2RT);

	void Debug_DrawDepthSRV(const XMFLOAT2& position = { 0.f,0.f }, const XMFLOAT2& scale = { 1.f,1.f }, const XMFLOAT2& pivot = { 0.f,0.f }) const;

protected:
	void Initialize() override;

private:
	friend class Singleton<PortalRenderer>;
	PortalRenderer() = default;
	~PortalRenderer();

	//Rendertarget to render the 'shadowmap' to (depth-only)
	//Contains depth information for all rendered shadow-casting meshes from a light's perspective (usual the main directional light)
	RenderTarget* m_pPortal1RenderTarget{ nullptr };
	RenderTarget* m_pPortal2RenderTarget{ nullptr };

	//Light ViewProjection (perspective used to render ShadowMap)
	XMFLOAT4X4 m_LightVP{};

	//Shadow Generator is responsible of drawing all shadow casting meshes to the ShadowMap
	//There are two techniques, one for static (non-skinned) meshes, and another for skinned meshes (with bones, blendIndices, blendWeights)
	enum class ShadowGeneratorType
	{
		Static,
		Skinned
	};

	ID3DX11Effect* m_pShadowMapGenerator{ nullptr };

	//Information about each technique (static/skinned) is stored in a MaterialTechniqueContext structure
	//This information is automatically create by the Material class, we only store it in a local array for fast retrieval 
	static int const NUM_TYPES{ 2 };
	MaterialTechniqueContext m_GeneratorTechniqueContext;

	static constexpr UINT m_VertexCount{ 4 };
	static ID3D11Buffer* m_pDefaultVertexBufferP;

	ID3D11InputLayout* m_pDefaultInputLayout{};
};

