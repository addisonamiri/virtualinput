/*ultima modifica 11/12/2010*/

#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 4
#define NUM_OF_BUTTONS 12
#define NameProgramm "virtualKey"

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>


void fButton1(Display *display, Window &win, Window &winRoot);
void fButton2(Display *display, Window &win, Window &winRoot);
void fButton3(Display *display, Window &win, Window &winRoot);
void fButton4(Display *display, Window &win, Window &winRoot);
void fButton5(Display *display, Window &win, Window &winRoot);
void fButton6(Display *display, Window &win, Window &winRoot);
void fButton7(Display *display, Window &win, Window &winRoot);
void fButton8(Display *display, Window &win, Window &winRoot);
void fButton9(Display *display, Window &win, Window &winRoot);
void fButton10(Display *display, Window &win, Window &winRoot);
void fButton11(Display *display, Window &win, Window &winRoot);
void fButton12(Display *display, Window &win, Window &winRoot);
void sendKey(Display *display, Window &win, Window &winRoot,int keycode);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);


typedef void(*funz)(Display *, Window &, Window &);
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);
funz listF_Buttons[12]={fButton1,fButton2,fButton3,fButton4,fButton5,fButton6,fButton7,fButton8,fButton9,fButton10,fButton11,fButton12};


void fButton1(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 1*/
{
 int keycode=XK_1;
 sendKey(display,win,winRoot,keycode); 
 //printf("1\n");
}

void fButton2(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 2*/
{
 int keycode=XK_2;
 sendKey(display,win,winRoot,keycode); 
 //printf("2\n");
}

void fButton3(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 3 */
{
 int keycode=XK_3;
 sendKey(display,win,winRoot,keycode); 
 //printf("3\n");
}

void fButton4(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 4 */
{
 int keycode=XK_4;
 sendKey(display,win,winRoot,keycode);
 //printf("4\n");
}

void fButton5(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 5*/
{
 int keycode=XK_5;
 sendKey(display,win,winRoot,keycode); 
 //printf("5\n");
}

void fButton6(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 6*/
{
 int keycode=XK_6;
 sendKey(display,win,winRoot,keycode); 
 //printf("6\n");
}

void fButton7(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 7*/
{
 int keycode=XK_7;
 sendKey(display,win,winRoot,keycode); 
 //printf("7\n");
}

void fButton8(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 8*/
{
 int keycode=XK_8;
 sendKey(display,win,winRoot,keycode); 
 //printf("8\n");
}

void fButton9(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 9*/
{
 int keycode=XK_9;
 sendKey(display,win,winRoot,keycode); 
 //printf("9\n");
}

void fButton10(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto '*' */
{
 int keycode=XK_sterling;
 for(int i=0;i<=1;i++){
   sendKey(display,win,winRoot,54); 
   //printf("  %i\n",i);
}
//printf("10\n");
}

void fButton11(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto 0 */
{
 int keycode=XK_0;
 sendKey(display,win,winRoot,keycode); 
 //printf("11\n");
}

void fButton12(Display *display, Window &win, Window &winRoot)
/*Funzione associata al tasto '#' */
{
 int keycode='#';
 sendKey(display,win,winRoot,keycode); 
 //printf("12\n");
}

/*void initF_Buttons()
/*La funzione inizzializza una lista di funzioni che andranno associati ai tasti*/
/*{
 funz f=fButton1;
 listF_Buttons[1]=fButton1;
 listF_Buttons[2]=fButton2;
 listF_Buttons[3]=fButton3;
 
}*/

/*funzione che notifica pressione e rilascio si un dato tasto ad un'applicazione*/
void sendKey(Display *display, Window &win, Window &winRoot,int keycode)
{
 // Send a fake key press event to the window.
 SendKeyEvent(display,win,winRoot,KeyPress,keycode,0);
 // Send a fake key release event to the window.
 //printf("send %c\n",keycode);
 SendKeyEvent(display,win,winRoot,KeyRelease,keycode,0);
}
/*Wrapper createKeyEvent*/ 
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers)
{
 Status err=sendKeyEvent(display,win, winRoot,type, keycode,modifiers);
 if (err==BadValue)
 {
  fprintf(stderr,"error : programm %s ,function createKeyEvent(..) return BadValue\n",NameProgramm); 
  exit (1);
 }
 if (err==BadWindow)
 {
  fprintf(stderr,"error : programm %s ,function createKeyEvent(..) return BadWindow\n",NameProgramm); 
  exit (1);
 }
}

// Function to create and send a keyboard event
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers)
{
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;
   event.type        = type;
   return XSendEvent(display, win, True, KeyPressMask, (XEvent *)&event); 
}

//gtk_get_name_button(widget);
void button_clicked(GtkWidget *widget, gpointer data)
{
 Window winFocus;
 int    revert;
 Display *display = XOpenDisplay(0);
   
// Get the root window for the current display.
 Window winRoot = XDefaultRootWindow(display);

// Find the window which has the current keyboard focus.
 XGetInputFocus(display, &winFocus, &revert);

   

 listF_Buttons[atoi(gtk_widget_get_name(widget))](display,winFocus,winRoot);
 
 XCloseDisplay(display);


}


int main(int argc, char *argv[])
{
 GtkWidget *window, *frame, *box, *table;
 GtkWidget **buttonArray;

 int i, x, y;
 char s[12][7]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","*","0 +","#"};
 char s_i[2];
 gtk_init (&argc, &argv);

 window = gtk_window_new (GTK_WINDOW_POPUP);
 gtk_window_set_default_size (GTK_WINDOW(window), 250, 250);

 /* Crea una Frame */
 frame = gtk_frame_new(NULL);
 gtk_container_add(GTK_CONTAINER(window), frame);
 /* Setta l'etichetta della frame */
 gtk_frame_set_label( GTK_FRAME(frame), "Tastiera" );
 gtk_widget_show (frame);

 box = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (frame), box);
 gtk_widget_show (box);

 buttonArray = (GtkWidget**)g_malloc(NUM_OF_BUTTONS*sizeof(GtkButton**));

 table = gtk_table_new(NUM_OF_ROWS, NUM_OF_COLUMNS, TRUE);
 gtk_table_set_row_spacings(GTK_TABLE(table), 2);
 gtk_table_set_col_spacings(GTK_TABLE(table), 2);
 x=y=0;
 
 for(i=0; i<NUM_OF_BUTTONS;i++)
 {
    buttonArray[i] = gtk_button_new_with_label(s[i]);
    gtk_table_attach_defaults(GTK_TABLE(table), buttonArray[i], x, x+1, y, y+1);
    sprintf(s_i,"%i",i);
    gtk_widget_set_name(buttonArray[i],s_i);
    g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked), NULL);
    x++;
    if(x==NUM_OF_COLUMNS)
    {
        x=0;
        y++;
    }
 }
 gtk_box_pack_start(GTK_BOX(box), table, FALSE, FALSE, 0);

  /* Mostra gli elementi */
 gtk_widget_show_all(GTK_WIDGET(window));
 
 gtk_main ();
 return 0;
}
 
