#!/usr/bin/env python3
################################################################################
# This script converts a binary file into a C style header, similar to the full
# xxd in many *nix systems.
################################################################################
import argparse
import os

# Entry point
def main():
    # parse command line
    parser = argparse.ArgumentParser(description='Convert binary files to C/C++ headers')
    parser.add_argument("binary", help="Path to the binary file to read in")
    parser.add_argument("output", help="Output for the header file")
    parser.add_argument("-n", "--name", help="Name for the data variable in the header")
    parser.add_argument("-p", "--prefix", help="String to prepend to the variable name")
    parser.add_argument("--const", help="Mark the data buffer as constant data",
                        action="store_true")
    args = parser.parse_args()

    # figure out the name
    varName = os.path.basename(args.binary).replace('.', '_')

    if args.name:
        varName = args.name
    if args.prefix:
        varName = args.prefix + varName

    # create directory for output
    os.makedirs(os.path.dirname(args.output), exist_ok=True)

    # open input file
    with open(args.binary, "rb") as inFile:
        # and open output file
        with open(args.output, "wt") as outFile:
            # write start of header
            outFile.write('#pragma once\n')
            outFile.write('#include <stddef.h>\n')
            outFile.write('#include <stdint.h>\n\n')
            if args.const:
                outFile.write('const ')
            outFile.write(f'uint8_t {varName}[] = ' + '{\n    ')

            # write data bytes
            numBytes = 0

            while 1:
                byt = inFile.read(1)
                if not byt:
                    break
                byte = byt[0]
                outFile.write('0x{0:02x}'.format(byte))

                if numBytes % 16 == 15:
                    outFile.write(',\n    ')
                else:
                    outFile.write(', ')
                numBytes = numBytes + 1

            # closing
            outFile.write('\n};\n');
            outFile.write(f'const size_t {varName}_len = {numBytes};' + '\n')

# run main method if invoked standalone
if __name__ == '__main__':
  main()

