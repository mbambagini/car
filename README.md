[![Build Status](https://travis-ci.org/mbambagini/car.svg?branch=master)](https://travis-ci.org/mbambagini/car)

# car

This repository contains all the software of the remote controlled car.

The software components are the followings:
* chassis: implementing the engine and body controllers within the car
* chassis_test: sw that tests all chassis functions (by simulating the interaction with a remote controller), running on a different board and accessing the CAN bus
* gateway & pc: the user on the PC can send/receive info to a gateway that forwards the messages the chassis

Note that libraries are compiled separately in another project.

