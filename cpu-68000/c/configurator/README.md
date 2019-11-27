# Configurator
The configurator is a small C application that lives in the Boot ROM that provides a BIOS-like interface over the default TTY (however that's defined by the ROM) to configure various settings of the system. These settings are stored in the NVRAM, and loaded by the ROM at bootup.

Parameters that can be configured include:

- Date/Time settings
- Serial ports (speed, parity, stop bits, flow control)
- Expansion slots
