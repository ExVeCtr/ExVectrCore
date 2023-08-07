# ExVectrCore
Part of the ExVeCtr project. 
ExVectrCore is a suite of utilities for program structure, control and data flow.
# The ExVeCtr project
## What is it?
ExVeCtr is a personal project to unify my previous work and simplify working on experimental projects. These can range from simple temperature controllers or timers all the way up to control of a model aircrafts or networked irrigation systems.
ExVectrCore is the main library and implements core functionality for data flow and program structure. It supplies an application with time, task scheduling, topics and subscribers and many more things. ExVectrCore expects a few simple things to be provided by each platform type (Windows, arduino etc.) these are implemented automatically by ExVectr...Platform (e.g ExVectrArduinoPlatform) libraries. They can also implement a few more things like HAL interfaces for SPI, I2C and other platform/hardware specific things.
There are also more ExVectr libraries for further functionality like networking, control theory etc. All these libraries build off of ExVectrCore and ExVectrHAL allowing for simplicity.

tldr. ExVeCtr helps speed up and simplify experimental project development by supplying structure and implementation.
## Why ExVeCtr and not [Your preference here] (E.g Betaflight, ROS etc.)
As mentioned above, ExVeCtr can be used for pretty much anything. ExVeCtr places emphasis on keeping things unified, this means using interface classes for many things. This allows previous functionality to be used reducing implementation time or easily changing out one thing for another, but ExVeCtr remains small, simple and flexable, allowing it to run on pretty much anything.
## When should ExVeCtr not be used?
It should not be used on critical long life applications due to the experimental nature and some trade-offs like the used of dynamic arrays that can cause heap fragmentation. Do not run on an Arduino Uno or similarily constrained controllers.

## **This project is under initial development. Things will probably break.**
## ToDo:
- [X] Port, modify and clean/fix utilities from EasyVTOL.
- [X] Add support for multiple time sources ~~(One is internal/Precise) !! High priority from Scheduler !!~~
- [X] ~~Add some string or character system. (Needed for debugging and message utilities)~~ Would be too inefficient and not really needed.
- [X] Add message and debug utilities.
- [X] ~~Add event utilities.~~ Topics makes this pretty much redundant.              
- [X] Redo scheduling for use of new tasks and time sources utilities.    ~~!! Currently broken. Upmost priority !!~~
- [X] Create math library
- [X] Create data processing and structure library.
- [ ] Create a buffer data handling class for storing raw data into pointers.
- [ ] Create a global message topic system.
- [ ] Create memory library (Storing memory onto EEPROM or Other stuff)
- [X] Create sensor library.
- [X] Create networking library.
- [ ] Finally implement starship test vehicle. (Probably create a vehicle library)
- [ ] Simulation library. 
- [ ] Create irrigation library. 
- [ ] 3D graphics library for simulations?
- [ ] Documentation.
## Issues:
- [ ] High priority! Redo subscriber, topic system as it has failure modes.
## To be implemented into a platform library:
A platform library is required for the ExVectrCore to run. The following functions must be implemented inside a .cpp file:
- `getPlatformClock();` Found inside the `time_base.hpp` header file. This returns a clock giving the time since startup.
- `void initialise();` Found inside the main `ExVectrCore.hpp` header file. This is used to initialise the internals of the platform implementation.
Yes thats all that currently need to be implemented. Literally max. 20 minutes to research and implement.
For systems that must use HAL features, these must also be implemented, but are optional depending on what is available on that platform.