Multiversal
===========

Multiversal is a 2D puzzle platformer game with parallel worlds. The goal is to get to the end of each level.

More information about this as development progresses can be found here: http://ayebear.com/.


Gameplay
--------

You play as a character who can move, jump, and lift objects. You are equipped with a powerful tool, which lets you peer into the alternate world. Everything seen through this window becomes part of the real world. Objects can be transferred through this window.

You can jump on platforms that exist only in the alternate world by viewing them with the window. You can also walk through walls by viewing empty space from the alternate world.

Boxes or carryable objects can be lifted from inside or outside of the window. While carrying, the object is in the real world. Dropping the object inside of the window causes the object to be transferred into the alternate world.

##### Switches

All switch types control the state of different objects or tiles. Here are the different types:

* Toggle switches
  * Retains state
  * Controlled by you (with the action key)
* Push-button switches
  * Does not retain state
  * Controlled by an object sitting on top of it
* Laser sensors
  * State retention is configurable
  * Controlled by a laser beam hitting it

##### Switch controlled stuff

* Force fields
  * When they are turned on, they block everything from passing through.
* Moving platforms
  * Moves to multiple points
  * Switching will change its direction
  * Carries objects, and/or the player
  * Can travel between worlds (TBD)
* Lasers
  * Used to activate laser sensors
  * Bounces off of mirrors
  * Travels between worlds both ways
    * From alternate to real: Simply view the laser itself, the beam will end up in the real world
    * From real to alternate: Have the laser hit something in the alternate world
* Mirrors
  * Angle is changed when controlled by a switch
  * Redirects laser beams at 90 degree angles


Controls
--------

#### Character

* A/D: Move left/right
* W: Lift/drop objects; Action key
* Space: Jump

#### Window

* Left click: Activate and place window
* Right click: Deactivate window


Code
----

The code is written in C++14, and uses SFML 2.2 (https://github.com/LaurentGomila/SFML).

The code is licensed under the GPLv3 license. See LICENSE.txt for more details.


Assets
------

The remaining required assets needed to run this game can be downloaded here: http://ayebear.com/multiversal/data.zip.


Author
------

Eric Hebert
