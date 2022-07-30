#pragma once
class Character;
class PostGrayscale;
class ScreenPortalMaterial;
class DiffuseMaterial;
class Portal;

class PortalScene : public GameScene
{
public:
	PortalScene() : GameScene(L"Portal Scene") {};
	~PortalScene() override;

	PortalScene(const PortalScene& other) = delete;
	PortalScene(PortalScene&& other) noexcept = delete;
	PortalScene& operator=(const PortalScene& other) = delete;
	PortalScene& operator=(PortalScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void PostDraw() override;
	void Draw() override;
	void OnGUI() override;

private:
	CameraComponent* m_pMainCamera{};

	std::vector<Portal*> m_pPortals{};

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		PrimaryFire,
		SecondaryFire
	};

	Character* m_pCharacter{};

	int m_ActionId_PrimaryFire{ -1 };
	int m_ActionId_SecondaryFire{ -1 };
};

