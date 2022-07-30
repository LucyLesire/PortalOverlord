#pragma once

class PortalGun final : public GameObject
{
public:
	PortalGun() = default;
	~PortalGun() override = default;
	PortalGun(const PortalGun& other) = delete;
	PortalGun(PortalGun&& other) noexcept = delete;
	PortalGun& operator=(const PortalGun& other) = delete;
	PortalGun& operator=(PortalGun&& other) noexcept = delete;

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override {};
	void PostDraw(const SceneContext&) override {};
private:

};