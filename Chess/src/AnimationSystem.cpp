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
	}

}
