/*virtualinput aggiornato 28/01/2011*/ 
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>//for bzero

#define NUM_OF_COLUMNS 3                    /* number columns of buttons*/ 
#define NUM_OF_ROWS 8                       /* number rows of buttons*/
#define NUM_OF_BUTTONS 24                   /*the total number of buttons*/ 
#define NameProgramm "virtualinput"         /*the name of the program */
#define LENB 5                              /*the number of symbols in the buttons 2,3,4,5,6,7,8,9*/ 
                                            /*LENB=LENB2=LENB3=LENB4=LENB6=LENB7=LENB8=LENB9*/
#define LENB1 8                             /*the number of symbols in the button 1*/
#define LENB5 4                             /*the number of symbols in the button 5*/
#define LENB0 19                            /*the number of symbols in the button 0*/
#define WINDOW_WIDTH 200                    /*width of main window*/
#define WINDOW_HEIGHT 420                   /*heigth of main window*/
#define S_W_MAIN 30                         /*heigth of scrolled_window in TP off*/  
                                            //il valore di scrolled window quando il tp è disabilitato

#define LEN_WORD_DB 20                      /* len of words saved on database */
#define N 5                                 /* number of element showed in the list of word predected*/

#define list_item_data_key "list_item_data" /*key for identicitazion list item insert*/ 

#define PATH_DB "/usr/share/virtualinput/virtualinput-0.1.2beta/parole.sqlite"
#define PATH_IMG "/usr/share/virtualinput/virtualinput-0.1.2beta/keyboard_icon.gif"

struct nodo {
  int frequenza;
  char parola[LEN_WORD_DB+1];
};


/*struttura che contiene tutti gli elementi per la gestione del t9 sviluppata da :     */
typedef struct text_prediction{
//Nuovi elementi per la lista t9
GtkWidget *gtklist;
GtkWidget *list_item, *scrolled_window;
int statot9;
int indice;
char codicet9[LEN_WORD_DB+1];
int luncodicet9;
int flagcaricat9;
int numparoletrovate;	//numero parole trovate dalla predizione del t9
struct nodo vetparole[N];
sqlite3 *db;
sqlite3_stmt *stmt;
}T_tp;



typedef struct Tlbutton{
 int len;//lunghezza caratteri associati
 int status;//modificatore associato alla lista di caratteri associati al pulsante
 int *simbol;//lista di caratteri associati al carattere
 time_t timeSelected;//l'ultima volta in cui il pulsante è stato cliccato
}lbutton;

typedef struct tinsert{ /*la struttura contiene i dati che servono neel caso di un nuovo inserimento nel dizionario*/
 int on;//modalità inserimento attiva
 int index;//indice 
 char key_numbers[LEN_WORD_DB+1];//numeri associati per il t9 alla nuova parola
 char new_word[LEN_WORD_DB+1];
}tins;

typedef struct tForMove{/*la struttura contiene i dati che servono nel caso di spostamento della finestra*/
int s_width, s_height,position_height,position_width,delay_verticale,delay_orizzontale;
}tmove;

typedef struct tstruttura{
 Display *display;
 Window win;
 Window winRoot;
 int modifier;
 int specialKey;
 lbutton listButton[10];
 int indexSimbol;/*rappresenta l'indice del listButton(generico) corrispondente all'ultimo carattere stampato*/  
 T_tp tp;
 //Gestione finestra
 GtkWidget *window,*mylabel,*mylabel2; 
 GtkStatusIcon *tray_icon;
 tmove move; 
 tins ins;
}struttura;
 










void finestra(struttura *str);



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
void fButtonAlt(struttura *str);
void fButtonCtrl(struttura *str);



void movetodown(struttura *str);
void movetoright(struttura *str);
void movetoleft(struttura *str);
void dockicon(struttura *str);
void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str);
void getMaxScreen(struttura *str);
void setMove(struttura *str);

void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);

void ins(struttura *str);
void updatedictionary(struttura *str);
void window_insert(struttura *str);
void button_pressed_insert(GtkWidget *widget,struttura *str);

void writeSimbol(struttura *str,lbutton *listSibolOnButton,int number);
int *setListButton(int len,int *vet,lbutton &l,int status);

void setModifier(struttura *str,int modifier,const gchar *s_label);
void printOnFocus(struttura *str,int key,int modifier);

void *thfilet9 (void *arg);



typedef void(*funz)(struttura *);

funz listF_Buttons[NUM_OF_BUTTONS-10]={fButtonShift,fButtonAltGR,fButtonDelete,fButtonSpace,fButtonEnter,fButtonTP,fButtonTab,fButtonIns,fButtonAlt,fButtonCtrl,dockicon,movetoleft,movetodown,movetoright};

void setModifier(struttura *str,int modifier,const gchar *s_label)
{
  str->modifier=modifier;
  gtk_label_set_text (GTK_LABEL (str->mylabel),s_label);
 
}
void fButtonShift(struttura *str)
/*Funzione associata al tasto shift => setta lo shift */
{
  if(str->modifier!=XK_ISO_Level5_Shift)
   setModifier(str,XK_ISO_Level5_Shift,"Shift on");
  else
   setModifier(str,0,"");
}


void fButtonAltGR(struttura *str)
/*Funzione associata al tasto 'AltGr' */
{
  if (str->modifier!=XK_Control_R)
   setModifier(str,XK_Control_R,"Alt Gr on");
  else
   setModifier(str,0,"");
}

void setSpecialButton(struttura *str,const char *s_system,int key,const char *s_label)
{
 system(s_system);
 str->specialKey=key;
 gtk_label_set_text(GTK_LABEL (str->mylabel),s_label);  
}

void fButtonAlt(struttura *str)
{
 if(str->specialKey==XK_Alt_L)
   setSpecialButton(str,"xdotool keyup alt",0,"");
 else
   setSpecialButton(str,"xdotool keydown alt",XK_Alt_L,"Alt on");
}
void fButtonCtrl(struttura *str)
{
 if(str->specialKey==XK_Control_L)
   setSpecialButton(str,"xdotool keyup ctrl",0,"");
 else
   setSpecialButton(str,"xdotool keydown ctrl",XK_Control_L,"Ctrl on");
}
void fButtonEsc(struttura *str)
{
  setSpecialButton(str,"xdotool keydown esc keyup esc",0,"");
}
void printOnFocus(struttura *str,int key,int modifier)
/*Funzione associata al tasto invio */
{
  int revert;
  XGetInputFocus(str->display, &str->win,&revert);
  sendKey(str->display,str->win,str->winRoot,key,modifier);
}
void fButtonDelete(struttura *str)
/*Funzione associata al tasto delete */
{
 printOnFocus(str,XK_BackSpace,0);
 if((str->ins.on)&&(str->ins.index>=0))
  {
   str->ins.index--;
  // printf("index-- %d\n",str->ins.index);
  }
}

void fButtonSpace(struttura *str)
{
printOnFocus(str,XK_space,0);
str->ins.index=-1;
}   
void fButtonIns(struttura *str)
{ 
   str->ins.index++;
   str->ins.key_numbers[str->ins.index]=str->ins.new_word[str->ins.index]=0;
  // printf("il vettore di numeri associato è %s ,la parola è %s\n",str->ins.key_numbers,str->ins.new_word);
   if (str->ins.index>0)//else inserimento annullato ripassiamo da modalità insert a modalità t9 
     window_insert(str);
   
}
void fButtonEnter(struttura *str)
/*Funzione associata al tasto invio */
{ 
  printOnFocus(str,XK_KP_Enter,0);
  str->ins.index=-1;
}
void fButtonTab(struttura *str)
/*Funzione associata al tasto tab */
{
  printOnFocus(str,XK_Tab,0);
  str->ins.index=-1;
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
   gtk_label_set_text (GTK_LABEL (str->mylabel2),"PT on");
  }
 else
  { 
   ins(str);
   str->tp.statot9=0;
   gtk_list_clear_items ((GtkList *) str->tp.gtklist,0,N);
   gtk_widget_set_usize(str->tp.scrolled_window, 0,S_W_MAIN); 
   gtk_label_set_text (GTK_LABEL (str->mylabel2),"PT off");
  }
}


void setMove(struttura *str)
/*funzione di semplice supporto alle funzioni move*/
{
 gtk_window_move(GTK_WINDOW(str->window),str->move.position_width,str->move.position_height);
 gdk_window_process_all_updates ();
}

void movetoleft(struttura *str)
{
 if (str->move.position_width>0)//se non sei a inizio schermo
 {	
   str->move.position_width-=str->move.delay_orizzontale;
   setMove(str);
 }
 
}

void movetodown(struttura *str)
{ //se (str->move.position_height==str->move.delay_verticale*2) è verificato =0 altrimenti + delay_verticale
  //delay verticale è lo spostamento verticale
  str->move.position_height=(str->move.position_height==str->move.delay_verticale*2)?0:str->move.position_height+str->move.delay_verticale;
  setMove(str);
}



void movetoright(struttura *str)
{
 if (str->move.position_width<str->move.delay_orizzontale*2)//se non sei a fine schermo	
 {
   str->move.position_width+=str->move.delay_orizzontale;  
   setMove(str);
 }
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
 str->move.s_width = gdk_screen_get_width(screen);
 str->move.s_height = gdk_screen_get_height(screen);
 str->move.delay_verticale=(str->move.s_height-WINDOW_HEIGHT)/2;
 str->move.delay_orizzontale=(str->move.s_width-WINDOW_WIDTH)/2;
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

//Update Dictionary
void updatedictionary(struttura *str)
{
       int retval;
       char query[200];
	//Check if the word is present on the global dictionary
	sprintf (query, " select parola from  personale where parola like \'%s\';",str->ins.new_word);
	//printf("\n%s\n",query);
        /* dato che la nuova parola dovrà essere inserita nella tabella delle parole personali
           facciamo una query per verificare se la nuova parola è già presente solo in personale,
           perchè non importa se è già presente in globale,in quanto alla prossima ricerca se
           era presente sarebbe stata trovato dalla query che verifica le parole globali e quindi
           inserita in quelle personali.Quindi anche se già presente in globale,facciamo un 
           inserimento in personale ora ma evitiamo una query di ricerca e successivo inserimento dopo
          (tanto quella di inserimento dovrà  essere fatta)*/ 
            
	if(sqlite3_prepare_v2(str->tp.db,query,-1,&str->tp.stmt,0))
        {      
               	fprintf(stderr,"error : programm %s ,function updatedictionary(..) sqlite3_prepare_v2 return error\n ",NameProgramm); 
        	return;
	} 
	retval = sqlite3_step(str->tp.stmt);
	
	if(retval != SQLITE_ROW)
	{
		
		sprintf (query, "insert into personale (codice,parola,frequenza) values (\'%s\',\'%s\',3);",str->ins.key_numbers,str->ins.new_word);
	        if (sqlite3_exec(str->tp.db,query,0,0,0))
                  fprintf(stderr,"error : programm %s ,function updatedictionary(..) sqlite3_exec return error\n ",NameProgramm); 	
                 	
        }
	gdk_window_process_all_updates ();
}

void ins(struttura *str)
{
 int revert;
 XGetInputFocus(str->display, &str->win, &revert);
 str->ins.on=1;
 str->ins.index=-1;
 gtk_widget_set_usize(str->tp.scrolled_window, 0,S_W_MAIN); 
}


void writeSimbol(struttura *str,lbutton *listSibolOnButton,int number)//listsibol puntatore alla lista simbolo del pulsante cliccato
{
 int revert;
 time_t tnow;
 double t_diff;
 tnow=time(NULL);
 t_diff=difftime(tnow,listSibolOnButton->timeSelected);
 listSibolOnButton->timeSelected=tnow;
 XGetInputFocus(str->display, &str->win, &revert);

 if(!((t_diff>=0)&&(t_diff<=1.0)))
 {
   str->indexSimbol=0;//indice 
   if(str->ins.index<LEN_WORD_DB)//aggiunto ora
    str->ins.index++;//indice del vettore dei numeri associati nel caso di nuovo inserimento t9 dizionario eventuale
   sendKey(str->display,str->win,str->winRoot,listSibolOnButton->simbol[str->indexSimbol],0+ str->modifier);//per lo 0
   
 }
 else
 {
   str->indexSimbol=(str->indexSimbol==listSibolOnButton->len-1)?0:str->indexSimbol+1;   
   sendKey(str->display,str->win,str->winRoot,XK_BackSpace,0);
   sendKey(str->display,str->win,str->winRoot,listSibolOnButton->simbol[str->indexSimbol],listSibolOnButton->status+ str->modifier);
 }
 
   str->ins.key_numbers[str->ins.index]='0'+number;  //numero etichetta del pulsante
   str->ins.new_word[str->ins.index]=listSibolOnButton->simbol[str->indexSimbol];
}




//gtk_get_name_button(widget);
void button_clicked(GtkWidget *widget,struttura *str)
{ 
 // Find the window which has the current keyboard focus.
 int revert;
 int button;
 button=atoi(gtk_widget_get_name(widget));
 if(button<=9){
   if ((str->tp.statot9==0))
     writeSimbol(str,&str->listButton[button],button);
   else 
    {
     if (str->tp.luncodicet9==0)//solo alla scelta della prima lettera che comporra la parola predetta
      {
       XGetInputFocus(str->display, &str->win, &revert);
       gtk_widget_set_usize(str->tp.scrolled_window, 0,75); 
      }      
     gestionet9 (str,button); 
    }  
 }
 else
   listF_Buttons[((button==10)?0:button-11)](str);
}




void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str)
{
// printf("Clicked on tray icon\n");
 gtk_widget_show(str->window);
 gtk_status_icon_set_visible(str->tray_icon, FALSE);
 setMove(str);
}

void supportoButtonTable(char s[3],int num)
/*la funzione è di esu esclusivo per la numerazione dei button nella tabella
  in particolare se riceve num compreso tra 1 e 10 o tra 12 e 99 lo scrive su s
  se invece riceve 11 su s scrive 0 cosi da avere corrisondenza tra il tastierino e i numeri*/
{
 s[2]=0;
 if (num==11) 
  s[0]=s[1]='0';
 else
  {
   s[1]='0'+num%10;
   s[0]=((num>11)||(num==10))?'0'+num/10:'0';  
  }
} 


void finestra(struttura *str)
{
 GtkWidget *frame, *winbox, *box,*table;//nuovo x lista

 GtkWidget **buttonArray;

 int i, x, y;
 char s_i[3];
 /*Definiamo vettore che contiene gli indici delle posizioni possibili dell'interfaccia*/
 char s[NUM_OF_BUTTONS][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","⇧ shift","0","alt gr","del","space","↵","PT","tab","ins","alt","ctrl",".","←","↓","→"};
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
 gtk_frame_set_label( GTK_FRAME(frame), "Keyboard" );
 gtk_widget_show (frame);

 box = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (frame), box);
 gtk_widget_show (box);

 buttonArray = (GtkWidget**)g_malloc((NUM_OF_BUTTONS)*sizeof(GtkButton**));

 table = gtk_table_new(NUM_OF_ROWS, NUM_OF_COLUMNS, TRUE);
 gtk_table_set_row_spacings(GTK_TABLE(table), 1);
 gtk_table_set_col_spacings(GTK_TABLE(table), 1);
 x=y=0; 
 for(i=1; i<=NUM_OF_BUTTONS;i++)
 {  
    buttonArray[i] = gtk_button_new_with_label(s[i-1]);
    gtk_widget_set_size_request(buttonArray[i],60,40);
    gtk_table_attach_defaults(GTK_TABLE(table), buttonArray[i], x, x+1, y, y+1);
    supportoButtonTable(s_i,i);
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
 str->mylabel = gtk_label_new ("");
 gtk_widget_set_usize(str->mylabel, 0, 1);
 gtk_container_add(GTK_CONTAINER(box), str->mylabel);
 gtk_widget_show (str->mylabel);


 
 /* Crea una Lista per il T9 */
 str->tp.gtklist=gtk_list_new();
 gtk_list_set_selection_mode((GtkList *)str->tp.gtklist, GTK_SELECTION_SINGLE);
 str->tp.scrolled_window=gtk_scrolled_window_new(NULL,NULL);
 gtk_widget_set_usize(str->tp.scrolled_window, 0,S_W_MAIN);//75//
 gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (str->tp.scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(str->tp.scrolled_window),str->tp.gtklist);
 gtk_container_add(GTK_CONTAINER(box),str->tp.scrolled_window);
 gtk_widget_show(str->tp.scrolled_window);
 gtk_widget_show(str->tp.gtklist);
 gtk_signal_connect(GTK_OBJECT(str->tp.gtklist),"selection_changed",GTK_SIGNAL_FUNC(sigh_print_selection),str);//nuovo x lista
 

 
 
 /* Crea una Label per il T9 */
 str->mylabel2 = gtk_label_new ("PT off");
 gtk_widget_set_usize(str->mylabel2, 0, 1);
 gtk_container_add(GTK_CONTAINER(box),str->mylabel2);
 gtk_widget_show (str->mylabel2);

 /* Crea uno status icon per la minimizzazione */
 str->tray_icon = gtk_status_icon_new();
 g_signal_connect(G_OBJECT(str->tray_icon), "activate",G_CALLBACK(tray_icon_on_click), str);
 gtk_status_icon_set_from_icon_name(str->tray_icon,GTK_STOCK_MEDIA_STOP);
 gtk_status_icon_set_from_file(str->tray_icon,PATH_IMG);
 gtk_status_icon_set_tooltip(str->tray_icon,"Keyboard Tray Icon");
 gtk_status_icon_set_visible(str->tray_icon, FALSE);

 gtk_widget_show_all(str->window);
 /*salviamo le dim massime dello schermo corrente*/
 getMaxScreen(str);
 ins(str);
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
		bzero(str->tp.vetparole[i].parola,LEN_WORD_DB+1);
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
	*s=0;//equals sprintf(s,"");
	//printf("\n%s\n",query);
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
		    	//printf ("\n");
		    	// sqlite3_column_text returns a const void* , typecast it to const char*
		    	for(int col=0 ; col<cols;col++)
		   	{
		        	const char *val = (const char*)sqlite3_column_text(str->tp.stmt,col);
		        //printf("%s = %s\t",sqlite3_column_name(str->tp.stmt,col),val);
				if (col==0) 
				{
			//		printf ("%s",val);
					sprintf(str->tp.vetparole[str->tp.numparoletrovate-1].parola,"%s",val);
					str->tp.list_item=gtk_list_item_new_with_label(val);
					dlist=g_list_append(dlist, str->tp.list_item);
					gtk_widget_show(str->tp.list_item);
					gtk_object_set_data(GTK_OBJECT(str->tp.list_item), list_item_data_key,s);

				}
				else 
				{
			//		printf ("\tfr=%s",val);
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
	//Display *display = XOpenDisplay(0);
	if(str->tp.numparoletrovate> 0)
          gtk_list_append_items((GtkList*)(str->tp.gtklist), dlist);
	else{
          ins(str);	
	  fButtonTP(str);
        }
        gdk_window_process_all_updates ();
	//XCloseDisplay(display);
	//printf ("\n");
	
}

void sigh_print_selection(GtkWidget *gtklist, struttura *str)
{
 if (str->tp.luncodicet9==0) return;

 GList   *dlist;
 GtkWidget *child;
 gtk_widget_set_usize(str->tp.scrolled_window, 0,S_W_MAIN); 
 // Fetch the doubly linked list of selected items of the List, remember to treat this as read-only!
 //g_print("Selection\n");
 dlist=GTK_LIST(gtklist)->selection;
 //g_print("Selection 1\n");
 // Individuiamo l'elemento che ha emesso il segnale e calcoliamo la sua posizione all'interno della lista
 child=gtk_container_get_focus_child(GTK_CONTAINER(gtklist));
 str->tp.indice=gtk_list_child_position((GtkList *)gtklist,child);
 //g_print("Selection 2\n");
 int dim_parola = strlen(str->tp.vetparole[str->tp.indice].parola);
  //g_print("Selection 3\n");
 // If there are no selected items there is nothing more to do than just telling the user so
 if(!dlist){
	//g_print("Selection cleared\n");
        return;
 }
   //g_print("Selection 4\n");
 // Ok, we got a selection and so we print it
 for (int kk=0; kk < dim_parola; kk++) 
 {
	gchar word[dim_parola];
	sprintf(word,"%s",str->tp.vetparole[str->tp.indice].parola);

	gchar  *let;
	let = (gchar*)malloc(sizeof(gchar));
	*let=0;//equals sprintf(let,"");

	sprintf(let,"%c",word[kk]);
        //sendKey(str->display,str->win,str->winRoot,XStringToKeysym(let),str->modifier);
        if  (XStringToKeysym(let))
          sendtofocus(str,XStringToKeysym(let));
        else 
          sendtofocus(str,XK_minus);
}
// printf("\n");

 //g_print("Selection 5\n");


 str->tp.vetparole[str->tp.indice].frequenza=str->tp.vetparole[str->tp.indice].frequenza+1;
   //     printf("\nNuova frequenza parola selezionata: %d\n",str->tp.vetparole[str->tp.indice].frequenza);

 char query[200];
 bzero (query,200);
 if (str->tp.vetparole[str->tp.indice].frequenza>1) 
	sprintf (query, "update personale set frequenza =%d where parola =\'%s\';",str->tp.vetparole[str->tp.indice].frequenza,str->tp.vetparole[str->tp.indice].parola);
 else 
	sprintf (query, "insert into personale (codice,parola,frequenza) values (\'%s\',\'%s\',1);",str->tp.codicet9,str->tp.vetparole[str->tp.indice].parola);

 //printf("\n%s\n",query);
 if (sqlite3_exec(str->tp.db,query,0,0,0))
   fprintf(stderr,"error : programm %s ,function sigh_print_selection(..) sqlite3_exec return error\n ",NameProgramm); 	
 gtk_list_clear_items ((GtkList *) gtklist,0,N);
 str->tp.luncodicet9 = 0;
 bzero(str->tp.codicet9,LEN_WORD_DB+1);
 gdk_window_process_all_updates ();
	//XCloseDisplay(display);
}

//Funzione thread che legge il file dizionazrio e riempe la lista per il T9
void *thfilet9 (void *arg)
{
	int rc;
	struttura *str=(struttura *)arg;
        rc = sqlite3_open(PATH_DB, &str->tp.db);
        if (rc)
 	{
 	       printf("\nerrore database del T9\n");
	       exit(EXIT_FAILURE);
        }
        else
	{
		//printf("\nConnessione al database del T9 avvenuta correttamente\n");
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





void button_pressed_insert(GtkWidget *widget,struttura *str)
{
  updatedictionary(str);
}


void window_insert(struttura *str)
{ 
  GtkWidget *winDialog,*button1,*button2,*label;
  GtkDialog *dialog;
  char slabel[LEN_WORD_DB+16];
  

  
  winDialog = gtk_dialog_new();
  dialog=GTK_DIALOG(winDialog);
  gtk_window_set_title(GTK_WINDOW(winDialog), "new word?");
  
  button1=gtk_dialog_add_button(dialog,"cancel",0);
 
  button2=gtk_dialog_add_button(dialog,"insert",1);
 

  sprintf(slabel,"insert '%s' in db?",str->ins.new_word);
  label = gtk_label_new (slabel);
  gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(dialog)),label, TRUE, TRUE, 0);
  
  gtk_widget_show (label);
  
  g_signal_connect(G_OBJECT(button1), "clicked",G_CALLBACK (gtk_widget_destroy), winDialog);
  g_signal_connect(G_OBJECT(button2), "clicked",G_CALLBACK(button_pressed_insert), str);  
  
  g_signal_connect_swapped (winDialog, "delete-event",
                              G_CALLBACK (gtk_widget_destroy), 
                              winDialog);
  gtk_dialog_run(dialog);
  gtk_widget_destroy(winDialog);
  ins(str);
}


int main(int argc, char *argv[])
{
 struttura str;
 str.display = XOpenDisplay(0);
 int i;
 int keycodemap[LENB*7+LENB1+LENB5+LENB0]={XK_1,XK_backslash,XK_comma,XK_plus,XK_minus,XK_less,XK_quoteright,XK_period,
                                          /*;1;\;,;+;-;<;';.;*/
                                          XK_a,XK_b,XK_c,XK_agrave,XK_2,/* a,b,c,à,2*/
                                          XK_d,XK_e,XK_f,XK_egrave,XK_3,/* d,e,f,è,3*/
                                          XK_g,XK_h,XK_i,XK_igrave,XK_4,/* g,h,i,ì,4*/
                                          XK_j,XK_k,XK_l,XK_5,                /* j,j,l,5 */
                                          XK_m,XK_n,XK_o,XK_ograve,XK_6,/* m,n,o,ò,6*/
                                          XK_p,XK_q,XK_r,XK_s,XK_7,     /* p,q,r,s,7*/
                                          XK_t,XK_u,XK_v,XK_ugrave,XK_8,/* t,u,v,ù,8*/
                                          XK_w,XK_x,XK_y,XK_z,XK_9,     /* w,x,y,z,9*/
                                          XK_0/*0*/,XK_question/*?*/,XK_exclam/*!*/,XK_equal/*=*/,XK_asterisk/* * */,XK_semicolon/*;*/,XK_colon/*:*/,XK_underscore/*_*/,XK_2/*"*/,XK_3/*£*/,XK_dollar/*$*/,XK_percent/*%*/,XK_ampersand/*&*/,XK_slash/*/*/,XK_bar/*|*/,XK_parenleft/*(*/,XK_parenright/*)*/,XK_asciicircum/*^*/,XK_greater/*>*/};
                        
 int sup; 
 str.indexSimbol=0;//semplice inizzializzazione
 str.modifier=0;//semplice inizzializzazione
 str.specialKey=0;//semplice inizzializzazione
 str.listButton[1].simbol=setListButton(LENB1,keycodemap,str.listButton[1],0);
 for(i=2,sup=LENB1;i<=4;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i],0);
 str.listButton[5].simbol=setListButton(LENB5,keycodemap+sup,str.listButton[5],0);
 for(i++,sup+=LENB5;i<=9;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i],0);
 str.listButton[0].simbol=setListButton(LENB0,keycodemap+sup,str.listButton[0],XK_ISO_Level5_Shift);
 
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
 str.move.position_height=0;
 str.move.position_width=0;
 str.ins.on=0;
 str.ins.index=0;
 str.ins.key_numbers[0]=0;
 // Get the root window for the current display.
 str.winRoot = XDefaultRootWindow(str.display);
 
 finestra(&str);
 
 XCloseDisplay(str.display);
 return 0;
}
 
