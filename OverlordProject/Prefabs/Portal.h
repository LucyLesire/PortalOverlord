#pragma once
#include "Materials/Post/PostGrayscale.h"
class PortalTraveller;
class ScreenPortalMaterial;
class DiffuseMaterial;

class Portal final : public GameObject
{
public:
	Portal() = default;
	Portal(Portal* pLinkedPortal, XMFLOAT4 color = XMFLOAT4{ Colors::Red });
	~Portal() override;
	Portal(const Portal& other) = delete;
	Portal(Portal&& other) noexcept = delete;
	Portal& operator=(const Portal& other) = delete;
	Portal& operator=(Portal&& other) noexcept = delete;

	void SetLinkedPortal(Portal* pPortal) { m_pLinkedPortal = pPortal; }
	Portal* GetLinkedPortal() const { return m_pLinkedPortal; }

	ScreenPortalMaterial* GetScreenMat() const { return m_pScreenMat; }
	PostPortalMaterial* GetPortalMat() const { return m_pPortalMat; }
	FreeCamera* GetCamera() const { return m_pCamera; }
	RenderTarget* GetRenderTarget() const { return m_pRenderTarget; }

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

	float GetHeight() const { return 20.f; }

	void OnTravellerEnterPortal(PortalTraveller* pTraveller);
	void LateUpdate();
	void Travel();
	void SetNearClipPlane();


	ModelComponent* GetPortalModel() const { return m_pPortalModel; }

	void SetDirection(float yDir) { m_PortalDir = yDir; }
	float GetPortalDir() const { return m_PortalDir; }
private:

	PostPortalMaterial* m_pPortalMat{};
	Portal* m_pLinkedPortal{};

	RenderTarget* m_pRenderTarget{};

	FreeCamera* m_pCamera{};

	ModelComponent* m_pPortalModel;

	CameraComponent* m_pMainCamera{};
	CameraComponent* m_pPortalCamera{};

	ScreenPortalMaterial* m_pScreenMat{};


	XMFLOAT4 m_Color{};

	std::vector<PortalTraveller*> m_pTravellers;

	float m_PortalDir{};
};

