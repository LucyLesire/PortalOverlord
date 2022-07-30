#pragma once
class SpherePrefab;
class CubePrefab;
class PongScene final : public GameScene
{
public:
	PongScene() : GameScene(L"PhongScene") {};
	~PongScene() = default;
	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	CubePrefab* m_pPeddle1 = nullptr;
	CubePrefab* m_pPeddle2 = nullptr;
	SpherePrefab* m_pSphere = nullptr;

	const float m_MoveSpeed = 25.f;

	bool m_Start = true;
};

