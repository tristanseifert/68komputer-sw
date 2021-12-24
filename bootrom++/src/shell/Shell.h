#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * A basic shell for entering commands for the boot ROM.
 */
class Shell {
    public:
        /**
         * Status codes indicating how a prompt session ended
         */
        enum class PromptReturn: int16_t {
            /// Failed to read a line
            ReadLineFailed              = -1,
            /// A command handler was invoked
            CommandInvoked              = 0,
            /// A line was read, but no command handler wanted to deal with it
            InvalidCommand              = 1,
        };

        /**
         * Definition of a command handler. This is a string command name, plus a function that is
         * invoked with the argumenents, if any, of the command.
         */
        struct Command {
            /// Name of the command; this must match exactly.
            const char *name;
            /**
             * A short description string, shown in the console help output if provided.
             */
            const char *shortHelp{nullptr};
            /**
             * Long help description string, printed when `help [command]` is invoked.
             */
            const char *longHelp{nullptr};
            /**
             * Function to be invoked when this command is executed.
             *
             * @param name Command string entered by the user
             * @param args If non-NULL, start of the argument string provided by the user
             *
             * @return Command status code; by convention, 0 indicates success.
             */
            int (*handler)(const char *name, char *args);
        };

    public:
        static void Init();

        static PromptReturn RunPrompt(const bool showErrors);

    private:
        static void PrintPrompt();
        static bool ReadLine();
        static bool ParseLine();

        static bool FindCommandDescriptor(const char *name, const Command **outCmd);

        static int HelpCommandHandler(const char *, char *param);

    private:
        /**
         * Maximum line length supported by the shell, in characters.
         */
        constexpr static const size_t kMaxLineLength{100};
        /**
         * Buffer to hold the line that was most recently read in.
         */
        static char gCurrentLine[kMaxLineLength];
        /**
         * Character index into the line buffer we're currently reading
         */
        static uint16_t gCurrentLineIndex;

        /**
         * Total number of commands registered with the shell.
         */
        constexpr static const size_t kNumCommands{5};
        /**
         * All commands known to the shell. On every command invocation, we'll search through this
         * list to find a matching handler.
         */
        static const Command gCommands[kNumCommands];
};
