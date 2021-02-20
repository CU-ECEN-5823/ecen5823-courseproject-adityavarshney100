Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements in the "Default" configuration of the profiler to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See [Shared document](https://docs.google.com/document/d/1Ro9G2Nsr_ZXDhBYJ6YyF9CPivb--6UjhHRmVhDGySag/edit?usp=sharing) for instructions.* 

1. What is the average current per period?
   Answer: 103.0uA
   <br>Screenshot:  
   ![Avg_current_per_period](screenshots/assignment4/avg_current_per_period.jpg)  

2. What is the average current when the Si7021 is Powered Off?
   Answer: 87.71uA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](screenshots/assignment4/avg_current_lpm_off.jpg)  

3. What is the average current when the Si7021 is Powered On?
   Answer:4.01mA
   <br>Screenshot:  
   ![Avg_current_LPM_Off](screenshots/assignment4/avg_current_lpm_on.jpg)  

4. How long is the Si7021 Powered On for 1 temperature reading?
   Answer:13ms
   <br>Screenshot:  
   ![duration_lpm_on](screenshots/assignment4/avg_current_lpm_on.jpg)  

5. Compute what the total operating time of your design for assignment 4 would be in hours, assuming a 1000mAh battery power supply?
   Answer: (1000 *1000)/103 = 9,708Hours
   
6. How has the power consumption performance of your design changed since the previous assignment?
   Answer: I see a substantial decrease in the power consumption when we compare our current readings from the previous assigment. My avg current in the previous assignment was about 240uA. 
	So the decrease in the current concumption is by a factor of 2.4 times
   
7. Describe how you have tested your code to ensure you are sleeping in EM1 mode during I2C transfers.
   Answer: When we are calling sleep function in the main to change the Energy modes of the board, I return its value and check if the board has actually been in that mode or not.


P.S. For this assignment as well I am assuming my current readings would be beyond the acceptable limts since there is a fault in my board.
Thanks
   

