<font size='5'>
<b>About VirtualInput</b></font>
<br><br>
VirtualInput is an open source program whose main function is to act as a real keyboard operating on Linux system. It could be particularly useful for tablets, netbook, PDA and other devices that require a pen-based interface.<br>
<br><br>
<font size='5'>
<b>Description</b></font>
<br><br>
The software has been developed by <a href='http://code.google.com/p/virtualinput/people/list'>two</a> students enrolled in Master Studies in Computer Engineering of the University of Messina (Italy), <a href='http://ww2.unime.it/ingegneria/new/index_1024.php'>Faculty of Engineering</a> and it is supervised by Prof. Giancarlo Iannizzotto, coordinator of Visilab (<a href='http://visilab.unime.it/new/'>The Computer Vision Laboratory</a>).<br>
This project is based on GTK libraries in order to define C interface and X11 libraries in order to establish the interaction with operating System.<br>
<br>
To make easier the operational use of the common keyboard, the software provides two approaches:<br>
<br>
<ul><li>Normal keyboard mode, in which single letters are associated with each keycode and the selection of a word requires multiple keystrokes of buttons. This mode corrisponds exactly to that of the mobile phones keypad;</li></ul>

<ul><li>Standard T9 text mode, used to mobile phones in which all words correspond to a sequence of numbers.</li></ul>

The second mode is defined with an embedded SQLite database containing an Italian words list. There each word consists of a record formed by a unique numeric code, a string of same word and a value of frequency use.<br>
<br><br>
<font size='5'>
<b>Working with the program</b></font>
<br><br>
The compilation of VirtualInput can be done on Ubuntu 9.x or 10.x and you should have the following packages or libraries installed:<br>
<br>
<ul><li>libgtk2.0-dev</li></ul>

<ul><li>xdotool (it is required in the latest version)</li></ul>

<ul><li>xorg-dev</li></ul>

<ul><li>libsqlite3-dev</li></ul>

You can install them from shell with following command:<br>
<pre><code>sudo apt-get install libgtk2.0-dev xdotool xorg-dev libsqlite3-dev<br>
</code></pre>
but you can also get this packages from Linux repository.<br><br>
VirtualInput needs to be installed in one of main directories of operating system as described by Makefile: it creates a virtualinput folder where executable program, dictionary of words for T9 and application icon are stored. This folder is located into <code> /usr/share </code>.<br><br>
So to install this software, write in the shell these instructions:<br>
<pre><code>tar xvzf virtualinput-version.tar.gz<br>
cd virtualinput-version<br>
make<br>
sudo make install<br>
virtualinput<br>
</code></pre>
If you want to uninstall it:<br>
<pre><code>sudo make uninstall<br>
rm -r virtualinput-version<br>
</code></pre>
In <a href='http://code.google.com/p/virtualinput/downloads/list'>Downloads</a>
you can find first release version with its user guide and last one.<br>
<br>
Page related:<br>
<br>
<blockquote><a href='http://dottorgianky.altervista.org/virtualinput/virtualinput-0.1beta/VirtualInput_manual-0.1beta.html'>manual version 0.1 beta</a>