![SGADE](//github.com/JaapSuter/SGADE/raw/master/documentation/sgade.gif)
![SGADE](//github.com/JaapSuter/SGADE/raw/master/documentation/animated.gif)

Hi, 

thank you for downloading and trying the Socrates Gameboy Advance Development Engine.

First I'll tell you what the directory structure of the SGADE looks like
and then I'll guide you to some quick steps to get something up and running on
an emulator or a GBA.

The directory structure on your computer should look something 
like this (seen from wherever you put the SGADE itself):

Directory                                       | Contains
------------------------------------------------|------------------------------------------
/SGADE/                                         | Base directory for the SGADE
                                                |
/SGADE/build/                                   | The Socrates GNU-Make file 
/SGADE/build/Visual Studio/                     | An MSVC project using the GNU-Make file
/SGADE/build/intermediate/                      | Intermediate thumb-interwork object files	
                                                |
/SGADE/documentation/                           | Doxygen documentation configuration files
/SGADE/documentation/html/                      | The Socrates library documentation (index.html)
                                                |
/SGADE/include/                                 | Include directory for Socrates headers (Socrates.h)
                                                |
/SGADE/lib/                                     | Thumb-interwork Socrates library (libSocrates.a)
                                                |
/SGADE/source/                                  | Source files (*.c and *.s) for Socrates 
                                                |
/SGADE/misc/Visual Studio/                      | Visual Studio Workspace
                                                |
/SGADE/sample/                                  | Base directory for the SGADE sample application
/SGADE/sample/build/                            | GNU-Make file for the sample 
/SGADE/sample/build/intermediate/               | Intermediate object files for the sample 
/SGADE/sample/build/Visual Studio/              | Visual Studio project for the sample, using the GNU-Make file
/SGADE/sample/include/                          | Header files for the sample 
/SGADE/sample/source/                           | Source files for the sample 
/SGADE/sample/data/                             | SGADE data files for the sample
/SGADE/sample/data/Original Data/               | Original data before conversion
                                                |
/SGADE/tools/                                   | SGADE tools directory
/SGADE/tools/source/                            | Common source used by more than one tool
/SGADE/tools/include/                           | Common headers used by more than one tool
/SGADE/tools/applications/                      | Actual tools themselves
/SGADE/tools/applications/SoConverter/          | Generic SGADE converter
/SGADE/tools/applications/SoConvert/            | Generic SGADE converter v2.0 (preview)

Ok, now do I do to get this working? 

Well, for the experienced users, just link in the correct version of the Socrates 
library (libSocrates.a, or -lSocrates as the GCC compiler flag) and include "Socrates.h" in 
your application. That's all you need.

For the less experienced users, try out the sample application first. I assume you have already 
installed DevKitAdvance, or another GCC version that targets the Gameboy Advance. Now make
sure you have a path to GNU-Make so you can call it everywhere. Then go to /SGADE/sample/build/
and open the Makefile with a text editor. Change the PROJECT_DIR, GCC_VERSION, and GCC_DIR variables
to the settings they have on your computer. Save and close the makefile, and then type "make"
on a commandline. Compilation should commence, and you should have a "Sample.gba" in the 
/SGADE/sample/ directory once it is finished.

For those who have Visual Studio, you can open the "sample.dsp" project located in the 
/SGADE/sample/build/Visual Studio/ directory, and make your changes to the makefile from there.
Pressing F7 (build) should then start compiling (this simply calls make). Still make sure you 
have a path to the GNU-Make tool though.

Those who want to edit Socrates themselves, can either use the GNU-Make file in 
the /SGADE/build/ directory, or use the Visual Studio project in /SGADE/build/Visual Studio/

Thanks and goodluck. You'll know where to find me with any questions.

Regards,

Jaap Suter

![SGADE](//github.com/JaapSuter/SGADE/raw/master/documentation/screens.jpg)

// --------------------------------------------------------------------------------------
// EOF;
// --------------------------------------------------------------------------------------
