#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void romentry();

void isr_reserved();

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

void isr_irq_uninitialized();
void isr_irq_spurious();
void isr_irq_unhandled_avec1();
void isr_irq_unhandled_avec2();
void isr_irq_unhandled_avec3();

// user vector $F0
void isr_irq_xr68c681();

void isr_rom_svc_trap();

// autogenerated reserved vectors
void isr_reserved_20();
void isr_reserved_21();
void isr_reserved_22();
void isr_reserved_23();
void isr_reserved_24();
void isr_reserved_25();
void isr_reserved_26();
void isr_reserved_27();
void isr_reserved_28();
void isr_reserved_29();
void isr_reserved_2A();
void isr_reserved_2B();
void isr_reserved_2C();
void isr_reserved_2D();
void isr_reserved_2E();

void isr_reserved_40();
void isr_reserved_41();
void isr_reserved_42();
void isr_reserved_43();
void isr_reserved_44();
void isr_reserved_45();
void isr_reserved_46();
void isr_reserved_47();
void isr_reserved_48();
void isr_reserved_49();
void isr_reserved_4A();
void isr_reserved_4B();
void isr_reserved_4C();
void isr_reserved_4D();
void isr_reserved_4E();
void isr_reserved_4F();
void isr_reserved_50();
void isr_reserved_51();
void isr_reserved_52();
void isr_reserved_53();
void isr_reserved_54();
void isr_reserved_55();
void isr_reserved_56();
void isr_reserved_57();
void isr_reserved_58();
void isr_reserved_59();
void isr_reserved_5A();
void isr_reserved_5B();
void isr_reserved_5C();
void isr_reserved_5D();
void isr_reserved_5E();
void isr_reserved_5F();

#ifdef __cplusplus
}
#endif
