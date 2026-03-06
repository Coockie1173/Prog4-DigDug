#ifndef _ICOMPONENTCOMMAND_H_
#define _ICOMPONENTCOMMAND_H_

#include "Command.h"
#include "../Components/Component.h"
#include <type_traits>

namespace dae
{
    template<typename T>
    class ComponentCommand : public Command
    {
    public:
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        explicit ComponentCommand(T* pComponent) noexcept : m_pComponent(pComponent) {}
        virtual ~ComponentCommand() = default;

        ComponentCommand(const ComponentCommand&) = delete;
        ComponentCommand& operator=(const ComponentCommand&) = delete;
        ComponentCommand(ComponentCommand&&) noexcept = delete;
        ComponentCommand& operator=(ComponentCommand&&) noexcept = delete;

    protected:
        T* GetAttachedComponent() const noexcept { return m_pComponent; }

    private:
        T* m_pComponent;
    };
}

#endif // !_ICOMPONENTCOMMAND_H_
