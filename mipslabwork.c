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
//  void *stdin, *stdout, *stderr;	// Used for -c99 in compiler
/* Interrupt Service Routine */
void user_isr(void)
{
    int input = 1;
    while (is_pressed(BTN4))
    {
        // TODO TURN THIS INTO a FUNCTION.
        if (input == 1)
        {
            drawCard(1);
            input = 0;
        }
    }
    int inputB = 1;
    while (is_pressed(BTN3))
    {
        // TODO TURN THIS INTO a FUNCTION.
        if (inputB == 1)
        {
            while (player_score > cpu_score && cpu_score < 21|| cpu_score <16){
                drawCard(0);
                show_all_hands();
                player_state = 0;
            }

            inputB = 0;
        }
    }
    IFS(0) = 0;
    return;
}

/* Initializer */
void init(void)
{
    init_buttons();
    setup_interrupt();
    return;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    show_all_hands();
}
