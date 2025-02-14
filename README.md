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
A nice tutorial to link, and add to the compiler SFML:
https://www.youtube.com/watch?v=rZE700aaT5I&t=429s
Don't forget to change your binaries in the exec folder if necessary !

## Run the protocol
Clone the repository, install SFML and you're ready to run the protocol.
However, you must select the mode, the topology and decide if the simulator will be connected with the visualizer or not.

For that, you simply needs to reach the common.hpp file and change the number of the COMMUNICATION_PERIOD and TOPOLOGY variables.
![image](https://github.com/user-attachments/assets/7284d9a0-f7c2-432e-a5fe-42ec557e1289)


Don't forget to launch the visualiser before running the simulator if you decide to connect them. In the case, you don't, set the visualiserConnected variable to false. (in common.hpp)


## Improvements

This is one of my largest project ever made, and the occasion to refresh my c++. Even though I emphasized clarity and maintanability, some newbies mistakes are likely to be present.
You're welcome to fork this report and improve it, make pull request.




