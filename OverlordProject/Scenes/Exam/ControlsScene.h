#pragma once
class ControlsScene : public GameScene
{
public:
	ControlsScene() : GameScene(L"Controls Scene") {};
	~ControlsScene() override = default;

	ControlsScene(const ControlsScene& other) = delete;
	ControlsScene(ControlsScene&& other) noexcept = delete;
	ControlsScene& operator=(const ControlsScene& other) = delete;
	ControlsScene& operator=(ControlsScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;

	GameObject* m_pSprite{};
};

