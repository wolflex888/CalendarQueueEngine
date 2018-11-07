/*
 * HW3_app.c
 *
 *  Created on: Oct 2, 2017
 *      Author: Felice Chan
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sim.h"
#include "FIFO_TEST.h"


// Function Prototype
double nextTime (double rateParameter);

// Event types (incoming calls, finishing calls)
#define	INCOMING        1
#define	FINISHING_INC   2
#define OUTGOING        3
#define OUTGOING_EVAL	4
#define FINISHING_OUT   5

// Call Center Variables
int numService = 700;
int numSales = 300;

//Initialize number of service and sales agents on the phone
int ServiceOnPhone = 0; // If ServiceOnPhone = 0, the service agent is free.
int	SalesOnPhone=0;	    // If SalesOnPhone = 0, the sales agent is free.
int inQueue = 0;        // Counts number of people that are currently in the queue
int totInQueue = 0;     // Counts total number of people in Queue.
int HungUp = 0;         // Will count number of people that hung up if no agents were available
int NoWaiting = 0;      // Counts number of people that didn't have to wait
int totalIncoming = 0;  // Counts total number of incoming calls
double avg_time_waited = 0;
double time_waited = 0;
double sum_waiting_time = 0;

//Outgoing calls global variables
int NSalesIdle = 0;
int SuccessfulOutgoing = 0;
int Abandoned = 0;
int totalOutgoing = 1;
int totalUnsuccessful = 0;
int loop = 10;
double S_est = 0.60;
double S_true = 0.6;


double rateParameter = 2;
//double sum_total_time = 0.0, avg_total_time = 0.0;  // records the total/average time a caller stays in the system

// Events have three parameters, the event type, and type of agent handling the call
struct EventData {
	int EventType;
	int ServiceAgent;
	int SalesAgent;
};

// prototypes for event handlers
// Event Handlers for incoming calls
void Incoming (struct EventData *e);        // Incoming call event
void Finishing_inc (struct EventData *e);	// Agent hangs up

//Will need three more for starting and ending outgoing calls
void Outgoing_eval (struct EventData *e);	//Evaluate num agents available
void Outgoing (struct EventData *e);        // Outgoing call event
void Finishing_out (struct EventData *e);	// SALES Agent hangs up - end outgoing call

///////////////////////////////////////////////////////////
//Structure of people in queue. Also records time entered
//queue and time left queue (when call was answered)
///////////////////////////////////////////////////////////
struct CallerQ {
	int caller_num;  //number of people who entered the queue
	double ts_start; //time entered queue
	double ts_end;   //time left queue
} CallerQ;


struct CallerQ * ptr_c;
//ptr_c = (struct CallerQ*)malloc(sizeof(struct CallerQ));

Queue *myList = NULL;

///////////////////////////////////////////////////////////
//Event Handler
///////////////////////////////////////////////////////////
void EventHandler (void *data){

	struct EventData *d;
	//int loop;

	// coerce type
	d = (struct EventData *) data;
	// call an event handler based on the type of event
	if (d->EventType == INCOMING) Incoming (d);
	else if (d->EventType == FINISHING_INC) Finishing_inc (d);
	else if (d->EventType == OUTGOING_EVAL) Outgoing_eval (d);
	else if (d->EventType == OUTGOING) Outgoing (d);
	else if (d->EventType == FINISHING_OUT) Finishing_out (d);
	//else if (fprintf (stderr, "Illegal event found\n")); exit(1); }
}

//////////////////////////////////////////////////////////
//Incoming Call function
//////////////////////////////////////////////////////////
void Incoming (struct EventData *e){
	struct EventData *d;
	double ts;
	//double ts_end = 0;


//		printf ("Processing Incoming Call event at time %f, ServiceOnPhone=%d\n", CurrentTime(), ServiceOnPhone);
//		printf ("Processing Incoming Call event at time %f, SalesOnPhone=%d\n", CurrentTime(), SalesOnPhone);
//		printf ("No Waiting %d\n",NoWaiting);
//		printf ("Currently in Queue %d\n", inQueue);
//		printf ("Hung up %d\n", HungUp);
//		printf ("Total In Queue%d\n", totInQueue);
//		printf ("Total Incoming %d\n",totalIncoming);

		int TOTAL = NoWaiting + HungUp + totInQueue; //This should be equal to total incoming
		printf("TOTAL NUM FOR CHECKING %d\n", TOTAL);
		printf ("-----------------------------------------------\n");

	//Error Checking
	if (e->EventType != INCOMING) {
		fprintf (stderr, "Unexpected event type\n");
		exit(1);
	}
	if((d=malloc(sizeof(struct EventData)))==NULL) {
		fprintf(stderr, "Memory Allocation error\n");
		exit(1);
	}

	//Schedule next incoming call event
	d->EventType = INCOMING;
	ts = CurrentTime() + nextTime(rateParameter);
	totalIncoming ++;
	Schedule (ts, d);

	printf("IN QUEUE CHECK BEFORE LOOPS%d\n",inQueue);

	//Check if agents are free (in queue = 0). First check if service agents are free, then check sales agents
	// if a service agent is free, will talk for call duration; schedule departure event
	if (inQueue == 0){

		//If service agents are free
		if (ServiceOnPhone < numService ) {
			printf("goes into in queue > 0 and SERVICEONPHONE==");
			NoWaiting ++;
			ServiceOnPhone ++;
			if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
			d->EventType = FINISHING_INC;
			d->ServiceAgent = 1; d->SalesAgent = 0;
			double CallDuration = rand() % 400 + 300;
			ts = CurrentTime() + CallDuration;
			Schedule (ts, d);
			free (e);
		}

		//If service agents are all busy, but sales agents are free
		else if(ServiceOnPhone == numService && SalesOnPhone < numSales ){
			NoWaiting ++;
			SalesOnPhone ++;
			if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
			d->EventType = FINISHING_INC;
			d->ServiceAgent = 0; d->SalesAgent = 1;
			double CallDuration = rand() % 400 + 300;
			ts = CurrentTime() + CallDuration;
			Schedule (ts, d);
			free (e);
		}

		//If service agents and sales agents are all busy
		else if (ServiceOnPhone == numService && SalesOnPhone == numSales){


			//The person has a 25% chance of hanging up.
			double rnHangUp = rand() % 101;
			if (rnHangUp <= 25){
				HungUp++;
			}
			//Else, they join the queue
			else {
				inQueue ++;
				totInQueue ++;
				struct CallerQ * ptr_c;
				ptr_c = (struct CallerQ*)malloc(sizeof(struct CallerQ));
				//ptr_c -> ts_end = ts_end;
				q_enq(myList, ptr_c);
				ptr_c -> ts_start = CurrentTime();
				printf("TIME START %f\n",ptr_c -> ts_start);
			}
			if (d->EventType == FINISHING_INC){
				q_deq(myList);
				ptr_c ->ts_end = CurrentTime();
				printf("TS END %f\n",ptr_c ->ts_end);
				double time_waited = ptr_c ->ts_end - ptr_c -> ts_start;
//				avg_time_waited += (double)time_waited/(double)inQueue;
				printf("Average Time Waited%f\n",time_waited);
			}
		}
		return;
	}

	//If there are already people in the queue
	/////////////////////////////////////////////////////////////////////////////////
	if (inQueue > 0) {

		if (ServiceOnPhone == numService && SalesOnPhone == numSales){

			//They have a 25% chance of hanging up
			int rnHangUp = rand() % 101;
			if (rnHangUp <= 25){
				HungUp++;
			}
			//Else, they join the queue too.
			else {
				inQueue ++;
				totInQueue ++;
				q_enq(myList, ptr_c);
				//****THIS makes it crash :(
				//ptr_c -> ts_start = CurrentTime();
				//printf("TIME START %f\n",ptr_c -> ts_start);
			}


		}

		//If Service agents are free, but sales agents are not free, the call gets routed to service agents
		else if (ServiceOnPhone < numService && SalesOnPhone == numSales) {
			NoWaiting ++;
			ServiceOnPhone ++;
			if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
			d->EventType = FINISHING_INC;
			d->ServiceAgent = 1;
			d->SalesAgent = 0;
			double CallDuration = rand() % 400 + 300;
			ts = CurrentTime() + CallDuration;
			Schedule (ts, d);
			free (e);
		}
		//If Service agents are all busy, but sales agents are free
		else if (ServiceOnPhone == numService && SalesOnPhone < numSales) {
			NoWaiting ++;
			SalesOnPhone ++;
			if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
			d->EventType = FINISHING_INC;
			d->ServiceAgent = 0;
			d->SalesAgent = 1;
			double CallDuration = rand() % 400 + 300;
			ts = CurrentTime() + CallDuration;
			Schedule (ts, d);
			free (e);
		}
	}
}
////////////////////////////////////////////////////////////
//Finishing Call function
////////////////////////////////////////////////////////////
void Finishing_inc (struct EventData *e){

	struct EventData *d;

	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	double ts;
	if (e->EventType != FINISHING_INC) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
	if(e->ServiceAgent == 1 && ServiceOnPhone > 0){
		ServiceOnPhone --;
	}

	else if(e->SalesAgent == 1 && SalesOnPhone > 0){
		SalesOnPhone --;
	}

	// If someone in queue, answer call and, schedule
	if (inQueue>0) {
		if(ServiceOnPhone<numService){
			d->ServiceAgent = 1;
			ServiceOnPhone ++;
		}
		else if(SalesOnPhone<numSales){
			d->SalesAgent = 1;
			SalesOnPhone++;
		}
		//Schedule its departure event and decrement the queue
		double CallDuration = rand() % 400 + 300;
		ts = CurrentTime() + CallDuration;
		d->EventType = FINISHING_INC;
		Schedule (ts, d);
		inQueue --;
		//THIS makes it crash
		//q_deq(myList);
		//ptr_c -> ts_end = CurrentTime();
		//time_waited = ptr_c ->ts_end - ptr_c -> ts_start;
	}

		printf ("Processing Ending call event at time %f, Service on Phone=%d\n", CurrentTime(), ServiceOnPhone);
		printf ("Processing Ending call event at time %f, Sales on Phone=%d\n", CurrentTime(), SalesOnPhone);
//	    printf ("Percentage No Waiting %f\n",100*(double)NoWaiting/(double)totalIncoming);
//	    printf ("Percentage In Queue %f\n", 100*(double)inQueue/(double)totalIncoming);
//	    printf ("Total Incoming %d\n",totalIncoming);
//	    printf ("-----------------------------------------------\n");
	free (e);	// release memory for data part of event
}


/////////////////////////////////////////////////////////////
void Outgoing_eval (struct EventData *e){
	double ts;
	struct EventData *d;

	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	if (e->EventType != OUTGOING_EVAL) {fprintf (stderr, "Unexpected event type\n"); exit(1);}

	ts = CurrentTime();
	//printf("Evaluating Number Sales Available at TIME SIXTY : ----%f\n",ts);

	NSalesIdle = numSales - SalesOnPhone;

	printf("IDLE____%d\n",NSalesIdle);
	if (totalOutgoing > 0){
		printf("TotalOutgoing%d\n",totalOutgoing);
		if (SuccessfulOutgoing>0) {
			S_est = (double)SuccessfulOutgoing / (double)totalOutgoing;

		}
		printf("SuccessfulOutgoing%d\n",SuccessfulOutgoing);
		printf("S_est,%f\n",S_est);
		loop = ceil((double)NSalesIdle/(S_est));
		printf("TESTING Loop%d\n",loop);
	}

	//Schedule the agents to answer in 10 seconds from now
	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	d->EventType = OUTGOING;
	ts = CurrentTime() + 10;
	printf("Time Stamp%f\n",ts);
	Schedule (ts, d);

//	printf("Processing Outgoing Call Evaluation at time %f, Sales on Phone = %d\n", CurrentTime(), SalesOnPhone);
//	printf("Sales Available  %d\n", NSalesIdle);
//	printf("----------------------------------------------");

	free(e);
}

///////////////////////////////////////////////////////////////
void Outgoing (struct EventData *e){

	double ts;


	struct EventData *d;
	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	if (e->EventType != OUTGOING) {fprintf (stderr, "Unexpected event type\n"); exit(1);}



	ts = CurrentTime();
	printf("TIME OF OUTGOING RECEIVED BY AGENTS : ----%f\n",ts);

	//Adjust the value of S_true. For the first hour it's 0.4, After that it's 0.6 for another 5 hours, then it changes
	//back to 0.4.

	printf("Outside of loop\n");
	printf("LOOP%d\n",loop);
	for(int i = 0; i < loop; i++){
		NSalesIdle = numSales - SalesOnPhone ;
		totalOutgoing ++;
		double rn_CustNoAns = ((double)rand()) / RAND_MAX;

		if (ts <= 3600 || ts >= 18000){
			S_true = 0.4;
		}

		else if (ts > 3600 && ts < 18000){
			S_true = 0.6;
		}

		if (rn_CustNoAns <= S_true){
			totalUnsuccessful ++;
		}

		else {
			//Customer picks up = Successful outgoing call
			SuccessfulOutgoing ++;

			if(NSalesIdle == 0) {
				Abandoned++;
			}
			//If there are sales agents available, they pick up
			if(NSalesIdle > 0){
				SalesOnPhone++;
				//Schedule Hang up event
				if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
				d->EventType = FINISHING_OUT;
				double CallDuration = rand() % 200 + 200;
				ts = CurrentTime() + CallDuration;
				Schedule (ts, d);
			}
		}

	} //End For Loop


	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
		d->EventType = OUTGOING_EVAL;
		ts = CurrentTime() + 60;
		printf("Time Stamp%f\n",ts);
		Schedule (ts, d);


//	printf("Total Number Outgoing Calls %d\n", totalOutgoing);
//	printf("Number Abandoned %d\n", Abandoned);
//	printf("Successful Outgoing %d\n", SuccessfulOutgoing);
//	printf("Total Unsuccessful %d\n", totalUnsuccessful);
//	printf("***************************************\n");

	free(e);
}//End outgoing function
/////////////////////////////////////////////////////////////
//FINISHING OUTGOING CALL FUNCTION
/////////////////////////////////////////////////////////////
void Finishing_out (struct EventData *e){

	struct EventData *d;
	double ts;

	//	printf ("Processing Outgoing Call Hangup at time %f, Sales On Phone=%d\n", CurrentTime(), SalesOnPhone);
	if (e->EventType != FINISHING_OUT) {fprintf (stderr, "Unexpected event type\n"); exit(1);}

	SalesOnPhone --;
//	    printf ("Processing End Outgoing Call event at time %f, Service on Phone=%d\n", CurrentTime(), ServiceOnPhone);
//	    printf ("Processing End Outgoing Call event at time %f, Sales on Phone=%d\n", CurrentTime(), SalesOnPhone);
//	    printf ("-----------------------------------------------\n");
	free (e);
}

/////////////////////////////////////////////////////////////
//Poisson Distribution times
/////////////////////////////////////////////////////////////
double nextTime (double rateParameter){

	double rn = rand() ;
	double poisson= -log(1.0 - rn / (double) RAND_MAX) / rateParameter;
	printf("Random Number from Poisson %f\n",poisson);
	return poisson;
}


/////////////////////////////////////////////////////////////
//Main Function
/////////////////////////////////////////////////////////////

int main (void){

//	srand(time(NULL));
	printf("Start Main\n");
	struct EventData *d;
	double ts;

	myList = q_init();

	init_queue();

	// initialize event list with first incoming call
	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	d->EventType = INCOMING;
	ts = CurrentTime() + nextTime(rateParameter);
	Schedule (ts, d);


	// initialize event list with first outgoing calls
	if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	d->EventType = OUTGOING_EVAL;
	ts = CurrentTime() + 60;
	Schedule (ts, d);


	RunSim(28800.0);
	float Pc_NoWaiting = 100*((float)NoWaiting/(float)totalIncoming);
	float Pc_HungUp = 100*((float)HungUp/(float)totalIncoming);
	float Pc_Waited = 100*((float)totInQueue/(float)totalIncoming);


	printf("FINAL NUMBERS:\n");
	printf("Percent No Waiting %f\n", Pc_NoWaiting);
	printf("Percent Hung Up %f\n", Pc_HungUp);
	printf("Percent Waited in Queue %f\n", Pc_Waited);
	//printf("Service On Phone %d\n",ServiceOnPhone); // If ServiceOnPhone = 0, the service agent is free.
	printf("Total Incoming %d\n",totalIncoming);
	printf("----------------------------------------------\n");

		printf("Total Outgoing = %d\n", totalOutgoing);
		printf("Successful Outgoing = %d\n", SuccessfulOutgoing);
		printf("Unsuccessful Outgoing = %d\n", totalUnsuccessful);
		printf("Abandoned = %d\n", Abandoned);
		printf("----------------------------------------------");



	printf("End Main\n");
}
