/*virtualinput aggiornato 04/02/2011*/ 
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
#define NUM_OF_ROWS 7                       /* number rows of buttons*/
#define NUM_OF_BUTTONS 21                   /*the total number of buttons*/ 
#define NameProgramm "virtualinput"         /*the name of the program */
#define LENB 5                              /*the number of symbols in the buttons 2,3,4,5,6,7,8,9*/ 
                                            /*LENB=LENB2=LENB3=LENB4=LENB6=LENB7=LENB8=LENB9*/
#define LENB1 17                             /*the number of symbols in the button 1*/
#define LENB5 4                             /*the number of symbols in the button 5*/
#define LENB0 15                            /*the number of symbols in the button 0*/
#define WINDOW_WIDTH 200                    /*width of main window*/
#define WINDOW_HEIGHT 380                   /*heigth of main window*/
#define S_W_MAIN 30                         /*heigth of scrolled_window in TP off*/  
                                            //il valore di scrolled window quando il tp è disabilitato

#define LEN_WORD_DB 20                      /* len of words saved on database */
#define N 20                                 /* number of element showed in the list of word predected*/

#define list_item_data_key "list_item_data" /*key for identicitazion list item insert*/ 

#define PATH_DB "/usr/share/virtualinput/virtualinput-0.3.1beta/dizionario.sqlite"
//#define PATH_DB "dizionario.sqlite"
#define PATH_IMG "/usr/share/virtualinput/virtualinput-0.3.1beta/keyboard_icon.gif"
#define N_SIMBOL_MODIFIER 127 /*numero di simboli a cui e' associato un modifier*/

struct nodo {
  char parola[LEN_WORD_DB+1];
  char keyparola[LEN_WORD_DB+1];
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
 int *simbol;//lista di caratteri associati al carattere
 time_t timeSelected;//l'ultima volta in cui il pulsante è stato cliccato
}lbutton;

typedef struct tinsert{ /*la struttura contiene i dati che servono neel caso di un nuovo inserimento nel dizionario*/
 int on;//modalità inserimento attiva
 int index;//indice 
 char key_numbers[LEN_WORD_DB+1];//numeri associati per il t9 alla nuova parola
 int new_word[LEN_WORD_DB+1];
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
 int modifiers[N_SIMBOL_MODIFIER];//modificatore associato ad ogni singolo carattere ascii
 lbutton listButton[10];
 int indexSimbol;/*rappresenta l'indice del listButton(generico) corrispondente all'ultimo carattere stampato*/  
 T_tp tp;
 //Gestione finestra
 GtkWidget *window,*mylabel,*mylabel2; 
 GtkStatusIcon *tray_icon;
 tmove move; 
 tins ins;
}struttura;
 

/*dichiarazioni per finestra2*/
GtkWidget *window, *box, *mylabel;

//window tag management
int position, s_width, s_height;

//key management
static int modifier = 0;

/*fine dichiarazioni per finestra2*/








void finestra(struttura *str);



void gestionet9 (struttura *str);
void sigh_print_selection(GtkWidget *gtklist,struttura *str);
void sendtofocus(struttura *str,int keycode);


 
void fButtonShift(struttura *str);

void fButtonDelete(struttura *str);
void fButtonSpace(struttura *str);
void fButtonEnter(struttura *str);
void fButtonTP(struttura *str);
void fButtonTab(struttura *str);
void fButtonIns(struttura *str);
void fButtonAlt(struttura *str);
void fButtonCtrl(struttura *str);
void fButtonSF(struttura *str);
void moveTodown(struttura *str);
void moveToright(struttura *str);

void movetodown(struttura *str,GtkWidget *win);
void movetoright(struttura *str,GtkWidget *win);
void dockicon(struttura *str);
void tray_icon_on_click(GtkStatusIcon *status_icon,struttura *str);
void getMaxScreen(struttura *str);
void setMove(struttura *str,GtkWidget *win);


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

void setModifiers(int *modifiers);/*setta i modificatori per tutti i caratteri ascii*/
int getModifier(struttura *str,char carattere);/*restituisce il modificatore associato a quel determinato carattere*/

typedef void(*funz)(struttura *);
funz listF_Buttons[NUM_OF_BUTTONS-10]={fButtonShift,fButtonTP,fButtonDelete,fButtonSpace,fButtonEnter,fButtonSF,fButtonTab,fButtonIns,moveToright,moveTodown,dockicon};

void printModifier(struttura *str,int modifier,const gchar *s_label)
{
  str->modifier=modifier;
  gtk_label_set_text (GTK_LABEL (str->mylabel),s_label);
 
}
void fButtonShift(struttura *str)
/*Funzione associata al tasto shift => setta lo shift */
{
  if(str->modifier!=XK_ISO_Level5_Shift)
   printModifier(str,XK_ISO_Level5_Shift,"Shift on");
  else
   printModifier(str,0,"Shift off");
}


char *intToChar(int *parola)
/*funziona se al massimo fra caratteri ascii e non che valgono per 2  non si supera la dim di 255*/
{
 char *s;
 int i,j;
 s=(char *)malloc(sizeof(char)*256);
 for(i=0,j=0;(parola[i]!=0);i++,j++)
  {
   if ((parola[i]>=0)&&(parola[i]<=126))
    s[j]=parola[i];
   else
   {
    if (parola[i]==XK_agrave)
     snprintf(&s[j],3,"à");
    else if (parola[i]==XK_igrave)
     snprintf(&s[j],3,"ì");
    else if (parola[i]==XK_egrave)
     snprintf(&s[j],3,"è");
    else if (parola[i]==XK_ograve)
     snprintf(&s[j],3,"ò");
    else if (parola[i]==XK_ugrave)
     snprintf(&s[j],3,"ù");
    j++;
   }
  } 
 s[j]=0;
 return s;
}


void fButtonSF(struttura *str)
/*Funzione associata al tasto 'sf' */
{
  setMove(str,window);
  gtk_widget_show(window);
  gtk_widget_hide(str->window);
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
   setSpecialButton(str,"xdotool keydown alt",0,"");
 else
   setSpecialButton(str,"xdotool keyup alt",XK_Alt_L,"Alt on");
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
 if((str->tp.statot9==1)&&(str->tp.luncodicet9>0))
  { 
   str->tp.luncodicet9-=2;
   gestionet9(str);
  }
 else
  {
   printOnFocus(str,XK_BackSpace,0);
   if((str->ins.on)&&(str->ins.index>=0))
     str->ins.index--;
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
   else 
    str->ins.index--;
   
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


void setMove(struttura *str,GtkWidget *win)
/*funzione di semplice supporto alle funzioni move*/
{
 gtk_window_move(GTK_WINDOW(win),str->move.position_width,str->move.position_height);
 gdk_window_process_all_updates();
}

void moveTodown(struttura *str)
{
 movetodown(str,str->window);
}

void movetodown(struttura *str,GtkWidget *win)
{ //se (str->move.position_height==str->move.delay_verticale*2) è verificato =0 altrimenti + delay_verticale
  //delay verticale è lo spostamento verticale
 getMaxScreen(str); 
 str->move.position_height=(str->move.position_height+str->move.delay_verticale+WINDOW_HEIGHT>str->move.s_height)?0:str->move.position_height+str->move.delay_verticale;
  setMove(str,win);
}


void moveToright(struttura *str)
{
 movetoright(str,str->window);
}

void movetoright(struttura *str,GtkWidget *win)
{
 getMaxScreen(str);
 str->move.position_width=(str->move.position_width+str->move.delay_orizzontale+WINDOW_WIDTH>str->move.s_width)?0:str->move.position_width+str->move.delay_orizzontale;
 setMove(str,win);
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
	sprintf (query, " select parola from  globale where parola like \'%s\';",intToChar(str->ins.new_word));
	//printf("\n%s\n",query);
        
            
	if(sqlite3_prepare_v2(str->tp.db,query,-1,&str->tp.stmt,0))
        {      
               	fprintf(stderr,"error : programm %s ,function updatedictionary(..) sqlite3_prepare_v2 return error\n ",NameProgramm); 
        	return;
	} 
	retval = sqlite3_step(str->tp.stmt);
	
	if(retval != SQLITE_ROW)
	{
		
		sprintf (query, "insert into globale (codice,parola,frequenza) values (\'%s\',\'%s\',3);",str->ins.key_numbers,intToChar(str->ins.new_word));
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
 }
 else
 {
   str->indexSimbol=(str->indexSimbol==listSibolOnButton->len-1)?0:str->indexSimbol+1;   
   sendKey(str->display,str->win,str->winRoot,XK_BackSpace,0);
 }
 sendKey(str->display,str->win,str->winRoot,listSibolOnButton->simbol[str->indexSimbol],getModifier(str,listSibolOnButton->simbol[str->indexSimbol])+ str->modifier);
 str->ins.key_numbers[str->ins.index]='0'+number;  //numero etichetta del pulsante
 str->ins.new_word[str->ins.index]=listSibolOnButton->simbol[str->indexSimbol];
}




//gtk_get_name_button(widget);
void button_clicked(GtkWidget *widget,struttura *str)
{ 
 // Find the window which has the current keyboard focus.
 int button;
 button=atoi(gtk_widget_get_name(widget));
 if(button<=9){
   if ((str->tp.statot9==0))
     writeSimbol(str,&str->listButton[button],button);
   else 
    {
     if (str->tp.luncodicet9==0)//solo alla scelta della prima lettera che comporra la parola predetta
       gtk_widget_set_usize(str->tp.scrolled_window, 0,75);         
     str->tp.codicet9[str->tp.luncodicet9]='0'+button;
     gestionet9 (str); 
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
 setMove(str,str->window);
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
 char s[NUM_OF_BUTTONS][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","⇧ shift","0 +","PT","del","space","↵","sf","tab","ins","→","↓","."};
 /* GDK_GRAVITY_SOUTH_WEST is the reference point corresponding at the lower left corner -> position 1
  GDK_GRAVITY_SOUTH is the reference point corresponding at the middle of the lower edge -> position 2
  GDK_GRAVITY_SOUTH_EAST is the reference point corresponding at the lower right corner -> position 3
*/
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
 str->mylabel = gtk_label_new ("Shift off");
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
 gtk_widget_set_usize(str->mylabel2,0, 1);
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
// gtk_main ();
}


void sendtofocus(struttura *str,int keycode)
/* funzione introdotta per compatibilità al t9*/
{
 sendKey(str->display,str->win,str->winRoot,keycode,getModifier(str,keycode)+str->modifier);
}
//Algoritmo T9 di predizione del testo basato su liste
void gestionet9 (struttura *str)
{
	gtk_list_clear_items ((GtkList *) str->tp.gtklist,0,N);
        if (str->tp.luncodicet9==-1) /*possibile solo se provenienti da un delete*/
         {
          gtk_widget_set_usize(str->tp.scrolled_window, 0,S_W_MAIN); 
          str->tp.luncodicet9=0; 
          return;
         }
        //fase inizializzazione
	str->tp.numparoletrovate=0;
	int i=0;
	for (i=0; i<N;i++)
	{
		bzero(str->tp.vetparole[i].parola,LEN_WORD_DB+1);
	}
	
	//aggiornamento
	str->tp.luncodicet9++;
	str->tp.codicet9[str->tp.luncodicet9]=0;
	//printf("\nTasti premuti: %s\tlunghezza:%d\n",str->tp.codicet9,str->tp.luncodicet9);
	char query[200];
	bzero (query,200);
	//se freq0 globale, se 1 pers
	sprintf (query, " select parola,codice,frequenza from globale where codice like \'%s%%\' order by codice,frequenza desc  limit 0,%d;",str->tp.codicet9,N);
  	//GtkWidget *list_item;
	GList *dlist=NULL;
	gchar *s;
	s = (gchar*)malloc(sizeof(gchar));
	*s=0;//equals sprintf(s,"");
	//printf("\n%s\n",query);
	int  retval = sqlite3_prepare_v2(str->tp.db,query,-1,&str->tp.stmt,0);    
	if(retval)
	{
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
		        	char *val = (char *)sqlite3_column_text(str->tp.stmt,col);
		                //printf("%s = %s\t",sqlite3_column_name(str->tp.stmt,col),val);
				if (col==0) 
			         {
                                        sprintf(str->tp.vetparole[str->tp.numparoletrovate-1].parola,"%s",val);
                                        str->tp.list_item=gtk_list_item_new_with_label(val);
					dlist=g_list_append(dlist, str->tp.list_item);
                                        gtk_widget_set_usize(str->tp.list_item,0,25);
					gtk_widget_show(str->tp.list_item);
					gtk_object_set_data(GTK_OBJECT(str->tp.list_item), list_item_data_key,s);

				}
				else
                                 strcpy(str->tp.vetparole[str->tp.numparoletrovate-1].keyparola,val);
				
		   	 }  
		}
		else
		{
			// Some error encountered
		    	return;
		}
	}
	str->tp.indice=0;
	if(str->tp.numparoletrovate> 0)
          gtk_list_append_items((GtkList*)(str->tp.gtklist), dlist);
	else{
          ins(str);	
	  fButtonTP(str);
        }
        gdk_window_process_all_updates ();
	
	//printf ("\n");
	
}

void sigh_print_selection(GtkWidget *gtklist, struttura *str)
{
 int revert,keycode;
 if (str->tp.luncodicet9==0) return;
 XGetInputFocus(str->display, &str->win,&revert);
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
 // Ok, we got a selection and so we print itbacca
 
 for (int kk=0,i=0; kk < dim_parola;i++, kk++) 
 {
   if(strncmp(&str->tp.vetparole[str->tp.indice].parola[i],"à",2)==0)
    sendtofocus(str,XK_agrave);
   else if(strncmp(&str->tp.vetparole[str->tp.indice].parola[i],"è",2)==0)
    sendtofocus(str,XK_egrave);
   else if(strncmp(&str->tp.vetparole[str->tp.indice].parola[i],"ì",2)==0)
    sendtofocus(str,XK_igrave);
   else if(strncmp(&str->tp.vetparole[str->tp.indice].parola[i],"ò",2)==0)
    sendtofocus(str,XK_ograve);
   else if(strncmp(&str->tp.vetparole[str->tp.indice].parola[i],"ù",2)==0)
    sendtofocus(str,XK_ugrave);
   sendtofocus(str,(int)str->tp.vetparole[str->tp.indice].parola[i]);
}

 char query[200];
 bzero (query,200);
 
 sprintf (query, "update globale set frequenza=frequenza+1 where parola =\'%s\';",str->tp.vetparole[str->tp.indice].parola);
     

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
 	       fprintf(stderr,"\nerror %s file database not open\n",PATH_DB);
	       exit(EXIT_FAILURE);
        }
        else
	{
		//printf("\nConnessione al database del T9 avvenuta correttamente\n");
		str->tp.flagcaricat9=1;
	}	
	fflush(stdout);	
}


int *setListButton(int len,int *vet,lbutton &l)
{
 l.len=len;
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
  

  gtk_widget_hide(str->window);
  winDialog = gtk_dialog_new();
  dialog=GTK_DIALOG(winDialog);
  gtk_window_set_title(GTK_WINDOW(winDialog), "new word?");
  
  button1=gtk_dialog_add_button(dialog,"cancel",0);
 
  button2=gtk_dialog_add_button(dialog,"insert",1);

  snprintf(slabel,LEN_WORD_DB+16,"insert '%s' in db?",intToChar(str->ins.new_word));
  label = gtk_label_new (slabel);
  gtk_box_pack_start (GTK_BOX(gtk_dialog_get_content_area(dialog)),label, TRUE, TRUE, 0);
  
  gtk_widget_show (label);
  
  g_signal_connect(G_OBJECT(button1), "clicked",G_CALLBACK (gtk_widget_destroy), winDialog);
  g_signal_connect(G_OBJECT(button2), "clicked",G_CALLBACK(button_pressed_insert), str);  
  
  g_signal_connect_swapped (winDialog, "destroy",
                              G_CALLBACK (gtk_widget_destroy), 
                              winDialog);
  gtk_dialog_run(dialog);
  setMove(str,str->window);
  gtk_widget_show(str->window);
  gtk_widget_destroy(winDialog);
  ins(str);
}

void setModifiers(int *modifiers)
/*setta i modificatori per tutti i caratteri ascii*/
{
 int i,modifiers_temp[N_SIMBOL_MODIFIER]={
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_Shift_R,XK_ISO_Level5_Shift,/* !"#$*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,0,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*%&'()*/
            XK_ISO_Level5_Shift,0,0,0,0,XK_ISO_Level5_Shift,0,0,0,0,0,0,0,0,0,0, /**+,-./0123456789*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,0,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*:;<=>*/
            XK_ISO_Level5_Shift,XK_Shift_R,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*?@ABC*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*DEFG*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*HIJK*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*LMNO*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*PQRS*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,/*TUVW*/
            XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,                    /*XYZ*/
            XK_Shift_R,0,XK_Shift_R,XK_ISO_Level5_Shift,XK_ISO_Level5_Shift,XK_Control_R,/*[\]^_`*/
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,/*abcdefghijklmnopqrstuvwxyz*/
            XK_Shift_R,XK_ISO_Level5_Shift,XK_Shift_R};/*{|}*/
 for (i=0;i<N_SIMBOL_MODIFIER;i++)
   *(modifiers+i)=modifiers_temp[i];            

}
 
int getModifier(struttura *str,char carattere)
/*restituisce il modificatore associato a quel determinato carattere*/
{
 return ((carattere>=0)&&(carattere<N_SIMBOL_MODIFIER))?*(str->modifiers+carattere):0;
}

/*funzioni relative alla seconda finestra*/


void button_clicked2(GtkWidget *widget, struttura *str)
{
 const gchar* buttonow=gtk_button_get_label(GTK_BUTTON(widget));
 int keycode;
 if(strcmp(buttonow,"z")==0)
 {
	keycode = XK_z;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+z");
		}
	}
 }
 if(strcmp(buttonow,"r")==0)
 {
	keycode = XK_r;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+r");
		}
	}
 }
 if(strcmp(buttonow,"x")==0)
 {
	keycode = XK_x;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+x");
		}
	}
 }
 if(strcmp(buttonow,"c")==0)
 {
	keycode = XK_c;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+c");
		}
	}
 }
 if(strcmp(buttonow,"v")==0)
 {
	keycode = XK_v;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+v");
		}
	}
 }
 if(strcmp(buttonow,"+")==0)
 {
	keycode = XK_plus;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+plus");
		}
	}
 }
 if(strcmp(buttonow,"-")==0)
 {
	keycode = XK_minus;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+minus");
		}
	}
 }
 if(strcmp(buttonow,"f")==0)
 {
	keycode = XK_f;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+f");
		}
		if(modifier==XK_Alt_L){
			system("xdotool key alt+f");
		}
	}
 }
 if(strcmp(buttonow,"a")==0)
 {
	keycode = XK_a;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Control_R){
			system("xdotool key ctrl+a");
		}
	}
 }
 if(strcmp(buttonow,"m")==0)
 {
	keycode = XK_m;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+m");
		}
	}
 }
 if(strcmp(buttonow,"f1")==0)
 {
	keycode = XK_F1;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F1");
		}
	}
 }
 if(strcmp(buttonow,"f2")==0)
 {
	keycode = XK_F2;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F2");
		}
	}
 }
 if(strcmp(buttonow,"f3")==0)
 {
	keycode = XK_F3;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F3");
		}
	}
 }
 if(strcmp(buttonow,"f4")==0)
 {
	keycode = XK_F4;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F4");
		}
	}
 }
 if(strcmp(buttonow,"f5")==0)
 {
	keycode = XK_F5;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F5");
		}
	}
 }
 if(strcmp(buttonow,"f9")==0)
 {
	keycode = XK_F9;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F9");
		}
	}
 }
 if(strcmp(buttonow,"f10")==0)
 {
	keycode = XK_F10;
	if(modifier==0){
 		printOnFocus(str,keycode,0);
	}
	else{
		if(modifier==XK_Alt_L){
			system("xdotool key alt+F10");
		}
	}
 }
 if(strcmp(buttonow,"ctrl")==0)
 {
 	if(modifier==XK_Control_R){
		modifier=0;
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new ("Ctrl and Alt off");
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
 }
 if(strcmp(buttonow,"alt")==0)
 {
 	if(modifier==XK_Alt_L){
		modifier=0;
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new ("Ctrl and Alt off");
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	else{
		modifier=XK_Alt_L;
		gtk_container_remove(GTK_CONTAINER(box), mylabel);
		mylabel = gtk_label_new (NULL);
		gtk_label_set_text (GTK_LABEL (mylabel),"activated Alt");
		gtk_container_add(GTK_CONTAINER(box), mylabel);
		gtk_widget_show (mylabel);
	}
	gdk_window_process_all_updates();
 }
 else if(strcmp(buttonow,"close")==0)
   gtk_main_quit();
 else if(strcmp(buttonow,".")==0)
  {
   gtk_status_icon_set_visible(str->tray_icon, TRUE);
   gtk_widget_hide(window);
  }
 else if(strcmp(buttonow,"→")==0)
   movetoright(str,window);
 else if(strcmp(buttonow,"↓")==0)
   movetodown(str,window);
 else if(strcmp(buttonow,"nf")==0)
  {
    setMove(str,str->window); 
    gtk_widget_show(str->window);
    gtk_widget_hide(window);
  }
  
}

void finestra2(struttura *str)
{
 GtkWidget *frame, *winbox, *table;
 GtkWidget **buttonArray;

 int i, x, y;
 char s[NUM_OF_BUTTONS+3][10]={"ctrl","alt","z","r","x","c","v","+","-","a","f","m","f1","f2","f3","f4","f5","f9","nf","f10",".","→","↓","close"};
 

 /* Create window */
 //gtk_init (NULL,NULL);
 
 window = gtk_window_new (GTK_WINDOW_POPUP);
 gtk_window_set_default_size (GTK_WINDOW(window),WINDOW_WIDTH, WINDOW_HEIGHT);

 winbox = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (window), winbox);
 gtk_widget_show (winbox);

  /* Crea una Frame */
 frame = gtk_frame_new("Special Functions");
 gtk_container_add(GTK_CONTAINER(winbox), frame);
 gtk_widget_show (frame);

 box = gtk_vbox_new (FALSE, 0);
 gtk_container_add (GTK_CONTAINER (frame), box);
 gtk_widget_show (box);

 buttonArray = (GtkWidget**)g_malloc((NUM_OF_BUTTONS+3)*sizeof(GtkButton**));

 table = gtk_table_new(NUM_OF_ROWS+1, NUM_OF_COLUMNS, TRUE);
 gtk_table_set_row_spacings(GTK_TABLE(table), 1);
 gtk_table_set_col_spacings(GTK_TABLE(table), 1);
 x=y=0;
 
 for(i=0; i<NUM_OF_BUTTONS+3;i++)
 {
   buttonArray[i] = gtk_button_new_with_label(s[i]);
   gtk_table_attach_defaults(GTK_TABLE(table), buttonArray[i], x, x+1, y, y+1);
   //gtk_widget_set_name(buttonArray[i],s[i]);
   gtk_widget_set_size_request(buttonArray[i],60,40);

   g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked2),str);
   x++;
   if(x==NUM_OF_COLUMNS)
   {
   	x=0;
    	y++;
   }
 }
 gtk_box_pack_start(GTK_BOX(box), table, FALSE, FALSE, 0);

 /* Create label for Shift and Ctrl buttons */
 mylabel = gtk_label_new ("Ctrl and Alt off");
 gtk_container_add(GTK_CONTAINER(box), mylabel);
 gtk_widget_show (mylabel);

 gtk_widget_show_all(window);
 gtk_widget_hide(window);


}
/*fine funzioni relative alla seconda finestra*/


int main(int argc, char *argv[])
{
 struttura str;
 str.display = XOpenDisplay(0);
 int i;
 int keycodemap[LENB*7+LENB1+LENB5+LENB0]={
                                          XK_1,XK_quoteright,XK_period,XK_question,XK_exclam,XK_comma,XK_colon,
                                          XK_semicolon,34,XK_asciicircum,XK_underscore,XK_bar,XK_percent,
                                          XK_ampersand,64,35,XK_dollar,/*1.?!,:;"^_|%&@#$*/
                                          XK_a,XK_b,XK_c,XK_agrave,XK_2,/* a,b,c,à,2*/
                                          XK_d,XK_e,XK_f,XK_egrave,XK_3,/* d,e,f,è,3*/
                                          XK_g,XK_h,XK_i,XK_igrave,XK_4,/* g,h,i,ì,4*/
                                          XK_j,XK_k,XK_l,XK_5,          /* j,j,l,5 */
                                          XK_m,XK_n,XK_o,XK_ograve,XK_6,/* m,n,o,ò,6*/
                                          XK_p,XK_q,XK_r,XK_s,XK_7,     /* p,q,r,s,7*/
                                          XK_t,XK_u,XK_v,XK_ugrave,XK_8,/* t,u,v,ù,8*/
                                          XK_w,XK_x,XK_y,XK_z,XK_9,     /* w,x,y,z,9*/
                                          XK_0,XK_equal,XK_asterisk,XK_plus,XK_minus,XK_slash,XK_backslash,XK_less,
                                          XK_greater,XK_parenleft,XK_parenright,91,93,
                                          123,125/*0=*+-/\<>()[]{}*/
                                          };
                        
 int sup,rest; 
 str.indexSimbol=0;//semplice inizzializzazione
 str.modifier=0;//semplice inizzializzazione
 str.specialKey=0;//semplice inizzializzazione
 str.listButton[1].simbol=setListButton(LENB1,keycodemap,str.listButton[1]);
 for(i=2,sup=LENB1;i<=4;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i]);
 str.listButton[5].simbol=setListButton(LENB5,keycodemap+sup,str.listButton[5]);
 for(i++,sup+=LENB5;i<=9;i++,sup+=LENB)
  str.listButton[i].simbol=setListButton(LENB,keycodemap+sup,str.listButton[i]);
 str.listButton[0].simbol=setListButton(LENB0,keycodemap+sup,str.listButton[0]);
 setModifiers(str.modifiers);
 //t9
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
 //system("export MALLOC_CHECK_=0");
 gtk_init (NULL,NULL);
 finestra(&str);
 finestra2(&str);
 gtk_main();
 
 XCloseDisplay(str.display);
 return 0;
}
 


