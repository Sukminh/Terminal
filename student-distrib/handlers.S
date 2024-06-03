# Checkpoint 1 New File Created
#define ASM       1

#define HANDLER(name,handler)   \
.globl name                    ;\
name:                           ;\
    pushal                      ;\
    call handler                ;\
    popal                       ;\
    iret                        
    # pushal = Push All General Registers

HANDLER(handle_pit,pit_handler)
HANDLER(handle_keyboard,keyboard_handler)
HANDLER(handle_rtc,rtc_handle)
