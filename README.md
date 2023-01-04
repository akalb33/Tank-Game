# Tank-Game

By Andrew Kalb.

This is my Tank game created around 2020. This game, when launched, opens to a menu where the user can either open a game lobby, or join a game lobby. 
The program checks the local network to see if a game is open if the join option is selected, and it will join that lobby.
This game is just a two player LAN game that I created to experiment with some TCP protocol and some basic networking principles.

There is no demo for this game because it is difficult to demonstrate the game working on two computers on the same network, but all of the code is provided here.

The two players are tanks set in an arena with walls, and the goal is to shoot rockets and destroy the other person. The rockets bounce off of the walls in the arena 
which adds a small bit of complexity to the game. There is collision detection between all items in the maze, so tanks and bullets can't clip out of the arena.
