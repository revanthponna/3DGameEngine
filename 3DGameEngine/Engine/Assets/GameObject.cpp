#include "GameObject.h"
#include <math.h>

using namespace eae6320;

eae6320::Assets::GameObject::GameObject(Graphics::Mesh* pMesh, Graphics::Effect* pEffect, Physics::sRigidBodyState pRigidBodyState)
{
	m_mesh = pMesh;
	m_effect = pEffect;
	// m_position = pPosition;

	m_pRigidBody = pRigidBodyState;

	m_mesh->IncrementReferenceCount();
	m_effect->IncrementReferenceCount();
}

eae6320::Assets::GameObject::~GameObject()
{
	m_mesh->DecrementReferenceCount();
	m_effect->DecrementReferenceCount();
}

void eae6320::Assets::GameObject::SetMesh(Graphics::Mesh* pMesh) noexcept
{
	m_mesh = pMesh;
}

void eae6320::Assets::GameObject::SetEffect(Graphics::Effect* pEffect) noexcept
{
	m_effect = pEffect;
}

void eae6320::Assets::GameObject::Update(float i_secondCountToIntegrate) noexcept
{
	m_pRigidBody.Update(i_secondCountToIntegrate);
}

void eae6320::Assets::GameObject::Rotate(const Math::cQuaternion& rotation) noexcept
{
	SetOrientation(rotation);
}

void eae6320::Assets::GameObject::SetVelocity(Math::sVector pVelocity) noexcept
{
	m_pRigidBody.velocity = pVelocity;
}

void eae6320::Assets::GameObject::AddVelocity(Math::sVector pVelocity) noexcept
{
	m_pRigidBody.velocity += pVelocity;
}

Math::cMatrix_transformation eae6320::Assets::GameObject::GetTransformLocalToWorld(float i_secondCountToExtrapolate) const noexcept
{
	return m_pRigidBody.PredictFutureTransform(i_secondCountToExtrapolate);
}

Graphics::Mesh* eae6320::Assets::GameObject::GetMesh() const noexcept
{
	return m_mesh;
}

Graphics::Effect* eae6320::Assets::GameObject::GetEffect() const noexcept
{
	return m_effect;
}

Math::sVector eae6320::Assets::GameObject::GetPosition() const noexcept
{
	Math::sVector returnPosition = m_pRigidBody.position;
	return returnPosition;
}

void eae6320::Assets::GameObject::SetPosition(const Math::sVector& i_position)
{
	m_position = i_position;
}

Math::cQuaternion eae6320::Assets::GameObject::GetOrientation() const
{
	return m_orientation;
}

void eae6320::Assets::GameObject::SetOrientation(const Math::cQuaternion& i_orientation)
{
	m_orientation = i_orientation;
}