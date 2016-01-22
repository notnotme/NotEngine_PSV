# NotEngine_PSV
A little framework to render graphics and audio (not yet audio) on PSVITA.

For now it is still in early stage of developpement, and the changes between each commit can
change a lot of things.

Install:
 - First install glm (http://glm.g-truc.net/) into your vitasdk toolchain
 - Install jsonxx for psvita too (https://github.com/notnotme/jsonxx)
 - Enter into the NotEngine project directory type "make -f Makefile.psp2 && make -f Makefile.psp2 install"
 - You're up to go

TODO:
 - Load textures from files using FrameCatalog infos (texture name, width and height, maybe more) see here: http://www.leshylabs.com/apps/sstool/
 - Add animation to sprite using FrameCatalog
 - Emulate basic immediate rendering (like opengl 1.2 days) with another Graphics class

Greetings:
 - PSP2SDK & VITASDK team
 - #vitadev - #demofr - #openpandora

