/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/*
* i8259_init
* Initialize the 8259 PIC  * Inputs: None
* Outputs: None
* Side Effect: Primary and secondary PICs are set
*/
void i8259_init(void) {

    //initalize master pic
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_DP);
    outb(ICW3_MASTER, MASTER_DP);
    outb(ICW4, MASTER_DP);

    //slave
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_DP);
    outb(ICW3_SLAVE, SLAVE_DP);
    outb(ICW4, SLAVE_DP);
    
    enable_irq(SLAVE_IRQ);

}

/*
* enable_irq
* Enable (unmask) the specified IRQ 
* Inputs: irq_num (indivating which irq to be unmasked). 0-7 = Primary PIC, 8-15 = Secondary PIC
* Outputs: none
* Side Effects: enable the irq specified so that it can receive interrupts
*/
void enable_irq(uint32_t irq_num) {
    //check if irq_num is in [0,15]
    if((irq_num < 0) | (irq_num > 15)) return;
        if(irq_num < MASTER_IRQ){
            // The irq is in [0,7], therefore on the primary pic
            master_mask &= ~(1<<irq_num);
            outb(master_mask, MASTER_DP);
        }
        else{
            //irq on slave
            slave_mask &= ~(1<<(irq_num-MASTER_IRQ));
            outb(slave_mask, SLAVE_DP);
        }
            
}


/*
* disable_irq
* Disable (mask) the specified IRQ
* Inputs: irq_num (indivating which irq to be unmasked). 0-7 = Primary PIC, 8-15 = Secondary PIC
* Outputs: none
* Side Effects: disable the specified irq so it cannot receive interrupts
*/
void disable_irq(uint32_t irq_num) {
    //check if irq_num is in [0,15]
    if((irq_num < 0) | (irq_num > 15)) return;

        if(irq_num < MASTER_IRQ){
            // The irq is in [0,7], therefore on the primary pic
            master_mask |= 1<<irq_num;
            outb(master_mask, MASTER_DP);
        }
        else{
            //irq on slave
            slave_mask |= (1<<(irq_num-MASTER_IRQ));
            outb(slave_mask, SLAVE_DP);
        }
}

/* 
* send_eoi
* Send end-of-interrupt signal for the specified IRQ 
* Inputs: irq_num (indivating which irq to be unmasked). 0-7 = Primary PIC, 8-15 = Secondary PIC
* Outputs: none
* Side Effects: PIC receives EOI and will allow interrupts to occur for the specified irq
*/
void send_eoi(uint32_t irq_num) {
    //check if irq_num is in [0,15]
    if((irq_num < 0) | (irq_num > 15)) return;
        if(irq_num < MASTER_IRQ){
            // The irq is in [0,7], therefore on the primary pic
            outb(EOI | irq_num, MASTER_8259_PORT);
        }
        else{
            //irq on slave
            outb(EOI | (irq_num - MASTER_IRQ), SLAVE_8259_PORT);
            outb(EOI | SLAVE_IRQ, MASTER_8259_PORT);
        }
}
