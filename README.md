# Demobox - a HTPC application for demos

Demobox is an application to run on your living room computer. It allows to run demos straight from your couch, bypassing launcher windows if there are any. And of course, you can watch videos or listen to music :)

### Uses
If, just like me - xtrium - you prefer to watch the real thing instead of a youtube capture, but you also like to watch demos sitting in your couch (or maybe you want to display a demo playlist at a party), you might already use a HTPC application like XBMC, but these programs don't run demos. In that case, Demobox is just for you !

### How to use
* Build using VS2013
* Put the Demobox.exe and the dlls from the Release (or Debug) folder into a common folder, and put the assets/ folder next to them.
* Run.

### How to add a new demo
* Unpack it in the assets/demos/<my_demo> folder
* create assets/demos/[my_demo]/[my_demo].demo file
* Use the following format :

      * Demo title
      * Author or group
      * Type value: 0 for 1k or less, 1 for 4k, 2 for 64k, 3 for demos
      * Platform value: always 0 (windows) for the moment
      * Release party
      * Release year
      * Rank at the demoparty it was released
      * executable to start
      * Launcher window title(0 if no launcher)
      * Launcher start button caption (0 if no launcher)

### State
Demobox is in a rather early stage. The plan is to get as many people to try it, report bugs, and for those who are brave enough, take part in its development :)

### Known issues
As said earlier, the app is in its early stages and follows the "release early, release often" philosophy. As such, it's not really ready for daily use, though it shouldn't burn your computer down. =)

 * App crashes when one of the section (demos / music / movies) is empty
 * Main app display gets translated down when exiting a demo that does not use the same screen resolution, leaving a black area above
 * No playlist mode
 * No custom folders, background animation, or actually no settings whatsoever

### License

           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                   Version 2, December 2004
 
Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 
Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.
 
           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 
 0. You just DO WHAT THE FUCK YOU WANT TO.
