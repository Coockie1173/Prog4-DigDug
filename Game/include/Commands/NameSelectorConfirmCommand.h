#ifndef _NAMESELECTORCONFIRMCOMMAND_H_
#define _NAMESELECTORCONFIRMCOMMAND_H_

#include <ComponentCommand.h>
#include <Components/NameSelectorComponent.h>

namespace dae
{
    // Fires when the player presses the confirm button (e.g. A / Enter).
    // Locks in the 3-letter name and triggers the OnNameConfirmed callback.
    class NameSelectorConfirmCommand final : public ComponentCommand<NameSelectorComponent>
    {
    public:
        explicit NameSelectorConfirmCommand(NameSelectorComponent* pComponent) noexcept
            : ComponentCommand<NameSelectorComponent>(pComponent) {}

        bool Execute() override
        {
            GetAttachedComponent()->Confirm();
            return true;
        }
    };
}

#endif // _NAMESELECTORCONFIRMCOMMAND_H_
