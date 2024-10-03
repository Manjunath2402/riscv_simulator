# RISC V ASSEMBLY CODE SIMULATOR

### FILE SYSTEM
The source files of the program are all kept in the sourceFiles directory. The test cases which the program is
through checked on are kept in testFiles directory. The sources files contain 3 major files main.cpp, alu_operations.cpp
and sim_operations.cpp. The alu_operations.cpp file contains the operations performed by a ALU on a RISC V machine.
It contains comparators, add, subraction and shifts functions. All these functions takes hexadecimal strings of length
16 bits as parameters and returns the result as a hexadecimal string of length 16 bits. This file exactly resembles 
an ALU on a basic RISC V processor.
The files main.cpp and sim_operations.cpp are files used for simulation. The main.cpp contains the main function 
which is used for taking commands from user and executing them. The sim_operations.cpp is the file which contains 
all the required function to perform the action requested by the user. The step function is called from the main function
whenever run or step is called, This step function internally calls the execute instruction function which decodes a 
instruction and calls function specific to the type of instruction supposed to be executed. The functions which are
are used to execute specific instructions use the ALU operation functions defined in alu_operations.cpp file.
These all functions are called in a way that is just like how the risc v hardware will process.

### PURPOSE OF THE PROJECT
To simulate a risc v assembly file to know how the instruction execution happen at hardware level.

### HOW TO RUN THE PROGRAM
The makefile in the directory can be used to get the final executable riscv_sim file. Use ***make all*** to get the riscv_sim file.
Later use the ***make clean*** to remove the object files created while compiling the riscv_siExecutedm file.
Run this riscv_sim file to use the simulator.

### INPUT FILE FORMAT
After running the riscv_sim file use ***load <pathTo/assembly/file/>*** command to load a risc v assembly file that is to be executed.
The path to be assembly file should be relative to the riscv_sim file(i.e., when you are using ".", "..") or give the exact path of the file or directly give the file name if it is in the same directory as the riscv_sim executable. 
The assembly file can also contain .data segment which supports .dword, .word, .half and .byte formats the value given can be of 
decimal or hexadecimal format. A single line can only contain size of single format, The values can be separated by commas and 
extra won't make any problem.
The command supported by the simulator is ***load*** to load a assembly file, ***mem*** to check memory at given location, 
***break*** to set break points, ***del break*** to delete break points, ***show-stack*** to see function calls. ***step***
to step over a instruction and ***run*** to run the execution until a set break point or end of the file.
