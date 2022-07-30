#pragma once
//Resharper Disable All

	class PostPortalMaterial final : public PostProcessingMaterial
	{
	public:
		PostPortalMaterial(float width = 50, float height = 50);
		~PostPortalMaterial() override = default;;
		PostPortalMaterial(const PostPortalMaterial& other) = delete;
		PostPortalMaterial(PostPortalMaterial&& other) noexcept = delete;
		PostPortalMaterial& operator=(const PostPortalMaterial& other) = delete;
		PostPortalMaterial& operator=(PostPortalMaterial&& other) noexcept = delete;

		//Can be overriden to implement multi-pass effects (like Bloom)
		//void Draw(const SceneContext& sceneContext, RenderTarget* pSource) override;
		void Initialize(const GameContext& ) override;
		void UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource) override;
		void SetWorldViewProj(XMMATRIX worldViewProj);

		RenderTarget* GetPortalRenderCameraRT() const { return m_PortalRenderCameraRT; }
	private:

		float m_Width{};
		float m_Heigth{};

		RenderTarget* m_PortalRenderCameraRT{};
	};

