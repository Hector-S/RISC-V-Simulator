# RISC-V Simulator
**A RISC-V Simulator for ECE 486/586**

**---| How to Compile (Linux) |---**

In the Linux directory, input the following into the terminal if you don't have SDL2 dev libraries/files installed.

apt-cache search libsdl2            //See latest versions of sdl2

apt-get install libsdl2-dev

apt-get install libsdl2-ttf-dev

apt-get install libsdl2-image-dev

Then you can use the makefile to compile the simulator.

**---| How to Compile (Windows) |---**

In the Windows directory there is a Code::Blocks project file.  
Open the file in Code::Blocks and hit the compile button. It should be that simple.

**---| Command Line Arguments |---**

**"-d"** = Debug mode. Extra debug info will show.

**"-h"** = Hex mode. Registers will display in hex format instead of decimal.

**"-tf filename"** = Trace file. All simulation terminal output will be redirected to a file with the name 'filename'.

**"-mtf"** = Memory trace file. Virtual memory will be written to file/s with the name 'MemoryData_PX-X.txt'.  
^ Name/s of file/s are dependent on the number of threads used.

**"-pi"** = Protect instructions. Instructions will be written to and read from memory that is separate from the memory they write to or read from.  
^ If enabled, instructions can't be overwritten. If disabled, instructions can overwrite themselves in memory.

**"-pc #"** = Set PC register. The PC register will be set to the hex number '#'.

**"-sp #"** = Set SP register.

**"-sfx .suf"** = Set suffix. Will only include files that end with '.suf' in the file select menu.  
^ Note: If you can't see suffixes in your normal file explorer, this may not show any files.

**"-f filename"** = Set input file name. The file 'filename' will be selected for simulation.

**"-s"** = Simulate. The simulator will completely run without opening the terminal menu.  
^ Useful to create several trace files of simulations (if using trace file command) with a script.

**---| The GUI (Explanation of View) |---**

After selecting a file and applying whatever settings you desire, you can launch the GUI from the terminal.  
The first column is a view of the addresses where instructions are stored.  
The second column is a view of the instructions at those addresses.  
The third column is a view of those instructions being decoded into RISC-V instructions.  
The 4th and 5th columns show the data in the registers. Through the settings you can change them to be in hex or decimal format.  
The 6th column shows a view of memory. This can be toggled to show the data at memory or the address of the memory.

If a row is light green, that means it was the last instruction executed if applicable.  
If a row is dark green, that means it will be the next instruction executed if applicable.

At the bottom left in grey and orange is the interactable portion of the GUI.  
^ If it's grey, it can't be directly interacted with. If it's orange, it's a button.

The first grey box in the address column shows the current offset of your view to the PC.  
^ The two small orange arrow buttons to the right will increment and decrement this offset.  
^ If your view is NOT locked and this offset is 0. Then the top row will be dark green.  
^ If your view is LOCKED, the offset will be equal to the address of the top row.

The second grey box in the address column is the address stored in the PC register.  
^ The next two small orange arrow buttons increment and decrement the PC register (move the simulation forward and backward in time).
^ The grey box between those orange buttons shows the amount of past states you can revert to (how many steps back in time you can take).

The orange "Lock" button will lock and unlock the view from the PC register.  
^ E.g. if not locked, your view will follow the PC register as time moves.  
^ If locked, your view will not follow the PC register as time moves.

The orange box to the right of the "Lock" button shows the current memory offset.  
^ The first row of the memory column will have the same address as this offset.  
^ You can click this box and manually type in the address of memory you want to view.
^ The two orange arrows directly below it can increment and decrement this offset.
^ The orange "DATA" button will toggle whether you'll see the data in memory or the addresses of memory in your current view.

The orange "Find" button will unlock your instruction/address view and set your view so that the address of the PC register is in the middle of your view.

**---| The GUI (Shortcuts) |---**

The left/right arrow keys will move the simulation forward and backward in time.  
The '+' and '-' keys will increment/decrement the instruction/address view offset.  
The '<' and '>' keys will increment/decrement the memory view offset.
HOLDING the tab key will toggle the view of memory while it's held.


**---| Credit |---**

Created By: Hector Soto

**---| Licensing/Software Used |---**

The GUI for this simulator was made using SDL2. For more licensing info about SDL2 or additional libraries used, you can look at the LICENSE files in the main directory.

The Inconsolata.ttf font was used for the GUI. Which is under the open font license: https://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL
