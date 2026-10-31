# dev

Visual Studio C++ console application. Open `MazePathfinder.sln` in Visual Studio and build the x64 configuration.

```
dev/
├── MazePathfinder.sln
└── MazePathfinder/
    ├── include/   header files (.h): class declarations
    ├── src/       source files (.cpp): class implementations and main()
    └── MazePathfinder.vcxproj (+ .filters)
```

The project adds `include/` to the compiler's include path, so source files can use `#include "Maze.h"` without relative paths.
