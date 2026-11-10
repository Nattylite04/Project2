# CS4348 Project 2 - Bank Simulation Devlog

## Session 1 - November 3rd, 2026 - 2:26PM
Goal for this session: Establish project environment and understand how the teller and customer threads will interact.

Plans: Initialize git repository, create all required project files, implement basic interaction between teller and customer. Try to understand how semaphores allow threads to wait for each other before moving on to multiple tellers.

## End of session 1 - November 3rd, 2026 - 3:58PM
Thoughts so far: I created initial C program using pthread to launch a teller and customer thread. I then used semaphores to ensure customer waited until teller was ready, then introduced themselves. Teller waited for customer before acknowledging the intro. I then expanded to create 3 teller threads, each with unique ID. I added synchronization around the shared teller assignment data so selecting a teller could occur safely. I tested that implementation with a small number of customers before increaseing thread count.

Plans for next session: Implement deposit and withdrawal behavior and introduce the shared safe and manager resources.

## Session 2 - November 4th, 2026 - 1:38PM

Goal for this session: Begin implementation of the actual customer behavior described in project. I want to develop the customer functionality gradually so I can understand how customer interact with the tellers and how the synchronization will prevent conflicts. 

Plan: Create customer structure to store each customer's unique ID, transaction type, and assigned teller. 
Modify existing customer thread to use this struct instead of obtaining its ID from teller. 
Implement random selection between deposit and withdrawal transaction. Add required random arrival delay. Begin designing how customers will enter bank and wait for available teller.

## End of session 2 - November 4th, 2026 - 5:58PM

Thoughts so far: I implemented the Customer structure to store independent customer IDs, transaction information, and a reference to an assigned teller. I modified the customer thread dot use its own Customer struct instead of receiving ID from Teller struct. Output shows correct teller announcements, customer to teller, and transaction request in correct orders. 

Plans for next session: Implement random transaction selection while keeping existing teller/customer synchronization intact.

## Session 3 - November 5th, 2026 - 10:33AM

Goal for this session: Improve the program's output so it follows format required by project specifications.

Plan: I plan to create a reusable function for printing teller and customer actions instead of manually writing separate printf() statements throughout the program. This will make output easier to maintain and keep everything consistent. I will test the program with a couple of tellers and customers to allow easier testing and validation.

## End of session 3 - November 5rd, 2026 - 5:38PM

Thoughts so far: I focused on improving output format. I also added a mutex to prvent multiple threads from printing at the same time, which helps keep output consistent. Using a separate logging function also makes code easier to organize.

Plans for next session: I plan to improve the deposit and withdrawal messages to make it easier to follow each transaction and without interfering with existing synchronization. I will still test with a few tellers and a few customers to make easier for testing. 

## Session 4 - November 8th, 2026 - 2:17PM

Goal for this session: Expand the logging system to provide more detailed information about customer arrival delays and transaction requests.

Plan: I will add function for generating random delays, implement customer arrival delay, print messages before and after delay, and improve deposit and withdrawal messages. Testing.

## End of Session 4 - November 8th, 2026 - 4:36PM

Thoughts so far: I focused on expanding customer output by adding random transaction selection and arrival delay. Output provides more info about each customer's actions, including selected transaction, wait time, and finishing waiting. I learned how random delays affect execution order of multiple threads.

Plans for next session: I plan to focus on implementing output for shared resources including manager, safe, and bank door. Hopefully I can finish main output requirements before moving on to implementing full customer queue.

# Session 5 - November 9th, 2026 - 2:10PM
Goal for this session: Program does not yet simulate access to bank shared resources. Add required manager, safe, and door functionality while continuing to improve output format.

Plan: Initialize semaphores to control access to manager, safe and door. Implement manager interaction for withdrawal transaction with delay, Implement safe access for both deposits and withdrawals including delay, add all output messages, implement door synch for customers. Test.

# End of session 5 - November 9th, 2026
Thoughts so far: Program is becoming more complete, but it still uses 3 temp customers with fixed tellers. The next major challenge will be allowing multiple customers to share 3 tellers without causing synchronization problems.

Plan for next session: Implement customer waiting line and teller assignment system. Hopefully increase number of customer threads for testing.
