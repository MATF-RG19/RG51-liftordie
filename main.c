#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "image.h"


#define BROJ_DOZVOLJENIH_POTEZA      (290)

#define PI 3.1415926535897
#define BROJ_PREPREKA      (45)
#define DUZINA_SKRETANJA   (50)   //Pomeranje po x osi
#define DUZINA_KORAKA      (20)   //Pomeranje po z osi 
#define DUZINA_STAZE       (9000)
#define VISINA_ORMARA      (60)   //Isto sto i visina lifta


//Kretanje glavnog igraca
#define TIMER_INTERVAL     (20)
#define TIMER_INTERVAL1    (10)
#define TIMER_ID_levo      (0)
#define TIMER_ID_desno     (1)
#define TIMER_ID_napred    (2)
#define TIMER_ID_nazad     (3)
#define TIMER_ID_covek     (4)


//Slike potrebne za teksture
static GLuint names[6];

#define FILENAME0          "lift.bmp"
#define FILENAME1          "ormari.bmp"
#define FILENAME2          "grb.bmp"
#define FILENAME3          "eksplozija.bmp"
#define FILENAME4          "ss.bmp"
#define FILENAME5          "pocetni_ekran.bmp"


//Kretanje glavnog igraca
static int pokrenuta_igrica    = 0;
static int on_animation_levo   = 0;
static int on_animation_desno  = 0;
static int on_animation_napred = 0;
static int on_animation_nazad  = 0;
static int covek_aktivan  = 0;


//Uglovi za rotiranje kamere
static float pi = 3.141592653589793;
static float alfa , beta; 
static float delta_alfa , delta_beta;

//Ugao rotacije loptice
static float ugao_rotacije = 0;

//koordinate za kameru 
static int x_kam = 0;
static int y_kam = 310;
static int z_kam = 315;


//Brojac odigranih pomeranja
static int brojac_poteza = 0;
static int preostali_broj_poteza;


//Kretanje coveculjka
static double arm1_angle;
static double arm2_angle;
static double leg1_angle;
static double leg2_angle;

static int arm1_return;
static int arm2_return;
static int leg1_return;
static int leg2_return;

//Clipping ravan za lift
static float clip_parametar;
static float podizanje_lifta = 0;

//Ispis teksta
static char ispis[100];

//Indikator koji oznacava kraj igre
static int ind_za_game_over = 0;


//Struktura glavnog igraca
typedef struct{
    double x;
    double y;
    double z;
    double kraj;//daljina koju loptica moze da predje levo i desno
}POZICIJA;

//Igrac
POZICIJA lopta;

//Struktura za pojedinacnu prepreku
typedef struct{
    //Pozicija:
    float x;
    float y;
    float z;
    
    int tip_prepreke;
    
/*Tip:
 * kocka        ==> kraj
 * teapot       ==> preostali_broj_poteza + 10 
 * torus        ==> na pocetak
 * epruveta     ==> ubrazanje
 * coveculjak   ==> preostali_broj_poteza - 10
 */    
    
    int traka;
    
    //Boje:
    float r;
    float g;
    float b;
    
}PREPREKA;

PREPREKA niz_prepreka[BROJ_PREPREKA];


//Izgled ekrana pri pokretanju igrice
void start_screen()
{
    
    glPushMatrix();
                
                glColor3f(0.9,0.1,0.2);
            
                glBindTexture(GL_TEXTURE_2D, names[5]);
                glBegin(GL_POLYGON);
                    glNormal3f(0,1,0);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-55,275,270);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(-55,320,225);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(55,320,225);
                    
                    
                    glTexCoord2f(0,1);
                    glVertex3f(55,275,270);
                    
                    
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
            
    glPopMatrix();
}
    
//Jedan tip prepreke jeste epruveta(asocijacija na Hemijski fakultet)
void napravi_epruvetu()
{
    glEnable(GL_BLEND);
    
    glEnable(GL_COLOR_MATERIAL);
    
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR );
    
    
    //Cilindar
    glPushMatrix();
    
        glColor4f(0.9,0.9,0.9,0.5);
        
        GLUquadric* q = gluNewQuadric();
        
        glTranslatef(-10,3,0);
        glRotatef(-90,1,0,0);
        glRotatef(45,0,1,0);
        gluCylinder(q,3.04,3.04,18,50,50);

    glPopMatrix();
    
    //Lopta
    glPushMatrix();
        
        glColor3f(0.7,0.8,0.7);
        glTranslatef(-10,3,0);
        glRotatef(45,0,1,0);
        glutSolidSphere(3,20,20);
        
    glPopMatrix();
    
    //Manji cilindar kao tecnost u epruveti
    glPushMatrix();
        
        glColor3f(0.7,0.8,0.7);
        
        GLUquadric* q1 = gluNewQuadric();
        
        glTranslatef(-10,3.1,0.1);
        glRotatef(-90,1,0,0);
        glRotatef(45,0,1,0);
        gluCylinder(q1,3.14,3.14,10.3,20,20);
    
    glPopMatrix();
    
    glDisable(GL_COLOR_MATERIAL);
    
    glDisable(GL_BLEND);
    
        
}


//Timer za kretanje coveculjka
void timer_covek(int value)
{
    if(value != TIMER_ID_covek)
        return;
    
    
    if(arm1_angle < 120 && !arm1_return)//jos uvek treba da se krece
    {
        arm1_angle += 1.5;
        
        if(arm1_angle == 120)
            arm1_return = 1;
    }
    else//inace,vraca se istom putanjom
    {
        arm1_angle -= 1.5;
        
        if(arm1_angle == 60)
            arm1_return = 0;
    }
    
    //ARM 2 ==> arm2_angle & arm2_return
    if(arm2_angle < 120 && !arm2_return)
    {
        arm2_angle += 1.5;
        
        if(arm2_angle == 120)
            arm2_return = 1;
    }
    else
    {
        arm2_angle -= 1.5;
            
        if(arm2_angle == 60)
            arm2_return = 0;
    }
    
    //LEG1 ==> leg1_angle & leg1_return
    if(leg1_angle < 30 && !leg1_return)
    {
        leg1_angle += 2;
        
        if(leg1_angle == 30)
            leg1_return = 1;
    }
    else
    {
        leg1_angle -= 2;
        
        if(leg1_angle == -30)
            leg1_return = 0;
    }
    
    
    //LEG 2 ==> leg2_angle & leg2_return
    if(leg2_angle > -30 && !leg2_return)
    {
        leg2_angle -= 2;
        
        if(leg2_angle == -30)
            leg2_return = 1;
    }
    else
    {
        leg2_angle += 2;
        
        if(leg2_angle == 30)
            leg2_return = 0;
    }
    
        
    if(covek_aktivan)
        glutTimerFunc(TIMER_INTERVAL1 , timer_covek , TIMER_ID_covek);
    
    

}


//Crtanje coveculjka
//Ruke i noge imaju svoje pokrete(zadati su krajnji uglovi izmedju kojih se krecu)
//ind_kretanja - za razlikovanje coveculjka koji se krece od onog koji miruje
//r,g,b        - za dobijanje razlicitih boja prilikom prikazivanja
void napravi_coveculjka(int ind_kretanja,float r,float g,float b)
{    
    
    //Telo = kvadar
    glPushMatrix();
        if(ind_kretanja == 1)
            glColor3f(1,1,1);
        else if(ind_kretanja == 2)
            glColor3f(0.3 , 0.3 , 0.3);
        else
            glColor3f(0.1 +g, 0.2+r , 0.9-b);
    
        glTranslatef(-1.3, 0, 0.5);
        glScalef(0.3,0.30,0.5);
        glutSolidCube(1);
    glPopMatrix();

    //Glava
    glPushMatrix();
        if(ind_kretanja == 1)
            glColor3f(0.8,0.9,0.8);
        else if(ind_kretanja == 2)
            glColor3f(0.7 , 0.8 , 0.7);
        else
            glColor3f(0.5 , 0.6 , 0.5);
    
        glTranslatef(-1.3,0,0.85);
        glScalef(0.1,0.1,0.1);
        glutSolidSphere(1, 20,10);
    glPopMatrix();
    
    //leva ruka
    glPushMatrix();
        if(ind_kretanja == 1)
            glColor3f(0.9,0.3,0.1);
        else if(ind_kretanja == 2)
            glColor3f(0.1 , 0.8 , 0.2);
        else
            glColor3f(0.2+r , 0.2+b , 0.3+g);
    
        glTranslatef(-1.1,0,0.55);
        
        //Pokreti:
        if(ind_kretanja)
        {
            
            glTranslatef(0,0,0.2);
            glRotatef(-arm1_angle,1,0,0);
            glTranslatef(0,0,-0.2);
        }
        glScalef(0.1,0.1,0.4);
        glutSolidCube(1);
    glPopMatrix();

    //desna ruka
    glPushMatrix();
        if(ind_kretanja == 1)
            glColor3f(0.9,0.3,0.1);
        else if(ind_kretanja == 2)
            glColor3f(0.1 , 0.8 , 0.2);
        else
            glColor3f(0.2+r , 0.2+b , 0.3+g);
    
        glTranslatef(-1.5,0,0.55);
        
        //Pokreti:
        if(ind_kretanja && ind_kretanja != 2)
        {
            glTranslatef(0,0,0.2);
            glRotatef(-arm2_angle,1,0,0);
            glTranslatef(0,0,-0.2);   
        }
        glScalef(0.1,0.1,0.4);
        glutSolidCube(1);
    glPopMatrix();

    //desna noga
    glPushMatrix();
        
        glTranslatef(-1.4,0,0.05);
        
        //Pokreti:
        if(ind_kretanja && ind_kretanja != 2)
        {
            glTranslatef(0,0,0.2);
            glRotatef(-leg2_angle,1,0,0);
            glTranslatef(0,0,-0.2);   
        }
        glScalef(0.12,0.13,0.4);
        glutSolidCube(1);
    glPopMatrix();

    //leva noga
    glPushMatrix();
        glTranslatef(-1.2,0,0.05);
        
        //Pokreti:
        if(ind_kretanja && ind_kretanja != 2)
        {
            glTranslatef(0,0,0.2);
            glRotatef(-leg1_angle,1,0,0);
            glTranslatef(0,0,-0.2);
        }
        glScalef(0.12,0.13,0.4);
        glutSolidCube(1);
    glPopMatrix();
    
}
        


//Ispis teksta koji je fiksiran na ekran i prati kretanje loptice
void drawString(float x, float y, float z, char *string ) {
  
    glDisable(GL_LIGHTING);
    
    glRasterPos3f(x, y, z);
    glColor3f(1,1,1);

    for (char* s = string; *s != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s);
    }
    
    glEnable(GL_LIGHTING);
}


//Ekran u slucaju neuspeha(istroseni koraci/sudar sa preprekom/nestajanje lifta)
void game_over_screen()
{
    glClearColor(0,0,0,1);
    
    //Kako se ovaj slucaj ne bi poklopilo sa ostavim slucajevima
    lopta.z = -2*DUZINA_STAZE;
 
    x_kam = lopta.x + 5;
    y_kam = lopta.y + 10;
    z_kam = lopta.z + 15;
    
}

//Mali hack: Ako se krene unazad (i udje u lift za hemijski fakultet)
void hack_screen()
{
    glClearColor(0.5,0,0,1);
    
    x_kam = lopta.x + 10;
    y_kam = 5*cos(ugao_rotacije*PI/180);
    z_kam = 1.2*lopta.z + 30;
    
}

//Ukoliko se udje u lift na kraju hodnika ==> uspeh
void winner_screen()
{
    glClearColor(0.1,0.9,0.1,1);
    
    //Kako ne bi bilo prelaza iz winner_screen u game_over_screen
    brojac_poteza = BROJ_DOZVOLJENIH_POTEZA/2;
    
    x_kam = lopta.x + 5;
    y_kam = lopta.y + 10;
    z_kam = lopta.z + 15;
}


//Kretanje glavnog igraca(lopta) i clipping ravni zaduzene za hodnik
//on_animation parametri se vracaju na 0 jer se broje potezi
//Kada skrecem levo ili desno, u isto vreme se pomeram napred!!!
void timer_movement(int value)
{
    if(value != TIMER_ID_levo && value != TIMER_ID_desno && value != TIMER_ID_napred && value != TIMER_ID_nazad)
        return;

    
    //Clipping ravan
    if(lopta.z > -300)//Na pocetku clipping ravan sporije ide
        clip_parametar -= 21;
    else              //Kasnije clipping ravan ubrzava
        clip_parametar -= 29;
    
    if(clip_parametar <= -DUZINA_STAZE+1)
        clip_parametar = -DUZINA_STAZE+1;
    
    
    //Ugao za koji loptica rotira
    ugao_rotacije += 20;

    
    
    //skretanje desno
    if(value == TIMER_ID_desno )
    {
        if(lopta.x < DUZINA_SKRETANJA)
        {
            on_animation_desno = 0;
        
            glutPostRedisplay();

            if(on_animation_desno)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);	
        }
        //Ako sam vec u desnoj traci,nastavljam pravo da se krecem po njoj
        else if(lopta.x == DUZINA_SKRETANJA)
        {
            lopta.z -= 10;
            
            on_animation_desno = 0;
        
            glutPostRedisplay();

            if(on_animation_desno)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);	
        }
        
    }
    //skretanje levo
    else if(value == TIMER_ID_levo)
    {
        if(lopta.x > -DUZINA_SKRETANJA)
        {
            on_animation_levo = 0;
        
            glutPostRedisplay();

            if(on_animation_levo)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);	
        }
        //Ako sam vec u levoj traci,nastavljam pravo da se krecem po njoj
        else if(lopta.x == -DUZINA_SKRETANJA)
        {
            lopta.z -= 10;
            
            on_animation_levo = 0;
        
            glutPostRedisplay();

            if(on_animation_desno)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);	
        }
    }
    //kretanje pravo
    else if(value == TIMER_ID_napred)
    {
        on_animation_napred = 0;
        
        glutPostRedisplay();
            
        if(on_animation_napred)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_napred);	
    

    }
    //kretanje unazad
    else if(value == TIMER_ID_nazad)
    {
        on_animation_nazad = 0;
        
        glutPostRedisplay();
            
        if(on_animation_nazad)
            glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_nazad);
    }
      
            
}


//Davanje osobina preprekama
void napravi_prepreke()
{
    //Random odredjivanje trake u kojoj ce se nalaziti prepreka i tipa prepreke
    int rand_traka;
    int tip_rand;
    
    
    for( int i = 0 ; i < BROJ_PREPREKA ; i++)
    {
        //traka = desna(0) / srednja(1) / leva(2)
        rand_traka = rand()%3;
        //tip = kocka(0) / cajnik(1) / torus(2) / epruveta(3) / coveculjak(4)
        tip_rand   = rand()%5;
        
        //traka
        niz_prepreka[i].traka = rand_traka;
        
        //x (zavisi od trake)
        if(rand_traka == 0)
            niz_prepreka[i].x = -DUZINA_SKRETANJA;
        else if(rand_traka == 1)
            niz_prepreka[i].x = 0;
        else
            niz_prepreka[i].x = DUZINA_SKRETANJA;

        //y
        niz_prepreka[i].y = 5;
        
        //z
        niz_prepreka[i].z =  (i+1)*200;
        
        //tip_prepreke
        niz_prepreka[i].tip_prepreke = tip_rand;
        
        
        //boja
        niz_prepreka[i].r = (float)rand()/RAND_MAX + rand_traka/10.0;
        niz_prepreka[i].g = (float)rand()/RAND_MAX + 2*rand_traka/10.0;
        niz_prepreka[i].b = (float)rand()/RAND_MAX + rand_traka/10.0;
    }
}


//Crtanje prepreka
void nacrtaj_prepreke()
{
    int i;
    
    glEnable(GL_COLOR_MATERIAL);
    
    for(i = 0 ; i < BROJ_PREPREKA ; i++)
    {
        glPushMatrix();
            glColor3f(niz_prepreka[i].r,niz_prepreka[i].g,niz_prepreka[i].b);
            
            glTranslatef(niz_prepreka[i].x,niz_prepreka[i].y,-niz_prepreka[i].z);
            
            switch(niz_prepreka[i].tip_prepreke)
            {
                case 0:
                    glEnable(GL_COLOR_MATERIAL);
                   
                    glutSolidCube(25);
                    
                    glDisable(GL_COLOR_MATERIAL);
                    break;
                case 1:
                    glutSolidTeapot(13);
                    break;
                case 2:
                    glTranslatef(0,4,0);
                    glRotatef(145, 1, 0, 0);
                    glutSolidTorus(5, 8, 36, 36);
                    break;
                case 3:
                    glDisable(GL_LIGHTING);
                    
                    napravi_epruvetu();
                    
                    glEnable(GL_LIGHTING);
                    break;
                case 4:
                    glDisable(GL_LIGHTING);
                    
                    glScalef(33,40,30);
                    glRotatef(-90 , 1,0,0);
                    napravi_coveculjka(0,niz_prepreka[i].r,niz_prepreka[i].g,niz_prepreka[i].b);
                    
                    glEnable(GL_LIGHTING);
                    break;
                default:
                    break;
            }
        glPopMatrix();
    
                
    }
    
}


//Pravljenje glavnog dela okruzenja igraca
void napravi_hodnik()
{
    glDisable(GL_LIGHTING);
    
    //'Podizanje' lifta kada clipping ravan dodje do kraja staze
    if(clip_parametar <= -DUZINA_STAZE+1)
        podizanje_lifta += 1;
    
    double clip_plane_lift[] = { 0 , -1 , 0 , VISINA_ORMARA - podizanje_lifta};
    
    glClipPlane(GL_CLIP_PLANE1 , clip_plane_lift);
    glEnable(GL_CLIP_PLANE1);
    
    //LIFT
    glPushMatrix();
                
                glColor3f(0.5,0.5,0.5);
            
                glBindTexture(GL_TEXTURE_2D, names[0]);
                glBegin(GL_POLYGON);
                    glNormal3f(0,0,1);
                
                    glTexCoord2f(0.2,0);
                    glVertex3f(-120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(120,VISINA_ORMARA,-DUZINA_STAZE);
                    
                    glTexCoord2f(0.2,1);
                    glVertex3f(-120,VISINA_ORMARA,-DUZINA_STAZE);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
            
    glPopMatrix();
    
    glDisable(GL_CLIP_PLANE1);
    
    
    //ORMARI
    glPushMatrix();
                
                glColor3f(1,1,1);
                
                glBindTexture(GL_TEXTURE_2D, names[1]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-120,0,0);
                    
                    glTexCoord2f(30,0);
                    glVertex3f(-120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(30,1);
                    glVertex3f(-120,VISINA_ORMARA,-DUZINA_STAZE);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(-120,VISINA_ORMARA,0);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glBindTexture(GL_TEXTURE_2D, names[1]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(120,0,0);
                    
                    glTexCoord2f(30,0);
                    glVertex3f(120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(30,1);
                    glVertex3f(120,VISINA_ORMARA,-DUZINA_STAZE);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(120,VISINA_ORMARA,0);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
    glPopMatrix();
    
    
    //PLAFON
    glPushMatrix();
                
                glColor3f(1,1,1);
                
                glBindTexture(GL_TEXTURE_2D, names[2]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-120,VISINA_ORMARA,0);
                    
                    glTexCoord2f(20,0);
                    glVertex3f(-120,VISINA_ORMARA,-DUZINA_STAZE);
                    
                    glTexCoord2f(20,1);
                    glVertex3f(120,VISINA_ORMARA,-DUZINA_STAZE);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(120,VISINA_ORMARA,0);
                    
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
    glPopMatrix();
    
    
    //HEMIJSKI FAKULTET ~ eksplozija
    glPushMatrix();
                
                glColor3f(1,1,1);
    
                glBindTexture(GL_TEXTURE_2D, names[3]);
                glBegin(GL_POLYGON);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-118,0,-1300);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(-118,0,-1520);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(-118,VISINA_ORMARA,-1520);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(-118,VISINA_ORMARA,-1300);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glBindTexture(GL_TEXTURE_2D, names[3]);
                glBegin(GL_POLYGON);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-118,0,-2870);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(-118,0,-3220);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(-118,VISINA_ORMARA,-3220);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(-118,VISINA_ORMARA,-2870);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glBindTexture(GL_TEXTURE_2D, names[3]);
                glBegin(GL_POLYGON);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-118,0,-5270);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(-118,0,-5550);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(-118,VISINA_ORMARA,-5550);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(-118,VISINA_ORMARA,-5270);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
    glPopMatrix();
    
    
    //STUDENTSKA SLUZBA
    glPushMatrix();
                
                glColor3f(1,1,1);
                
                glBindTexture(GL_TEXTURE_2D, names[4]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(119,0,-3860);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(119,0,-4028);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(119,VISINA_ORMARA,-4028);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(119,VISINA_ORMARA,-3860);
                    
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                
    glPopMatrix();
    
    
    //Prvi covek u redu za STUDENTSKU SLUZBU - miruje
    glPushMatrix();
        glTranslatef(119.3 , 0 , -3830);
        glScalef(29,42,32);
        glRotatef(-90 , 1,0,0);
        napravi_coveculjka(0,0,0,0);
    glPopMatrix();
    
    //Drugi covek u redu za STUDENTSKU SLUZBU - mase desnom rukom
    glPushMatrix();
        glColor3f(0.1,0.9,0.1);
        
        glTranslatef(40 , 0 , -3775);
        glScalef(31,41,30);
        glRotatef(-90 , 1,0,0);
        glRotatef(180 , 0,0,1);
        napravi_coveculjka(2,0,0,0);
    glPopMatrix();
    
    //Covek u prvoj ekspolziji - ima kretnju
    glPushMatrix();
        glTranslatef(-119 , 0 , -1366);
        glScalef(30,45,39);
        glRotatef(-90 , 1,0,0);
        glRotatef( 90 , 0,0,1);
        napravi_coveculjka(1,0,0,0);
    glPopMatrix();
    
    //Covek u trecoj ekspolziji - ima kretnju
    glPushMatrix();
        glTranslatef(-118.7 , 0 , -5346);
        glScalef(28,44,36);
        glRotatef(-90 , 1,0,0);
        glRotatef( 90 , 0,0,1);
        napravi_coveculjka(1,0,0,0);
    glPopMatrix();
    
    
    glEnable(GL_LIGHTING);
    
}




static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Kamera prati lopticu
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        7*cos(beta)*cos(alfa)+x_kam, 
        7*cos(beta)*sin(alfa)+y_kam,
        7*sin(beta)+lopta.z+z_kam,
        0, 5 ,lopta.z+5,
        0, 1, 0);
    
    
    if(!pokrenuta_igrica)
        start_screen();
    
    
    //Koeficijenti za osvetljenje igraca:
    GLfloat material_ambient[]  = { 0.1, 0.7, 0.1, 1 };
    GLfloat material_diffuse[]  = { 0.4, 0.4, 0.1, 1 };
    GLfloat material_specular[] = { 1, 1, 1, 1 };
    GLfloat high_shininess[]    = { 80 };
    GLfloat no_material[]       = { 0, 0, 0, 1 };
    
    //Koeficijenti za osvetljenja  srednje staze:
    GLfloat material_ambient1[]  = { 0.4, 0.4, 0.4, 1 };
    GLfloat material_diffuse1[]  = { 0.3, 0.3, 0.3, 1 };
    GLfloat material_specular1[] = { 1, 1, 1, 1 };
    GLfloat high_shininess1[]    = { 100 };
    GLfloat no_material1[]       = { 0, 0, 0, 1 };
    
    //Koeficijenti za osvetljenja  leve i desne staze:
    GLfloat material_ambient2[]  = { 0.1, 0.7, 0.4, 1 };
    GLfloat material_diffuse2[]  = { 0.9, 0.3, 0.9, 1 };
    GLfloat material_specular2[] = { 1, 1, 1, 1 };
    GLfloat high_shininess2[]    = { 100 };
    GLfloat no_material2[]       = { 0, 0, 0, 1 };
    
    
    
    //Igrac
    glPushMatrix();
    
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);//prava boja sfere
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);//odredjuje boju svetlosti
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);//isticanje materijala
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);//veliki odsjaj
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material);
        
        
        glTranslatef(lopta.x, lopta.y, lopta.z );
        glRotatef(ugao_rotacije , 1 , 0 , 0);
        glColor3f(0,0.5,0.1);
        glutWireSphere(7,250,250);
        
    glPopMatrix();
    
    
    //srednja linija podloge
    glPushMatrix();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess1);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material1);
        
        
        glColor3f(1,1,0.3);
        glBegin(GL_POLYGON);
            glVertex3f( 30,  0,  100);
            glVertex3f(-30,  0,  100);
            glVertex3f(-30,   0, -DUZINA_STAZE);
            glVertex3f( 30,   0, -DUZINA_STAZE);
        glEnd();
        
    glPopMatrix();
    
    
    //leva linija podloge
    glPushMatrix();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess2);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material2);
        
        
        glColor3f(0.5,0.7,0.5);
        glBegin(GL_POLYGON);
            glVertex3f(-30,  0,  100);
            glVertex3f(-120,  0,  100);
            glVertex3f(-120,   0, -DUZINA_STAZE);
            glVertex3f(-30,   0, -DUZINA_STAZE);
        glEnd();
        
    glPopMatrix();
    
    
    //desna linija podloge
    glPushMatrix();
    
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess2);
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material2);
        
        glColor3f(0.5,0.7,0.5);
        glBegin(GL_POLYGON);
            glVertex3f( 30,  0,  100);
            glVertex3f( 120,  0,  100);
            glVertex3f( 120,   0, -DUZINA_STAZE);
            glVertex3f( 30,   0, -DUZINA_STAZE);
        glEnd();

    glPopMatrix();
    
    
    //Prepreke 
    glPushMatrix();
        nacrtaj_prepreke();
    glPopMatrix();
    
    
    //Clipping ravan za hodnik
    double clip_plane[] = {0,0,-1 ,clip_parametar};
    glClipPlane(GL_CLIP_PLANE0 , clip_plane);
    
    glEnable(GL_CLIP_PLANE0);
        napravi_hodnik();
    glDisable(GL_CLIP_PLANE0);
    
        
        
    //Pomeranje loptice napred prilikom svakog pokreta levo ili desno
    //Osim ako se udje u hack_screen
    if(lopta.z <= 10)
        lopta.z -= 20;
    
    //Pocetni polozaj
    glTranslatef(0,0,lopta.z);
    
    
    //Ispis teksta i racunanje pokreta
    preostali_broj_poteza = BROJ_DOZVOLJENIH_POTEZA-brojac_poteza;
    
    //Igrac ima jos poteza ; Igrac je jos uvek je na stazi ; Lift jos uvek nije otisao
    if(preostali_broj_poteza > 0 && (lopta.z < 40 && lopta.z > DUZINA_STAZE*(-1)) && podizanje_lifta < VISINA_ORMARA)
    {
        sprintf(ispis, "PREOSTALI BROJ POTEZA: %d" ,preostali_broj_poteza);
    }
    //Igrac nema vise poteza / Udario je u kocku / Lift je otisao 
    else if(preostali_broj_poteza <= 0 || ind_za_game_over == 1 || podizanje_lifta >= VISINA_ORMARA || (preostali_broj_poteza <= 0 && lopta.z < DUZINA_STAZE*(-1)))
    {
        if(podizanje_lifta >= VISINA_ORMARA)
            sprintf(ispis, "LIFT JE OTISAO,ALI TE STEPENICE CEKAJU! :(");
        else if(ind_za_game_over == 1)
            sprintf(ispis, "KRAJ IGRE!");
        game_over_screen();
    }
    //Vratio se unazad odmah na pocetku
    else if(lopta.z >= 40 && preostali_broj_poteza > 0)
    {
        sprintf(ispis, "LIFT HEMIJSKOG FAKULTETA ");
        
        hack_screen();
    }
    //Igra u toku
    else if(preostali_broj_poteza > 0 && lopta.z <= DUZINA_STAZE*(-1) && podizanje_lifta < VISINA_ORMARA)
    {
        sprintf(ispis, "USLI STE U LIFT!!! :)");
        
        winner_screen();
    }

    //Ispis teksta
    drawString(55,20,-20,ispis);
    
    
    glutSwapBuffers();
}



        

static void initialize(void)
{
    glClearColor(0.75,0.75,0.75,0);
    
    
    srand(time(NULL));
    
    //Ambijentalna boja
    GLfloat light_ambient[] = { 0, 0, 0, 1 };
    
    //Difuzna boja
    GLfloat light_diffuse[] = { 1, 1, 1, 1 };
    
    //Spekularna boja
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    
    //MODEL OSVETLJENJA(biram ambijentalni model)
    GLfloat model_ambient[] = { 0.4, 0.4, 0.4, 1 };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
     
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
    
    
    //Lepljenje tekstura
    Image* image;
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE
            );
    
    image = image_init(0,0);
    
    
    //ORMARI
    image_read(image,FILENAME1);
    glGenTextures(6,names);
    
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    
    
    //HEMIJSKI FAKULTET
    image_read(image,FILENAME3);
    
    glBindTexture(GL_TEXTURE_2D,names[3]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    
    
    //LIFT
    image_read(image,FILENAME0);
    
    glBindTexture(GL_TEXTURE_2D,names[0]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    
    //STUDENTSKA SLUZBA
    image_read(image,FILENAME4);
    
    glBindTexture(GL_TEXTURE_2D,names[4]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
    
    //PLAFON
    image_read(image,FILENAME2);
    
    glBindTexture(GL_TEXTURE_2D,names[2]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);

    
    //POCETNI EKRAN
    image_read(image,FILENAME5);
    
    glBindTexture(GL_TEXTURE_2D,names[5]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,image->width,image->height,0,GL_RGB,GL_UNSIGNED_BYTE,image->pixels);
    glBindTexture(GL_TEXTURE_2D,0);
      
   
    image_done(image);
    
    
    //Vrednosti uglova koji se koriste za pomeranje kamere
    alfa = 10;
    delta_alfa = pi/90;
    
    beta = 10;
    delta_beta = pi/90;
    
    
    //Kretanje coveculjka(krajnji uglovi)
    arm1_angle               =  45;
    arm2_angle               = -45;
    leg1_angle               = -10;
    leg2_angle               =  10;
    
    
    //Clipping ravan
    clip_parametar = 11;
    
    
    //Prepreke
    napravi_prepreke();
}


//Resavanje kolizija sa preprekama
int provera_sudara()
{
    //Igrac moze najvise da napreduje za 40
    //Sto znaci ako je prepreka na z = 100 , on moze da je dohvati sa lopta.z = 70/80/90/100 
    //Zato imam vise trenutnih pozicija
    int trenutna_pozicija_z = abs((int)lopta.z);
    int trenutna_pozicija_z1=trenutna_pozicija_z+20;
    int trenutna_pozicija_z2=trenutna_pozicija_z+10;
    int trenutna_pozicija_z3=trenutna_pozicija_z+30;
    int trenutna_pozicija_x = (int)lopta.x;
    
    //Indeks prepreke
    int i;
    
    if(trenutna_pozicija_z != 0 && 
        (trenutna_pozicija_z%200 == 0 || trenutna_pozicija_z1%200 == 0 ||trenutna_pozicija_z2%200 == 0 || trenutna_pozicija_z3%200 == 0) )
    {
        //Indeks zavisi od trenutne pozicije koja je ispunila uslov
        if(trenutna_pozicija_z%200 == 0)
            i = (trenutna_pozicija_z / 200) - 1;
        else if(trenutna_pozicija_z2%200 == 0)
            i = (trenutna_pozicija_z2 / 200) - 1;
        else if(trenutna_pozicija_z2%200 == 0)
            i = (trenutna_pozicija_z1 / 200) - 1;
        else
            i = (trenutna_pozicija_z3 / 200) - 1;
        
        
        //SUDAR(ovaj dodatni slucaj u zagradi je zbog dodatnih translacija coveculjka)
        if( ((int)niz_prepreka[i].x == trenutna_pozicija_x)
            || (niz_prepreka[i].tip_prepreke == 4 && niz_prepreka[i].x*trenutna_pozicija_x >=0)
        )
        {
            if(niz_prepreka[i].tip_prepreke == 0)//kocka
            {
                ind_za_game_over = 1;
                sprintf(ispis, "KRAJ IGRE!");
                //printf("kocka\n");
                glutPostRedisplay();
                return 1;
            }
            else if(niz_prepreka[i].tip_prepreke == 1)//cajnik
            {
                brojac_poteza -= 10;
                //printf("cajnik\n");
                glutPostRedisplay();
                return 0;
            }
            else if(niz_prepreka[i].tip_prepreke == 2)//torus
            {
                //printf("torus\n");
                alfa = 0; 
                beta = 0;
            
                x_kam = -5;
                y_kam = 35;
                z_kam = 30;
                
                lopta.x = 0;
                lopta.y = 5;
                lopta.z = 0;
                
                brojac_poteza = 0;
                
                glClearColor(0.5,0.5,0.5,1);
                
                glutPostRedisplay();
                    
                return 0;
            }
            else if(niz_prepreka[i].tip_prepreke == 3)//epruveta
            {
                lopta.z -= 40;
                //printf("epruveta\n");
                glutPostRedisplay();
                return 0;
            }
            else if(niz_prepreka[i].tip_prepreke == 4)//coveculjak
            {
                clip_parametar -= 10;
                brojac_poteza += 10;
                //printf("coveculjak\n");
                glutPostRedisplay();
                return 0;
            }
        }
    }
            
    
    return 0;
}


static void on_keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        //Skretanje levo
        case 'A':
        case 'a':
            if(pokrenuta_igrica)
            {
                brojac_poteza++;
                if(!on_animation_levo )
                {
                    lopta.x -= lopta.kraj;
                    if(lopta.x < -DUZINA_SKRETANJA)
                        lopta.x = -DUZINA_SKRETANJA;
                    
                    if(lopta.x < 0)
                        lopta.z -= DUZINA_KORAKA-30;
                    
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_levo = 1;
                    covek_aktivan     = 1;
                    
                    glutPostRedisplay();
                    
                    if(provera_sudara())
                    {
                        sprintf(ispis, "KRAJ IGRE!");
                        game_over_screen();
                    }
                    else
                        glutPostRedisplay();
                    
                    
                }
            }
            break;
        
        //Skretanje desno
        case 'D':
        case 'd':
            if(pokrenuta_igrica)
            {
                brojac_poteza++;
                if(!on_animation_desno)
                {   
                    lopta.x += lopta.kraj;
                    if(lopta.x > DUZINA_SKRETANJA)
                        lopta.x = DUZINA_SKRETANJA;
                    
                    if(lopta.x > 0)
                        lopta.z -= DUZINA_KORAKA-30;
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_desno = 1;
                    covek_aktivan      = 1;
                    
                    if(provera_sudara())
                    {
                        sprintf(ispis, "KRAJ IGRE!");
                        game_over_screen();
                    }
                    else
                        glutPostRedisplay();
                    
                    glutPostRedisplay();
                }
            }
            break;
        
        //Kretanje napred
        case 'W':
        case 'w':
            if(pokrenuta_igrica)
            {
                brojac_poteza++;
                if(!on_animation_napred)
                {
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_napred);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_napred = 1;
                    covek_aktivan       = 1;
                    
                    glutPostRedisplay();
                }
                
                if(provera_sudara())
                {
                        sprintf(ispis, "KRAJ IGRE!");
                        game_over_screen();
                }
                else
                    glutPostRedisplay();
            }
            break;
        
        //Kretanje unazad
        case 'S':
        case 's':
            if(pokrenuta_igrica)
            {
                if(!on_animation_nazad)
                {
                    lopta.z += 4*DUZINA_KORAKA;
                }  
                
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_nazad);

                on_animation_nazad = 1;
                
                glutPostRedisplay();
            }   
            break;
            
        //Okretanje kamere oko z-ose(desno na levo)
        case 'E':
        case 'e':
            alfa -= delta_alfa;
            if( alfa > 2*pi)
                alfa -= 2*pi;
            else if( alfa < -2*pi)
                alfa += 2*pi;
            
            //Da bi loptica ostala na svom mestu
            lopta.z += DUZINA_KORAKA;
            
            glutPostRedisplay();
            break;
        
        //Okretanje kamere oko z-ose(levo na desno)
        case 'Q':
        case 'q':
            alfa += delta_alfa;
            if (alfa > 2 * pi)  
                alfa -= 2 * pi;
            else if (alfa < 0) 
                alfa += 2 * pi;
            
            //Da bi loptica ostala na svom mestu
            lopta.z += DUZINA_KORAKA;
            
            glutPostRedisplay();
            break;
        
        //Resetovanje kamere i loptice na pocetne koordinate
        case 'R':
        case 'r':
            alfa = 0; 
            beta = 0;
            
            x_kam = -5;
            y_kam = 35;
            z_kam = 30;
            
            lopta.x = 0;
            lopta.y = 5;
            lopta.z = 0;
            
            brojac_poteza = 0;
            
            clip_parametar = 11; 

            podizanje_lifta = 0;
            
            ind_za_game_over = 0;
            
            glClearColor(0.5,0.5,0.5,1);
            
            glutPostRedisplay();
            break;
        
        //Pokretanje igrice
        case 'G':
        case 'g':
            if(!pokrenuta_igrica)
            {
                x_kam = -5;
                y_kam = 35;
                z_kam = 30;
            
                pokrenuta_igrica = 1;
                
                glutPostRedisplay();
            }
            break;
        default:
            break;
    }
    
    
    
}




static void on_reshape(int width , int height)
{
    glViewport(0,0, width , height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 120, width/(float)height , 1 , 100000 );
}



int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    
    initialize();

    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);
    
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    
    //Pocetna pozicija igraca
    lopta.x = 0;
    lopta.y = 5;
    lopta.z = -20;
    lopta.kraj = DUZINA_SKRETANJA;

    
    glutFullScreen();

    glutMainLoop();

    return 0;
}



