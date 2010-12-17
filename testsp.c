#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 4
#define NUM_OF_BUTTONS 15
#define NUM_CLICKS 0

#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static int count = 0;
static int buttonprev = -1;
static int modifier = 0;
time_t tprev,tnow;

// The key code to be sent.
// A full list of available codes can be found in /usr/include/X11/keysymdef.h
//#define KEYCODE XK_Down

// Function to create a keyboard event
XKeyEvent createKeyEvent(Display *display, Window &win, Window &winRoot, bool press, int keycode, int modifiers)
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

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

void sendtofocus(int keycode)
{
// Obtain the X11 display.
   Display *display = XOpenDisplay(0);
   //if(display == NULL)
      //return -1;

// Get the root window for the current display.
   Window winRoot = XDefaultRootWindow(display);

// Find the window which has the current keyboard focus.
   Window winFocus;
   int    revert;

   XGetInputFocus(display, &winFocus, &revert);

// Send a fake key press event to the window.
   XKeyEvent event = createKeyEvent(display, winFocus, winRoot, TRUE, keycode, modifier);
   XSendEvent(event.display, event.window, TRUE, KeyPressMask, (XEvent *)&event);

// Send a fake key release event to the window.
   event = createKeyEvent(display, winFocus, winRoot, FALSE, keycode, modifier);
   XSendEvent(event.display, event.window, TRUE, KeyPressMask, (XEvent *)&event);

// Done.
   XCloseDisplay(display);
}

void setshift(GtkWidget *widget, gpointer data)
{
	if(modifier==XK_Control_R)//XK_ISO_Level5_Shift
		modifier=0;
	else
		modifier=XK_Control_R;
}
void button_clicked(GtkWidget *widget, int tasto[10][5])
{
 const gchar* buttonow=gtk_button_get_label(GTK_BUTTON(widget));
 (void) time(&tnow);
 int keycode;
 if(strcmp(buttonow,"1")==0)
 {
	if(buttonprev!=1){
		count=0;
		tprev=0;
	}
	if(!tprev){
 		keycode = tasto[0][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[0][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[0][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=1;
 }
 if(strcmp(buttonow,"2 abc")==0)
 {
	if(buttonprev!=2){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[1][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[1][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[1][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=2;	
 }
 if(strcmp(buttonow,"3 def")==0)
 {
	if(buttonprev!=3){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[2][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[2][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[2][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=3;
 }
 if(strcmp(buttonow,"4 ghi")==0)
 {
	if(buttonprev!=4){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[3][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[3][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[3][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=4;
 }
 if(strcmp(buttonow,"5 jkl")==0)
 {
	if(buttonprev!=5){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[4][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[4][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[4][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=5;
 }
 if(strcmp(buttonow,"6 mno")==0)
 {
	if(buttonprev!=6){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[5][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[5][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[5][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=6;
 }
 if(strcmp(buttonow,"7 pqrs")==0)
 {
	if(buttonprev!=7){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[6][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[6][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[6][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=7;
 }
 if(strcmp(buttonow,"8 tuv")==0)
 {
	if(buttonprev!=8){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[7][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[7][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[7][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=8;
 }
 if(strcmp(buttonow,"9 wxyz")==0)
 {
	if(buttonprev!=9){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[8][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[8][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[8][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=9;
 }
 if(strcmp(buttonow,"0 +")==0)
 {
	if(buttonprev!=0){
		count=0;
		tprev=0;
	}
 	if(!tprev){
 		keycode = tasto[9][count];
		sendtofocus(keycode);
		count++;
	}
	else{
		if(tnow-tprev<2){
			keycode = XK_BackSpace;
			sendtofocus(keycode);
 			keycode = tasto[9][count];
			sendtofocus(keycode);
			if(count<4)
				count++;
			else
				count=0;
		}
		else{
			count=0;
			keycode = tasto[9][count];
			sendtofocus(keycode);
		}
	}
	(void) time(&tprev);
	buttonprev=0;
 }
 if(strcmp(buttonow,"⇧ shift")==0)
 {
 	if(modifier==XK_ISO_Level5_Shift)
		modifier=0;
	else
		modifier=XK_ISO_Level5_Shift;
	count=0;
	tprev=0;
	buttonprev=-1;
 }
 if(strcmp(buttonow,"alt gr")==0)
 {
 	if(modifier==XK_Control_R)
		modifier=0;
	else
		modifier=XK_Control_R;
	count=0;
	tprev=0;
	buttonprev=-1;
 }
 if(strcmp(buttonow,"←")==0)
 {
	keycode = XK_BackSpace;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
 }
 if(strcmp(buttonow,"space")==0)
 {
	keycode = XK_space;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
 }
 if(strcmp(buttonow,"↵")==0)
 {
	keycode = XK_KP_Enter;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
 }
}

void loadkeynormal(int keycodemap[10][5])
{//creare array tridimensionale []=min, []=maiu, []=num
 keycodemap[0][0]=XK_period;
 keycodemap[0][1]=XK_comma;
 keycodemap[0][2]=XK_minus;
 keycodemap[0][3]=XK_apostrophe;
 keycodemap[0][4]=XK_1;
 keycodemap[1][0]=XK_a;
 keycodemap[1][1]=XK_b;
 keycodemap[1][2]=XK_c;
 keycodemap[1][3]=XK_agrave;
 keycodemap[1][4]=XK_2;
 keycodemap[2][0]=XK_d;
 keycodemap[2][1]=XK_e;
 keycodemap[2][2]=XK_f;
 keycodemap[2][3]=XK_egrave;
 keycodemap[2][4]=XK_3;
 keycodemap[3][0]=XK_g;
 keycodemap[3][1]=XK_h;
 keycodemap[3][2]=XK_i;
 keycodemap[3][3]=XK_igrave;
 keycodemap[3][4]=XK_4;
 keycodemap[4][0]=XK_j;
 keycodemap[4][1]=XK_k;
 keycodemap[4][2]=XK_l;
 keycodemap[4][3]=XK_backslash;
 keycodemap[4][4]=XK_5;
 keycodemap[5][0]=XK_m;
 keycodemap[5][1]=XK_n;
 keycodemap[5][2]=XK_o;
 keycodemap[5][3]=XK_ograve;
 keycodemap[5][4]=XK_6;
 keycodemap[6][0]=XK_p;
 keycodemap[6][1]=XK_q;
 keycodemap[6][2]=XK_r;
 keycodemap[6][3]=XK_s;
 keycodemap[6][4]=XK_7;
 keycodemap[7][0]=XK_t;
 keycodemap[7][1]=XK_u;
 keycodemap[7][2]=XK_v;
 keycodemap[7][3]=XK_ugrave;
 keycodemap[7][4]=XK_8;
 keycodemap[8][0]=XK_w;
 keycodemap[8][1]=XK_x;
 keycodemap[8][2]=XK_y;
 keycodemap[8][3]=XK_z;
 keycodemap[8][4]=XK_9;
 keycodemap[9][0]=XK_plus;
 keycodemap[9][1]=XK_less;
 keycodemap[9][2]=XK_grave;
 keycodemap[9][3]=XK_division;
 keycodemap[9][4]=XK_0;
}
int main(int argc, char *argv[])
{
 GtkWidget *window, *frame, *box, *table;
 GtkWidget **buttonArray;

 int i, x, y;
 char s[15][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","⇧ shift","0 +","alt gr","←","space","↵"};
 int tasto1norm[10][5];
 loadkeynormal(tasto1norm);

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
    gtk_widget_set_name(buttonArray[i],s[i]);
    if(i==9)
	g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(setshift), NULL);
    else
    	g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked), tasto1norm);
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
