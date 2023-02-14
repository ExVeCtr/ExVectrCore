# ExVectrCore
Part of the ExVeCtr project. 
ExVectrCore adds a suite of utilities for program structure, control and data flow.
## **This project is under initial development. Things will probably break.**
## ToDo:
- [X] Port, modify and clean/fix utilities from EasyVTOL.
- [X] Add support for multiple time sources (One is internal/Precise) ~~!! High priority from Scheduler !!~~
- [-] ~~Add some string or character system. (Needed for debugging and message utilities)~~ Would be too inefficient and not really needed.
- [X] Add message and debug utilities.
- [-] ~~Add event utilities.~~ Topics makes this pretty much redundant.              
- [ ] Redo scheduling for use of new tasks and time sources utilities.    !! Currently broken. Upmost priority !!
- [ ] Documentation.
## To be implemented into a platform library:
A platform library is required for the ExVectrCore to run. The following functions must be implemented inside a .cpp file:
- `getPlatformClock();` Found inside the `time_base.hpp` header file