# ArcLoRaM Simulator


ArcLoRaM simulator is part of a larger project available at  https://github.com/Rorschak84/ArcLoRaM-Hub , please consult the linked report for a total explanation of the protocol whereabouts and implementations.

It features a real time event simulator, a simple but robut physical layer virtualization, an event scheduler and many more.
3 modes out of the 6 are implemented in the current state that can be run with two different topologies (simple line and mesh).

## Installation

The simplest way to run the protocol is by using docker. 
(on its way...)

However, if you wish to implement your own logic, improve the protocol, optimize the simulator engine, it is better to use it with an IDE as it enables live debugging, performance check..
I personnaly used Visual Studio Code with the latest C++ extention.
The engine is able to connect to the visualiser with local host using the port 5000 through tcp packets.
Consequently, SFML base and network module are required. If you wish to run the visualizer too, you will need the other  modules too. 




## Run the protocol



