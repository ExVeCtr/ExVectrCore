# ExVectrCore
Part of the ExVeCtr project. 
ExVectrCore adds a suite of utilities for program structure, control and data flow.
## **This project is under initial development. Things will probably break.**
## ToDo:
- [X] Port, modify and clean/fix utilities from EasyVTOL.
- [ ] Add message and debug utilities.
- [ ] Add event utilities.
- [ ] Add timing utilities
- [ ] Redo scheduling for use of event and timing utilities. !! Currently broken. Upmost priority !!
- [ ] Documentation.
## To be implemented into a platform library:
A platform library is required for the ExVectrCore to run. The following functions must be implemented inside a .cpp file:
- `int64_t timeBase();` Found inside the `time_base.hpp` header file