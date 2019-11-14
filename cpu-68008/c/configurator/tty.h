#ifndef TTY_H
#define TTY_H

// function keys (receive $9b first, then these three bytes)
static const uint8_t  kEscF6[] = {0x31, 0x37, 0x7e};
static const uint8_t  kEscF7[] = {0x31, 0x38, 0x7e};
static const uint8_t  kEscF8[] = {0x31, 0x39, 0x7e};
static const uint8_t  kEscF9[] = {0x32, 0x30, 0x7e};
static const uint8_t kEscF10[] = {0x32, 0x31, 0x7e};
static const uint8_t kEscF11[] = {0x32, 0x33, 0x7e};
static const uint8_t kEscF12[] = {0x32, 0x34, 0x7e};
static const uint8_t kEscF13[] = {0x32, 0x35, 0x7e};
static const uint8_t kEscF14[] = {0x32, 0x36, 0x7e};
static const uint8_t kEscF15[] = {0x32, 0x38, 0x7e}; // help
static const uint8_t kEscF16[] = {0x32, 0x39, 0x7e}; // do
static const uint8_t kEscF17[] = {0x33, 0x31, 0x7e};
static const uint8_t kEscF18[] = {0x33, 0x32, 0x7e};
static const uint8_t kEscF19[] = {0x33, 0x33, 0x7e};
static const uint8_t kEscF20[] = {0x33, 0x34, 0x7e};

#endif
