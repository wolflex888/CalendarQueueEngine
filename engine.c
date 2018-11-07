/*
 * engine.c
 *
 *  Created on: Sep 29, 2017
 *      Author: DavidLu
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sim.h"
/// structure definition ///
typedef struct Event {
	double timestamp;		// event timestamp
    void *AppData;			// pointer to event parameters
	struct Event *Next;		// priority queue pointer
}node;



/* function prototypes */
void Schedule(double ts, void *data);
double CurrentTime(void);
void resize(int new_size);
void PrintList (int i);
node* Deque(void);
void local_ini(int qbase, int nbuck, double bwidth, double startprio);
void init_queue(void);
double new_width(void);
double rand_time (double d);
double random_number (void);

/// initialize necessary variable ///
// set the simulation time to zero //
double Now = 0.0;
// initialize the future event list structure //

// declare initial length of the bucket list //
int length = 64;
// declare width of the bucket //
double width = 1;
// initialize the queue counter //
int qsize = 0;
node **bucket_list;  // initialize original bucket list //
node **the_list;
//initialize variable that stores bucket range//
double bucket_top = 1;
int last_bucket=0;
double last_prior = 0;
int first_sub;
int the_list_length = 128000;
int top_threshold;
int bot_threshold;
int resizeability=0;


/// main loop (event processing loop) ///
void RunSim(double EndTime) {
	printf("engine engaged\n");
	struct Event *e;

		printf ("Initial event list:\n");
		for(int i = 0; i < length; i++){
			PrintList(i);
		}

		// Main scheduler loop
		while ((e=Deque()) != NULL) {
			Now = e->timestamp;//+rand_time(1.0);
	        if (Now > EndTime) break;
	        //printf("now is %s\n", e->AppData);
			EventHandler(e->AppData);
			//printf("oh... another loop\n");
			free (e);	// it is up to the event handler to free memory for parameters
//			printf("Schedule event list checke\n");
//						for(int i = 0; i < length; i++){
//							PrintList(i);
//						}
		}
//printf("final event list: \n");
for(int i = 0; i < length; i++){
			PrintList(i);
}


//int i = 0;
//Schedule(4.0, &i);
//Schedule(2.0, &i);
//Schedule(6.0, &i);
//Schedule(0.34, &i);
//Schedule(7.0, &i);
//Schedule(17.0, &i);
//Schedule(13.0, &i);
//Schedule(11.0, &i);
//printf("list length = %d\n", length);
//for(int i = 0; i < length; i++){
//		PrintList(i);
//	}
//Schedule(22.0, &i);
//Schedule(18.0, &i);
//Schedule(12.0, &i);
//printf("before dequeuing process:\n");
//PrintList(0);
//PrintList(1);
//printf("============dequeue in process===============\n");
//int count = qsize;
//
//for(int i = 0; i < count; i++){
//	node *yo = Deque();
//	printf("time stamp %f has been dequeued\n", yo->timestamp);
		free(the_list);
}

//void main(){
//	int test_queue_size = 2000;
//	struct timeval stop, start;
//	node *e;
//	char data = "random";
//	init_queue();
//	gettimeofday(&start, NULL);
//	//do stuff
//	for (int i = 0; i < test_queue_size; i++){
//			Schedule(i*rand_time(0.1), data);
//		}
//		for(int i = 0; i < test_queue_size; i++){
//			e = Deque();
//			Schedule(e->timestamp+rand_time(0.6), data);
//		}
//	gettimeofday(&stop, NULL);
//	printf("took %lu\n", stop.tv_usec - start.tv_usec);
//
//
//}





void resize(int new_size){

	if(resizeability==0){
		printf("resize disabled\n");
	}
	else{
//		printf("===========================================================\n");
//		printf("================ resize function started ==================\n");
//		printf("===========================================================\n");
//		printf("===========================================================\n");
//		printf("resizing\n");
//		printf("new_size = %d, old size = %d\n", new_size, length);
	int resize_qsize = qsize;
	//printf("resize disabled!\n");
	resizeability = 0;
	//initialize and record variables//
	int old_length;
	node **old_bucket;
	width = new_width();
	// record old variables //
	old_length = length;
	old_bucket = bucket_list;
	//initialize new calendar//
//	if(the_list_length-new_size <0){
//		the_list_length = 2*the_list_length;
//		the_list =(node**)realloc(the_list_length*sizeof(node *));
//	}
	if(first_sub == 0){
		local_ini(the_list_length-new_size, new_size, width, last_prior);
	}else{
		local_ini(0, new_size, width, last_prior);
	}

	//transfer events to new list//************** Start here tomorrow ****************
	for(int i = 0; i < old_length; i++){
		if(old_bucket[i] != NULL){
			do{node *temp = old_bucket[i];
			old_bucket[i]=temp->Next;
			qsize = resize_qsize;
			Schedule(temp->timestamp, temp->AppData);
			}while(old_bucket[i] != NULL);
		}
	}
	//printf("*************************************************************LIST CHECK 123 **************************************************************\n");
			for(int i = 0; i < length; i++){
				PrintList(i);
			}
//	printf("=========================================================\n");
//	printf("================== resize function end ==================\n");
//	printf("=========================================================\n");
//	printf("=========================================================\n");
	resizeability = 1;
	qsize = resize_qsize;
	//printf("after resize the qsize is: %d\n", qsize);
	}

}

void init_queue(void){
	the_list =(node**) malloc(the_list_length*sizeof(node *));
	local_ini(0, 2, 1.0, 0.0);
	resizeability=1;
}

double new_width(void){
//	printf("=========================================================\n");
//	printf("================ new width function start =================\n");
//	printf("=========================================================\n");
//	printf("=========================================================\n");

	double average;
	int n_samples;
	double temp_last_prio;
	int temp_last_bucket;
	double temp_bucket_top;
	double temp_qsize;
	// deside sample size //
	resizeability=0; // disable resize //
	//printf("variable initialized\n");
	if (qsize<2){
		return(1.0);
	}else if(qsize <= 5){
		n_samples = qsize;
	}else{
		n_samples = 5 + qsize/10;
	}
	if (n_samples > 25){
			n_samples=25;
		}
	node **temp_storage;
	//printf("sample size determined\n");


	temp_last_prio = last_prior;
	temp_last_bucket = last_bucket;
	temp_bucket_top = bucket_top;
	temp_qsize = qsize;
	//printf("variable recorded\n");
	double sum=0;
	temp_storage = (node**)malloc(n_samples*sizeof(node *));
	//printf("number of samples = %d\n", n_samples);
	for(int i = 0; i < n_samples; i++){
	//	printf("list check for new_width\n");
		for(int i = 0; i < length; i++){
			PrintList(i);
		}
	//	printf("qsize = %d\n", qsize);
		node *temp=Deque();
	//	printf("check point !\n");
		temp_storage[i]=temp;
	//	printf("number of i = %d\n", i);
	}
//	printf("node stored\n");
//	printf("testing storage: %f\n", temp_storage[1]->timestamp);
	int dist_count = 0;
//	printf("storage checks out!\n");
	for(int i = 0; i < n_samples; i++){
		for(int j = i+1; j< n_samples; j++){
			//printf("entered nested for loop\n");
			//printf("i = %d, j = %d\n", i, j);
			double first =temp_storage[i]->timestamp;
			double second = temp_storage[j]->timestamp;
			double difference = first - second;
			//printf("difference = %f, i_store = %f, j_store = %f\n", difference, first, second);
			if (difference < 0){
				difference = -difference;
			}
			sum += difference;
			dist_count++;
			//printf("dis_count = %d, sum is now %f\n", dist_count, sum);
		}
	}
	//printf("reach first average calculation");
	average = sum/dist_count;
	//printf("first average = %f, first dist_count = %d, first sum = %f\n", average, dist_count, sum	);
	sum = 0;
	dist_count = 0;
	for(int i = 0; i < n_samples; i++){
			for(int j = i+1; j< n_samples; j++){
				double difference = temp_storage[i]->timestamp - temp_storage[j]->timestamp;
				if (difference < 0){
					difference = -difference;
				}
				if(difference < 2*average){
				sum += difference;
				dist_count++;
				}
			}
		}
	average = sum/dist_count;
	//printf("new width = %f, dist_count = %d, sum = %f\n", average, dist_count, sum);
	//printf("reached enqueue process\n");
	for (int i = 0; i < n_samples; i++){

		Schedule(temp_storage[i]->timestamp, temp_storage[i]->AppData);
	//	printf("enqueing process->%f\n", temp_storage[i]->timestamp);
	}
	//printf("exited enqueing process\n");
	//printf("restoring variables\n");
	last_prior = temp_last_prio;
	last_bucket = temp_last_bucket;
	bucket_top = temp_bucket_top;
	qsize = temp_qsize;
	//printf("*************************************************************LIST CHECK **************************************************************\n");
	for(int i = 0; i < length; i++){
		PrintList(i);
	}
	resizeability=1;
	//printf("-----------=============== resize re-enabled ===========---------------\n");
	free(temp_storage);
//	printf("=========================================================\n");
//	printf("================ new width function end =================\n");
//	printf("new width obtained: %f\n", average);
//	printf("=========================================================\n");
//	printf("=========================================================\n");
	return(average);
}

void local_ini(int qbase, int nbuck, double bwidth, double startprio){
	long int n;

	first_sub = qbase;
	int width = bwidth;
	int nbuckets = nbuck;
	//printf("before execution of suspicious code\n");
	bucket_list = &the_list[qbase];
	//printf("pointer %p %p\n", &bucket_list, &the_list);
	/* initialize array as empty */
	for(int i = 0; i < nbuck; i++){
		bucket_list[i]=NULL;
	}
	last_prior = startprio;
	n=startprio/width;
	last_bucket=n % nbuckets;
	bucket_top = (n+1)*width+0.5*width;
	bot_threshold = nbuckets/2 -2;
	top_threshold = 2 * nbuckets;
	length = nbuckets; // update length //
}



void Schedule(double ts, void *data){
	//printf("entered schedule function\n");
	//printf("the queue priority is %f\n", ts);
	node *e, *p, *q;
	if ((e = malloc (sizeof (struct Event))) == NULL) exit(1);
		e->timestamp = ts;
		e->AppData = data;
		e->Next = NULL;
	//printf("value assignment completed\n");
	//printf("priority assignment check ts = %f\n", e->timestamp);
	int i = ts/width; // find virtual bucket (bucket number from start to finish //
	//printf("virtual bucket is %d\n", i);
	i = i % length; // find actual bucket (regardless how many cycles it went through) //
	//printf("actual bucket is %d\n", i);
	//code to insert the event into the bucket list//
	//printf("bucket selection completed\n");
	//printf("bucket choice is %d\n", i);
	if (bucket_list[i] == NULL){ // if the bucket is empty, replace null to the node //
		//printf("first element in bucket detected!\n");
		bucket_list[i] = e;
		//printf("first bucket ts = %f\n", bucket_list[i]->timestamp);
	}else if(e->timestamp < bucket_list[i]->timestamp){
		node *temp;
		temp = bucket_list[i];
		bucket_list[i] = e;
		e->Next = temp;
	}
	else{ // insert the node at the right position otherwise //
	for (q=bucket_list[i], p=bucket_list[i]->Next; p!=NULL; p=p->Next, q=q->Next) {
		if (p->timestamp >= e->timestamp) break;
		}
	e->Next = q->Next;
	q->Next = e;
	}


	//printf("find the insertion point\n");
	//printf("post-insertion node value check %f\n", bucket_list[i]->timestamp);
	// insert after q (before p)



	qsize++;
	// print something to know the progress //
	printf("=================Event scheduled, queue size is now %d=================\n", qsize);
	//code to check the threshold, decide whether to resize the list//
	if (qsize > top_threshold){
		resize(2*length);
	}

//	printf("Schedule event list checke\n");
//			for(int i = 0; i < length; i++){
//				PrintList(i);
//			}
}

double CurrentTime(void){
	return (Now);
}





node* Deque(void){
	//code to delete the first event at the bucket //
	 if (qsize == 0){
		 return (NULL);
	 }
	 node *e;
	 int i;
	 for(i = last_bucket;;){  //search bucket// ** from Brown paper **
		 //printf("bucket top is %f, and last_bucket is %d\n", bucket_top, last_bucket);
		 if (bucket_list[i] != NULL && bucket_list[i]->timestamp < bucket_top){
			 //printf("***************************check point 2!!!!!!!***************************\n");
			//printf("entered removing process\n");
			e = bucket_list[i];		// remove first event in list
			bucket_list[i] = e->Next;
			last_prior = e->timestamp;
			--qsize; // update qsize //
			last_bucket = i;
			last_prior = e->timestamp;
//			printf("dequeueing.... dequeued prior is %f\n", e->timestamp);
//			printf("prior = %f\n", last_prior);
			/* Halve calendar size if needed */
			if(qsize < length/2){
				resize(length/2);
			}
			return(e);
		 }else{
			 ++i;
			 if(i == length){
				 i=0;
			 }
			 bucket_top += width;

			 if (i == last_bucket) break;
		 }
	 }
	 // direct search for minimum time stamp //
		float minimum = INFINITY;
		for (int i = 0; i < length; i++){
			if(bucket_list[i]==NULL){
				continue;
			}else if(bucket_list[i]->timestamp < minimum){
				last_bucket = i;
				}
			}
		bucket_top += width;
		return(Deque());

}
	//searching for the bucket smallest time stamp//

	// update qsize //
	//resize the bucket_list as needed//
//	printf("=================Event deleted, the queue size is now %d=================\n", qsize);
//	PrintList(last_bucket);
//	printf("=================Searching for the smallest time stamp in buckets ==========\n");

//	printf("The new bucket is %d\n", last_bucket);
//	printf("After Dequeue, the new list is the following:\n");
//	 return (e);


void PrintList (int i)
{
    struct Event *p;

    printf ("Event List: ");

    for (p=bucket_list[i]; p!=NULL; p=p->Next) {
        printf ("%f ", p->timestamp);
    }
    printf ("\n");
}

double random_number (void){
	return (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

double rand_time (double d){
	double time = -(d)*(log(1.0 - random_number()));
	return (time);
}

