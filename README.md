# ExVectrCore
Part of the ExVeCtr project. 
ExVectrCore adds a suite of utilities for program structure, control and data flow.
# The ExVeCtr project
## What is it?
ExVeCtr is a personal project to unify my previous work and simplify working on experimental projects. These can range from simple temperature controllers or timers all the way up to control of a model aircrafts or networked irrigation systems.
ExVectrCore is the main library and implements core functionality for data flow and program structure. It supplies an application with time, task scheduling, topics and subcribers and many more things. ExVectrCore expects a few simple things to be provided by each platform type (Windows, arduino etc.) these are implemented automatically by ExVectr...Platform (e.g ExVectrArduinoPlatform) libraries. They also can implement a few more things like HAL interfaces for SPI, I2C etc. busses.
We can simply add more ExVectr libraries for further functionality like networking, control theory etc. All these libraries are built similarily, hence the unification.

tldr. ExVeCtr helps speed up and simplify experimental project development by supplying structure and implementation.
## Why ExVeCtr and not [Your preference here] (E.g Betaflight, etc.)
As above mentioned, ExVeCtr can be used for pretty much anything. ExVeCtr places emphasis on keeping things unified, this means using interface classes for many things. This allows previous functionality to be used reducing implementation time or easily changing out one thing for another.
For example: if I have an interesting idea for a drone propeller orientation, all I need to do is implement the system dynamics using an interface class. With that the drone is ready to fly as all the other functionality (sensor fusion, control etc.) is expecting simply the same interface. This also allows the programmer to fully and easily customise the system however they wish.
## When should ExVeCtr not be used?
Due to the design constraints to create a unified structure, we pay the cost of performance. ExVeCtr could run on an Arduino Uno board but would probably be much faster without. 
Also it should not be used on critical long life applications due to the experimental nature and some trade-offs like the used of dynamic arrays that can cause heap fragmentation. 

## **This project is under initial development. Things will probably break.**
## ToDo:
- [X] Port, modify and clean/fix utilities from EasyVTOL.
- [X] Add support for multiple time sources ~~(One is internal/Precise) !! High priority from Scheduler !!~~
- [X] ~~Add some string or character system. (Needed for debugging and message utilities)~~ Would be too inefficient and not really needed.
- [X] Add message and debug utilities.
- [X] ~~Add event utilities.~~ Topics makes this pretty much redundant.              
- [X] Redo scheduling for use of new tasks and time sources utilities.    ~~!! Currently broken. Upmost priority !!~~
- [ ] Create math library
- [ ] Create data processing and structure library.
- [ ] Create memory libary (Storing memory onto EEPROM or Other stuff)
- [ ] Create sensor library.
- [ ] Create networking library.
- [ ] Finally implement starship test vehicle. (Probably create a vehicle library)
- [ ] Simulation library. 
- [ ] Create irrigation library. 
- [ ] 3D graphics library for simulations?
- [ ] Documentation.
## To be implemented into a platform library:
A platform library is required for the ExVectrCore to run. The following functions must be implemented inside a .cpp file:
- `getPlatformClock();` Found inside the `time_base.hpp` header file.
- `void initialise();` Found inside the main `ExVectrCore.hpp` header file. This is used to initialise the internals of the platform implementation.
Yes thats all that currently need to be implemented. Literally max. 20 minutes to research and implement.