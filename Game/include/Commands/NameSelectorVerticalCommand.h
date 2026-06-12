#ifndef _NAMESELECTORVERTICALCOMMAND_H_
#define _NAMESELECTORVERTICALCOMMAND_H_

#include <AxisComponentCommand.h>
#include <Components/NameSelectorComponent.h>

namespace dae
{
    class NameSelectorVerticalCommand final : public AxisComponentCommand<NameSelectorComponent>
    {
    public:
        explicit NameSelectorVerticalCommand(NameSelectorComponent* pComponent) noexcept
            : AxisComponentCommand<NameSelectorComponent>(pComponent) {}

        bool Execute() override
        {
            const float axis = GetAxisValue();
            if (axis != 0.f)
            {
                GetAttachedComponent()->ScrollLetter(axis < 0.f ? 1 : -1);
            }
            return true;
        }
    };
}

#endif // _NAMESELECTORVERTICALCOMMAND_H_
