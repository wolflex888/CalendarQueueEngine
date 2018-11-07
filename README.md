# CalendarQueueEngine
This is a homework done in Georgia Intitute of Technology CSE 6010
HW3 - Call center simulation


The simulation application is named: HW3_app.c
The engine is named: engine.c
There is an additional file called Queue.c that contains the FIFO queue for incoming callers
The header file associated with that is FIFO_TEST.h
The header file for the simulation is called sim.h and was derived from the sample sim.h
file that Dr. Fujimoto gave us as a sample. 

The programs need to be compiled together:gcc HW3_App.c engine.c

## Function
This program simulates a call center and try to queue the customer to each representative. The application is separated into two parts. The simulation part simulates the incoming calls and outgoing calls and the queueing engine schedules the queues using calandar queue described in Brown paper.
