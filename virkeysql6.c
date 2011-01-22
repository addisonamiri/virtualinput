/*per l'inserimento nuova parola vai a riga 933
 void updatedictionary() riga 966 

*/



#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 6
#define NUM_OF_BUTTONS 20
#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 325

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sqlite3.h>

#define N 5

void gestionet9 (int tasto);

GtkWidget *window, *gtklist, *list_item, *box, *tbox, *ubox, *mylabel, *mylabel2;
GtkStatusIcon *tray_icon;

//window tag management
int position, s_width, s_height;

//key management
static int count = 0;
static int buttonprev = -1;
static int modifier = 0;
time_t tprev,tnow;

//PT(Predictive Text) management
struct nodo {
  int frequenza;
  char parola[30];
};
struct nodo vetparole[N];

int statot9;
int indice;
char codicet9[30];
char nuovaparola[30];//utilizziamo adesso la variabile
int lungparola;//nuovo
char tmpc[30];//nuovo
int keycode;//nuovo
int luncodicet9;
int flagcaricat9;
gchar* parole[N];
int numparoletrovate;
const gchar *list_item_data_key="list_item_data";
sqlite3 *db;
sqlite3_stmt *stmt;

void tray_icon_on_click(GtkStatusIcon *status_icon,gpointer user_data)
{
 printf("Clicked on tray icon\n");
 gtk_widget_show(window);
 gtk_status_icon_set_visible(tray_icon, FALSE);
}

void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button,guint activate_time, gpointer user_data)
{
 printf("Popup menu\n");
}

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

void button_clicked(GtkWidget *widget, int tasto[10][5])
{
 const gchar* buttonow=gtk_button_get_label(GTK_BUTTON(widget));
 (void) time(&tnow);
 //int keycode;
 if(strcmp(buttonow,"1")==0)
 {
	if(!statot9){
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
		//NUOVO
		bzero(nuovaparola,30);
	}
	else
		gestionet9 (1);
 }
 if(strcmp(buttonow,"2 abc")==0)
 {
	if(!statot9){
		if(buttonprev!=2){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[1][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_a || keycode==XK_b || keycode==XK_c || keycode==XK_agrave || keycode==XK_2)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[1][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_a || keycode==XK_b || keycode==XK_c || keycode==XK_agrave || keycode==XK_2){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[1][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_a || keycode==XK_b || keycode==XK_c || keycode==XK_agrave || keycode==XK_2)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=2;
	}
	else
		gestionet9(2);	
 }
 if(strcmp(buttonow,"3 def")==0)
 {
	if(!statot9){
		if(buttonprev!=3){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[2][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_d || keycode==XK_e || keycode==XK_f || keycode==XK_egrave || keycode==XK_3)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[2][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_d || keycode==XK_e || keycode==XK_f || keycode==XK_egrave || keycode==XK_3){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[2][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_d || keycode==XK_e || keycode==XK_f || keycode==XK_egrave || keycode==XK_3)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=3;
	}
	else
		gestionet9(3);
 }
 if(strcmp(buttonow,"4 ghi")==0)
 {
	if(!statot9){
		if(buttonprev!=4){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[3][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_g || keycode==XK_h || keycode==XK_i || keycode==XK_igrave || keycode==XK_4)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[3][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_g || keycode==XK_h || keycode==XK_i || keycode==XK_igrave || keycode==XK_4){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[3][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_g || keycode==XK_h || keycode==XK_i || keycode==XK_igrave || keycode==XK_4)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=4;
	}
	else
		gestionet9(4);
 }
 if(strcmp(buttonow,"5 jkl")==0)
 {
	if(!statot9){
		if(buttonprev!=5){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[4][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_j || keycode==XK_k || keycode==XK_l || keycode==XK_5)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[4][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_j || keycode==XK_k || keycode==XK_l || keycode==XK_5){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[4][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_j || keycode==XK_j || keycode==XK_l || keycode==XK_5)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=5;
	}
	else
		gestionet9(5);
 }
 if(strcmp(buttonow,"6 mno")==0)
 {
	if(!statot9){
		if(buttonprev!=6){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[5][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_m || keycode==XK_n || keycode==XK_o || keycode==XK_ograve || keycode==XK_6)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[5][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_m || keycode==XK_n || keycode==XK_o || keycode==XK_ograve || keycode==XK_6){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
				count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[5][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_m || keycode==XK_n || keycode==XK_o || keycode==XK_ograve || keycode==XK_6)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=6;
	}
	else
		gestionet9(6);
 }
 if(strcmp(buttonow,"7 pqrs")==0)
 {
	if(!statot9){
		if(buttonprev!=7){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[6][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_p || keycode==XK_q || keycode==XK_r || keycode==XK_s || keycode==XK_7)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
				keycode = tasto[6][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_p || keycode==XK_q || keycode==XK_r || keycode==XK_s || keycode==XK_7){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[6][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_p || keycode==XK_q || keycode==XK_r || keycode==XK_s || keycode==XK_7)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=7;
	}
	else
		gestionet9(7);
 }
 if(strcmp(buttonow,"8 tuv")==0)
 {
	if(!statot9){
		if(buttonprev!=8){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[7][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_t || keycode==XK_u || keycode==XK_v || keycode==XK_ugrave || keycode==XK_8)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[7][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_t || keycode==XK_u || keycode==XK_v || keycode==XK_ugrave || keycode==XK_8){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[7][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_t || keycode==XK_u || keycode==XK_v || keycode==XK_ugrave || keycode==XK_8)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=8;
	}
	else
		gestionet9(8);
 	}
 if(strcmp(buttonow,"9 wxyz")==0)
 {
	if(!statot9){
		if(buttonprev!=9){
			count=0;
			tprev=0;
		}
 		if(!tprev){
 			keycode = tasto[8][count];
			sendtofocus(keycode);
			count++;
			//NUOVO
			if (keycode==XK_w || keycode==XK_x || keycode==XK_y || keycode==XK_z || keycode==XK_9)
				sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
			else
				bzero(nuovaparola,30);//
		}
		else{
			if(tnow-tprev<2){
				keycode = XK_BackSpace;
				sendtofocus(keycode);
 				keycode = tasto[8][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_w || keycode==XK_x || keycode==XK_y || keycode==XK_z || keycode==XK_9){
					lungparola=strlen(nuovaparola);
					strncpy(tmpc,nuovaparola,lungparola-1);
					strcpy(nuovaparola,tmpc);
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				}
				else
					bzero(nuovaparola,30);//
				if(count<4)
					count++;
				else
					count=0;
			}
			else{
				count=0;
				keycode = tasto[8][count];
				sendtofocus(keycode);
				//NUOVO
				if (keycode==XK_w || keycode==XK_x || keycode==XK_y || keycode==XK_z || keycode==XK_9)
					sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
				else
					bzero(nuovaparola,30);//
			}
		}
		(void) time(&tprev);
		buttonprev=9;
	}
	else
		gestionet9(9);
 }
 if(strcmp(buttonow,"0 +")==0)
 {
	if(!statot9){
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
				bzero(nuovaparola,30);//nuovo
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
				bzero(nuovaparola,30);//nuovo
			}
		}
		(void) time(&tprev);
		buttonprev=0;
		//NUOVO
		if (keycode==XK_0)
			sprintf(nuovaparola,"%s%c",nuovaparola,keycode);
		else
			bzero(nuovaparola,30);
	}
	else
		gestionet9(0);
 }
 if(strcmp(buttonow,"⇧")==0)
 {
 	if(modifier==XK_ISO_Level5_Shift){
		modifier=0;
		bzero(nuovaparola,30);//nuovo
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new (NULL);
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	else{
		modifier=XK_ISO_Level5_Shift;
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new (NULL);
		gtk_label_set_text (GTK_LABEL (mylabel),"activated Shift");
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	gdk_window_process_all_updates();
	count=0;
	tprev=0;
	buttonprev=-1;
 }
 if(strcmp(buttonow,"ctrl")==0)
 {
 	if(modifier==XK_Control_R){
		modifier=0;
		bzero(nuovaparola,30);//nuovo
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new (NULL);
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	else{
		modifier=XK_Control_R;
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new (NULL);
		gtk_label_set_text (GTK_LABEL (mylabel),"activated Ctrl");
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	gdk_window_process_all_updates();
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
	//NUOVO
	lungparola=strlen(nuovaparola);
	strncpy(tmpc,nuovaparola,lungparola-1);
	strcpy(nuovaparola,tmpc);//
 }
 if(strcmp(buttonow,"] [")==0)
 {
	keycode = XK_space;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
	bzero (nuovaparola,30);//nuovo
 }
 if(strcmp(buttonow,"↵")==0)
 {
	keycode = XK_KP_Enter;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
	bzero(nuovaparola,30);//nuovo
 }
 if(strcmp(buttonow,"tx pr")==0)
 {
	if(statot9==0){
		statot9=1;
		printf("\nactivated PT\n");
		gtk_container_remove(GTK_CONTAINER(ubox), mylabel2);
		mylabel2 = gtk_label_new (NULL);
		gtk_label_set_text (GTK_LABEL (mylabel2),"activated PT");
		gtk_container_add(GTK_CONTAINER(ubox), mylabel2);
		gtk_widget_show (mylabel2);
	}
	else{
		statot9=0;
		luncodicet9=0;
		bzero(codicet9,30);
		printf("\ndisabled PT");
		gtk_container_remove(GTK_CONTAINER(ubox), mylabel2);
		mylabel2 = gtk_label_new (NULL);
		gtk_label_set_text (GTK_LABEL (mylabel2),"disabled PT");
		gtk_container_add(GTK_CONTAINER(ubox), mylabel2);
		gtk_widget_show (mylabel2);
	}
	gdk_window_process_all_updates();
	count=0;
	tprev=0;
	buttonprev=-1;
	bzero(nuovaparola,30);//nuovo
 }
 if(strcmp(buttonow,"→|")==0)
 {
	keycode = XK_Tab;
	sendtofocus(keycode);
	count=0;
	tprev=0;
	buttonprev=-1;
	bzero(nuovaparola,30);//nuovo
 }
}

void movetoleft()
{
 if(position==2)
 {
	gtk_window_set_gravity(GTK_WINDOW(window),GDK_GRAVITY_SOUTH_WEST);
 	gtk_window_move(GTK_WINDOW(window),0,s_height-WINDOW_HEIGHT);
	position=1;
	gdk_window_process_all_updates ();
 }
 if(position==3)
 {
 	gtk_window_set_gravity(GTK_WINDOW(window),GDK_GRAVITY_SOUTH);
 	gtk_window_move(GTK_WINDOW(window),(s_width/2)-WINDOW_WIDTH,s_height-WINDOW_HEIGHT);
	position=2;
	gdk_window_process_all_updates ();
 }
}

void movetoright()
{
 if(position==2)
 {
	gtk_window_set_gravity(GTK_WINDOW(window),GDK_GRAVITY_SOUTH_EAST);
 	gtk_window_move(GTK_WINDOW(window),s_width-185,s_height-WINDOW_HEIGHT);
	position=3;
	gdk_window_process_all_updates ();
 }
 if(position==1)
 {
 	gtk_window_set_gravity(GTK_WINDOW(window),GDK_GRAVITY_SOUTH);
 	gtk_window_move(GTK_WINDOW(window),(s_width/2)-WINDOW_WIDTH,s_height-WINDOW_HEIGHT);
	position=2;
	gdk_window_process_all_updates ();
 }
}

void dockicon()
{
	gtk_status_icon_set_visible(tray_icon, TRUE);
 	gtk_widget_hide(window);
}

void loadkeynormal(int keycodemap[10][5])
{
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

//PT algorithm based on list
void gestionet9 (int tasto)
{
	//initialization phase
	numparoletrovate=0;
	int i=0;
	for (i=0; i<N;i++)
	{
		vetparole[i].frequenza=0;
		bzero(vetparole[i].parola,30);
	}
	gtk_list_clear_items ((GtkList *) gtklist,0,N);

	//update
	luncodicet9=luncodicet9+1;
	sprintf(codicet9,"%s%d",codicet9,tasto);

	char query[200];
	bzero (query,200);
	//query
	sprintf (query, " select parola dist,frequenza from personale where codice like \'%s%%\' union select parola dist,frequenza from globale where 		codice like \'%s%%\' order by 2 desc, 1 asc limit 0,5;",codicet9,codicet9);
	GList *dlist=NULL;
	gchar *str;
	str = (gchar*)malloc(sizeof(gchar));
	sprintf(str,"");
	printf("\n%s\n",query);
	int  retval = sqlite3_prepare_v2(db,query,-1,&stmt,0);    
	if(retval)
	{
        	printf("\nerrore database\n");
        	return;
	}
        
	// Read the number of rows fetched
	int cols = sqlite3_column_count(stmt);
	    
	while(1)
	{
		// fetch a row's status
		retval = sqlite3_step(stmt);
		if(retval == SQLITE_DONE) break;
                else if(retval == SQLITE_ROW)
		{
			// SQLITE_ROW means fetched a row
		   	 numparoletrovate=numparoletrovate+1;
		    	printf ("\n");
		    	// sqlite3_column_text returns a const void* , typecast it to const char*
		    	for(int col=0 ; col<cols;col++)
		   	{
		        	const char *val = (const char*)sqlite3_column_text(stmt,col);
				if (col==0) 
				{
					printf ("%s",val);
					sprintf(vetparole[numparoletrovate-1].parola,"%s",val);
					list_item=gtk_list_item_new_with_label(val);
					dlist=g_list_append(dlist, list_item);
					gtk_widget_show(list_item);
					gtk_object_set_data(GTK_OBJECT(list_item), list_item_data_key,str);

				}
				else 
				{
					printf ("\tfr=%s",val);
					vetparole[numparoletrovate-1].frequenza=atoi(val);
				}
		   	 }  
		}
		else
		{
			//Some error encountered
		   	printf("errori query\n");
		    	return;
		}
	}
	indice=0;
	Display *display = XOpenDisplay(0);
	if(numparoletrovate> 0)
		gtk_list_append_items((GtkList*)(gtklist), dlist);



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++     
     





                else modalità inserimento nuova parola







++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/














	gdk_window_process_all_updates ();
	XCloseDisplay(display);
	printf ("\n");
}

//Convertion single character of new word to int
int calcola(char *carattere)
{
 int tmpcodicet9;
 const char *simbolo=carattere;
 if (strcmp(simbolo,"a")==0 || strcmp(simbolo,"b")==0 || strcmp(simbolo,"c")==0 || strcmp(simbolo,"à")==0 || strcmp(simbolo,"2")==0)
	tmpcodicet9=2;
 if (strcmp(simbolo,"d")==0 || strcmp(simbolo,"e")==0 || strcmp(simbolo,"f")==0 || strcmp(simbolo,"è")==0 || strcmp(simbolo,"3")==0)
	tmpcodicet9=3;
 if (strcmp(simbolo,"g")==0 || strcmp(simbolo,"h")==0 || strcmp(simbolo,"i")==0 || strcmp(simbolo,"ì")==0 || strcmp(simbolo,"4")==0)
	tmpcodicet9=4;
 if (strcmp(simbolo,"j")==0 || strcmp(simbolo,"k")==0 || strcmp(simbolo,"l")==0 || strcmp(simbolo,"5")==0 || strcmp(simbolo,"0")==0)
	tmpcodicet9=5;
 if (strcmp(simbolo,"m")==0 || strcmp(simbolo,"n")==0 || strcmp(simbolo,"o")==0 || strcmp(simbolo,"ò")==0 || strcmp(simbolo,"6")==0)
	tmpcodicet9=6;
 if (strcmp(simbolo,"p")==0 || strcmp(simbolo,"q")==0 || strcmp(simbolo,"r")==0 || strcmp(simbolo,"s")==0 || strcmp(simbolo,"7")==0)
	tmpcodicet9=7;
 if (strcmp(simbolo,"t")==0 || strcmp(simbolo,"u")==0 || strcmp(simbolo,"v")==0 || strcmp(simbolo,"ù")==0 || strcmp(simbolo,"8")==0)
	tmpcodicet9=8;
 if (strcmp(simbolo,"w")==0 || strcmp(simbolo,"x")==0 || strcmp(simbolo,"y")==0 || strcmp(simbolo,"z")==0 || strcmp(simbolo,"9")==0)
	tmpcodicet9=9;
 return tmpcodicet9;
}

//Update Dictionary
void updatedictionary()
{
	if(nuovaparola[0]=='\0')
		return;
	char query[200];
	bzero (query,200);

	//Check if the word is present on the global dictionary
	sprintf (query, " select * from  globale where parola like \'%s\';",nuovaparola);
	printf("\n%s\n",query);

	int  retval = sqlite3_prepare_v2(db,query,-1,&stmt,0);    
	if(retval)
	{
        	printf("\nerrore database\n");
        	return;
	}

	retval = sqlite3_step(stmt);
	
	if(retval != SQLITE_ROW)
	{
		bzero (query,200);
		int tmpcodicet9;
		char* carattere;
		lungparola=strlen(nuovaparola);
		for(int i=0;i<lungparola;i++){
			strncpy(carattere,&nuovaparola[i],1);
			tmpcodicet9=calcola(carattere);
			sprintf(codicet9,"%s%d",codicet9,tmpcodicet9);printf("codicet9= %s\n",codicet9);printf("tmp=%s\n",codicet9);
		}
		sprintf (query, "insert into globale (codice,parola,frequenza) values (\'%s\',\'%s\',0);",codicet9,nuovaparola);
		printf("\n%s\n",query);
		retval = sqlite3_exec(db,query,0,0,0);
		bzero (query,200);
		sprintf (query, "insert into personale (codice,parola,frequenza) values (\'%s\',\'%s\',1);",codicet9,nuovaparola);
		printf("\n%s\n",query);
		retval = sqlite3_exec(db,query,0,0,0);
		lungparola = 0;
 		bzero (nuovaparola,30);
 		bzero (codicet9,30);
	}
	gtk_container_remove(GTK_CONTAINER(box), mylabel);
	mylabel = gtk_label_new (NULL);
	gtk_label_set_text (GTK_LABEL (mylabel),"updated dictionary");
	gtk_container_add(GTK_CONTAINER(box), mylabel);
	gtk_widget_show (mylabel);
	gdk_window_process_all_updates ();
}

void sigh_print_selection(GtkWidget *gtklist, gpointer func_data)
{
 if (luncodicet9==0) return;

 GList   *dlist;
 GtkWidget *child;
    
 // Fetch the doubly linked list of selected items of the List, remember to treat this as read-only!
 dlist=GTK_LIST(gtklist)->selection;

 //We find element that has generated the signal and compute its position inside list
 child=gtk_container_get_focus_child(GTK_CONTAINER(gtklist));
 indice=gtk_list_child_position((GtkList *)gtklist,child);

 int dim_parola = strlen(vetparole[indice].parola);

 // If there are no selected items there is nothing more to do than just telling the user so
 if(!dlist){
	g_print("Selection cleared\n");
        return;
 }
 // Ok, we got a selection and so we print it
 for (int kk=0; kk < dim_parola; kk++) 
 {
	gchar word[dim_parola];
	sprintf(word,"%s",vetparole[indice].parola);

	gchar  *let;
	let = (gchar*)malloc(sizeof(gchar));
	sprintf(let,"");

	sprintf(let,"%c",word[kk]);

        sendtofocus(XStringToKeysym(let));
 }
 printf("\n");

 sendtofocus(XK_space);
 vetparole[indice].frequenza=vetparole[indice].frequenza+1;
        printf("\nNuova frequenza parola selezionata: %d\n",vetparole[indice].frequenza);

 char query[200];
 bzero (query,200);
 if (vetparole[indice].frequenza>1) 
	sprintf (query, "update personale set frequenza =%d where parola =\'%s\';",vetparole[indice].frequenza,vetparole[indice].parola);
 else 
	sprintf (query, "insert into personale (codice,parola,frequenza) values (\'%s\',\'%s\',1);",codicet9,vetparole[indice].parola);

 printf("\n%s\n",query);
 int  retval = sqlite3_exec(db,query,0,0,0);
 gtk_list_clear_items ((GtkList *) gtklist,0,N);
 luncodicet9 = 0;
 bzero(codicet9,30);
 gdk_window_process_all_updates ();
}

//PT thread reads dictionary file and fills list with PT terms
void *thfilet9 (void *arg)
{
	int rc;
	rc = sqlite3_open("parole.sqlite", &db);
        if (rc)
 	{
 	       printf("\nerrore database del T9");
	       exit(EXIT_FAILURE);
        }
        else
	{
		printf("\nConnessione al database del T9 avvenuta correttamente\n");
		flagcaricat9=1;
	}	
	fflush(stdout);	
}

int main(int argc, char *argv[])
{
 GtkWidget *frame, *framet9, *winbox, *cbox, *table, *ubutton;//Nuovi
 GdkScreen *screen;
 GtkWidget **buttonArray;

 int i, x, y;
 char s[NUM_OF_BUTTONS][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","←","0 +","↵","⇧","] [","ctrl","→|","tx pr","←","→","."};
/*Definiamo vettore che contiene gli indici delle posizioni possibili dell'interfaccia
  GDK_GRAVITY_SOUTH_WEST is the reference point corresponding at the lower left corner -> position 1
  GDK_GRAVITY_SOUTH is the reference point corresponding at the middle of the lower edge -> position 2
  GDK_GRAVITY_SOUTH_EAST is the reference point corresponding at the lower right corner -> position 3
*/
 position=3; 
 int tasto1norm[10][5];
 loadkeynormal(tasto1norm);

 //Create thread for t9
 int rest;
 pthread_t t9_thread;
 flagcaricat9=0;
 rest = pthread_create(&t9_thread, NULL, thfilet9, NULL);
 if (rest != 0) 
 {
	printf("\nerrore partenza thread del T9");
	exit(EXIT_FAILURE);
 }
 luncodicet9=0;
 statot9=0;

 gtk_init (&argc, &argv);

 /* Create window */
 window = gtk_window_new (GTK_WINDOW_POPUP);
 gtk_window_set_default_size (GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
 /* Set window position */
 screen=gtk_window_get_screen(GTK_WINDOW(window));
 gtk_window_set_gravity(GTK_WINDOW(window),GDK_GRAVITY_SOUTH_EAST);
 s_width=gdk_screen_get_width(screen);
 s_height=gdk_screen_get_height(screen);
 gtk_window_move(GTK_WINDOW(window),s_width-185,s_height-WINDOW_HEIGHT);

 winbox = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (window), winbox);
 gtk_widget_show (winbox);

 /* Create Frame */
 frame = gtk_frame_new(NULL);
 gtk_widget_set_usize(frame, 0, 95);
 gtk_container_add(GTK_CONTAINER(winbox), frame);
 /* Set label of the Frame */
 gtk_frame_set_label( GTK_FRAME(frame), "Keyboard" );
 gtk_widget_show (frame);

 box = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (frame), box);
 gtk_widget_show (box);

 buttonArray = (GtkWidget**)g_malloc(NUM_OF_BUTTONS*sizeof(GtkButton**));

 table = gtk_table_new(NUM_OF_ROWS, NUM_OF_COLUMNS, TRUE);
 gtk_table_set_row_spacings(GTK_TABLE(table), 0);
 gtk_table_set_col_spacings(GTK_TABLE(table), 0);
 x=y=0;
 
 for(i=0; i<19;i++)
 {
   buttonArray[i] = gtk_button_new_with_label(s[i]);
   if(i==17){
	gtk_widget_set_name(buttonArray[i],s[i]);
	//Insert box in order to define displacement buttons
	cbox = gtk_hbox_new (FALSE, 0);
	gtk_table_attach_defaults(GTK_TABLE(table), cbox, x, x+1, y, y+1);
	gtk_widget_set_size_request(cbox,60,25);
	gtk_widget_set_size_request(buttonArray[i],23,0);
	gtk_container_add(GTK_CONTAINER (cbox), buttonArray[i]);
	g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(movetoleft), NULL);
	buttonArray[i+1] = gtk_button_new_with_label(s[i+1]);
        gtk_widget_set_name(buttonArray[i+1],s[i+1]);
	gtk_widget_set_size_request(buttonArray[i+1],23,0);
	gtk_container_add(GTK_CONTAINER (cbox), buttonArray[i+1]);
	g_signal_connect(G_OBJECT(buttonArray[i+1]), "clicked", G_CALLBACK(movetoright), NULL);
	buttonArray[i+2] = gtk_button_new_with_label(s[i+2]);
        gtk_widget_set_name(buttonArray[i+2],s[i+2]);
	gtk_widget_set_size_request(buttonArray[i+2],18,0);
	gtk_container_add(GTK_CONTAINER (cbox), buttonArray[i+2]);
	g_signal_connect(G_OBJECT(buttonArray[i+2]), "clicked", G_CALLBACK(dockicon), NULL);
   }
   else{
    	gtk_table_attach_defaults(GTK_TABLE(table), buttonArray[i], x, x+1, y, y+1);
    	gtk_widget_set_name(buttonArray[i],s[i]);
    	gtk_widget_set_size_request(buttonArray[i],60,25);
    	g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked), tasto1norm);
    	x++;
    	if(x==NUM_OF_COLUMNS)
    	{
    	    x=0;
    	    y++;
    	}
    }
 }
 gtk_box_pack_start(GTK_BOX(box), table, FALSE, FALSE, 0);

 /* Create label for Shift and Ctrl buttons */
 mylabel = gtk_label_new (NULL);
 gtk_container_add(GTK_CONTAINER(box), mylabel);
 gtk_widget_show (mylabel);

/* Create FrameT9 */
 framet9 = gtk_frame_new(NULL);
 gtk_widget_set_usize(framet9, 0, 50);
 gtk_container_add(GTK_CONTAINER(winbox), framet9);
 /* Set label of the FrameT9 */
 gtk_frame_set_label( GTK_FRAME(framet9), "PT" );
 gtk_widget_show (framet9);

 tbox = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (framet9), tbox);
 gtk_widget_show (tbox);

/* Create PT list */
 gtklist=gtk_list_new();
 gtk_list_set_selection_mode((GtkList *) gtklist, GTK_SELECTION_SINGLE);
 gtk_widget_set_usize(gtklist, 0, 100);
 gtk_container_add(GTK_CONTAINER(tbox), gtklist);
 gtk_widget_show(gtklist);
 gtk_signal_connect(GTK_OBJECT(gtklist),"selection_changed",GTK_SIGNAL_FUNC(sigh_print_selection),NULL);//nuovo x lista

/* Create container consisting of a label and a PT management button */	//NUOVO
 ubox = gtk_hbox_new (FALSE, 0);
 gtk_widget_set_usize(ubox, 0, 1);
 gtk_container_add (GTK_CONTAINER (tbox), ubox);
 gtk_widget_show (ubox);

/* Create PT button */					//NUOVO
 ubutton = gtk_button_new_with_label("↓");
 gtk_widget_set_name(ubutton,"↓");
 gtk_container_add(GTK_CONTAINER (ubox), ubutton);
 g_signal_connect(G_OBJECT(ubutton), "clicked", G_CALLBACK(updatedictionary), NULL);

/* Create PT Label */
 mylabel2 = gtk_label_new (NULL);
 gtk_label_set_text (GTK_LABEL (mylabel2),"disabled PT");
 gtk_container_add(GTK_CONTAINER(ubox), mylabel2);
 gtk_widget_show (mylabel2);

 gtk_widget_show_all(window);

/* Create status icon for minimizing virtual keyboard */
 tray_icon = gtk_status_icon_new();
 g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(tray_icon_on_click), NULL);
 g_signal_connect(G_OBJECT(tray_icon),"popup-menu",G_CALLBACK(tray_icon_on_menu), NULL);
 gtk_status_icon_set_from_icon_name(tray_icon,GTK_STOCK_MEDIA_STOP);
 gtk_status_icon_set_from_file(tray_icon,"keyboard_icon.gif");
 gtk_status_icon_set_tooltip(tray_icon,"Keyboard Tray Icon");
 gtk_status_icon_set_visible(tray_icon, FALSE);

 gtk_main ();
 return 0;
}
