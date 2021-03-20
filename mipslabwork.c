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
    int hitting = 1;
    while (is_pressed(BTN4))
    {
        // TODO TURN THIS INTO a FUNCTION.
        if (hitting == 1)
        {
            draw_card(PLAYER);
            hitting = 0;
        }
    }
    int standing = 1;
    while (is_pressed(BTN3))
    {
        // TODO TURN THIS INTO a FUNCTION.
        if (standing == 1)
        {
            while (player_score > dealer_score && dealer_score < BLACKJACK || dealer_score < 17)
            {
                player_state = 0;
                draw_card(DEALER);
                display_all_hands();
            }

            standing = 0;
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
	srand(generate_seed());
    return;
}

/* This function is called repetitively from the main program */
void labwork(void)
{
    display_all_hands();
}
