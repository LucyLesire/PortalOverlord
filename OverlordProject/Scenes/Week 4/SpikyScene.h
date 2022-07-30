#pragma once
class SpikyMaterial;

class SpikyScene final : public GameScene
{
public:
	SpikyScene() : GameScene(L"SpikyScene"){};
	~SpikyScene() = default;
	SpikyScene(const SpikyScene& other) = delete;
	SpikyScene(SpikyScene&& other) noexcept = delete;
	SpikyScene& operator=(const SpikyScene& other) = delete;
	SpikyScene& operator=(SpikyScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	SpikyMaterial* m_pSpikyMat = nullptr;
	GameObject* m_pSphere = nullptr;

	float m_Speed = 20.f;
};

