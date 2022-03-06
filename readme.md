# RISC-V Simulator
**A RISC-V Simulator for ECE 486/586**

**---| How to Compile (Linux) |---**

In the Linux directory, input the following into the terminal if you don't have SDL2 dev libraries/files installed.

apt-cache search libsdl2            //See latest versions of sdl2

apt-get install libsdl2-dev

apt-get install libsdl2-ttf-dev

apt-get install libsdl2-image-dev

**---| How to Compile (Windows) |---**

In the Windows directory there is a Code::Blocks project file.
Open the file in Code::Blocks and hit the compile button. It should be that simple.

**---| Command Line Arguments |---**

**"-d"** = Debug mode. Extra debug info will show.

"-tf filename" = Trace file. All simulation terminal output will be redirected to a file with the name 'filename'.

"-mtf" = Memory trace file. Virtual memory will be written to file/s with the name 'MemoryData_PX-X.txt'. Name/s of file/s are dependent on the number of threads used.

"-pi" = Protect instructions. Instructions will be written to and read from memory that is separate from the memory they write to or read from.

^ If enabled, instructions can't be overwritten. If disabled, instructions can overwrite themselves in memory.

"-pc #" = Set PC register. The PC register will be set to the hex number '#'.

"-sp #" = Set SP register.

"-sfx .suf" = Set suffix. Will only include files that end with '.suf' in the file select menu.

^ Note: If you can't see suffixes in your normal file explorer, this may not show any files.

"-f filename" = Set input file name. The file 'filename' will be selected for simulation.

"-s" = Simulate. The simulator will completely run without opening the terminal menu.

^ Useful to create several trace files of simulations (if using trace file command) with a script.

**---| Credit |---**

Created By: Hector Soto

**---| Licensing/Software Used |---**

The GUI for this simulator was made using SDL2. For more licensing info about SDL2 or additional libraries used, you can look at the LICENSE files in the main directory.
