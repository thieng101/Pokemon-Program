/* COP 3502C Assignment 2
 This program is written by: Hong Thy Nguyen*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXNAMESIZE 15
#define NUMLINES = 12;
#define MAXITEMS = 100;
#define TIMEPERITEM = 5;
#define TIMEPERCUSTOMER = 30;
#define MAXTIME = 1000000000;
//#include "leak_detector_c.h";


typedef struct customer {
    char* name;
    int numOfTickets;
    int lineNumber;
    int timeEnteringLine;
} customer;


typedef struct node {
    customer* customerPtr;
    struct node* next;
} node;


typedef struct queue {
    struct node* front;
    struct node* back;
} queue;

void init(queue* myQueue);
int enqueue(queue* ptr, customer* pointer);
customer* createCustomer();
int checkOutTime(customer* custPtr);
int empty(queue* qPtr);
customer* dequeue(queue* qPtr);
customer* peek(queue* qPtr);
int getNext(queue lines[], int* currTime);

int main() {
    int c = 0;         // test cases
    int n = 0;         // num of customer
    queue myQueue[12]; // declare queues
    int queueIndex;

    // initialize queues 
    for (int l = 0; l < 12; l++) {
        init(&myQueue[l]);
    }

    scanf("%d", &c);

    if (c <= 25) {
        for (int i = 0; i < c; i++) {
            scanf("%d", &n);
            if (n <= 500000) {

                for (int j = 0; j < n; j++) {
                    customer* c = createCustomer(); // get the customer
                    int a = c->lineNumber;
                    enqueue(&myQueue[a - 1], c); // add customer to the queue
                }
                int currTime = 0;
                for (int m = 0; m < n; m++) {
                    queueIndex = getNext(myQueue, &currTime);

                    customer* tmpNode = dequeue(&myQueue[queueIndex]);

                    // So we type less...
                    customer* tmpPtr = tmpNode;

                    // Update starting time, if necessary.
                    if (currTime < tmpPtr->timeEnteringLine)
                        currTime = tmpPtr->timeEnteringLine;

                    // Now, process this customer.
                    currTime += checkOutTime(tmpPtr);

                    // Print out the status.
                    printf("%s from line %d checks out at time %d.\n", tmpPtr->name,
                        tmpPtr->lineNumber, currTime);

                    free(tmpNode->name);
                    free(tmpNode);
                }
            }
            else {
                printf("please enter less than or equal 500000");
            }
        }

    }
    else {
        printf("Please enter less than or equal 25");
    }

    return 1;
}

void init(queue* myQueue) {
    myQueue->back = NULL;
    myQueue->front = NULL;
}

customer* createCustomer() {
    char name[16];

    customer* customer = malloc(sizeof(customer));
    scanf("%d %d %s %d", &customer->timeEnteringLine, &customer->lineNumber, name,
        &customer->numOfTickets);
    int length = strlen(name) + 1;
    customer->name = malloc(sizeof(char) * length);
    strcpy(customer->name, name);

    return customer;
}

int enqueue(queue* ptr, customer* pointer) {
    // create a temp node with the customer
    node* temp = malloc(sizeof(node));
    temp->customerPtr = pointer;
    temp->next = NULL;

    if (temp != NULL) {
        if (ptr->back != NULL)
            ptr->back->next = temp;
        ptr->back = temp;
        if (ptr->front == NULL)
            ptr->front = temp;
        return 1;
    }
    else {
        return 0;
    }
}

customer* peek(queue* qPtr) {
    if (qPtr->front != NULL) {
        return qPtr->front->customerPtr;
    }
    else {
        return NULL;
    }
}

int checkOutTime(customer* custPtr) {
    return TIMEPERCUSTOMER + custPtr->numOfTickets * TIMEPERITEM;
}

int empty(queue* qPtr) { return qPtr->front == NULL; }

customer* dequeue(queue* qPtr) {                        // dequeue will return a pointer
    customer* saveval;

    if (qPtr->front == NULL) {
        return NULL;
    }

    saveval = qPtr->front->customerPtr;
    node* temp = qPtr->front;

    qPtr->front = qPtr->front->next;

    free(temp);

    if (qPtr->front == NULL)
        qPtr->back = NULL;

    return saveval;
}

int getNext(queue lines[], int* currTime) {
    // Store the line number in res.
    int res = -1, minTickets = MAXITEMS + 1;

    // declare bestime (for first customer cases)
    int bestTime = MAXTIME + 1;

    // First just look for active lines. 
    for (int i = 0; i < NUMLINES; i++) {

        // check if the current line if empty(skip if it is empty)
        if (empty(&lines[i]) == 1)
            continue;


        // This is the person we are considering.
        customer* tmp = peek(&lines[i]);

        // if the current customer has a time in line that is bigger than currTime
        // simply continue to the next iteration
        if (tmp->timeEnteringLine > *currTime)
            continue;


        // Only improve if we lower minimum # of items.
        if (tmp->numOfTickets < minTickets) {
            // update your result variable to store the current index of the line this
            // customer is at
            res = i;
            // update minTickets with the new smaller number of tickets (the one of
            // tmp)
            minTickets = tmp->numOfTickets;
        }
    }

    // we check if our initial processing did lead to a valid result, if that is
    // the case
    // then just return it no need to continue comparing
    if (res != -1)
        return res;

    // If we get here, then we are just looking for the first active line.
    for (int j = 0; j < NUMLINES; j++) {
        // check if the current line if empty(skip if it is)
        if (empty(&lines[j]) == 1)

            continue;

        // if that empty check fails then, This is the person we are considering.
        customer* tmp = peek(&lines[j]);

        // In this pass, we skip these.
        if (tmp->timeEnteringLine < bestTime) {
            // update your result variable with i
            res = j;
            // update bestTime with the current time of tmp that is less than what
            // besttime stores
            bestTime = tmp->timeEnteringLine;
        }
    }

    return res;
}
