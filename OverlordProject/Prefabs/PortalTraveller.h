#pragma once
class PortalTraveller : public GameObject
{
public:
	PortalTraveller() = default;
	~PortalTraveller() override = default;
	PortalTraveller(const PortalTraveller& other) = delete;
	PortalTraveller(PortalTraveller&& other) noexcept = delete;
	PortalTraveller& operator=(const PortalTraveller& other) = delete;
	PortalTraveller& operator=(PortalTraveller&& other) noexcept = delete;

	bool IsFirstEntry() const { return m_IsFirstEntry; }
	void SetFirstEntry(bool enable) { m_IsFirstEntry = enable; }

	XMFLOAT3 GetPrevDistanceFromPortal() const { return m_PrevDisFromPortal; }
	void SetPrevDistanceFromPortal(const XMFLOAT3& dis) { m_PrevDisFromPortal = dis; }

	void SetYOffsetAngle(float angle) { m_YOffsetAngle = angle; }

protected:

	float m_YOffsetAngle{};

private:
	bool m_IsFirstEntry{false};

	XMFLOAT3 m_PrevDisFromPortal{};

};

