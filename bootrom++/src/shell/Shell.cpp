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
    const auto commandLen = strnlen(command, kMaxLineLength);

    // TODO: we could optimize this if the command list is sorted
      for(size_t i = 0; i < kNumCommands; i++) {
          const auto &h = gCommands[i];

        // does the name match exactly?
        if(!strncmp(command, h.name, commandLen)) {
            h.handler(command, args);
            return true;
        }
    }

    // if we get here, no handler is suitable
    return false;
}
