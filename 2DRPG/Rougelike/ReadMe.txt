Roguelike

Solution file: Rougelike.sln


How To Play:
Player Action is broken into 2 stages
 - Movement
	+ Tiles player can move to have a green overlay on them
	+ Keyboard/Controller cursor is green
	+ Pressing rest will end turn (Don't move, Don't attack)
 - Attack
	+ Tiles player can attack have a red overlay on them
	+ Keyboard/Controller cursor is red
	+ Pressing rest will end turn (Don't attack)
Rest will end the player's turn

Controls(Keyboard/Controller):
 When Playing
   -Showstats screen      = Backspace, P, Tab / Back button
   -Open Main Menu 	  = Esc / Start button
   -Move Cursor direction = arrow, wasd / Dpad
   -Rest 		  = Spacebar / X button
   -Select this tile      = Enter / A button
   -Switch Camera View    = C / Right Thumb button
 When In Menu
   -Quit = Esc, 3
   -Resume = 1 / Back button
 When in Statscren
   -Resume = Backspace, P, Tab, Esc / Back button, Start button
 Mouse (Only works when playing)
   -Select this tile = Left Click
	+ Will select the tile that the cursor is over
        + Cursor is the blue square with diamonds or your actual computer cursor




Console: (Press ~ to activate)
help = loads the help menu listing all console commands

add_map RPTest = Creates a map using the map definition named RPTest and puts it into the world
load_map RPTest = Loads named RPTest
map RPTest = Creates a map using the map definition named RPTest and loads it as the current map
map_defs = Writes the name of all map definitions to the console
list_maps = Writes the name of all maps in the world

adventure Old = Destorys the world and creates a new one using an adventure definition named Old

play_music Data/Audio/TestSound.mp3 .5 = Play a sound using this file path at .5 volume (volume is option)

quit = Quits the game

[built-in engine console functions]
cls = Clears the console of messages
help = Loads the help menu listing all console commands
cmd_toggle_font = Which switch the consoles font between "Gamarmound32.fnt" and "SquirrelFixedFont.png"
cmd_txt_size 24 = Changes the font size of messages in the console to 24




Known issues:
I mispelled Roguelike when I made the project.
The game runs at a consistent 59 frames instead of 60, so I maybe rounding too much when locking my framerate.
The font glyphs aren't perfectly centered since they are a bitmap font.
