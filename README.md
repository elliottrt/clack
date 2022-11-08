# clack
A simple expression solver in C++, with support for custom variables and functions.

### Usage

Include "include/clack.h"

Create a Clack::Solver and either send it commands via runCommand(str) 
or using the other functions declared in clack.h

### Commands

clear - clears the screen, only works if ansi escape codes are supported

def - defines a function, in the format 'def <name>(<args>) <body expression>'
      functions with the same <name> must have different argument counts
	
deflist - lists all defined functions along with their body or 'system function' 
		  for functions defined in the code
	
file - opens a file and runs commands found in that file
	
fsilent - toggles file silent mode. while in the mode, files will only 
		  display the result of a command

help - lists commands
	
reset - resets the system, clearing all non system variables and functions
	
save - dumps the user's variables and functions to a file
	
store - stores the last result into a variable
	
var - defines a variable with the form 'var <name> <value expression>'
	
varlist - lists all variables and a value or 'system variable'
	
### Other
	
Function pointers may be passed as an argument to a function with the prefix '&'.
