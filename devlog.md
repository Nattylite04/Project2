# CS4348 Project 2 - Bank Simulation Devlog

## Session 1 - November 3rd, 2026 - 2:26PM
Goal: Establish project environment and understand how the teller and customer threads will interact.

Plans for this session: Initialize git repository, create all required project files, implement basic interaction between teller and customer. Try to understand how semaphores allow threads to wait for each other before moving on to multiple tellers.

## End of session 1 - November 3rd, 2026 - 3:58PM
Thoughts so far: I created initial C program using pthread to launch a teller and customer thread. I then used semaphores to ensure customer waited until teller was ready, then introduced themselves. Teller waited for customer before acknowledging the intro. I then expanded to create 3 teller threads, each with unique ID. I added synchronization around the shared teller assignment data so selecting a teller could occur safely. I tested that implementation with a small number of customers before increaseing thread count.

Plans for next session: Implement deposit and withdrawal behavior and introduce the shared safe and manager resources.

## Session 2 - October 4th, 2026 - 1:38PM

Goal: Begin implementation of the actual customer behavior described in project. I want to develop the customer functionality gradually so I can understand how customer interact with the tellers and how the synchronization will prevent conflicts. 

Plans for this session: Create customer structure to store each customer's unique ID, transaction type, and assigned teller. 
Modify existing customer thread to use this struct instead of obtaining its ID from teller. 
Implement random selection between deposit and withdrawal transaction. Add required random arrival delay. Begin designing how customers will enter bank and wait for available teller.

## End of session 2 - November 3rd - 2026
