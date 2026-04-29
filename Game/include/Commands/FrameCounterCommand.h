#ifndef _FRAMECOUNTERCOMMAND_H_
#define _FRAMECOUNTERCOMMAND_H_

#include <ComponentCommand.h>
#include <Components/PlayerControllerComponent.h>

//purpose of this is to animate the player sprite
//this'll just invoke a function on PlayerControllerComponent
namespace dae
{
	class FrameCounterCommand final : public ComponentCommand<PlayerControllerComponent>
	{
	public:
		FrameCounterCommand(PlayerControllerComponent* pPCComponent) noexcept : ComponentCommand(pPCComponent) {};
		virtual ~FrameCounterCommand() = default;
		bool Execute() override;
	private:
	};
};

#endif