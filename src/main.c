#include <MKL25Z4.H>
#include <stdbool.h>
#include "SysTick.h"
#include "gpio.h"


/* ------------------------------------------
       ECS642 Lab for week 2 2018
   Demonstration of simple digital input
   Use of RGB LED on Freedom board
  -------------------------------------------- */
/*
	External LED functionality added by:

  setExternalLED(int onOff): turns on/off an external LED.
	initFlash()      : now initialises the External LED as OFF.
	nextFlash()			 : now includes an external LED state transition.
	configureOutput(): now makes a pin with id number (EXTERNAL_POS) of PortD a GPIO 
										 for external LED and sets the pin as output.
	
	gpio.h now includes the following variables/macros to control external LED:
	
	EXT_LEDON 5		   : ON state of external LED
	EXT_LEDOFF 6		 : OFF state of external LED
	EXTERNAL_POS (5) : Pin number on Freedom KL25Z used by external LED
	
*/

/*----------------------------------------------------------------------------
  Turn LEDs on or off 
    onOff can be ON or OFF
*----------------------------------------------------------------------------*/
void setRedLED(int onOff) {
    if (onOff == ON) {
        PTB->PCOR = MASK(RED_LED_POS) ;               
    } 
    if (onOff == OFF) {
        PTB->PSOR =  MASK(RED_LED_POS) ;
    }
    // no change otherwise
}

void setBlueLED(int onOff) {
    if (onOff == ON) {
			PTD->PCOR = MASK(BLUE_LED_POS) ; // Active Low: external LED on              
    } 
    if (onOff == OFF) {
			PTD->PSOR = MASK(BLUE_LED_POS) ; // Active HighL: external LED off
    }
    // no change otherwise
}

void setExternalLED(int onOff) {
		if (onOff == ON) {
			PTD->PSOR = MASK(EXTERNAL_POS) ; // Active high: external LED on               
    } 
    if (onOff == OFF) {
			PTD->PCOR =  MASK(EXTERNAL_POS) ;// Active low: external LED off
    }
    // no change otherwise
}
/*----------------------------------------------------------------------------
  isPressed: test the switch

  Operating the switch connects the input to ground. A non-zero value
  shows the switch is not pressed.
 *----------------------------------------------------------------------------*/
bool isPressed(void) {
    if (PTD->PDIR & MASK(BUTTON_POS)) {
        return false ;
    }
    return true ;
}

/*----------------------------------------------------------------------------
  checkButton

This function checks whether the button has been pressed
*----------------------------------------------------------------------------*/
int buttonState ; // current state of the button
bool pressed ; // signal if button pressed

void initButton() {
    buttonState = BUTTONUP ;
    pressed = false ; 
}


void checkButton() {
    switch (buttonState) {
        case BUTTONUP:
            if (isPressed()) {
                buttonState = BUTTONDOWN ;
                pressed = true ; 
            }
            break ;
        case BUTTONDOWN:
            if (!isPressed()) {
                buttonState = BUTTONUP ;
            }
            break ;
    }                               
}

/*----------------------------------------------------------------------------
  nextFlash 

This function evaluates whether the system should change state. 
The system stays in each state for a number of cycles, counted by 
the 'count' variable. It changes state of the button is pressed.
*----------------------------------------------------------------------------*/
int state ; 
int count ;

void initFlash() {
    count = PERIOD ;
    state = REDON ;
    setRedLED(ON) ;
    setBlueLED(OFF) ;
	  setExternalLED(OFF); // Initialise external LED as Off 
}

void nextFlash() {
   if (count > 0) count -- ;
       switch (state) {
           case REDON:
	             if (count == 0) {   // The time transition has priority
                   setRedLED(OFF) ;
                   state = REDOFF ;
                   count = PERIOD ;
	             } else if (pressed) { // The button transition can occur on next cycle 
                   pressed = false ;
                   setRedLED(OFF) ;
                   setBlueLED(ON) ;
                   state = BLUEON ;
               }
               break ;

           case REDOFF:
               if (count == 0) {
                   setRedLED(ON) ;
                   state = REDON ;
                   count = PERIOD ;
               } else if (pressed) {
                   pressed = false ;
                   state = BLUEOFF ;
               }
               break ;
                        
           case BLUEON:
               if (count == 0) {
                   setBlueLED(OFF) ;
                   state = BLUEOFF ;
                   count = PERIOD ;
               } else if (pressed) {
                   pressed = false ;
                   setBlueLED(OFF) ;
								   setExternalLED(ON); // Set external LED ON if button pressed
									 state = EXT_LEDON;	 // Current state: External LED is ON
               }
               break ;

           case BLUEOFF: 
               if (count == 0) {
                   setBlueLED(ON) ;
                   state = BLUEON ;
                   count = PERIOD ;
               } else if (pressed) {
                   pressed = false ;
								   state = EXT_LEDOFF; // Current state: External LED is OFF
               }
               break ;
							 
					 case EXT_LEDON:
						 if (count == 0) {
                   setExternalLED(OFF) ;
                   state = EXT_LEDOFF ; // Current state: External LED is OFF
                   count = PERIOD ;
               } else if (pressed) {
                   pressed = false ;
                   setExternalLED(OFF) ;
                   setRedLED(ON) ;
                   state = REDON ;
               }
						 break;
							 
					 case EXT_LEDOFF:
								if (count == 0) {
                   setExternalLED(ON) ;
                   state = EXT_LEDON ;  // Current state: External LED is ON
                   count = PERIOD ;
               } else if (pressed) {
                   pressed = false ;
                   state = REDOFF ;
               }
					   break;
        }
}

/*----------------------------------------------------------------------------
  Configuration 
     The configuration of the GPIO port is explained in week 2
     Enabling the clocks will be covered in week 3.
     Configuring the PORTx peripheral, which controls the use of each pin, will
       be covered in week 3
*----------------------------------------------------------------------------*/
void configureOutput() {
     // Configuration steps
     //   1. Enable clock to GPIO ports
     //   2. Enable GPIO ports
     //   3. Set GPIO direction to output
     //   4. Ensure LEDs are off

     // Enable clock to ports B and D
     SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
        
     // Make 3 pins GPIO
     PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
     PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
     PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
     PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
     PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;	
     PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);   
		 // Make a pin with id (EXTERNAL_POS) of PortD a GPIO to use external LED
     PORTD->PCR[EXTERNAL_POS] &= ~PORT_PCR_MUX_MASK;	
     PORTD->PCR[EXTERNAL_POS] |= PORT_PCR_MUX(1);   

     // Set ports to outputs
     PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
     PTD->PDDR |= MASK(BLUE_LED_POS)| MASK(EXTERNAL_POS); // Set external LED as output

     // Turn off LEDs
     PTB->PSOR = MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
     PTD->PSOR = MASK(BLUE_LED_POS); 
}

/*----------------------------------------------------------------------------
  GPIO Input Configuration

  Initialse a GPIO port D pin as an input (GPIO data direction register)
  Bit number given by BUTTON_POS
  Configure PORTD (not covered until week 3) so that the pin has no interrupt
     and a pull up resistor is enabled.
 *----------------------------------------------------------------------------*/
// 
void configureInput(void) {
    SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

    /* Select GPIO and enable pull-up resistors and no interrupts */
    PORTD->PCR[BUTTON_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | 
	           PORT_PCR_IRQC(0x0);
        
    /* Set port D switch bit to be an input */
    PTD->PDDR &= ~MASK(BUTTON_POS);

}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    configureInput() ;  // configure the GPIO input for the button
    configureOutput() ;  // configure the GPIO outputs for the LED 
    initButton() ;
    initFlash() ;
    Init_SysTick(1000) ; // initialse SysTick every 1ms
    waitSysTickCounter(10) ; // cycle every 10ms
    while (1) {
        checkButton() ; // check button
        nextFlash() ; // flash LEDs 
        waitSysTickCounter(10) ; // wait to end of cycle
    }
}

