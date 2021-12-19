#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void romentry();

void isr_bus_error();
void isr_address_error();
void isr_illegal_instruction();
void isr_zero_divide();
void isr_chk();
void isr_trapv();
void isr_privilege_violation();
void isr_trace();
void isr_line1010();
void isr_line1111();

#ifdef __cplusplus
}
#endif

