#pragma once
class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() = default;
	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void SetCameraVariables(CameraComponent* pCamera);
	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetSpecularTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetOpacityMap(const std::wstring& assetFile);
	void SetEnvironmentTexture(const std::wstring& assetFile);

protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{};
	TextureData* m_pSpecularTexture{};
	TextureData* m_pNormalTexture{};
	TextureData* m_pOpacityTexture{};
	TextureData* m_pCubeTexture{};
};

