/*
 * Team Id: 1972
 
 * Author List:		R.Ajith Kumar, V.M.Gokul Nath, M.Jyothikannan
 
 * Filename:		graph.c
 
 * Theme:			Bothoven
 
 * Functions:		void add_edge(uint8_t v1,uint8_t v2,uint8_t val), void del_edge(uint8_t v1,uint8_t v2), void add_dir( int8_t d1, int8_t d2, int8_t val),void create_graph()
                    void create_dir().
					
 * Global Variables: uint8_t map[N][N], int8_t dir[N][N],  const int8_t mnp[34][6]
					 
 */

/*********************************************************************/

#include "header.h"             //header file

uint8_t map[N][N];             //map[][]: array to store the cost of travel between one node to other. 0 indicates there is no edge
int8_t dir[N][N];              //dir[][]: array to store direction of node i,j with x axis as reference ( +ve x-axis)
  
 const int8_t mnp[34][6]={{-1,-1,-1,-1,-1,-1},			//mnp[][]:array to know valid nodes of a MNP, //mnp0 does not exist
						  {0,-1,-1,-1,-1,-1},			//mnp1
						  {1,-1,-1,-1,-1,-1},			//mnp2	  	
						  {2,-1,-1,-1,-1,-1},			//mnp3	
						  {3,-1,-1,-1,-1,-1},			//mnp4
						  {4,-1,-1,-1,-1,-1},			//mnp5
						  {5,-1,-1,-1,-1,-1},			//mnp6
						  {6,-1,-1,-1,-1,-1},			//mnp7
						  {7,-1,-1,-1,-1,-1},			//mnp8
						  {8,-1,-1,-1,-1,-1},			//mnp9			
						  {9,-1,-1,-1,-1,-1},			//mnp10
						  {10,-1,-1,-1,-1,-1},			//mnp11
						  {11,-1,-1,-1,-1,-1},			//mnp12
						  {12,-1,-1,-1,-1,-1},			//mnp13
						  {13,-1,-1,-1,-1,-1},			//mnp14
						  {14,-1,-1,-1,-1,-1},			//mnp15
						  {15,-1,-1,-1,-1,-1},			//mnp16
						  {16,-1,-1,-1,-1,-1},			//mnp17
						  {17,-1,-1,-1,-1,-1},			//mnp18
						  {18,-1,-1,-1,-1,-1},	  		//mnp19
						  {19,-1,-1,-1,-1,-1},	  		//mnp20
						  {20,-1,-1,-1,-1,-1},	  		//mnp21
						  {21,-1,-1,-1,-1,-1},	  		//mnp22
						  {22,-1,-1,-1,-1,-1},			//mnp23
						  {23,-1,-1,-1,-1,-1},	  		//mnp24
						  {0,35,36,24,-1,-1},	  		//mnp25
						  {24,25,36,37,42,43},	  		//mnp26
						  {4,25,37,26,-1,-1},			//mnp27
						  {8,27,38,28,-1,-1},	  		//mnp28
						  {38,28,29,39,44,45},	  		//mnp29
						  {12,29,39,30,-1,-1},	  		//mnp30
						  {16,31,32,40,-1,-1},	  		//mnp31
						  {32,33,40,41,46,47},	  		//mnp32
						  {20,33,34,41,-1,-1},}; 		//mnp33

/*********************************************************************
* Function Name:void add_edge(uint8_t v1,uint8_t v2,uint8_t val)

* Input:		v1,v2,val------> v1, v2 two nodes and val distance of nodes.

* Output:		Path was created

* Logic:		It is used for bot to know the possible movement from one node to next one.
   
* Example Call: add_edge(1,2,2) ---->map[1][2]=map[2][1]=2 means it tells there is a possible way from 1 to 2 and also from node 2 to node 1 and its cost is 2

*********************************************************************/ 	
void add_edge(uint8_t v1,uint8_t v2,uint8_t val)
 	{
	 	map[v1][v2]=map[v2][v1]=val;		
	}
	
/*********************************************************************
* Function Name:void del_edge(uint8_t v1,uint8_t v2)

* Input:		v1,v2------>two nodes where obstacles were detected	

* Output:		Delete the edge

* Logic:		It is used to delete the possible movement from one node to next node ,when it detects obstacles between two nodes
				It helps in future to know there is a obstacle in between the two node.

* Example Call: del_edge(1,2);  deletes the edge between node 1 and 2

*********************************************************************/	
void del_edge(uint8_t v1,uint8_t v2)
	{
		map[v1][v2]=map[v2][v1]=0;	
	}

/*********************************************************************
* Function Name:void add_dir( int8_t d1, int8_t d2, int8_t val)

* Input:		d1, d2,val------->d1,d2 two nodes, val-->angle between nodes

* Output:		None

* Logic:		It forms an array to make angle of node reference to +ve x-axis. It was just a call function.
				
	We have used a scale of 15 because using exact angles requires memory of 2 bytes 
	since max value is 180, and also it is global N x N matrix which will take so much data memory.
	
	The angle is measured between node a and node b by.... At node a the bot is in along +ve x axis 
	ie dir_value=0, then the angle required to rotate to move to the node b is the dir_value given 
	in the matrix dir[a][b] and dir[b][a]=180-dir[a][b].
	
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
		
	if angle from node a to node b is some x degree, then from node b to node a angle will be 180-x degree.
 
* Example Call: add_dir(2,24,-4);---> from node 2 to 24 node we require -60 degree of rotation
********************************************************************/
	
void add_dir( int8_t d1, int8_t d2, int8_t val)
{
	int8_t t;
	t=val-12;    	     // 180 degree---->12
	if(val>12)	val-=24; //if conditions used to reduce rotating time . if bot want to rotate for 240 degree 
	if(val<-12)	val+=24; //then instead of rotating left  of 250deree we can rotate (360-240) 120 degree right 
	if(val==-12) val=12;
	dir[d1][d2]=val;
	if(t>12) t-=24;
	if(t<-12) t+=24;	//if conditions used to reduce rotating time . if bot want to rotate for 240 degree 
	if(t==-12) t=12;	//then instead of rotating left  of 250deree we can rotate (360-240) 120 degree right 
	dir[d2][d1]=t;		//if orientation angle from a to b note is some x degree, then from b node to a will 180-x degree.
}

/*********************************************************************
* Function Name:void create_graph()

* Input:		None

* Output:		None

* Logic:		all edges are created using add_edge() 

* Example Call: create graph()---> it implement the map . connecting and non connecting nodes are created

*********************************************************************/
	
void create_graph()
	{	 int q=24,p;
		
		for( p=0;p<23;p++)    //this loop for circular path of arena. Node we are taken from 0 to 47 
		add_edge(p,p+1,2);	 //by plotting clockwise from start and from outer to inner part
		
		for(p=2;p<23;p+=4)      // loop for inner nodes connecting outer circle
		{	
			add_edge(p,q,2);
			q++;
			add_edge(p,q,2);	
			q++;
		}
		add_edge(23,0,2);
		
		
		add_edge(26,37,2);  //separately path of nodes are created
		add_edge(27,38,2);
		add_edge(30,39,2);
		add_edge(31,40,2);
		add_edge(34,41,2);
		add_edge(35,36,2);
		
		add_edge(24,25,2);
		add_edge(25,37,2);
		add_edge(37,43,3);
		add_edge(43,42,3);
		add_edge(42,36,3);
		add_edge(36,24,2);
		
		add_edge(38,28,2);
		add_edge(28,29,2);
		add_edge(29,39,2);
		add_edge(39,45,3);
		add_edge(45,44,3);
		add_edge(44,38,3);
		
		add_edge(40,32,2);
		add_edge(32,33,2);
		add_edge(33,41,2);
		add_edge(41,47,3);
		add_edge(47,46,3);
		add_edge(46,40,3);
		
		add_edge(38,37,2);		
		add_edge(40,39,2);
		add_edge(41,36,2);
		
	}
	
/*********************************************************************
* Function Name:create_dir()

* Input:		None

* Output:		None

* Logic:		all directions are created using add_dir() function 

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
if angle from a to b note is some x degree, then from b node to a will 180-x degree.

* Example Call: add_dir(2,24,-4);---> from node 2 to 24 node we require -60 degree of rotation
                create_dir();
********************************************************************/
void create_dir()
{
	int8_t x=6,p;
	
	for( p=0;p<23;p++)   //loop for outer ciclre
	add_dir(p,p+1,x--);
	
	add_dir(23,0,x);    

	add_dir(2,24,-4);   //separately calculated values
	add_dir(2,25,0);
	
	add_dir(6,26,-8);
	add_dir(6,27,-4);
	
	add_dir(10,28,12);
	add_dir(10,29,-8);
	
	add_dir(14,30,8);
	add_dir(14,31,12);
	
	add_dir(18,32,4);
	add_dir(18,33,8);
	
	add_dir(22,34,0);
	add_dir(22,35,4);
	
	add_dir(26,37,-8);
	add_dir(27,38,-4);
	add_dir(30,39,8);
	add_dir(31,40,12);
	add_dir(34,41,0);
	add_dir(35,36,4);
	
	add_dir(24,25,4);
	add_dir(25,37,0);
	add_dir(37,43,-4);
	add_dir(43,42,-8);
	add_dir(42,36,12);
	add_dir(36,24,8);
	
	add_dir(38,28,0);
	add_dir(28,29,-4);
	add_dir(29,39,-8);
	add_dir(39,45,12);
	add_dir(45,44,8);
	add_dir(44,38,4);
	
	add_dir(40,32,-8);
	add_dir(32,33,12);
	add_dir(33,41,8);
	add_dir(41,47,4);
	add_dir(47,46,0);
	add_dir(46,40,-4);
	
	add_dir(38,37,12);
	add_dir(40,39,4);
	add_dir(41,36,8);
}

/*********************************************************************/
    