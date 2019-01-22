/*
 * Team Id: 1972
 
 * Author List:		R. Ajith Kumar, V. M. Gokul Nath
 
 * Filename:		main.c
 
 * Theme:			Bothoven
 
 * Functions:		int dijkstra(int source,int target),void get_path(uint8_t parent[],int8_t path[],int8_t source,int8_t target),
					int8_t node_for_mnp(int current_no,int mnp_next)
					

 * Global Variables: path[N],map[N][N],dir[N][N]
					 
 */

#include "header.h"										//Header file	

/*********************************************************************
* Function Name:	int dijkstra(int source,int target)

* Input:			source,target,map[N][N]

* Output:			returns the path length from the source node to target node and also creates 
					array sequence of parent nodes from the target node to source node

* Logic:			Dijkstra algorithm 


* Example Call:		length=dijkstra(2,6);

*********************************************************************/

int dijkstra(int source,int target)
{
	int i,j=0;										//	i: loop variablr, j: temproary variable to store array index
	for(i=0;i<N;i++)//
	{
		path[i]=-1;									// setting all to -1 to mark the end of the array
	}
	
	if((source==-1) || (target==-1))				//returns inf value when source or target node is -1. This is because in node_for_mnp function, which finds the 
													//minimum distance from the current node to the target mnp, it will pass -1 values as the mnp[N] array has -1 values.	
	{return inf;}//
		
	uint8_t len[N],visited_set[N]={0},parent[N];	//len[N] : length from source node to each of other nodes, visited_set[N]: to mark the nodes whose 
													//distances are calculated ,parent[N]:To store parent of each node
	
	for(i=0;i<N;i++)
	{//
		if(map[source][i]==0)						//initially checking if edge is present between source node and other nodes and setting them to inf if not
		{											// and setting them to the value given in the map[N][N] matrix if yes and also setting source node as parent
			len[i]=inf;					
			parent[i]=0;
		}
		else
		{
			len[i]=map[source][i];
			parent[i]=source;
		}
	}
	visited_set[source]=1;							//source node is visited.
	len[source]=0;									// length of source to source is 0
	while(!visited_set[target])						//find  nodes with minimum distance from source node, check for 
	{												//other alternate better path till the target is reached.
		int min,i,min_index;
		min=inf;
		for(i=0;i<N;i++)
		{
			if((!visited_set[i])&&(len[i]<min))		//checking for minimum length
			{
				min=len[i];
				min_index=i;
			}
		}
		
		visited_set[min_index]=1;
		for(i=0;i<N;i++)
		{
			if((!visited_set[i])&&(map[i][min_index]!=0))
			{
				if(len[min_index]+map[i][min_index]<len[i])			//checking if any other alternate path is shotest than the 
				{													//current calculated path and replacing if so found
					len[i]=len[min_index]+map[i][min_index];
					parent[i]=min_index;
				}
			}
		}
	}
	get_path(parent,source,target);									// function call to get_path to convert the parent array into path array.
	return len[target];												//returns the shortest length from the source to target.
}

/*********************************************************************
* Function Name:	void get_path(uint8_t parent[],int8_t source,int8_t target)

* Input:			parent array, source_node, target_node

* Output:			creates an array sequence of nodes from the source_node to target_node

* Logic:			It reads the parent[target] value , this value is the parent of the target and continuously 
					reads the parent values from the parent array till the source node and stores the correct 
					sequence of nodes from source to target in path array

* Example Call:		get_path(parent[],2,6);

*********************************************************************/

void get_path(uint8_t parent[],int8_t source,int8_t target)
{
	int8_t t,temp[N],k=0,flag=0;							//temp[N] : temporary array used in conversion of parent array to path array
	int j=0,i=target;
	while(i!=source)										//tracing back from target to source by looking the parent node of each node
	{														//and storing in temp[] array, ahich would be in reverse order.
		t=parent[i];
		temp[j]=t;
		i=t;
		j++;k++;
	}
	for(j=k-1,i=0;j>=0;j--,i++)								//reversing the order of the temp [] array and storing in path[] array in correct order 
	{														//from source to target, k: used to get length of the temp[] array
		path[i]=temp[j];
		if(path[i]==target)
		flag=1;
	}
	if(flag!=1)
	path[i]	=target;
	
}

/*********************************************************************
* Function Name:	int8_t node_for_mnp(int current_no,int mnp_next)

* Input:			current_node, next_MNP

* Output:			returns the node for a MNP that is closest from current node

* Logic:			It reads the mnp[34] array, it finds the distance of the all valid nodes 
					for the MNP from current node and choses the node with minimum distance

* Example Call:		node_for_mnp(2,7);

*********************************************************************/

int8_t node_for_mnp(int current_no,int mnp_next)
{
	int8_t m,min_dist=inf,next_target_node,temp_dist;					//min_dist : initially minimum distance is set to inf
	for (m=0;m<6;m++)				
	{																	//from the current node , the distance of all valid nodes of the target MNP
		temp_dist=dijkstra(current_no,mnp[mnp_next][m]);				//is calculated and the node with minimum distance is chosen.
		if(temp_dist<min_dist)
		{
			min_dist=temp_dist;
			next_target_node=mnp[mnp_next][m];
		}
	}
	temp_dist = dijkstra(current_no,next_target_node);					//finally call the dijikstra function with node having shortest distance so that
	return next_target_node;											//path[] array has the required path and it returns the minimum distance target node
}

/**************************************************************/

int8_t node_for_note(int current_node,int note_next)
{
	int8_t m,n,min_dist=inf,next_target_node,temp_dist,mnp_next;
	for(n=0; note[note_next][n]!=-1;n++)
	{
		mnp_next=note[note_next][n];
		for (m=0;m<6;m++)
		{
			temp_dist=dijkstra(current_node,mnp[mnp_next][m]);
			if(temp_dist<min_dist)
			{
				min_dist=temp_dist;
				next_target_node=mnp[mnp_next][m];
				mnp_for_slave=mnp_next;
			}
		}
	}
	
	temp_dist = dijkstra(current_node,next_target_node);
	return next_target_node;
}

/**************************************************************/

char overall_shortest_path(int8_t current_node_master,int8_t current_node_slave,int8_t note1,int8_t note2)
{
	int8_t m1,m2,s1,s2,x,m1m2,s1s2,m1s2,m2s1;
	
	x= node_for_note(current_node_master,note1);
	m1= dijkstra(current_node_master,x);
	
	x= node_for_note(current_node_master,note2);
	m2= dijkstra(current_node_master,x);
	
	x= node_for_note(current_node_slave,note1);
	s1= dijkstra(current_node_slave,x);
	
	x= node_for_note(current_node_slave,note2);
	s2= dijkstra(current_node_slave,x);
	
	m1s2 = m1+s2;				//A4
	m2s1 = m2+s1;				//B2
	m1m2 = m1+m2;				//C2
	s1s2 = s1+s2;				//D4
	
	
	if((m1s2<=m2s1)&&(m1s2<=m1m2)&&(m1s2<=s1s2))
	{
		return 'A';
	}
	if((m2s1<m1s2)&&(m2s1<m1m2)&&(m2s1<=s1s2))
	{
		return 'B';
	}
	if((m1m2<m1s2)&&(m1m2<m2s1)&&(m1m2<s1s2))
	{
		return 'C';
	}
	if((s1s2<m1s2)&&(s1s2<m2s1)&&(s1s2<m1m2))
	{
		return 'D';
	}
}




