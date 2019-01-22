
/*********************************************************************
DATA					Value		ms		sm

current_node_slave	=>	0 to 47				~
next_node_slave 	=>	50 to 97			~
MNP1				=>	101 to 133	`
MNP2				=>	141 to 173	`
mnp_strike			=>	180			`
mnp_striked			=>	185					~
mnp_wait			=>	187			`
mnp_reached			=>	189					~
collision			=>	191			`
collision,wait		=>	193			`
collision,adj_node	=>	195			`
stop				=>  197			`
obstacle			=>	200 to 247	`		~


*********************************************************************/



#include "header.h"


uint8_t data_rec=250;
int8_t current_mnp1=-1;
int8_t current_mnp2=-1;
int _2_mnp_flag=0;
int obstacle_val_1 = -1;
int obstacle_val_2 = -1;


/*********************************************************************/
//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UBRR0L = 0x5F; //set baud rate lo
	UBRR0H = 0x00; //set baud rate hi
	UCSR0B = 0x98;
}
/*********************************************************************/
void send_data(uint8_t data)
{
	
	while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
	UDR0 = data; //send the data
	delay(1);
	
}

/****************************************************************/
ISR(USART0_RX_vect)
{
	data_rec = UDR0;
	
	lcd_print(2,12,data_rec,3);
		
	if(data_rec>=101 && data_rec<=133)
	{
		current_mnp1=data_rec-100;
		data_rec=250; 
	}
	
	if(data_rec>=141 && data_rec<=173)
	{
		current_mnp2=data_rec-140; // mnp receive from master
		data_rec=250;
	}
	
	if (current_mnp1!=-1 && current_mnp2!=-1)
	{
		_2_mnp_flag=1;
	}
	
	if (data_rec==180)
	{
		strike_pipe();
		
		if (current_mnp1!=-1)
		{
			current_mnp1=-1;
		}
		else if (current_mnp2 != -1)
		{
			current_mnp2 =-1;
			_2_mnp_flag = 0;
		}
		
		send_data(185);
		data_rec=250;
	}
	
	if (data_rec==187)
	{
		wait();
		data_rec=250;
	}
	
	if (data_rec>=200 && data_rec<=247)
	{
		if (obstacle_val_1==-1)
		{
			obstacle_val_1=data_rec-200;
		}
		else
		{
			obstacle_val_2=data_rec-200;
			del_edge(obstacle_val_1,obstacle_val_2);
			obstacle_val_1=-1;
			obstacle_val_2=-1;
		}
		data_rec=250;
	}
	
	if (data_rec==197)
	{
		stop();
		data_rec=250;
	}
}

/*********************************************************************/

void wait()
{
	while(data_rec==180)
	{
		send_data(189);
		delay(25);	
	}
}

/*********************************************************************/