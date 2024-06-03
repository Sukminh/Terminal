// Checkpoint 1 New File
#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "terminal.h"


// flags that indicates if modifier keys are pressed
uint8_t leftShiftFlag = 0;
uint8_t rightShiftFlag = 0;
uint8_t ctrlFlag = 0; 
uint8_t capFLag = 0;
uint8_t alt_flag = 0;

static char key_buf[3][MAX_BUFFER] = {{0}};        //create buffer with length 128
// int cur_terminal_num = 1;
int counterB = 0;               //counter for backspaces

int counterB1 = 0;
int counterB2 = 0;
int counterB3 = 0;
char printChar;

int flag_alt1 = 0;
int flag_alt2 = 0;
int flag_alt3 = 0;

//Store every key on the keyboard with the key resulting from holding shift and the key
const char table[58][2] = {
    {0x00, 0x00}, {0x00,0x00},
    {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'},
    {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, 
    {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'},
    {BS,BS}, {'\t', '\t'},  //backspace and tab
    {'q','Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
    {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'},
    {'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'},
    {ENTR,ENTR}, {0x0, 0x0}, //enter and also left control
    {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'},
    {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'},
    {'l', 'L'}, {';', ':'}, {'\'', '"'}, {'`', '~'},
    {0x0, 0x0}, {'\\', '|'}, // left shift
    {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'},
    {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'},
    {'.', '>'}, {'/', '?'}, 
    {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {' ', ' '},
};


/*
* init_key
* Inputs: none
* Outputs: none
* Side Effects: Initalize the keyboard by enabling irq 1 on PIC
*/
void init_key(void){
    // initialize globalKeyboardFlag for each terminal to 0
    globalKeyboardFlag[0] = 0;
    globalKeyboardFlag[1] = 0;
    globalKeyboardFlag[2] = 0;
    // cur_terminal_num = 1;
    enable_irq(KEY_IRQ);
}

/*
* keyboard_handler
* Inputs: none
* Outputs: none
* Side Effects: Read input from keyboard dataport (when interrupt occurs), and put it to video memory and terminal buffer
*/
void keyboard_handler(void){
    uint8_t scanCode = inb(KEY_DP);
    send_eoi(KEY_IRQ);
    //if (globalKeyboardFlag == 1){
        int i;

        if (scanCode == LEFT_SHIFT){
            leftShiftFlag = 1;
            send_eoi(KEY_IRQ);
            return;
        }
        else if (scanCode == LEFT_RELEASE){
            leftShiftFlag = 0;
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == RIGHT_SHIFT){
            rightShiftFlag = 1;
            send_eoi(KEY_IRQ);
            return;
        }
        else if (scanCode == RIGHT_RELEASE){
            rightShiftFlag = 0;
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == CTRL){
            ctrlFlag = 1;
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == CTRL_RELEASE){
            ctrlFlag = 0;
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == CAPS){
            capFLag = ~(capFLag);
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == ALT){
            alt_flag = 1;
            send_eoi(KEY_IRQ);
            return;
        }
        else if(scanCode == ALT_RELEASE){
            alt_flag = 0;
            send_eoi(KEY_IRQ);
            return;
        }

        if (alt_flag && (scanCode == F1)) {
            // // for (i = 0; i < counterB; i++) {
            // //     key_buf[cur_terminal_num][i] = 0x00;
            // // }
            // if (cur_terminal_num == 2) {
            //     counterB2 = counterB;
            // }
            // else if (cur_terminal_num == 3) {
            //     counterB3 = counterB;
            // }
            // counterB = counterB1;
            // cur_terminal_num = 1;
            // terminal_switch(1);
            // terminal_switch_buffer(1);    
            // set flag for terminal 1 to 1 if F1 is pressed    
            flag_alt1 = 1; 
        } else if (alt_flag && (scanCode == F2)) {
            // for (i = 0; i < counterB; i++) {
            // //     key_buf[cur_terminal_num][i] = 0x00;
            // // }

            // if (cur_terminal_num == 1) {

            //     counterB1 = counterB;
            // }
            // else if (cur_terminal_num == 3) {
            //     counterB3 = counterB;
            // }

            // counterB = counterB2;
            // cur_terminal_num = 2;
            // terminal_switch(2);
            
            // terminal_switch_buffer(2);
            // set flag for terminal 2 to 1 if F2 is pressed  
            flag_alt2 = 1;
        } else if (alt_flag && (scanCode == F3)) {
            // // for (i = 0; i < counterB; i++) {
            // //     key_buf[cur_terminal_num][i] = 0x00;
            // // }
            // if (cur_terminal_num == 1) {
            //     counterB1 = counterB;
            // }
            // else if (cur_terminal_num == 2) {
            //     counterB2 = counterB;
            // }
            // counterB = counterB3;
            // cur_terminal_num = 3;
            // terminal_switch(3);
            // terminal_switch_buffer(3);
            // set flag for terminal 3 to 1 if F3 is pressed  
            flag_alt3 = 1;
         }

        // also check keyboard flag is whether en/disabled
        else if(((scanCode > 1) && (scanCode < MAX_SCAN)) && (globalKeyboardFlag[cur_terminal_num] == 1)){
            switch(table[scanCode][0]){
                case ENTR:
                    for (i = 0; i < counterB; i++) {
                        key_buf[cur_terminal_num][i] = 0x00;
                    }
                    // if (globalKeyboardFlag[cur_terminal_num] == 1){
                        keyboard_input('\n');
                        key_putc('\n'); // go to next line
                    // }
                    counterB = 0;
                    break;
                case BS:
                    // if buffer has no element it does nothing
                    if (counterB > 0) {
                        // if (key_buf[cur_terminal_num][counterB] =='\t') {//tab has to be handled separately for deleting 4 spaces at a time
                        //     counterB--;       // decrement counterB by 1
                        //     for (i = 0; i < 5; i++) {
                        //         key_buf[cur_terminal_num][counterB] = 0x00;    // clear last element in key_buf
                        //         // if (globalKeyboardFlag[cur_terminal_num] == 1){
                        //             keyboard_input('\b');   // terminal driver receives keyboard input
                        //             key_putc('\b');             // delete last character
                        //         // }
                                
                        //     }

                        // } else {
                        //     // if (globalKeyboardFlag[cur_terminal_num] == 1){
                        //         keyboard_input('\b');       // terminal driver receives keyboard input
                        //         key_putc('\b');                 // delete last character
                        //     // }
                        // }
                        keyboard_input('\b');       // terminal driver receives keyboard input
                        key_putc('\b');                 // delete last character
                        counterB--;   // decrement counterB by 1
                        key_buf[cur_terminal_num][counterB] = 0x00;       // clear last element in key_buf
                    }
                    break;

                default:
                    if (scanCode == L && (ctrlFlag == 1)) {     // ctrl+L
                        clear();
                        int i;
                        for (i = 0; i < counterB; i++) {
                            //edit
                            //key_buf[i] = 0x00;
                            key_putc(key_buf[cur_terminal_num][i]);
                        }
                        //edit
                        //counterB = 0; // set counterB to zero after clearing buffer
                        //edit
                        //clear(); //clear screen
                    } else if (scanCode == TAB){   // TAB
                        if (counterB + 4 < MAX_BUFFER - 1){         //tab will take 4 spaces, so we check if that value will exceed the max buffer length (128-1 = 127)
                           
                            for (i = 0; i < 4; i++) {
                                key_buf[cur_terminal_num][counterB] = '\t';
                                counterB++;
                                //counterB++;
                                // if (globalKeyboardFlag[cur_terminal_num] == 1){
                                    keyboard_input(' ');        // terminal driver receives keyboard input
                                    key_putc(' ');                  // draw 4 spaces for tab key
                                // }
                            }
                        }
                    
                    // struct
                        
                    }else{
                        if (counterB < MAX_BUFFER - 1){
                            if(!((capFLag || leftShiftFlag) || rightShiftFlag)){
                                printChar = table[scanCode][0];
                            }
                            if (((Q<=scanCode) && (scanCode<= P)) || ((A<=scanCode) && (scanCode<= L)) || ((Z<=scanCode) && (scanCode<= M))){
                                //now we know that it has to be a letter
                                if ((capFLag && rightShiftFlag) || (capFLag && leftShiftFlag)){
                                    printChar = table[scanCode][0];
                                }
                                else if (rightShiftFlag || leftShiftFlag || capFLag){
                                    printChar = table[scanCode][1];
                                }
                                else{
                                    printChar = table[scanCode][0];
                                }
                            }
                            else{
                                //not a letter
                                if (leftShiftFlag || rightShiftFlag){
                                    printChar = table[scanCode][1];
                                }
                                else{
                                    printChar = table[scanCode][0];
                                }
                            }
                            key_buf[cur_terminal_num][counterB] = printChar;
                            counterB ++;
                            // if (globalKeyboardFlag[cur_terminal_num] == 1){
                                keyboard_input(printChar);
                                key_putc(printChar);             // draw new character
                            // }
                        }
                        
                    }
                    
            }
            send_eoi(KEY_IRQ);
            return;
        }
    //}
    send_eoi(KEY_IRQ);
    return;
}

void keyboard_switch_buffer(int index) {
    if (cur_terminal_num == 0) {
        counterB1 = counterB;
    }
    else if (cur_terminal_num == 1) {
        counterB2 = counterB;
    }
    else {
        counterB3 = counterB;
    }
    // cur_terminal_num = index;
    if (index == 0) {
        counterB = counterB1;
    }
    else if (index == 1) {
        counterB = counterB2;
    }
    else {
        counterB = counterB3;
    }
}
