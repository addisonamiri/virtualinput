We report the function prototypes as follows:
```
//functions called when VirtualInput software is in PT (Predictive Text) mode
void gestionet9 (struttura *str,int tasto);
/*str is a pointer of the general structure struttura(see global_structures page);
  tasto is a integer,and rappresents the number expressed on the label of button pressed(only 0-9);
This function is copied from free project http://code.google.com/p/lit9/;
it is just modified for adaptation on this project*/

void sigh_print_selection(GtkWidget *gtklist,struttura *str);
/*gtklist is a pointer to GtkWidget,that rappresents list of word
   str is a pointer to the general structure struttura(see global_structures page);
This function is copied from free project http://code.google.com/p/lit9/;
it is just modified for adaptation on this project*/

void sendtofocus(struttura *str,int keycode);
/*str is a pointer to the general structure struttura(see global_structures page);
   keycode is an integer that rappresents, a code of specific character,that would be send on the focus;*/

//functions invoked by keypress of Shift, Alt Gr, Del, Space, Enter, PT, Tab, Ins
void fButtonShift(struttura *str);
/*str is a pointer to the general structure struttura(see global_structures page);
This function changes status modifier keycode into Shift*/

void fButtonAltGR(struttura *str);
/*str is a pointer to the general structure struttura(see global_structures page);
This function changes status modifier keycode into Alt Gr*/

void fButtonDelete(struttura *str);
void fButtonSpace(struttura *str);
void fButtonEnter(struttura *str);
void fButtonTab(struttura *str);

/*****Alt and Ctrl Buttons are used into unstable version 0.1.2 where they should have more features about real Alt and Ctrl of keyboard*****/
void fButtonAlt(struttura *str);
void fButtonCtrl(struttura *str);

/*This function ables or disables the predictive text mode*/
void fButtonTP(struttura *str);

void fButtonIns(struttura *str);
/*This function inserts a new entry in the database.*/

//methods dedicated to usability
void movetodown(struttura *str);
/*str is a pointer to the general structure struttura(see global_structures page);
This function displaces the main window down in 3 positions,if the window is in the end of the screen, move it in the starting point of screen;*/

void movetoright(struttura *str);
/*str is a pointer to the general structure struttura(see global_structures page);
This function displaces the main window to the right in 3 positions;*/

void movetoleft(struttura *str);/*****defined only on unstable version 0.1.2*****/
/*str is a pointer to the general structure struttura(see global_structures page);
This function displaces the main window to the left in 3 positions;*/

void getMaxScreen(struttura *str);
/*str is a pointer of the general structure struttura(see global_structures page);
This function calculates vertical and orizzontal delay for moving main window.*/

//functions used to manage icon minimization
void dockicon(struttura *str);
void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str);


//Create a keycode associated to a symbol and generate send key event
void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);

//functions referring update of dictionary and manual insert of new word from normal mode
void ins(struttura *str);
void updatedictionary(struttura *str);
void window_insert(struttura *str);

//Writing single symbol on focused window
void writeSimbol(struttura *str,lbutton *listSibolOnButton,int number);
```