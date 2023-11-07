# Simulate-2
Simulate 2 project

This project was to make a smart trashbin. 
All over the city, there are public bins. Some are used frequently, some are not. 
So this means not all of the bins have to be emtied every week. 

In this project, we used a Ultra Sonic sensor to measure how full the bin is. 
There is also a magnetic sensor attached to see if the bin is open or closed. 
When the magnet sensor is not touching, means the bin is open. 
Then the esp32 wakes up from deepsleep, waits until the bin is closed again, 
waits 5 seconds and then measures how full the bin is and sends it to the database. 
I also added a counter to the project, that counts when the bin has opend. 
So you can see how frequent the bin is used.
This is all programmed in C++ in PIO in Visual Studio Code

I used a database to collect all the data.

I also used Grafana to visualise how full the bin was, also added a treshhold.
With a gauge, it was the best way to visualise this.

As an extra, we added that you get a message from a bot on Telegram when it went over the treshhold or you could ask for an update. 
This was programmed in Python
