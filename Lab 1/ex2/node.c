/*************************************
* Lab 1 Exercise 2
* Name:
* Student No:
* Lab Group:
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "node.h"

// copy in your implementation of the functions from ex1
// there is on extra function called map which you have to fill up too
// feel free to add any new functions as you deem fit

// inserts a new node with data value at index (counting from the front
// starting at 0)
void insert_node_from_head_at(list *lst, int index, int data)
{
    node *n = (node *)malloc(sizeof(node));
	n->data = data;

	if (!lst->head){
		lst->head = n;
		lst->tail = n;
		n->prev = NULL;
		n->next = NULL;
	}
	//if index == 0
	else if (index==0){
		node *firstNode = lst->head;
		firstNode->prev= n;
		n->next = firstNode;
		n->prev = NULL;
		lst->head = n;
	}
	else{
		node *curr = lst->head;
		while(index){
			curr = curr->next;
			index--;
		}
		if(curr){
			n->next = curr;
			n->prev = curr->prev;
			curr->prev->next = n;
			curr->prev = n;
		}
		else{
			n->next = NULL;
			n->prev = lst->tail;
			lst->tail->next = n;
			lst->tail = n;
		}
	}

}

// inserts a new node with data value at index (counting from the back
// starting at 0)
void insert_node_from_tail_at(list *lst, int index, int data)
{
	node *n = (node *)malloc(sizeof(node));
	n->data = data;

	if (!lst->tail){
		lst->head = n;
		lst->tail = n;
		n->prev = NULL;
		n->next = NULL;
	}
	//if index == 0
	else if (index==0){
		lst->tail->next = n;
		n->next = NULL;
		n->prev = lst->tail;
		lst->tail = n;
	}
	else{
		node *curr = lst->tail;
		while(index){
			curr = curr->prev;
			index--;
		}
		if(curr){
			n->next = curr->next;
			n->prev = curr;
			curr->next->prev = n;
			curr->next = n;
		}
		else{
			n->next = lst->head;
			n->prev = NULL;
			lst->head->prev = n;
			lst->head = n;
		}
	}
}

// deletes node at index counting from the front (starting from 0)
// note: index is guaranteed to be valid
void delete_node_from_head_at(list *lst, int index)
{
    node *n;

	if (!lst->head){
		return;
	}
	//if index == 0
	else if (index==0){
		n = lst->head;
		lst->head = n->next;
		if(lst->head == NULL){
			lst->tail= NULL;
		}
		else{
			lst->head->prev = NULL;
		}
	}
	else{
		n = lst->head;
		while(index){
			n = n->next;
			index--;
		}
		if(n->next){
			n->prev->next = n->next;
			n->next->prev = n->prev;
		}
		else{
			lst->tail = n->prev;
			lst->tail->next = NULL;
		}
	}

	free(n);
	n = NULL;
}

// deletes node at index counting from the back (starting from 0)
// note: index is guaranteed to be valid
void delete_node_from_tail_at(list *lst, int index)
{
    node *n;

	if (!lst->tail){
		return;
	}
	//if index == 0
	else if (index==0){
		n = lst->tail;
		lst->tail = n->prev;
		if(lst->tail == NULL){
			lst->head = NULL;
		}
		else lst->tail->next = NULL;
	}
	else{
		n = lst->tail;
		while(index){
			n = n->prev;
			index--;
		}
		if(n->prev){
			n->prev->next = n->next;
			n->next->prev = n->prev;
		}
		else{
			lst->head = n->next;
			lst->head->prev = NULL;
		}
	}

	free(n);
	n = NULL;
}

// resets list to an empty state (no nodes) and frees any allocated memory in
// the process
void reset_list(list *lst)
{
    while(lst->head){
		delete_node_from_head_at(lst, 0);
	}
}

// traverses list and applies func on data values of all elements in the list
void map(list *lst, int (*func)(int))
{
    node* curr_ptr = lst->head;
	while(curr_ptr){
		curr_ptr->data = func(curr_ptr->data);
		curr_ptr = curr_ptr->next;
	}
}

// traverses list and returns the sum of the data values of every node
// in the list
long sum_list(list *lst)
{
	node* curr_ptr = lst->head;
	long sum = 0;
	while(curr_ptr){
		sum += curr_ptr->data;
		curr_ptr = curr_ptr->next;
	}
	return sum;
}
