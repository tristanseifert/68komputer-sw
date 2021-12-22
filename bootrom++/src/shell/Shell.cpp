#include "Shell.h"

#include "io/Console.h"

#include <string.h>

char Shell::gCurrentLine[kMaxLineLength];
uint16_t Shell::gCurrentLineIndex{0};


/**
 * Initializes the shell.
 */
void Shell::Init() {

}

/**
 * Prints the shell prompt and accepts input from the console device. Once a full line has been
 * acquired and the user hits return, the line is parsed and the appropriate command handler is
 * invoked.
 *
 * @param showErrors When set, errors with command invocation are printed.
 *
 * @return Status of the prompt execution
 */
Shell::PromptReturn Shell::RunPrompt(const bool showErrors) {
    PrintPrompt();

    gCurrentLineIndex = 0;
    if(!ReadLine()) {
        return PromptReturn::ReadLineFailed;
    }

    if(!ParseLine()) {
        if(showErrors) {
            Console::Put("Invalid command!\r\n\n");
        }
        return PromptReturn::InvalidCommand;
    }
    return PromptReturn::CommandInvoked;
}

/*
 * Print a prompt for the user to enter input at.
 */
void Shell::PrintPrompt() {
    Console::Put("> ");
}

/**
 * Read a line of input from the console.
 *
 * A line can be terminated by receiving a Ctrl+C (abort) or a return (accept). The line editor
 * supports basic scrolling for large entries.
 */
bool Shell::ReadLine() {
    while(1) {
        if(Console::GetCharPending()) {
            const auto ch = Console::GetChar();

            switch(ch) {
                // return
                case 0x0A:
                    gCurrentLine[gCurrentLineIndex] = '\0';
                    return true;

                // regular char
                default:
                    if(gCurrentLineIndex != (kMaxLineLength - 1)) {
                        gCurrentLine[gCurrentLineIndex++] = ch;
                    } else {
                        // XXX: handle line too long
                    }
                    break;
            }
        }
    }
}

/**
 * Parses the command line to execute the appropriate handler.
 *
 * @remark If an empty line is entered, it's treated as a success.
 */
bool Shell::ParseLine() {
    char *command{gCurrentLine}, *args{nullptr};
    bool commandEnd{false};

    if(!gCurrentLineIndex) return true;

    // try to split out command name and start of arg string
    for(size_t i = 0; i < gCurrentLineIndex; i++) {
        switch(gCurrentLine[i]) {
            // end of the string
            case '\0':
                goto beach;
            case ' ':
                // end of the first token, so null terminate it and look for next regular char
                if(!commandEnd) {
                    gCurrentLine[i] = '\0';
                    commandEnd = true;
                }
                break;
            // found a regular char
            default:
                // if command name has ended, this is the start of args
                if(commandEnd) {
                    args = gCurrentLine + i;
                    goto beach;
                }
        }
    }

beach:;
    // try to find the appropriate command and invoke it
    const Command *cmdInfo{nullptr};
    if(FindCommandDescriptor(command, &cmdInfo)) {
        cmdInfo->handler(command, args);
        return true;
    }

    // if we get here, no handler is suitable
    return false;
}

/**
 * Finds a command descriptor that can handle the command with the given name.
 *
 * @param name Command name to search for
 * @param outCmd If found, this variable receives the command descriptor's address
 *
 * @return Whether a command was found
 */
bool Shell::FindCommandDescriptor(const char *name, const Command **outCmd) {
    const auto commandLen = strnlen(name, kMaxLineLength);

    // TODO: we could optimize this if the command list is sorted
    for(size_t i = 0; i < kNumCommands; i++) {
          const auto &h = gCommands[i];

        if(!strncmp(name, h.name, commandLen)) {
            *outCmd = &h;
            return true;
        }
    }

    return false;
}



/**
 * Handler for the help command.
 *
 * We'll print either a list of all commands, or if a parameter is specified, find that command's
 * detailed info and print it.
 *
 * @param params Optional argument string to the help command
 *
 * @return 0 on success, or -1 if the specified detail command name couldn't be found
 */
int Shell::HelpCommandHandler(const char *, char *params) {
    // no parameter: print all commands
    if(!params) {
        Console::Put("***** Available Commands *****\r\n");
        for(size_t i = 0; i < kNumCommands; i++) {
            const auto &h = gCommands[i];
            Console::Print("%12s: %s\r\n", h.name, h.shortHelp ? h.shortHelp : "");
        }
    }
    // grab the first arg as a command to get the long help text for
    else {
        const auto commandName = strtok(params, " ");
        if(!commandName) {
            Console::Put("usage: help [command name]\r\n");
            return -1;
        }

        const Command *cmdInfo{nullptr};
        if(!FindCommandDescriptor(commandName, &cmdInfo)) {
            Console::Print("No command `%s` found\r\n", commandName);
            return -1;
        }

        // print its long help text if available
        if(cmdInfo->longHelp) {
            Console::Put(cmdInfo->longHelp);
        } else {
            Console::Print("No detailed help is available for `%s`.\r\n", commandName);
        }
    }

    return 0;
}
