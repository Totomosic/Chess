#pragma once
#include "Bolt.h"

namespace Chess
{

	struct Animator
	{
	public:
		Vector3f Target;
		float Speed;
	};

	struct BarAnimator
	{
	public:
		float TargetProportion;
		float Speed;
	};

	class AnimationSystem : public System<AnimationSystem>
	{
	public:
		void Update(EntityManager& entities, TimeDelta dt) override;
	};

}
