
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

uint8_t data_rec;
int z=0;
int obstacle_val_1 = -1;
int obstacle_val_2 = -1;


/*********************************************************************/
// Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
// zigbee

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
// Function To Initialize UART2
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
// usb

void uart2_init(void)
{
	UCSR2B = 0x00; //disable while setting baud rate
	UCSR2A = 0x00;
	UCSR2C = 0x06;
	UBRR2L = 0x5F; //set baud rate lo
	UBRR2H = 0x00; //set baud rate hi
	UCSR2B = 0x98;
}
/****************************************************************/

void send_data(uint8_t data) 
{
	//lcd_print(1,9,data,5); 
    while ((UCSR0A & (1 << UDRE0)) == 0);//make sure the data register is cleared
    UDR0 = data; //send the data
	delay(1);
	
}

/****************************************************************/

ISR(USART0_RX_vect) 
{  
	data_rec = UDR0;
	lcd_print(2,12,data_rec,3);

	
	if(data_rec>=0 && data_rec<=47)
	{
		current_node_slave=data_rec;    // slave current node
		data_rec=250;
	}
	if(data_rec>=50 && data_rec<=97)
	{
		next_node_slave=data_rec-50; //slave next node
		data_rec=250;
	}
	if (data_rec==185)
	{
		flag_update_slave(n,s);
		data_rec=250;
	}
	if (data_rec==189)
	{
		if (check_flag_slave(n,s))
		{
			send_data(180);
		}
		else
		{
			send_data(187);
		}
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
}

/****************************************************************/
ISR(USART2_RX_vect)
{
	data_rec = UDR0;
	lcd_wr_char(data_rec);
	note_string[z]=data_rec;
	lcd_print(2,9,note_string[0],3);
	z++;
		
}


/****************************************************************/
void slave_command(int slave_mnp)
{
	send_data(100+slave_mnp);	
}


/*********************************************************************/


void master_command(int note_numb)
{
	int8_t target_node_,source_node_;
	
	while(note_flag[note_numb]==0)
	{	
	source_node_ = current_node;
	target_node_ = node_for_note(source_node_, note_string[note_numb]);
	source_to_target(source_node_,target_node_);	
	}
	
}

/****************************************************************/


char check_collision()
{
	if (next_node_slave == next_node)
	{
		while (next_node_slave==next_node)
		{
			delay(1);
		}
	}
}

/****************************************************************/


void flag_update_slave(int loop_num, char algorithm_type )
{
	if (algorithm_type == 'A')
	{
		note_flag[loop_num+1]=1;
	}
	if (algorithm_type == 'B')
	{
		note_flag[loop_num]=1;
	}
	if (algorithm_type == 'C')
	{
		note_flag[loop_num+2]=1;
	}
	if (algorithm_type == 'D')
	{
		if (note_flag[loop_num]==0)
		{
			note_flag[loop_num]=1;
		}
		else if (note_flag[loop_num]==1)
		{
			note_flag[loop_num+1]==1;
		}
	}
}

/****************************************************************/

int check_flag_slave(int loopnum,char algorithmtype)
{
	if (algorithmtype == 'A')
	{
		if (note_flag[loopnum]==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if (algorithmtype == 'B')
	{
		if (loopnum-1 <0)
		{
			return 1;
		}
		else if (note_flag[loopnum-1]==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if (algorithmtype == 'C')
	{
		if (note_flag[loopnum+1]==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if (algorithmtype == 'D')
	{
		if (loopnum-1 < 0)
		{
			return 1;
		}
		else if (note_flag[loopnum]==1)
		{
			return 1;
		}
		else if (note_flag[loopnum-1]==1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}	
}

/****************************************************************/

void flag_update_master(int loop_numb,char algo_type)
{
	if (algo_type == 'A')
	{
		note_flag[loop_numb]=1;
	}
	if (algo_type == 'B')
	{
		note_flag[loop_numb+1]=1;
	}
	if (algo_type == 'C')
	{
		if (note_flag[loop_numb]==0)
		{
			note_flag[loop_numb]=1;
		}
		else if (note_flag[loop_numb]==1)
		{
			note_flag[loop_numb+1]==1;
		}
	}
	if (algo_type == 'D')
	{
		note_flag[loop_numb+2]=1;
	}	
}

/****************************************************************/
