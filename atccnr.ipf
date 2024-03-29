.* Generated by VyperHelp on 06/23/23.
:userdoc.
:docprof toc=123456.
:title.Help for Air Traffic Control Container
.* Subject: 
.* Version: 
.* Copyright: 
.* Author: 
:h1 res=1 id='res_00001'.
General Help
:i1 id=30000.General Help
:i1 id=30001.OS/2 Bay Area User Group
:i1 id=30002.Developers SIG
:i1 id=30003.Program Author
:p.Welcome to :hp2.Air Traffic Control Container:ehp2.&comma. a very simple ATC
radar simulator whose main purpose is to provide an interesting example of
OS&slash.2 PM Container programming&per. Its design was stimulated by the
author&apos.s visit in October&comma. 1993&comma. to Bay TRACON&comma. the main
air traffic control location for the San Francisco Bay Area&per.  
:p.ATC Container was originally written for a presentation by the author to the
OS&slash.2 Bay Area User Group&apos.s :link reftype=hd refid='devsig'.Developers
SIG:elink.&comma. on container programming&per.  
:p.ATC Container illustrates several areas of container programming&colon. 
.br
Multiple views 
.br
Inserting multiple container records 
.br
Explicitly positioning records 
.br
Enumerating container contents 
.br
Using a PM Timer to move the container objects at regular intervals 
.br
Multiple context menus 
.br
Container background owner&endash.drawing 
.br
Bitmaps for screen drawing performance 
.br
Randomly generating aircraft &lpar.always good to know&rpar.  
:p.:link reftype=hd refid='db'.Dave Briccetti &amp. Associates:elink. 
.br
P&per.O&per. Box 1713 
.br
Lafayette&comma. CA  94549&endash.7013 
.br
USA 
.br
510 945&endash.7565 
.br
CompuServe&colon. 74475&comma.1072&comma. Internet&colon.
daveb&atsign.netcom&per.com 
:p.Copyright &lpar.c&rpar. 1993&comma. David C&per. Briccetti 
.br
All Rights Reserved&per.    
:h1 res=30005 id='30005'.
Views
:i1 id=30004.Views
:p.ATC Container starts up in the :hp2.Radar View:ehp2.&comma. which bears a
slight resemblance to the ARTS III radar scopes in use in some facilities&per.
 The jet icons represent targets being tracked&comma. and the numbers below each
icon represent the aircraft&apos.s altitude and speed&per. The altitude shown
has the two rightmost zeroes removed for brevity&per. The speed has one zero
removed&per.  
:p.The other views do not provide positional information&per.  The
:hp2.Name:ehp2. view just shows the icons and title text&per.  The
:hp2.Text:ehp2. view just shows the title text&per.  
:p.The :hp2.Details:ehp2. view shows much more information about each
aircraft&comma. including the current and assigned heading&comma.
altitude&comma. and speed&per.  
:p.To change views&comma. click in the container with the context menu mouse
button when the mouse pointer is not over an aircraft&per.  
:h1 res=30007 id='30007'.
Controlling Aircraft
:i1 id=30006.Controlling Aircraft
:p.ATC Container allows you to control aircraft&per.  Click on an aircraft to
bring up a context menu&per.  Select the :hp2.Control Aircraft:ehp2. menu
item&per.  
:p.A dialog box appears with spin buttons which allow you to assign a new
heading&comma. altitude&comma. and speed&per.  The aircraft will gradually
maneuver to comply with your instructions&per.  
:h1 res=30009 id='30009'.
Limitations
:i1 id=30008.Limitations
:p.ATC Container has many limitations&per.  
:p.Aircraft Maneuvering Rates 
.br
The rates at which the aircraft change heading&comma. altitude&comma. and speed
are not realistic&per.  For example&comma. a standard rate turn is 3 degrees per
second&comma. and these aircraft turn at 1 degree per second&per.  Also&comma.
the program doesn&apos.t know enough about the compass to know that in order to
change from heading 010 to 350 a 20 degree left turn would be appropriate&per.
 Instead&comma. it will turn right&per. 
:p.Scroll Bars 
.br
As aircraft move off the scope&comma. scroll bars appear&per.  The program does
not have proper support for scrolling&per. 
:p.Crashing 
.br
The program crashes occasionally 
:p.Lack of cleanup 
.br
The program does not free the memory it allocates&comma. but sloppily relies on
the C runtime library and&slash.or OS&slash.2 to clean up at program
termination&per. 
:p.Limited Usefulness 
.br
ATC Container is an interesting container example&comma. but it is far from a
real ATC simulator&per. 
:h1 res=30010 id='db' hide.
Footnote
:p.Dave Briccetti &amp. Associates has been developing sophisticated OS&slash.2
applications for users and clients since 1987&per. 
:p.As you might have guessed&comma. Dave Briccetti is a private pilot&per. 
:h1 res=30011 id='devsig' hide.
Footnote
:p.The :hp2.Developers SIG:ehp2. is a very technical subset of the OS&slash.2
Bay Area User Group membership&per.  The group meets once a month in Silicon
Valley&comma. and covers such topics as&colon. 
.br
PM Programming 
.br
Workplace Shell programming 
.br
SOM programming 
.br
C&plus.&plus. and other object&endash.oriented languages and facilities 
.br
Productive application development 
:p.For more information about the Developers SIG&comma. contact Dave
Briccetti&per.  
:euserdoc.
