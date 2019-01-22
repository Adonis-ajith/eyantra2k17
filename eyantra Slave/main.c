
/*
 * Team Id: 1972
 
 * Author List:		R. Ajith Kumar, V. M. Gokul Nath
 
 * Filename:		main.c
 
 * Theme:			Bothoven
 
 * Functions:		void main_init(), void init_pins(), void adc_init(), void timer5_init(), unsigned char adc_conv(unsigned char channel),
					unsigned int sharp_dist(unsigned char adc_reading), void position_encoder_interrupt_init ()

 * Global Variables: uint8_t mnp_string[9], int8_t current_dir, int8_t current_node, long int ShaftCountRight,
					 long int ShaftCountLeft=0, int8_t obstacle_flag=0
					 
 */

#include "header.h"					//header file


/*********************************************************************/

		
int8_t path[N];													//path[] : Array to store the path of nodes from source to target

int8_t current_dir = 6;											//current_dir :keep tracks of the bot orientation

int8_t current_node=12;											//current_node :To store the node at which the bot currently at

int8_t next_node;

int8_t previous_node;

long int ShaftCountRight=0;										//ShaftCountRight :used with position encoders and interrupts for movement

long int ShaftCountLeft=0;										//ShaftCountLeft :used with position encoders and interrupts for movement

int8_t obstacle_flag=0;											//obstacle_flag :flag variable for the presence of obstacles

/*********************************************************************
* Function Name:	int main(void) 

* Input:			None

* Output:			None

* Logic:			It has a for loop that runs for each MNP and rings continuous buzzer at the end.

* Example Call:		NA

*********************************************************************/

int main(void)
{	int _source,_target;											//source: source node, target : target node				
	main_init();
	
		servo_1(90);	
	while(1)
	{	
		while(current_mnp1!=-1)
		{
			_source = current_node;
			_target = node_for_mnp(_source,current_mnp1);
			source_to_target(_source,_target);			
		}
		if (_2_mnp_flag==1)
		{
			while(current_mnp2!=-1)
			{
				_source = current_node;
				_target = node_for_mnp(_source,current_mnp1);
				source_to_target(_source,_target);
			}
		}		
	}
}							
	
/*********************************************************************
* Function Name:  main_init()

* Input:		  None

* Output:		  None

* Logic:		  All the initialization are done in this function.

* Example Call:	  main_init();

*********************************************************************/


void main_init()
{
	cli();						//Clears the global interrupts
	init_pins();
	position_encoder_interrupt_init();				//Pin Initialization
	lcd_set_4bit();
	lcd_init();
	adc_init();
	timer5_init();
	timer1_init();
	create_graph();
	create_dir();
	uart0_init();	
	sei();						//Enables the global interrupts
}

/*********************************************************************
* Function Name:	init_pins()

* Input:			None

* Output:			None

* Logic:			Initializes all the input and output pins

* Example Call:		init_pins();

*********************************************************************/
void init_pins()
{
	
	//Motors PA0-PA3 , PL3-PL4	
	DDRA = DDRA | 0x0F;			//Setting PA0-PA3 as output
	PORTA = PORTA & 0xF0;		//Setting PA0-PA3 low
	DDRL = DDRL | 0x18;			//Setting PL3-PL4 as output for PWM generation
	PORTL = PORTL | 0x18;		//Setting PL3-PL4 high for velocity control using PWM.
	
	//Position Encoder PE4-PE5
	DDRE  = DDRE & 0xCF;		//Setting PE4-PE5 as input
	PORTE = PORTE | 0x30;		//Enable internal pull-up for PE4-PE5 
	
	//Sharp Sensor PK1-PK5
	DDRK = 0xF7;				//Setting PK3 as input ADC
	PORTK = 0xF7;				//Disable internal pull-up for PK3 for floating
	
	//LCD,Buzzer PC0-PC7
	DDRC = DDRC | 0xFF;			//Setting PC0-PC7 as output
	PORTC = PORTC & 0x80;		// Setting PC0-PC6 low except PORTC 7
	
	//Black Line Sensor PF3-PF1
	DDRF  =0x00;				//Setting PF3-PF1  as input ADC
	PORTF = 0x00;				//Disable internal pull-up for PF3-PF1 for floating
	
	//Servo Motor
	DDRB = DDRB | 0x20;			//making PORTB 5 pin output
	PORTB = PORTB | 0x20;		//setting PORTB 5 pin to logic 1
	
	//Proximity sensor turn off
	DDRH = DDRH | 0x04;			//making PH2 pin output
	PORTH = PORTH | 0x04;		//setting PH2 pin to logic 1
	
	//Sharp sensor turn off
	//	DDRH = DDRH | 0x08;			//making PH3 pin output
	//	PORTH = PORTH | 0x08;		//setting PH3 pin to logic 1

}

/*********************************************************************
* Function Name:	adc_init()

* Input:			None

* Output:			None

* Logic:			initializes adc registers for the sensors

* Example Call:		adc_init()

*********************************************************************/

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

/*********************************************************************
* Function Name:	timer5_init()

* Input:			None

* Output:			None

* Logic:			Initializes timer 5 for the PWM for the motor speed control.

* Example Call:		timer5_init();

*********************************************************************/

void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

/*********************************************************************
* Function Name:	adc_conv(unsigned char channel)

* Input:			corresponding sensor channel number 

* Output:			8 bit analog values

* Logic:			NA

* Example Call:		sensor_val = adc_conv(3);

*********************************************************************/

unsigned char adc_conv(unsigned char channel)
{
	unsigned char a;
	if(channel>7)
	{
		ADCSRB = 0x08;
	}
	channel = channel & 0x07;
	ADMUX= 0x20| channel;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

/*********************************************************************
* Function Name:	sharp_dist(unsigned char adc_reading)

* Input:			The adc value got from the adc_conv function

* Output:			Returns the distance of the obstacle detected by the sharp sensor

* Logic:			The distance of the obstacle depends on the angle of reflection of the IR ray.

* Example Call:		Distance = sharp_dist(adc_val)

*********************************************************************/

unsigned int sharp_dist(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}

/*********************************************************************
* Function Name:	Position_encoder_interrupt_init ()

* Input:			None

* Output:			None

* Logic:			Initializes the interrupts PE4 PE5 for the position encoder.

* Example Call:		Position_encoder_interrupt_init ();

*********************************************************************/

void position_encoder_interrupt_init () //Interrupt 4 enable
{
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
	sei();   // Enables the global interrupt
	cli(); //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();   // Enables the global interrupt	
}

/*********************************************************************/


void timer1_init()
{
	TCCR1A = 0x00;
	
	ICR1 = 1023; //TOP = 1023
	TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
	TCNT1L = 0x01; //Counter low value to which OCR1xH value is to be compared with
	OCR1A = 1023;
	TCCR1A = 0xAB;
	//COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0
	//For Overriding normal port functionality to OCRnA outputs. WGM11=1, WGM10=1. Along With
	//WGM12 in TCCR1B for Selecting FAST PWM Mode
	TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

/*********************************************************************/



/*********************************************************************/





/*
	E- Yantra Robotics Competition
	
	
	The orientation angles are calculated with positive x-axis and with 15' scale.

	Orientation :										angle (degrees/15)
		1 - Right										 0
		2 - Left										12 or -12
		3 - Forward										 6
		4 - Back										-6
		5 - Forward - Right - 60' with  +ve x axis		 4
		6 - Forward - Left	- 120' with +ve x axis		 8
		7 - Backward - Left	- 240' with +ve x axis		-8
		8 - Backward - Right- 300' with +ve x axis		-4
		
**************************************************************************/