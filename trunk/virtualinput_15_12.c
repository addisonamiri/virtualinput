/*virtualinput aggiornato 15/12/2010 */ 
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_OF_COLUMNS 3
#define NUM_OF_ROWS 4
#define NUM_OF_BUTTONS 12
#define NameProgramm "virtualKey"
#define LENB 5  /*LENB=LENB2,LENB3,LENB4,LENB6,LENB7,LENB8,LENB9*/
#define LENB1 8
#define LENB5 4
#define LENB10 19

typedef struct Tlbutton{
 int len;
 int status;
 int *simbol;
}lbutton;

typedef struct tstruttura{
 Display *display;
 Window win;
 Window winRoot;
 lbutton listButton[10];
}struttura;
 
 
 
 
void fButton1(struttura *str);
void fButton2(struttura *str);
void fButton3(struttura *str);
void fButton4(struttura *str);
void fButton5(struttura *str);
void fButton6(struttura *str);
void fButton7(struttura *str);
void fButton8(struttura *str);
void fButton9(struttura *str);
void fButton10(struttura *str);
void fButton11(struttura *str);
void fButton12(struttura *str);
void sendKey(Display *display, Window &win, Window &winRoot,int keycode,int state);
void SendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);

typedef void(*funz)(struttura *);
Status sendKeyEvent(Display *display, Window &win, Window &winRoot, int type, int keycode, int modifiers);
funz listF_Buttons[12]={fButton1,fButton2,fButton3,fButton4,fButton5,fButton6,fButton7,fButton8,fButton9,fButton10,fButton11,fButton12};

//int mappaChar[10][15]={,'"','\','/','(',')','?','^','-','+','*','#',

void fButton1(struttura *str)
/*Funzione associata al tasto 1*/
{
 sendKey(str->display,str->win,str->winRoot,str->listButton[0].simbol[3],str->listButton[0].status); 
 //printf("1\n");
}

void fButton2(struttura *str)
/*Funzione associata al tasto 2*/
{
 int keycode=XK_2;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("2\n");
}

void fButton3(struttura *str)
/*Funzione associata al tasto 3 */
{
 int keycode=XK_3;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("3\n");
}

void fButton4(struttura *str)
/*Funzione associata al tasto 4 */
{
 int keycode=XK_4;
 sendKey(str->display,str->win,str->winRoot,keycode,0);
 //printf("4\n");
}

void fButton5(struttura *str)
/*Funzione associata al tasto 5*/
{
 int keycode=XK_5;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("5\n");
}

void fButton6(struttura *str)
/*Funzione associata al tasto 6*/
{
 int keycode=XK_6;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("6\n");
}

void fButton7(struttura *str)
/*Funzione associata al tasto 7*/
{
 int keycode=XK_7;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("7\n");
}

void fButton8(struttura *str)
/*Funzione associata al tasto 8*/
{
 int keycode=XK_8;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("8\n");
}

void fButton9(struttura *str)
/*Funzione associata al tasto 9*/
{
 int keycode=XK_9;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("9\n");
}

void fButton10(struttura *str)
/*Funzione associata al tasto '*' */
{
 int keycode=42;//XK_sterling;
 sendKey(str->display,str->win,str->winRoot,54,0); 
}

void fButton11(struttura *str)
/*Funzione associata al tasto 0 */
{
 int keycode=XK_0;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("11\n");
}

void fButton12(struttura *str)
/*Funzione associata al tasto '#' */
{
 for(int i=0;i<10;i++){
   for(int j=0;j<str->listButton[i].len;j++)
     printf("%c",str->listButton[i].simbol[j]);
   printf("\n");
}
 int keycode=XK_Left;
 sendKey(str->display,str->win,str->winRoot,keycode,0); 
 //printf("12\n");
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
void button_clicked(GtkWidget *widget,struttura *str)
{ 
 // Find the window which has the current keyboard focus.
 int revert;
 XGetInputFocus(str->display, &str->win, &revert);
 listF_Buttons[atoi(gtk_widget_get_name(widget))](str);
}

void setshift(GtkWidget *widget, gpointer data)
{
 int modifier;
 	if(modifier==1)
		modifier=0;
	else
		modifier=1;
}

void finestra(struttura *str)
{
 GtkWidget *window, *frame, *box, *table;
 GtkWidget **buttonArray;
 
 int i, x, y;
 char s[12][7]={"1","2 abc","3 def","4 ghi","5 jkl","6 mno","7 pqrs","8 tuv","9 wxyz","*","0 +","#"};
 char s_i[2];
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
    if(i==9)
	g_signal_connect(G_OBJECT(buttonArray[i]), "clicked", G_CALLBACK(setshift), NULL);
    else
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

int *setListButton(int len,int status,int *vet,lbutton &l)
{
 l.len=len;
 l.status=status;
 return vet;
}

void stampa(int *v,int len)
{
 int i;
 for(i=0;i<len;i++)
   printf("%c",v[i]);
 printf("\n");
}

int main(int argc, char *argv[])
{
 struttura str;
 str.display = XOpenDisplay(0);
 int i;
 //int vetb1[LENB1]={XK_quotedbl,XK_comma,XK_plus,XK_minus,XK_less,XK_quoteright,XK_period};
 /*;\;,;+;-;<;';.;*/
 //int vetb10[LENB10]={XK_bar,XK_exclam,XK_2,XK_3,XK_dollar,XK_percent,XK_ampersand,XK_slash,XK_parenleft,XK_parenright,XK_equal,XK_question,XK_asciicircum,XK_asterisk,XK_greater,XK_semicolon,XK_colon,XK_underscore};
 /*,|,!,£,$,&,/,(,),=,?,^,*,>,;,:,_, */
 //int vetb5[LENB5]={XK_j,XK_k,XK_l,XK_5};                /* j,j,l,5 */
 int keycodemap[LENB*7+LENB1+LENB5+LENB10]={XK_1,XK_quotedbl,XK_comma,XK_plus,XK_minus,XK_less,XK_quoteright,XK_period,
                                          /*;1;\;,;+;-;<;';.;*/
                                          XK_a,XK_b,XK_c,XK_agrave,XK_2,/* a,b,c,à,2*/
                                          XK_d,XK_e,XK_f,XK_egrave,XK_3,/* d,e,f,è,3*/
                                          XK_g,XK_h,XK_i,XK_igrave,XK_4,/* g,h,i,ì,4*/
                                          XK_j,XK_k,XK_l,XK_5,                /* j,j,l,5 */
                                          XK_m,XK_n,XK_o,XK_ograve,XK_6,/* m,n,o,ò,6*/
                                          XK_p,XK_q,XK_r,XK_s,XK_7,     /* p,q,r,s,7*/
                                          XK_t,XK_u,XK_v,XK_ugrave,XK_8,/* t,u,v,ù,8*/
                                          XK_w,XK_x,XK_y,XK_z,XK_9,     /* w,x,y,z,9*/
                                          XK_0,XK_bar,XK_exclam,XK_2,XK_3,XK_dollar,XK_percent,XK_ampersand,XK_slash,XK_parenleft,XK_parenright,XK_equal,XK_question,XK_asciicircum,XK_asterisk,XK_greater,XK_semicolon,XK_colon,XK_underscore
 /* ,0,|,!,£,$,&,/,(,),=,?,^,*,>,;,:,_, */
                         };
 int sup; 
 //str.listButton[0].simbol=setListButton(LENB1,0,keycodemap,str.listButton[0]);  
 str.listButton[0].simbol=setListButton(LENB1,0,keycodemap,str.listButton[0]);
 stampa(str.listButton[0].simbol,str.listButton[0].len);
 for(i=1,sup=LENB1;i<4;i++,sup+=LENB){
  str.listButton[i].simbol=setListButton(LENB,str.listButton[i].status,keycodemap+sup,str.listButton[i]);
  
  stampa(str.listButton[i].simbol,str.listButton[i].len);
 }
 str.listButton[4].simbol=setListButton(LENB5,0,keycodemap+sup,str.listButton[4]);
 stampa(str.listButton[i].simbol,str.listButton[i].len);
 for(i++,sup+=LENB5;i<9;i++,sup+=LENB){
  str.listButton[i].simbol=setListButton(LENB,str.listButton[i].status,keycodemap+sup,str.listButton[i]);
 stampa(str.listButton[i].simbol,str.listButton[i].len);
 }
 str.listButton[9].simbol=setListButton(LENB10,1,keycodemap+sup,str.listButton[9]);
 stampa(str.listButton[9].simbol,str.listButton[9].len);

 printf("%d   %d\n",str.listButton[1].simbol[0],XK_a);
 // Get the root window for the current display.
 str.winRoot = XDefaultRootWindow(str.display);
 finestra(&str); 
 XCloseDisplay(str.display);
 return 0;
}
 
