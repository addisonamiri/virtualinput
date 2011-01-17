/*virtualinput aggiornato 27/12/2010 no t9*/ 
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 4
#define NUM_OF_BUTTONS 18
#define NameProgramm "virtualKey"
#define LENB 5  /*LENB=LENB2,LENB3,LENB4,LENB6,LENB7,LENB8,LENB9*/
#define LENB1 8
#define LENB5 4
#define LENB10 20
#define N_SIMBOL_MODIFIER 128 /*numero di simboli a cui e' associato un modifier*/

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
}struttura;
 
 
 
 
void fButtonShift(struttura *str);
void fButtonAltGR(struttura *str);
void fButtonDelete(struttura *str);
void fButtonSpace(struttura *str);
void fButtonEnter(struttura *str);
void fButtonTP(struttura *str);
void fButtonTab(struttura *str);
void fButtonIns(struttura *str);




void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);

typedef void(*funz)(struttura *);
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);
funz listF_Buttons[NUM_OF_BUTTONS]={fButtonDelete,fButtonEnter,fButtonShift,fButtonTab,fButtonAltGR,fButtonTP,fButtonSpace,fButtonIns};

void fButtonShift(struttura *str)
/*Funzione associata al tasto shift => setta lo shift */
{
 str->modifier=(str->modifier!=XK_ISO_Level5_Shift)?XK_ISO_Level5_Shift:0;  
}


void fButtonAltGR(struttura *str)
/*Funzione associata al tasto 'AltGr' */
{
 str->modifier=(str->modifier!=XK_Control_R)?XK_Control_R:0;  
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
  sendKey(str->display,str->win,str->winRoot,XK_Tab,0);
}
void fButtonIns(struttura *str)
/*Funzione associata al tasto inseriemento nuova parola */
{
  sendKey(str->display,str->win,str->winRoot,XK_Tab,0);
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
   XGetInputFocus(str->display, &str->win, &revert);
   writeSimbol(str,&str->listButton[button]);
  }
 else
   listF_Buttons[((button==9)?0:button-10)](str);
}


void finestra(struttura *str)
{
 GtkWidget *window, *frame, *box, *table;
 GtkWidget **buttonArray;
 
 int i, x, y;
 char s[NUM_OF_BUTTONS][10]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","←","0 ","↵","⇧ shift","tab","alt gr","tp","space","ins"}; 
 char s_i[3];
 gtk_init (NULL,NULL);

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
    g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(button_clicked),str);
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
 

 // Get the root window for the current display.
 str.winRoot = XDefaultRootWindow(str.display);
 finestra(&str); 
 
 XCloseDisplay(str.display);
 return 0;
}
 
