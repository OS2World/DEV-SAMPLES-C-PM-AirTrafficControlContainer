:userdoc.
:title.Help for Air Traffic Control Container
:docprof.
:ctrldef.
:ctrl ctrlid=a controls='PREVIOUS SEARCH PRINT INDEX CONTENTS ESC BACK FORWARD' coverpage.
:ectrldef.
:body.

:h1 res=1.General Help
:i1.General Help
:p.Welcome to :hp2.Air Traffic Control Container:ehp2.,
a very simple ATC radar simulator whose main purpose is to
provide an interesting example of OS/2 PM Container programming.
Its design was stimulated by the author's visit in October, 1993,
to Bay TRACON, the main air traffic control location for the San
Francisco Bay Area.

:i1.OS/2 Bay Area User Group
:i1.Developers SIG
:p.ATC Container was originally written for a presentation by the
author to the OS/2 Bay Area User Group's
:link reftype=fn refid=devsig.Developers SIG:elink., on container
programming.

:p.ATC Container illustrates several areas of
container programming:
:ul.
:li.Multiple views
:li.Inserting multiple container records
:li.Explicitly positioning records
:li.Enumerating container contents
:li.Using a PM Timer to move the container objects at regular intervals
:li.Multiple context menus
:li.Container background owner-drawing
:li.Bitmaps for screen drawing performance
:li.Randomly generating aircraft (always good to know)
:eul.

:i1.Program Author
:p.:link reftype=fn refid=db.Dave Briccetti &amp. Associates:elink.
.br
P.O. Box 1713
.br
Lafayette, CA  94549-7013
.br
USA
.br
510 945-7565
.br
CompuServe: 74475,1072, Internet: daveb@netcom.com
:p.Copyright (c) 1993, David C. Briccetti
.br
All Rights Reserved.

:fn id=db.
:p.Dave Briccetti &amp. Associates
has been developing sophisticated OS/2 applications
for users and clients since 1987.
:p.As you might have guessed,
Dave Briccetti is a private pilot.
:efn.

:fn id=devsig.
:p.The :hp2.Developers SIG:ehp2. is a very technical subset of the
OS/2 Bay Area User Group membership.  The group meets once a month
in Silicon Valley, and covers such topics as:
:ul.
:li.PM Programming
:li.Workplace Shell programming
:li.SOM programming
:li.C++ and other object-oriented languages and facilities
:li.Productive application development
:eul.
:p.For more information about the Developers SIG, contact Dave
Briccetti.
:efn.

:h1.Views
:i1.Views
:p.ATC Container starts up in the :hp2.Radar View:ehp2., which bears
a slight resemblance to the ARTS III radar scopes in use in some
facilities.  The jet icons represent targets being tracked, and the
numbers below each icon represent the aircraft's altitude and speed.
The altitude shown has the two rightmost zeroes removed for brevity.
The speed has one zero removed.

:p.The other views do not provide positional information.  The
:hp2.Name:ehp2. view just shows the icons and title text.  The
:hp2.Text:ehp2. view just shows the title text.

:p.The :hp2.Details:ehp2. view shows much more information about
each aircraft, including the current and assigned heading,
altitude, and speed.

:p.To change views, click in the container with the context menu
mouse button when the mouse pointer is not over an aircraft.

:h1.Controlling Aircraft
:i1.Controlling Aircraft
:p.ATC Container allows you to control aircraft.  Click on an
aircraft to bring up a context menu.  Select the :hp2.Control
Aircraft:ehp2. menu item.

:p.A dialog box appears with spin buttons which allow you to
assign a new heading, altitude, and speed.  The aircraft will
gradually maneuver to comply with your instructions.

:h1.Limitations
:i1.Limitations
:p.ATC Container has many limitations.

:dl break=all.
:dt.Aircraft Maneuvering Rates
:dd.The rates at which the aircraft change heading, altitude,
and speed are not realistic.  For example, a standard rate turn is
3 degrees per second, and these aircraft turn at 1 degree per
second.  Also, the program doesn't know enough about the compass
to know that in order to change from heading 010 to 350 a 20 degree
left turn would be appropriate.  Instead, it will turn right.
:dt.Scroll Bars
:dd.As aircraft move off the scope, scroll bars appear.  The
program does not have proper support for scrolling.
:dt.Crashing
:dd.The program crashes occasionally
:dt.Lack of cleanup
:dd.The program does not free the memory it allocates, but
sloppily relies on the C runtime library and/or OS/2 to clean
up at program termination.
:dt.Limited Usefulness
:dd.ATC Container is an interesting container example, but it
is far from a real ATC simulator.
:edl.

:index.
:euserdoc.
