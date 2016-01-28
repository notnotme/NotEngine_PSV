# NotEngine_PSV
A little framework to render graphics and audio (not yet audio) on PSVITA.

For now it is still in early stage of developpement, and the changes between each commit can
change a lot of things.

Install:
 - First install glm (http://glm.g-truc.net/) into your vitasdk toolchain
 - Install jsonxx for psvita too (https://github.com/notnotme/jsonxx)
 - Enter into the NotEngine project directory type "make -f Makefile.psp2 && make -f Makefile.psp2 install"
 - You're up to go

Todo:
 - Emulate basic immediate rendering (like opengl 1.2 days) with another Graphics class

Note:
 - It's your task to load images and update texture datas into the created Textures
 - Spritesheets can be loaded using FrameCatalog, see http://www.leshylabs.com/apps/sstool/ and export as TP-JSON-Hash
 - You can animate sprite by swapping it's FrameCatalog::Frame
 - To generate font textures, you can use LMNOPC Font Builder (http://www.lmnopc.com/bitmapfontbuilder/) and choose to export the full ascii set.

Greetings:
 - PSP2SDK & VITASDK team
 - #vitadev - #demofr - #openpandora

