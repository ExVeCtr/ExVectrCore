# ExVectrCore
Part of the ExVeCtr project. 
ExVectrCore adds a suite of utilities for program structure, control and data flow.
## **This project is under initial development. Things will probably break.**
## Current goal roadmap:
- [X] Port, modify and clean/fix utilities from EasyVTOL.
- [ ] Add event utilities.
- [ ] Add timing utilities
- [ ] Redo scheduling for use of event and timing utilities.
- [ ] Documentation.
## To be implemented into a platform library:##
A platform library is required for the ExVectrCore to run. The following functions must be implemented inside a .cpp file:
- `int64_t timeBase();` Found inside the `time_base.hpp` header file