#pragma once

#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Graphics/Graphics.h>

namespace eae6320 {
	namespace Assets {
		class GameObject {
		public:
			GameObject(Graphics::Mesh* pMesh, Graphics::Effect* pEffect, Physics::sRigidBodyState);
			~GameObject();

			void SetMesh(Graphics::Mesh* pMesh) noexcept;
			void SetEffect(Graphics::Effect* pEffect) noexcept;

			void Update(float i_secondCountToIntegrate) noexcept;
			void Rotate(const Math::cQuaternion& rotation) noexcept;

			void SetVelocity(Math::sVector pVelocity) noexcept;
			void AddVelocity(Math::sVector pVelocity) noexcept;
			
			Math::cMatrix_transformation GetTransformLocalToWorld(float i_secondCountToExtrapolate) const noexcept;

			Graphics::Mesh* GetMesh() const noexcept;
			Graphics::Effect* GetEffect() const noexcept;

			Math::sVector GetPosition() const noexcept;
			void SetPosition(const Math::sVector& i_position);
			Math::cQuaternion GetOrientation() const;
			void SetOrientation(const Math::cQuaternion& i_orientation);

		private:
			Graphics::Mesh* m_mesh;
			Graphics::Effect* m_effect;
			Math::sVector m_position;
			Math::cQuaternion m_orientation;
			Physics::sRigidBodyState m_pRigidBody;
		};
	}
}