#include "stdafx.h"
#include "ScreenPortalMaterial.h"


ScreenPortalMaterial::ScreenPortalMaterial()
	: Material<ScreenPortalMaterial>(L"Effects/PostPortal.fx")
{

}

void ScreenPortalMaterial::SetWorldViewProj(const XMFLOAT4X4& wvp)
{
	SetVariable_Matrix(L"gWorldViewProj", wvp);
}

void ScreenPortalMaterial::InitializeEffectVariables()
{
}