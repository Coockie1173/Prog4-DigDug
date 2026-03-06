#ifndef _ICOMMAND_H_
#define _ICOMMAND_H_

namespace dae
{
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual bool Execute() = 0;

        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;
        Command(Command&&) noexcept = delete;
        Command& operator=(Command&&) noexcept = delete;
    };
}

#endif