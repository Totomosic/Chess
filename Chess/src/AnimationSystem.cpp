#include "AnimationSystem.h"

namespace Chess
{

	void AnimationSystem::Update(EntityManager& entities, TimeDelta dt)
	{
		for (const EntityHandle& entity : entities.GetEntitiesWith<Transform, Animator>())
		{
			ComponentHandle<Transform> transform = entity.GetTransform();
			ComponentHandle<Animator> animator = entity.GetComponent<Animator>();

			Vector3f position = transform->Position();
			Vector3f toTarget = animator->Target - position;
			float length = toTarget.Length();

			float movementLength = animator->Speed * dt.Seconds();
			if (length < movementLength)
			{
				transform->SetLocalPosition(animator->Target);
				entity.Remove<Animator>();
			}
			else
			{
				transform->Translate(toTarget * movementLength / length);
			}
		}

		for (const EntityHandle& entity : entities.GetEntitiesWith<BarAnimator, Model>())
		{
			ComponentHandle<BarAnimator> animator = entity.GetComponent<BarAnimator>();
			ComponentHandle<Model> model = entity.GetComponent<Model>();

			Matrix4f& whiteTransform = model->Meshes[0].Transform;
			float currentScale = whiteTransform.Element(1, 1);
			Matrix4f& blackTransform = model->Meshes[1].Transform;

			float toCurrentScale = animator->TargetProportion - currentScale;
			float movement = animator->Speed * dt.Seconds();
			if (toCurrentScale < 0)
				movement *= -1;
			float newScale = currentScale + movement;
			if (std::abs(movement) > std::abs(toCurrentScale))
			{
				newScale = animator->TargetProportion;
			}

			whiteTransform = Matrix4f::Translation(0, -0.5f + newScale / 2.0f, 0) * Matrix4f::Scale(1.0f, newScale, 1.0f);
			blackTransform = Matrix4f::Translation(0, +0.5f - (1.0f - newScale) / 2.0f, 0) * Matrix4f::Scale(1.0f, 1.0f - newScale, 1.0f);
		}
	}

}
