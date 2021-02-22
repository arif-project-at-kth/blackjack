/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

/* Interrupt Service Routine */
void user_isr(void)
{
    return;
}

/* Initializer */
void init(void)
{
    init_buttons();
    //setup_interrupt();
    return;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    display_string(0, "Hello World");
    show_all_hands();

    /** HIT **/ // TODO FIX HOW TO HIT
    if (is_pressed(BTN4) && check_turn())
    {
        hit();
    }

    /** WINNERS? **/
    if (check_player_hand())
    {
        display_string(0, "PLAYER 1 WINNER");
    }
    if (check_cpu_hand())
    {
        display_string(0, "PLAYER 2 WINNER");
    }
    
    // DELAY
    delay(500);
    next_turn();

    return;
}
