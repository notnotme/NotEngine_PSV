# NotEngine_PSV
A little framework for the ps vita to render graphics and audio on PSVITA.

For now it is still in early stage of developpement, and the changes between each commit can
change a lot of things.

Install:
 - First install glm (http://glm.g-truc.net/) into your psp2sdk toolchain
 - Enter into the NotEngine project directory type "make -f Makefile.psp2 && make -f Makefile.psp2 install"
 - You're up to go

Uninstall:
 - Enter into the NotEngine project directory type "make -f Makefile.psp2 uninstall"


TODO:
 - Add rotation & scale to sprites (computed inside the shader?)
 - Make SpriteSheet & json loading (see here: http://www.leshylabs.com/apps/sstool/)
 - Add animation to sprite
 - Wrap playptmod & StSoundLibrary into Audio classes

This project embed some code that i've not wrote. It may be slightly modified.
 - StSoundLibrary (http://leonard.oxg.free.fr/)
 - playptmod (http://16-bits.org/)

Greetings:
 - StSoundLibrary & playptmod authors
 - PSP2DEV team


A Sample will be available inside the "vitamine",
folder but it's not yet ready for a release..
