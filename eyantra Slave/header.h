/*
 * Team Id: 1972
 
 * Author List:		R. Ajith Kumar, V. M. Gokul Nath
 
 * Filename:		header.h
 
 * Theme:			Bothoven
 
 * Functions:		All the functions are declared in this header file
					
 * Global Variables:All the global variables are declared in this header file
					 
 */

#ifndef HEADER_H_
#define HEADER_H_

/**************************************************************/

#define  F_CPU 14745600UL							

/**************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

/**************************************************************/

#define N 48
#define inf 99
#define set(reg,bit)	reg |= (1<<bit)
#define clr(reg,bit)	reg &= ~(1<<bit)
#define buz_on        set(PORTC,3)									//Buzzer on
#define buz_off       clr(PORTC,3)									//Buzzer off
#define delay(x)     _delay_ms(x)

/**************************************************************/

					
extern uint8_t map[N][N];												//The adjacency matrix or cost matrix of the map
extern int8_t dir[N][N];												//The matrix that has the angle between the two node i,j with postitive x axis
extern const int8_t mnp[34][6];											//array to store the valid nodes for the MNPs
extern int8_t obstacle_flag;	
extern int8_t path[N];													//array that has the node sequence from source to target
extern int8_t current_dir;												//current orientation
extern int8_t current_node;
extern int8_t next_node;												//current node
extern int8_t previous_node;
extern long int ShaftCountRight;
extern long int ShaftCountLeft;
extern uint8_t data_rec;
extern int8_t current_mnp1;
extern int8_t current_mnp2;
extern int _2_mnp_flag;
extern int obstacle_val_1;
extern int obstacle_val_2;



/**************************************************************/

void add_edge(uint8_t v1,uint8_t v2,uint8_t val);						//to add edge between two nodes
void del_edge(uint8_t v1,uint8_t v2);									//to delete edge between two nodes
void add_dir( int8_t d1, int8_t d2, int8_t val);						//to add direction between two nodes
void create_graph();													//creates the graph ie adjacency matrix
void create_dir();														//creates the direction matrix

/**************************************************************/

int dijkstra(int source, int target);									//shortest path algorithm
void get_path(uint8_t parent[],int8_t source,int8_t target);			//to get the sequence of nodes from source to target
int8_t node_for_mnp(int current_node,int mnp_next);						//nearest valid node from current node for the target MNP

/**************************************************************/

void fwd();																//to move forward
void right();															//to rotate right
void left();
void soft_left();
void soft_right();															//to rotate left 
void stop();															//to stop
void angle_rotate(unsigned int dir_val);								//to rotate through given angle
void angle_rotate_r(unsigned int dir_val);
void speed (unsigned char left_motor, unsigned char right_motor);		//to set speed for the both motors
void line_follow(int currentnode,int nextnode);														//line follow
void source_to_target(int8_t source,int8_t target);						//to move from source to target
void node_to_node(int8_t current_no,int8_t next_no);					//to move from node to node
void check_obstacle(int8_t current_nod, int8_t next_nod);				//to check obstacle
int constrain(int variable,int up_limit,int low_limit);
void servo_1(unsigned char degrees);
ISR(INT4_vect);															//interrupt service routine for interrupts PE4 PE5
ISR(INT5_vect);
void strike_pipe();


/**************************************************************/

void main_init();														//all initialization
void init_pins();														//pin initialization
void adc_init();														//adc initialization
void timer5_init();														//timer 5 initialization
void timer1_init();														//timer 1 initialization
void position_encoder_interrupt_init ();								//position encoder initialization
unsigned char adc_conv(unsigned char channel);							//to get adc value
unsigned int sharp_dist(unsigned char adc_reading);						//to get the distance detected by the sharp sensor							




/**************************************************************/

void lcd_set_4bit();
void init_ports();
void lcd_reset_4bit();
void lcd_init();
void lcd_wr_command(unsigned char cmd);
void lcd_wr_char(char);
void lcd_home();
void lcd_cursor(char, char);
void lcd_print(char, char, unsigned int, int);
void lcd_string(char*);

/**************************************************************/


void uart0_init(void);
void send_data(uint8_t data);
ISR(USART0_RX_vect);
void wait();

#endif /* HEADER_H_ */