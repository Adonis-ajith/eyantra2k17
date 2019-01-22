/*
 * Team Id: 1972
 
 * Author List:		R. Ajith Kumar, Balendra Dageendran
 
 * Filename:		motion.c
 
 * Theme:			Bothoven eYRC-#BV
 
 * Functions:		void fwd(), void right(), void left(), void stop(),void angle_rotate(unsigned int dir_val), void angle_rotate(unsigned int dir_val),
					void angle_rotate_r(unsigned int dir_val), void angle_rotate_l(unsigned int dir_val), void speed (unsigned char left_motor, unsigned char right_motor),
					void line_follow(),	void source_to_target(int8_t source,int8_t target), void node_to_node(int8_t current_no,int8_t next_no), 
					void check_obstacle(int8_t current_nod, int8_t next_nod), ISR(INT4_vect), ISR(INT5_vect);
                   
 * Global Variables:uint8_t mnp_string[9], int8_t path[N], int8_t current_dir, int8_t current_node, long int ShaftCountRight,
					 long int ShaftCountLeft=0, int8_t obstacle_flag=0
					 
 */

#include "header.h"					//header file

/*********************************************************************
* Function Name:void fwd(),void right(), void left(), void stop()

* Input:		None

* Output:		Movement of bot forward or right or left or stop

* Logic:		Two motors, left and right, are connected in PORTA (PA0-PA3)
				fwd = 0110; make two motor to run forward
				right = 1010; make right motor run in backward while left motor runs in forward.
				left = 0101; make left motor run in backward while right motor runs in forward
				stop = 0000; make two motor to stop.
* Example Call: fwd();

*********************************************************************/

void fwd()
{
	PORTA = 0x06; // make two motor to run forward
}
/*********************************************************************/
void right()
{
	PORTA = 0x0A;  // make right motor run in backward while left motor runs in forward.
}
/*********************************************************************/
void left()
{
	PORTA = 0x05;  //make left motor run in backward while right motor runs in forward
}
/*********************************************************************/
void soft_left (void) //Left wheel stationary, Right wheel forward
{
	PORTA = 0x04;
}
/*********************************************************************/
void soft_right (void) //Left wheel forward, Right wheel is stationary
{
	PORTA = 0x02;
}
/*********************************************************************/
void stop()
{
	PORTA = 0x00; //make two motor to stop.
}

/*********************************************************************
* Function Name: angle_rotate(unsigned int dir_val),angle_rotate_r(unsigned int dir_val),angle_rotate_l(unsigned int dir_val)

* Input:		direction value which is angle made with positive x axis divided by 15

* Output:		Rotation of bot right or left.

* Logic:		Rotates for an angle less than given angle and continuous to rotate till the black line is sensed. 
				Uses interrupts and position encoders to rotate.

* Example Call: angle_rotate(12); -----> 12*15=180 --> rotates 180 degree

*********************************************************************/

void angle_rotate(unsigned int dir_val)				//this function used for circular path
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) dir_val/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;

	while (1) 
	{
		if((ShaftCountRight >= ReqdShaftCountInt) || (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	stop();
}

/*********************************************************************/

void angle_rotate_r(unsigned int dir_val)		//this function used to rotate right
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) (dir_val)/ 4.090; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;
	int r=0,l=0,m=0;

	while (1)
	{
		if((ShaftCountRight >= ReqdShaftCountInt) || (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	while(l<40 && m<40 && r<40)					//checks for black line
	{
		r=adc_conv(1);
		m=adc_conv(2);
		l=adc_conv(3);		
	}
	stop();
}

/*********************************************************************/



/*********************************************************************
* Function Name:void  speed (unsigned char left_motor, unsigned char right_motor) 

* Input:		left_motor pwm and right_motor pwm value

* Output:		speed differs w.r.t pwm... 255--->full speed, 0--->stop

* Logic:		PWM - Pulse width Modulation. On time of the motor can be varies with the pwm value 
				and hence the average power delivered varies. And so the speed of the motor varies.

* Example Call: speed(128,128); ---->two motors set to run in half of rated speed

*********************************************************************/

void speed (unsigned char left_motor, unsigned char right_motor) 
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

/*********************************************************************
* Function Name:void source_to_target(int8_t source,int8_t taarget)

* Input:		source node and taarget node

* Output:		Bot moves from source to target with connecting nodes

* Logic:		The bot moves from source to target through the nodes in the path[] array, also after traversing each node,
				the current_node value is updated.If obstacle is detected the loop breaks.

* Example Call: source_to_target(1,3);------>source_node current node 1 ,target node is 3.

*********************************************************************/
void source_to_target(int8_t source,int8_t taarget)
{
	int8_t y;
		
	for (y=1;path[y]!=-1;y++)
	{	
		next_node=path[y];	
				
		check_collision();
		
		node_to_node(current_node,next_node);	//function to travel from node to node
		stop();
		
		
		if(obstacle_flag==1)    //obstacles was detected
		{
			del_edge(current_node,next_node);
			send_data((int8_t)200+current_node);
			send_data((int8_t)200+next_node);
			break;
		}
		previous_node=current_node;
		current_node=next_node;			//Current node is updated
		if(path[y]!=-1)
		stop();		
	}
	if (current_node==taarget)   // target reached buzzer for one second
	{ 
		if (s=='A')
		{
			if (n==0)
			{
				strike_pipe();
				flag_update_master(n,s);
			}
			else 
			{
				while(note_flag[n-1]==0)
				delay(1);
				strike_pipe();
				flag_update_master(n,s);
			}
			
		}
		if (s=='B')
		{
			while(note_flag[n]==0)
			{
				delay(1);
			}
			strike_pipe();
			flag_update_master(n,s);
		}
		if (s=='C')
		{
			if (n==0)
			{
				strike_pipe();
				flag_update_master(n,s);
			}
			else
			{
				while(note_flag[n-1]==0)
				{
					delay(1);
				}
				strike_pipe();
				flag_update_master(n,s);
			}
		}
		
	}	
}

/*********************************************************************
* Function Name:void node_to_node(int8_t current_no,int8_t next_no)

* Input:		None				

* Output:		Bot moves from  current node to next node as per the order in the path[] array

* Logic:		It finds the current orientation of bot and angle required to rotate to reach the next node. 
				It rotates and moves to next node and new orientation is updated.

* Example Call:	node_to_node(1,2); two nodes lies in circular forward with 15degree variation
*********************************************************************/
void node_to_node(int8_t current_no,int8_t next_no)
{
	int angle_to_rot;
	
		angle_to_rot = dir[current_no][next_no]-current_dir;	//angle to rotate = required orientation - current orientation
		
		if(angle_to_rot>12)	  angle_to_rot-=24;					//setting angles greater than 180 to negative angles
		if(angle_to_rot<-12)  angle_to_rot+=24;					//setting angles less than -180 to positive angles
		if(angle_to_rot==-12) angle_to_rot=12;
		
		angle_to_rot=angle_to_rot*15;							// angles = dir_val * 15
		
		
	 if(abs(angle_to_rot)>15)									
		{
			if(angle_to_rot==180 || angle_to_rot==-180)			//u-turn
			{
				speed(250,250);
				right();
				angle_rotate(180);
			}
		else if (angle_to_rot>0)								//If positive then rotate left
			{
				speed(250,250);
				left();
				angle_rotate_r(abs(angle_to_rot));
			}
		else if (angle_to_rot<0)								//If negative then rotate right
			{
				speed(250,250);
				right();
				angle_rotate_r(abs(angle_to_rot));
			}
		}
		delay(50);
		check_obstacle(current_no,next_no);						//after rotation, checks for obstacles
		delay(50);
		
		if(obstacle_flag==0)									// is no obstacles then follows line to next node
		{
			line_follow(current_no,next_no);	
		}
		stop();
		current_dir=dir[current_no][next_no];					//updates the current orientation
}

/*********************************************************************
* Function Name:void line_follow()

* Input:		NA

* Output:		NA

* Logic:		**left sensor and right sensor at white and middle sensor at line(black)---->moves forward
				**middle sensor and right sensor at white and left sensor at line(black)---->moves left
				**middle sensor and left sensor at white and right sensor at line(black)---->moves right
				**all three sensor at black----> node detection
				**middles sensor and right sensor at black and left sensor at white---->node detection
				**middle sensor and left sensor at black and right sensor at white---->node detection

* Example Call: line follow(); --->follows black line and also detects nodes
*********************************************************************/

void line_follow(int currentnode,int nextnode)
{
	int l,r,m,th=50; //th =thershold adc value of line follower sensor. Below th --->white layer. Above th--->black layer
	int max=180,vr=0,vl=0;
	int i=0;
	while(1)
	{
		l=adc_conv(3);  //l= left white line follower sensor adc value
		m=adc_conv(2);  //m= middle white line follower sensor adc value
		r=adc_conv(1);  //r= right white line follower sensor adc value
		
		
		
		
		vr = (int)((r*r*0.001192)+(r*-1.183)+268.3);		//bot1
		vl = (int)((l*l*-0.003072)-(l*0.8015)+248.5);		//bot1
		
		vl=constrain(vl,250,180);
		vr=constrain(vr,250,180);		
		i++;
		
		lcd_print(2,1,i,3);
	
		if (((currentnode<nextnode)&&(currentnode<24)&&(nextnode<24))||(currentnode==23 && nextnode==0))
		{
			speed(vl,max);
			fwd();
		}
		if (((currentnode>nextnode)&&(currentnode<24)&&(nextnode<24))||(currentnode==0 && nextnode==23))
		{
			speed(max,vr);
			fwd();
		}
		
		if (((l>th && m>th)||(m>th && r>th))&&(i>20))
		{
			i=0;
			delay(300);
			stop();
			break;
		}
		if (currentnode>23 || nextnode>23)
		{
			if (l<th && m>th && r<th) speed(max,max);
		//	if (l>th && m>th && r<th) speed(230,max);
			if (l>th && m<th && r<th) speed(210,max);
		//	if (l<th && m>th && r>th) speed(max,230);
			if (l>th && m<th && r>th) speed(max,210);
			if (((l>th && m>th) || (m>th && r>th)) &&(i>20))
			{
				delay(300);
				break;
			}
			
		}
	}
}


/*********************************************************************
* Function Name:void check_obstacle(int8_t current_nod, int8_t next_nod)

* Input:		NA

* Output:		obstacles detection

* Logic:		if sensor detects there is obstacle between current and next node, the path was deleted in global map[] array for future use

* Example Call: check_obstacles(1,2); ----->if obstacles were there flag become 1, otherwise flag be 0
*********************************************************************/

void check_obstacle(int8_t current_nod, int8_t next_nod)
{
	int distance,adc_val;
	adc_val = adc_conv(11);     
	distance =sharp_dist(adc_val);
//	lcd_print(2,6,distance/10,2);
	if(distance<200 && distance>50) //detect obstacle from 5 to 20 cm
	{
		del_edge(current_nod,next_nod); //delete the path of two nodes
		obstacle_flag=1;                    
	}
	else
	{
		obstacle_flag=0;
	}
}

/*********************************************************************/

int constrain(int variable,int up_limit,int low_limit)
{
	if(variable>up_limit) variable=up_limit;
	if (variable<low_limit) variable=low_limit;
	return variable;	
}
/*********************************************************************/

void servo_1(unsigned char degrees)
{
	float regval = ((float)degrees * 0.512) + 34.56;
	OCR1A = (uint16_t) regval;
}



/*********************************************************************
* Function Name:	ISR(INT4_vect)

* Input:			None

* Output:			None

* Logic:			The control goes to this function whenever there is PE4 is triggered with falling edge.

* Example Call:		NA

/*********************************************************************/

ISR(INT4_vect)
{
	ShaftCountLeft++; //increment left shaft position count
}

/*********************************************************************
* Function Name:	ISR(INT5_vect)

* Input:			None

* Output:			None

* Logic:			The control goes to this function whenever there is PE5 is triggered with falling edge.

* Example Call:		NA

*********************************************************************/

ISR(INT5_vect)
{
	ShaftCountRight++; //increment right shaft position count2
}

/*********************************************************************/
void strike_pipe()
{
	servo_1(0);
	buz_on;
	delay(1000);
	buz_off;
	servo_1(90);
}

