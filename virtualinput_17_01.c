/*virtualinput aggiornato 17fff/01/2011*/ 
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>//for bzero

#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 4
#define NUM_OF_BUTTONS 21
#define NameProgramm "virtualKey"
#define LENB 5  /*LENB=LENB2,LENB3,LENB4,LENB6,LENB7,LENB8,LENB9*/
#define LENB1 8
#define LENB5 4
#define LENB10 20
#define N_SIMBOL_MODIFIER 128 /*numero di simboli a cui e' associato un modifier*/
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 550



#define N 5
#define list_item_data_key "list_item_data"
struct nodo {
  int frequenza;
  char parola[30];
};


/*struttura che contiene tutti gli elementi per la gestione del t9 sviluppata da :     */
typedef struct text_prediction{
//Nuovi elementi per la lista t9
GtkWidget *gtklist;
GtkWidget *list_item,*mylabel, *mylabel2;
int statot9;
int indice;
char codicet9[30];
int luncodicet9;
int flagcaricat9;
int numparoletrovate;	//numero parole trovate dalla predizione del t9
struct nodo vetparole[N];
sqlite3 *db;
sqlite3_stmt *stmt;
}T_tp;



typedef struct Tlbutton{
 int len;
 int status;
 int *simbol;
 time_t timeSelected;
}lbutton;

typedef struct tstruttura{
 Display *display;
 Window win;
 Window winRoot;
 int modifier;
 lbutton listButton[11];//listButton[9] non viene utilizza /viene posta a null per semplificazioni
 int indexSimbol;/*rappresenta l'indice del listButton(generico) corrispondente all'ultimo carattere stampato*/  
 T_tp tp;
 //Gestione finestra
 GtkWidget *window,*mylabel,*mylabel2; 
 GtkStatusIcon *tray_icon;
 int s_width, s_height,position_height,position_width,delay_verticale,delay_orizzontale;
 
}struttura;
 














void gestionet9 (struttura *str,int tasto);
void sigh_print_selection(GtkWidget *gtklist,struttura *str);
void sendtofocus(struttura *str,int keycode);


 
void fButtonShift(struttura *str);
void fButtonAltGR(struttura *str);
void fButtonDelete(struttura *str);
void fButtonSpace(struttura *str);
void fButtonEnter(struttura *str);
void fButtonTP(struttura *str);
void fButtonTab(struttura *str);
void fButtonIns(struttura *str);



void movetoleft(struttura *str);
void movetoup(struttura *str);
void movetoright(struttura *str);
void dockicon(struttura *str);
void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str);
void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button,guint activate_time, struttura *str);
void getMaxScreen(struttura *str);

void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);

typedef void(*funz)(struttura *);
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);
funz listF_Buttons[NUM_OF_BUTTONS-10]={fButtonShift,fButtonAltGR,fButtonDelete,fButtonSpace,fButtonEnter,fButtonTP,fButtonTab,dockicon,movetoleft,movetoup,movetoright};

void setModifier(struttura *str,int modifier,const gchar *s_label)
{
  str->modifier=modifier;
  gtk_label_set_text (GTK_LABEL (str->mylabel),s_label);
 
}
void fButtonShift(struttura *str)
/*Funzione associata al tasto shift => setta lo shift */
{
 if(str->modifier!=XK_ISO_Level5_Shift)
  setModifier(str,XK_ISO_Level5_Shift,"shift on");
 else
  setModifier(str,0,"");
}


void fButtonAltGR(struttura *str)
/*Funzione associata al tasto 'AltGr' */
{
 if (str->modifier!=XK_Control_R)
  setModifier(str,XK_Control_R,"alt gr on");
 else
  setModifier(str,0,"");
}

void fButtonDelete(struttura *str)
/*Funzione associata al tasto delete */
{
 sendKey(str->display,str->win,str->winRoot,XK_BackSpace,0);
}
void fButtonSpace(struttura *str)
/*Funzione associata al tasto '*' */
{
  sendKey(str->display,str->win,str->winRoot,XK_space,0);
}
void fButtonEnter(struttura *str)
/*Funzione associata al tasto invio */
{
  sendKey(str->display,str->win,str->winRoot,XK_KP_Enter,0);
}

void fButtonTab(struttura *str)
/*Funzione associata al tasto tab */
{
  sendKey(str->display,str->win,str->winRoot,XK_Tab,0);
}
void fButtonTP(struttura *str)
/*Funzione associata al tasto TP */
{
 if (str->tp.statot9==0)
  {
   str->tp.statot9=1;
   str->tp.indice=0;
   str->tp.luncodicet9=0;
   str->tp.codicet9[0]=0;
   gtk_label_set_text (GTK_LABEL (str->mylabel2),"txt pr on");
  }
 else
    str->tp.statot9=0;
 
}
void fButtonIns(struttura *str)
/*Funzione associata al tasto inseriemento nuova parola */
{
  sendKey(str->display,str->win,str->winRoot,XK_Tab,0);
}

void setMove(struttura *str)
/*funzione di semplice supporto alle funzioni move*/
{
 gtk_window_move(GTK_WINDOW(str->window),str->position_width,str->position_height);
 gdk_window_process_all_updates ();
}

void movetoleft(struttura *str)
{
 if (str->position_width>0)
 {	
   str->position_width-=str->delay_orizzontale;
   setMove(str);
 }
 printf("%d/n",str->position_width);
}

void movetoup(struttura *str)
{ 
  str->position_height=(str->position_height==str->delay_verticale*2)?0:str->position_height+str->delay_verticale;
  setMove(str);
}



void movetoright(struttura *str)
{
 if (str->position_width<str->delay_orizzontale*2)	
 {
   str->position_width+=str->delay_orizzontale;  
   setMove(str);
 }
  g_print("posizione%d/n",str->position_width);
}

void dockicon(struttura *str)
{
	gtk_status_icon_set_visible(str->tray_icon, TRUE);
 	gtk_widget_hide(str->window);
}

void getMaxScreen(struttura *str)
/*Questa funzione le variabili S_width e s_height nel corrispettivo valore massimo di larghezza e altezza dello schermo*/
/*Nota: al momento la funzione viene richiamata una sola volta nel main però è utile capire come è possibile intercettare
  un cambiamento delle dimensioni dello schermo,in questi casi la funzione dovrebbe essere nuovamente richiamata*/
{
 GdkScreen* screen = NULL;
 screen = gtk_window_get_screen(GTK_WINDOW(str->window));
 str->s_width = gdk_screen_get_width(screen);
 str->s_height = gdk_screen_get_height(screen);
 str->delay_verticale=(str->s_height-WINDOW_HEIGHT)/2;
 str->delay_orizzontale=(str->s_width-WINDOW_WIDTH)/2;
 printf("lar %d len%d delay verticale %d\n",str->s_width,str->s_height,str->delay_orizzontale); 
}
/*funzione che notifica pressione e rilascio si un dato tasto ad un'applicazione*/
void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state)
{
 SendKeyEvent(display,win,winRoot,KeyPress,keycode,state );
 SendKeyEvent(display,win,winRoot,KeyRelease,keycode,state);
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
 XFlush(display);//forza l'effettiva scrittura degli eventuali dati presenti nel buffer,in questo caso il display 
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


void writeSimbol(struttura *str,lbutton *listSibolOnButton)
{
 time_t tnow;
 double t_diff;
 tnow=time(NULL);
 t_diff=difftime(tnow,listSibolOnButton->timeSelected);
 listSibolOnButton->timeSelected=tnow;
 if(!((t_diff>=0)&&(t_diff<=1.5)))
   str->indexSimbol=0;
 else
 {
   str->indexSimbol=(str->indexSimbol==listSibolOnButton->len-1)?0:str->indexSimbol+1;   
   sendKey(str->display,str->win,str->winRoot,XK_BackSpace,0);
   //g_print("%c",XK_BackSpace);
 }
 //g_print("%c",listButton->simbol[str->indexSimbol]);
 sendKey(str->display,str->win,str->winRoot,listSibolOnButton->simbol[str->indexSimbol],listSibolOnButton->status+ str->modifier);
}

//gtk_get_name_button(widget);
void button_clicked(GtkWidget *widget,struttura *str)
{ 
 // Find the window which has the current keyboard focus.
 int revert;
 int button;
 button=atoi(gtk_widget_get_name(widget));
 if((button!=9)&&(button<11)){//&!T9
   if (str->tp.statot9==0){
     XGetInputFocus(str->display, &str->win, &revert);
     writeSimbol(str,&str->listButton[button]);
    }
   else 
    {
     if (str->tp.luncodicet9==0)//solo alla scelta della prima lettera che comporra la parola predetta
       XGetInputFocus(str->display, &str->win, &revert);
     gestionet9 (str,(button==10)?0:button+1); 
    }  
 }
 else
   listF_Buttons[((button==9)?0:button-10)](str);
}




void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str)
{
 printf("Clicked on tray icon\n");
 gtk_widget_show(str->window);
 gtk_status_icon_set_visible(str->tray_icon, FALSE);
}

void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button,guint activate_time, struttura *str)
{
 printf("Popup menu\n");
}
void finestra(struttura *str)
{
 GtkWidget *frame, *framet9, *winbox, *box,*cbox, *tbox, *table, *scrolled_window;//nuovo x lista

 GtkWidget **buttonArray;

 int i, x, y;
 char s_i[3];
 /*Definiamo vettore che contiene gli indici delle posizioni possibili dell'interfaccia*/
 char s[NUM_OF_BUTTONS][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","⇧ shift","0 +","alt gr","←","space","↵","txt pr","tab",".","←","↓","→"};
 /* GDK_GRAVITY_SOUTH_WEST is the reference point corresponding at the lower left corner -> position 1
  GDK_GRAVITY_SOUTH is the reference point corresponding at the middle of the lower edge -> position 2
  GDK_GRAVITY_SOUTH_EAST is the reference point corresponding at the lower right corner -> position 3
*/
 gtk_init (NULL,NULL);
 str->window = gtk_window_new (GTK_WINDOW_POPUP);
 gtk_window_set_default_size (GTK_WINDOW(str->window),  WINDOW_WIDTH, WINDOW_HEIGHT);
 
 winbox = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (str->window), winbox);
 gtk_widget_show (winbox);

 /* Crea una Frame */
 frame = gtk_frame_new(NULL);
 gtk_container_add(GTK_CONTAINER(winbox), frame);
 /* Setta l'etichetta della frame */
 gtk_frame_set_label( GTK_FRAME(frame), "Tastiera" );
 gtk_widget_show (frame);

 box = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (frame), box);
 gtk_widget_show (box);

 buttonArray = (GtkWidget**)g_malloc((NUM_OF_BUTTONS)*sizeof(GtkButton**));

 table = gtk_table_new(NUM_OF_ROWS, NUM_OF_COLUMNS, TRUE);
 gtk_table_set_row_spacings(GTK_TABLE(table), 1);
 gtk_table_set_col_spacings(GTK_TABLE(table), 1);
 x=y=0; 
 for(i=0; i<NUM_OF_BUTTONS;i++)
 {  
    buttonArray[i] = gtk_button_new_with_label(s[i]);
    gtk_widget_set_size_request(buttonArray[i],60,50);
    gtk_table_attach_defaults(GTK_TABLE(table), buttonArray[i], x, x+1, y, y+1);
    sprintf(s_i,"%i",i);
    gtk_widget_set_name(buttonArray[i],s_i);
    g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked),str);
    if(++x==NUM_OF_COLUMNS)
     {
      x=0;
      y++;
     }
  }

 gtk_box_pack_start(GTK_BOX(box), table, FALSE, FALSE, 0);

 /* Crea una Label per i tasti Shift e Ctrl */
 str->mylabel = gtk_label_new (NULL);
 gtk_container_add(GTK_CONTAINER(box), str->mylabel);
 gtk_widget_show (str->mylabel);

 /* Crea una FrameT9 */
 framet9 = gtk_frame_new(NULL);
 gtk_container_add(GTK_CONTAINER(winbox), framet9);
 
/* Setta l'etichetta della frame */
 gtk_frame_set_label( GTK_FRAME(framet9), "txt prediction" );
 gtk_widget_show (framet9);

 tbox = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (framet9), tbox);
 gtk_widget_show (tbox);
 
 /* Crea una Lista per il T9 */
 str->tp.gtklist=gtk_list_new();
 gtk_list_set_selection_mode((GtkList *)str->tp.gtklist, GTK_SELECTION_SINGLE);
 gtk_widget_set_usize(str->tp.gtklist, 0, 100);
 gtk_container_add(GTK_CONTAINER(tbox), str->tp.gtklist);
 gtk_widget_show(str->tp.gtklist);
 gtk_signal_connect(GTK_OBJECT(str->tp.gtklist),"selection_changed",GTK_SIGNAL_FUNC(sigh_print_selection),str);//nuovo x lista
 

 
 
 /* Crea una Label per il T9 */
 str->mylabel2 = gtk_label_new (NULL);
 gtk_label_set_text (GTK_LABEL (str->mylabel2),"txt pr disattivato");
 gtk_widget_set_usize(str->mylabel2, 0, 1);
 gtk_container_add(GTK_CONTAINER(tbox),str->mylabel2);
 gtk_widget_show (str->mylabel2);

 /* Crea uno status icon per la minimizzazione */
 str->tray_icon = gtk_status_icon_new();
 g_signal_connect(G_OBJECT(str->tray_icon), "activate",G_CALLBACK(tray_icon_on_click), str);
 g_signal_connect(G_OBJECT(str->tray_icon),"popup-menu",G_CALLBACK(tray_icon_on_menu), str);
 gtk_status_icon_set_from_icon_name(str->tray_icon,GTK_STOCK_MEDIA_STOP);
 gtk_status_icon_set_from_file(str->tray_icon,"keyboard_icon.gif");
 gtk_status_icon_set_tooltip(str->tray_icon,"Keyboard Tray Icon");
 gtk_status_icon_set_visible(str->tray_icon, FALSE);

 gtk_widget_show_all(str->window);
 /*salviamo le dim massime dello schermo corrente*/
 getMaxScreen(str);
 
 gtk_main ();
}


void sendtofocus(struttura *str,int keycode)
/* funzione introdotta per compatibilità al t9*/
{
 sendKey(str->display,str->win,str->winRoot,keycode,str->modifier);
}
//Algoritmo T9 di predizione del testo basato su liste
void gestionet9 (struttura *str,int tasto)
{
	//fase inizializzazione
	str->tp.numparoletrovate=0;
	int i=0;
	for (i=0; i<N;i++)
	{
		str->tp.vetparole[i].frequenza=0;
		bzero(str->tp.vetparole[i].parola,30);
	}
	gtk_list_clear_items ((GtkList *) str->tp.gtklist,0,N);

	//aggiornamento
	str->tp.luncodicet9=str->tp.luncodicet9+1;
	sprintf(str->tp.codicet9,"%s%d",str->tp.codicet9,tasto);
	//printf("\nTasti premuti: %s\tlunghezza:%d\n",str->tp.codicet9,str->tp.luncodicet9);
	char query[200];
	bzero (query,200);
	//se freq0 globale, se 1 pers
	sprintf (query, " select parola dist,frequenza from personale where codice like \'%s%%\' union select parola dist,frequenza from globale where 		codice like \'%s%%\' order by 2 desc, 1 asc limit 0,5;",str->tp.codicet9,str->tp.codicet9);
  	//GtkWidget *list_item;
	GList *dlist=NULL;
	gchar *s;
	s = (gchar*)malloc(sizeof(gchar));
	sprintf(s,"");
	printf("\n%s\n",query);
	int  retval = sqlite3_prepare_v2(str->tp.db,query,-1,&str->tp.stmt,0);    
	if(retval)
	{
        	printf("\nerrore database\n");
        	return;
	}
        
	// Read the number of rows fetched
	int cols = sqlite3_column_count(str->tp.stmt);
	    
	while(1)
	{
		// fetch a row's status
		retval = sqlite3_step(str->tp.stmt);
		if(retval == SQLITE_DONE) break;
                else if(retval == SQLITE_ROW)
		{
			// SQLITE_ROW means fetched a row
		   	 str->tp.numparoletrovate=str->tp.numparoletrovate+1;
		    	printf ("\n");
		    	// sqlite3_column_text returns a const void* , typecast it to const char*
		    	for(int col=0 ; col<cols;col++)
		   	{
		        	const char *val = (const char*)sqlite3_column_text(str->tp.stmt,col);
		        //printf("%s = %s\t",sqlite3_column_name(str->tp.stmt,col),val);
				if (col==0) 
				{
					printf ("%s",val);
					sprintf(str->tp.vetparole[str->tp.numparoletrovate-1].parola,"%s",val);
					str->tp.list_item=gtk_list_item_new_with_label(val);
					dlist=g_list_append(dlist, str->tp.list_item);
					gtk_widget_show(str->tp.list_item);
					gtk_object_set_data(GTK_OBJECT(str->tp.list_item), list_item_data_key,s);

				}
				else 
				{
					printf ("\tfr=%s",val);
					str->tp.vetparole[str->tp.numparoletrovate-1].frequenza=atoi(val);
				}
		   	 }  
		}
		else
		{
			// Some error encountered
		   	 printf("errori query\n");
		    	return;
		}
	}
	str->tp.indice=0;
	Display *display = XOpenDisplay(0);
	if(str->tp.numparoletrovate> 0)
	{ 
		gtk_list_append_items((GtkList*)(str->tp.gtklist), dlist);
		
			
	}
		gdk_window_process_all_updates ();
		XCloseDisplay(display);
	    printf ("\n");
	
}

void sigh_print_selection(GtkWidget *gtklist, struttura *str)
{
 if (str->tp.luncodicet9==0) return;

 GList   *dlist;
 GtkWidget *child;
 
 
 // Fetch the doubly linked list of selected items of the List, remember to treat this as read-only!
 g_print("Selection\n");
 dlist=GTK_LIST(gtklist)->selection;
 g_print("Selection 1\n");
 // Individuiamo l'elemento che ha emesso il segnale e calcoliamo la sua posizione all'interno della lista
 child=gtk_container_get_focus_child(GTK_CONTAINER(gtklist));
 str->tp.indice=gtk_list_child_position((GtkList *)gtklist,child);
 g_print("Selection 2\n");
 int dim_parola = strlen(str->tp.vetparole[str->tp.indice].parola);
  g_print("Selection 3\n");
 // If there are no selected items there is nothing more to do than just telling the user so
 if(!dlist){
	g_print("Selection cleared\n");
        return;
 }
   g_print("Selection 4\n");
 // Ok, we got a selection and so we print it
 for (int kk=0; kk < dim_parola; kk++) 
 {
	gchar word[dim_parola];
	sprintf(word,"%s",str->tp.vetparole[str->tp.indice].parola);

	gchar  *let;
	let = (gchar*)malloc(sizeof(gchar));
	sprintf(let,"");

	sprintf(let,"%c",word[kk]);
        //sendKey(str->display,str->win,str->winRoot,XStringToKeysym(let),str->modifier);

        sendtofocus(str,XStringToKeysym(let));
 }
 printf("\n");

 g_print("Selection 5\n");

 sendtofocus(str,XK_space);
 str->tp.vetparole[str->tp.indice].frequenza=str->tp.vetparole[str->tp.indice].frequenza+1;
        printf("\nNuova frequenza parola selezionata: %d\n",str->tp.vetparole[str->tp.indice].frequenza);

 char query[200];
 bzero (query,200);
 if (str->tp.vetparole[str->tp.indice].frequenza>1) 
	sprintf (query, "update personale set frequenza =%d where parola =\'%s\';",str->tp.vetparole[str->tp.indice].frequenza,str->tp.vetparole[str->tp.indice].parola);
 else 
	sprintf (query, "insert into personale (codice,parola,frequenza) values (\'%s\',\'%s\',1);",str->tp.codicet9,str->tp.vetparole[str->tp.indice].parola);

 printf("\n%s\n",query);
 int  retval = retval = sqlite3_exec(str->tp.db,query,0,0,0);
 gtk_list_clear_items ((GtkList *) gtklist,0,N);
 str->tp.luncodicet9 = 0;
 bzero(str->tp.codicet9,30);
		//XWarpPointer(display, None, None, 0, 0, 0, 0, -10000,-10000);
		//XWarpPointer(display, None, None, 0, 0, 0, 0, 90, 1);
 gdk_window_process_all_updates ();
	//XCloseDisplay(display);
}

//Funzione thread che legge il file dizionazrio e riempe la lista per il T9
void *thfilet9 (void *arg)
{
	int rc;
	struttura *str=(struttura *)arg;
        rc = sqlite3_open("parole.sqlite", &str->tp.db);
        if (rc)
 	{
 	       printf("\nerrore database del T9");
	       exit(EXIT_FAILURE);
        }
        else
	{
		printf("\nConnessione al database del T9 avvenuta correttamente\n");
		str->tp.flagcaricat9=1;
	}	
	fflush(stdout);	
}


int *setListButton(int len,int *vet,lbutton &l,int status)
{
 l.len=len;
 l.status=status;
 l.timeSelected=(time_t)DBL_MAX;
 return vet;
}


int main(int argc, char *argv[])
{
 struttura str;
 str.display = XOpenDisplay(0);
 int i;
 int keycodemap[LENB*7+LENB1+LENB5+LENB10]={XK_1,XK_backslash,XK_comma,XK_plus,XK_minus,XK_less,XK_quoteright,XK_period,
                                          /*;1;\;,;+;-;<;';.;*/
                                          XK_a,XK_b,XK_c,XK_agrave,XK_2,/* a,b,c,à,2*/
                                          XK_d,XK_e,XK_f,XK_egrave,XK_3,/* d,e,f,è,3*/
                                          XK_g,XK_h,XK_i,XK_igrave,XK_4,/* g,h,i,ì,4*/
                                          XK_j,XK_k,XK_l,XK_5,                /* j,j,l,5 */
                                          XK_m,XK_n,XK_o,XK_ograve,XK_6,/* m,n,o,ò,6*/
                                          XK_p,XK_q,XK_r,XK_s,XK_7,     /* p,q,r,s,7*/
                                          XK_t,XK_u,XK_v,XK_ugrave,XK_8,/* t,u,v,ù,8*/
                                          XK_w,XK_x,XK_y,XK_z,XK_9,     /* w,x,y,z,9*/
                                          XK_space/* */,XK_0/*0*/,XK_question/*?*/,XK_exclam/*!*/,XK_equal/*=*/,XK_asterisk/* * */,XK_semicolon/*;*/,XK_colon/*:*/,XK_underscore/*_*/,XK_2/*"*/,XK_3/*£*/,XK_dollar/*$*/,XK_percent/*%*/,XK_ampersand/*&*/,XK_slash/*/*/,XK_bar/*|*/,XK_parenleft/*(*/,XK_parenright/*)*/,XK_asciicircum/*^*/,XK_greater/*>*/};
                        
 int sup; 
 str.indexSimbol=0;//semplice inizzializzazione
 str.modifier=0;//semplice inizzializzazione
 str.listButton[0].simbol=setListButton(LENB1,keycodemap,str.listButton[0],0);
 for(i=1,sup=LENB1;i<4;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i],0);
 str.listButton[4].simbol=setListButton(LENB5,keycodemap+sup,str.listButton[4],0);
 for(i++,sup+=LENB5;i<9;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i],0);
 str.listButton[9].simbol=NULL;
 str.listButton[10].simbol=setListButton(LENB10,keycodemap+sup,str.listButton[10],XK_ISO_Level5_Shift);
 
 //t9
 int rest;
 pthread_t t9_thread;
 str.tp.flagcaricat9=0;
 rest = pthread_create(&t9_thread, NULL, thfilet9,(void *)&str);     
 if (rest != 0) 
 {
	printf("\nerrore partenza thread del T9");
	exit(EXIT_FAILURE);
 }
 str.tp.luncodicet9=0;
 str.tp.statot9=0;
 str.tp.codicet9[0]=0;
 str.position_height=0;
 str.position_width=0;
 // Get the root window for the current display.
 str.winRoot = XDefaultRootWindow(str.display);
 finestra(&str);
 XCloseDisplay(str.display);
 return 0;
}
 
