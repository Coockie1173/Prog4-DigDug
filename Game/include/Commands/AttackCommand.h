#ifndef _ATTACK_COMMAND_H_
#define _ATTACK_COMMAND_H_

#include <ComponentCommand.h>
#include <Components/PlayerControllerComponent.h>

namespace dae
{
	class AttackCommand final : public ComponentCommand<PlayerControllerComponent>
	{
	public:
		AttackCommand(PlayerControllerComponent* pComponent, bool StartAttack) : ComponentCommand(pComponent), m_StartAttack(StartAttack) {}
		bool Execute() override;
	private:
		bool m_StartAttack{ true };
	};
}

#endif