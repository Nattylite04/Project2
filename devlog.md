# CS4348 Project 2 - Bank Simulation Devlog

## Session 1 - October 8th, 2026 - 2:26PM
Goal: Establish project environment and understand how the teller and customer threads will interact.

Plans for this session: Initialize git repository, create all required project files, implement basic interaction between teller and customer. Try to understand how semaphores allow threads to wait for each other before moving on to multiple tellers.

## End of session 1 - October 9th, 2026
Thoughts so far: I created initial C program using pthread to launch a teller and customer thread. I then used semaphores to ensure customer waited until teller was ready, then introduced themselves. Teller waited for customer before acknowledging the intro. I then expanded to create 3 teller threads, each with unique ID. I added synchronization around the shared teller assignment data so selecting a teller could occur safely. I tested that implementation with a small number of customers before increaseing thread count.

Plans for next session: Implement deposit and withdrawal behavior and introduce the shared safe and manager resources.

