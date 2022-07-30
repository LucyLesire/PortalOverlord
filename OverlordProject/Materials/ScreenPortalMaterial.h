#pragma once
class ScreenPortalMaterial final : public Material<ScreenPortalMaterial>
{
public:
	ScreenPortalMaterial();
	~ScreenPortalMaterial() override = default;
	ScreenPortalMaterial(const ScreenPortalMaterial& other) = delete;
	ScreenPortalMaterial(ScreenPortalMaterial&& other) noexcept = delete;
	ScreenPortalMaterial& operator=(const ScreenPortalMaterial& other) = delete;
	ScreenPortalMaterial& operator=(ScreenPortalMaterial&& other) noexcept = delete;

	void SetWorldViewProj(const XMFLOAT4X4& wvp);

protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{};
};

