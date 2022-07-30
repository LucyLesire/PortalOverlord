#pragma once
class MainMenuScene : public GameScene
{
public:
	MainMenuScene() : GameScene(L"Main Menu Scene") {};
	~MainMenuScene() override = default;

	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnSceneActivated() override;
private:
	GameObject* m_pSprite{};
};

