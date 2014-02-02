"Watch out!  He's got a compiler -- and he's not afraid to use it!"

First, the nasty legal stuff:

This is Cardfile/2, an OS/2 PM program which reads and writes Windows cardfiles.
Copyright (C) 1997 Richard Price.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Contacting the Author:

Please send comments/encouragement to me at 100022.2211@compuserve.com; I should have this address through 1997.  I also occasionally check mail at rprice@mnsinc.com.  I welcome your comments.  (Do you like the icon?  I did it myself. :))  If you think this program is really useful, and you want my girlfriend to let me continue writing programs for OS/2 (instead of paying attention to her all the time) you could even send me a postcard:  1916 17th St. NW, #305, Washington, DC 20009.

Okay, now on to the good stuff.  

Description

This is an OS/2 PM program which is compatible with the Windows cardfile program, i.e. it reads and writes cardfiles of the same format as the Windows program.  In addition, I have partly replicated the functionality and interface of the Windows program, along with many of its idiosyncrasies [like those two-dollar words?]  I have, however, improved on the interface (in my opinion) with some nice big buttons to click on to switch cards.  Otherwise, the program follows the same paradigm [looks like a $2.00 word, but it's only worth 20 cents] as the Windows applet.  The only thing that isn't obvious from the interface is that you can jump to a card beginning with a certain letter by hitting Ctrl+Shift+(letter); this is also in the Windows program, and is a useful feature (much quicker than flipping through the cards.)

Requirements

This was compiled with EMX 0.9b, so it will need the EMX .DLL files, readily available at Hobbes (ftp://hobbes.nmsu.edu/pub/os2/emx09c) and elsewhere. 

OS/2 -- I haven't done anything with the code to make it specific to a particular version, so I assume it will run in all of them beyond 2.0, although I have only tested it on Warp.  If you find out otherwise, let me know.   If you are running it on Warp, I recommend mashing it down with LXLite or LXOPT (which is free for executables under 256k) -- saves disk space, and maybe runs faster (not that it's terribly slow anyway.)

Limitations

There are a few of them...  First, this should be considered a beta, since I haven't had anybody else test it.  If you find problems, please let me know.

As noted, this requires the EMX .DLLs.

The reader can't read cardfiles with embedded objects or pictures in them.  Also, it doesn't do a number of things that the Windows reader does -- print cards, for example.  With the exception of showing bitmaps, the things this version doesn't do are things I think aren't worth much anyway -- printing, for example.  (Printing ONE card on a sheet of 8x11 paper?  For this, they killed a tree?)  If you REALLY want some function that is in the Windows program, let me know, and if it's easy I will write it in.  (Or you could do it yourself...  Use the source, Luke!  Sorry, I couldn't resist...)

Note that there is no on-line help -- just this file -- but it should be pretty easy to figure out the program without it if you have used the Windows cardfile program.  Sorry about CUA and all that, but I am tired of messing with the program and I don't feel like writing a help file right now.  Look at the Windows program's help file, if you really can't figure it out; those functions which are implemented are practically identical.  If enough people are interested, I will put out a version with on-line help and other improvements.

Also, there is practically NO ERROR CHECKING in the code.  If you try to open a non-existent file (for example) the program will just fold up on you.  Again, if I decide to improve on this program, I will put in at least some basic error checking.  It might be a good idea to have a kill utility (Watchcat, Kill, Go) around in case the thing hangs up on you (it hasn't happened to me since a couple revisions back, but it's not out of the question, depending on what out-of-the-ordinary thing you do.)

I am running at 1024 x 768, and a 14-point font looks good for this program at that resolution; I have put in code to switch that to 12 point for 800x600, and 10 point for 640 x 480, but I haven't checked to see if the code works (though it should -- it's pretty trivial) because I don't have a 15- or 14-inch monitor to see if the font sizes are appropriate for resolutions other than 1024 x 768.  If you have a complaint about it, let me know what size looks better to you; that can be changed very easily.  For compatibility reasons, the font really needs to be System VIO, or some other monospaced font.

Default behavior at the moment is to save to the same directory the program is in -- you will have to use "Save As" to save it elsewhere.  Again, if anyone cares, let me know.

Bugs

It would be a surprise if there weren't any...  However, I have killed all the ones I know of.  There is one bug involving an access violation (crash) when editing the index string of a card which I am not certain is gone; when this happened, it usually meant that the program had somehow generated an invalid card.  I think I figured out what was happening, but I didn't trace the logic through all the way to make sure the problem was 100% fixed; if you have this problem, or any other problem which generates cards that the Windows program will not read, please let me know. (See Q&A for a caveat, however.)

Installation

Put it in the directory with your card files, create a desktop object for it if you want.  It's just an executable, and doesn't touch your .INI files (which also means it appears in the same location every time, just like the Windows one.)  If you want this to be your default cardfile viewer, open the Properties notebook on the Windows reader and remove the *CRD association; then open the Properties notebook for Cardfile/2 and add the *CRD association. 

How to Use It

Open up your Windows-style cardfile; add, delete, and edit cards using the functions provided in the menu.  Type text onto the cards just by typing.  Move from card to card by hitting the big fat buttons, or by clicking on any visible part of the card you want.  Hotkeys are provided (see the menu), and Ctrl+Shift+(letter) moves you to the next card beginning with that letter.

Other Info:  Why and How I Wrote It

Six months or so ago, I discovered that about 30% of the instability in my OS/2 Warp system was caused by Windows.  (The other 70% is a faulty graphics driver for my Diamond [ugh] video card...)  This wasn't much of a problem, since I only had a few Windows applications that I used regularly:  WordPerfect 6.1 and the Windows cardfile reader.  It immediately occurred to me that there ought to be a native OS/2 cardfile reader, so I went looking for one.  I actually did find one, but it was shareware ($15 for an application you can get for free with an outdated DOS shell!) and I didn't like the interface (that ugly yellow Warp 3 MLE box!)  Even worse, the shareware version could easily make card files that were no longer compatible with the Windows reader -- a real no-no, in my opinion.  

Since I was, at the time, severely underemployed, and had a lot more time than money, I decided to see how easy it would be to write my own version.  I was inspired to do this by an amazing set of free or almost-free tools for OS/2.  

The first, and most important, is of course the EMX compiler.  A free, full-featured compiler!  Incredible!  Next is a little program called Project Manager for EMX/GCC, which eliminates the need for makefiles and automates the compiling process.  If I had had to learn all the ins and outs of makefiles and the various make programs, this program never would have been done; my head would probably still be spinning.  I tried EPM for an editor, and found it OK, but I ended up registering a programming editor called Mr. Ed.  Only $25 from BMT Micro, upgraded frequently, and very capable!  Finally, for a hex editor (every programmer needs a hex editor) I used a beta version of an OS/2 native hex editor called HeD/2, which proved very capable and well laid out (features I wasn't using didn't get in the way.)  So, a big thank you to the authors of all of these.

Using the hex editor, I was able to figure out the structure of the basic Windows cardfile (not including graphics) in about 20 minutes.  Given that it was so simple, I figured it would be a piece of cake to get a quick-and-dirty command line program together.  First, though, I had to learn to program in C and C++.  My most recent programming experience was a course in structured programming, taught in Pascal, over a dozen years ago; but the little I had seen of C and C++ suggested that it wasn't completely different.  I was able to pick up the essentials by going through a C++ programming book -- C++ Primer Plus, by Stephen Prata.  In spite of the annoying use of cutesy and sometimes-confusing variable names, this book covered C++ at just about the right pace and level of detail; I would recommend it to anyone getting started on C++ (be prepared to spend some time reading, though; it's 800 pages...) [A note to those people who were debating C/C++ difficulty in comp.os.os2.programming.* a few months back -- you're all wrong!  C++ is not easy, nor is it hard -- like any other language, it has a learning _profile_; C++ starts out easy, and gets very difficult once you start using its most powerful features.  My considered opinion.]

I was right about the command-line part being easy -- I had a command-line program which would read cardfiles and spit them out after a couple of days' work.  That was the easy part...  Then came the PM programming work.  Since I had never worked with a GUI environment before, I figured I would start from ground zero, which is Petzold's "OS/2 Presentation Manager Programming."  Practically the whole program was written with reference to just these two books (Prata and Petzold), which shows you how useful they were (and how basic it is to program this stuff!)   My total layout on this project was about $65 for the books, and $25 more for Mr. Ed...  and my time, of course.  Everything else was free.  I strongly recommend this approach for anyone considering getting started in OS/2 programming who doesn't have the money to take classes.

Anyway, writing the rest of the program was NOT easy -- I probably ended up spending about 250 hours total on it over the course of six months, with about half of that time spent scratching my head and going back to RTFM.  If you look at it as an activity to produce something that I wasn't going to get paid for, it was quite a bit of time down the tubes; but if you look at it as a six-week course in C/C++, with a useful application at the end of it, I got a good deal :)  If I had to do it again, it would take me about a third the time, I think, or maybe even less; I learned a lot by writing the program.

Questions and Answers

Q.	Why can't you do case-sensitive searches, and why can't you do multiple searches without reopening the dialog box?

A.	It would actually be very easy to implement both of these -- case-sensitive searching is provided for in the MLE API, and multiple searches could be done just by making the dialog box modeless -- but both of these feel like featuritis to me.  I have never used either of these features, and I am a bit skeptical that they would really be useful to anyone else.  It seems to me to be easier to have the dialog box disappear automatically, instead of having to dismiss it; on the rare occasion when you might search for the same thing twice in a row, you can just restart the search.  "Just because it's easy doesn't mean it should be done."

Q. 	Is is possible to create a card that the Windows reader can't read?

A.	The answer to this is a qualified yes.  The qualification has to do with a built-in limitation of the MLE box.  To see it, put the cursor at the end of a line in a text-wrapped MLE box (the E editor with word wrap on, for example), press the space key, and hold it down.  The cursor will scroll blithely along forever, while your text disappears off the left side of the screen.  Why IBM chose to implement the control in this brain-dead fashion -- and has not fixed it -- is beyond me.  I have actually figured out a workaround for this, which involves capturing the space-bar keystrokes and replacing them with ASCII 255 (a blank space in almost all fonts -- it certainly is in System VIO, the font used here -- which is treated as a character, and therefore serves as a breaking space.)  Implementation is a bit complicated, though -- you have to take care of potential input from the clipboard -- so I haven't bothered to do it in this version.  If I get lots of requests for it, I may put it in in a later version, assuming there is one (or somebody else could code it, and send it to me...)  Anyway, the buffer I have set up for the MLE is a bit bigger than the space available on the cards, so if you create a card with trailing spaces on the right the Windows reader may not be able to read it.  Just don't do it.  (Patient:  It hurts when I do this.  Doctor:  Don't do that.)  Also, if you put in text which is larger than the MLE buffer (you would have to include these trailing spaces to do that) the extra text will be truncated beyond a certain limit.

Q.	Can I use this code for my version of (program x)?

A.	I would be happy to have you take whatever is useful in this code and use it in your own programs.  However, bear in mind the terms of the GNU public license -- you can't use the code in a program that you ask people to pay for.  Also, it would be nice to get a little credit.  :)  I can't really imagine that there's anything that striking in this program that you can't get elsewhere, though, unless it's the Linkedlist class, which is the most general possible implementation of a linked list -- I haven't seen such an example anywhere else, though it's probably out there on the web someplace.

Q.	Do you plan to make a Windows version?

A.	Don't be a smartass.

Credits

I wrote the program, but I want to give credit to the people who gave me all the great free tools that I used to build it.

We can start with Eberhard Mattes, for producing the EMX compiler; next would be Roland Knospe, for producing Project Manager for EMX/GCC; Dimitris Kotsonis for producing HeD/2.  Then you can include the books I used -- Prata and Petzold -- a great start for a beginner (these weren't free, though.)  Thanks, guys!

Also, I was inspired by the producers of my favorite OS/2 freeware:  NPSWPS, the granddaddy WPS enhancer of them all; KBDREDEF, the excellent free keyboard remapper; HPFSREM, which gives you the capability to use the HPFS format on removable drives (and have them remain removable); and too many others to list.

History

2/21/97 -- beta 0.9 released.