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
void *stdin, *stdout, *stderr;
/* Interrupt Service Routine */
void user_isr(void)
{
    int input = 1;
    while (is_pressed(BTN4))
    {
        if (input == 1)
        {
            // display_score (0, "hest",rand()%21);
            ///hit                                          //hit
         //   display_update();
            input = 0;
        }

        
    }
    int inputB = 1;
        while (is_pressed(BTN3))
        {
            if (inputB == 1)
            {
               // display_string(1, "stand");
               // display_update();
                                                    //stand

                    inputB = 0;
            }
        }
    // display_string(0,"hello");
    // display_update();
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
    display_string(0, "hello");
    display_update();
}
