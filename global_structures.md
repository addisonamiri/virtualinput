The following code shows the main elements used by the functions:
```
/*Structure containing all gtk widgets and variables used to PT management*/
typedef struct text_prediction{
GtkWidget *gtklist;
GtkWidget *list_item, *scrolled_window;
int statot9;//status of TP mode
int indice;
char codicet9[LEN_WORD_DB+1];
int luncodicet9;
int flagcaricat9;
int numparoletrovate;// number of words returned by database query 
struct nodo vetparole[N];
sqlite3 *db;
sqlite3_stmt *stmt;
}T_tp;

/*Structure referring buttons from 0 to 9*/
typedef struct Tlbutton{
 int len;//lenght of character
 int status;//Status modifier key
 int *simbol;//handler of character list associated to buttons
 time_t timeSelected;//last time of keypress
}lbutton;

/*Structure managing new eventually insert into dictionary*/
typedef struct tinsert{
 int on;//mode on
 int index;
 char key_numbers[LEN_WORD_DB+1];//PT code associated to word
 char new_word[LEN_WORD_DB+1];
}tins;

/*Structure storing displacement parameters of main window*/
typedef struct tForMove{
int s_width, s_height,position_height,position_width,delay_verticale,delay_orizzontale;
}tmove;

/*Structure showing elements of usability*/
typedef struct tstruttura{
 Display *display;
 Window win;
 Window winRoot;
 int modifier;
 int specialKey;/*****defined only on unstable version 0.1.2 in order to controll the simulation of more features about Alt and Ctrl buttons*****/
 lbutton listButton[10];
 int indexSimbol;/*index of last printed character from a particular button*/  
 T_tp tp;
 //Main window management
 GtkWidget *window,*mylabel,*mylabel2; 
 GtkStatusIcon *tray_icon;
 tmove move; 
 tins ins;
}struttura;
```