/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stdint.h>	 /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

//	void *stdin, *stdout, *stderr;	// Used for -c99 in compiler

/** STARTING UP PROJECT (FROM LAB3) **/
void startup(void)
{
	/*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655; /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA; /* Unlock OSCCON, step 2 */
	while (OSCCON & (1 << 21))
		;				  /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while (OSCCON & (1 << 21))
		;		  /* Wait until PBDIV ready */
	SYSKEY = 0x0; /* Lock OSCCON */

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
	SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
}

int main(void)
{
	/** INITIALIZE **/
	startup();
	display_init();
	init();
	generate_deck();
	/** GAMEPLAY **/
NEWGAME:

	reset_game();
	int not_game_over = player_money > 0 ? 1 : 0;
	not_game_over = cards_left > 0 ? 1 : 0;
	// bet
	reset_display();
	while (1 && player_money > 0)
	{
		reset_display();
		display_string(0, "Time for Bets");
		display_score(1, "P Money : ", player_money);
		bet_more();
		display_score(3, "bet :", bet);
		display_update();

		if (is_pressed(SW1) && bet > 0)
		{
			break;
		}
	}

	if (not_game_over)
	{
		draw_card(PLAYER);
		player_state = 0; // DEALER DRAWS
		draw_card(DEALER);
		player_state = 1; // PLAYER DRAWS
		draw_card(PLAYER);
	}

	/** PLAY GAME **/
	if (not_game_over)
	{
		while (1)
		{
			if (is_game_over())
			{
				reset_display();
				display_update();

				break;
			}
			labwork(); /* Do lab-specific things again and again */
		}
	}

	/** GAME END **/
	reset_display();
	display_winner();

	while (1)
	{
		if (is_pressed(BTN2) && player_money > 0 && cards_left > 0)
		{
			goto NEWGAME;
		}
		if (is_pressed(BTN1)) // || player_money <= 0 || cards_left <= 0)
		{
			break;
		}

		display_string(2, DISPLAY_NEW_GAME);
		display_string(3, DISPLAY_QUIT);
		display_update();
	}

	/** GAMEPLAY END **/
	reset_display();
	display_string(0, DISPLAY_GOOD_BYE);
	display_update();

	return 0;
}
