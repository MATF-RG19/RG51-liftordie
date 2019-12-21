#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "image.h"


#define BROJ_DOZVOLJENIH_POTEZA      (400)

#define PI 3.1415926535897
#define DUZINA_KORAKA      (10)   //Pomeranje po z osi 
#define DUZINA_STAZE       (7000)
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
static GLuint names[5];

#define FILENAME0          "images.bmp"
#define FILENAME1          "ormari3.bmp"
#define FILENAME2          "grb-okrenut.bmp"
#define FILENAME3          "eksplozija.bmp"
#define FILENAME4          "ss2.bmp"


//Kretanje glavnog igraca
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
static int x_kam = -5;
static int y_kam = 35;
static int z_kam = 30;


//Brojac odigranih pomeranja
int brojac_poteza = 0;


//Kretanje coveculjka
static double arm1_angle;
static double arm2_angle;
static double leg1_angle;
static double leg2_angle;

static int arm1_return;
static int arm2_return;
static int leg1_return;
static int leg2_return;

static float clip_parametar;



typedef struct{
    double x;
    double y;
    double z;
    double kraj;//daljina koju loptica moze da predje levo i desno
}POZICIJA;

POZICIJA lopta;//igrac


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
void napravi_coveculjka(int ind_kretanja)
{
    
    //Telo = kvadar
    glPushMatrix();
        if(ind_kretanja == 1)
            glColor3f(1,1,1);
        else if(ind_kretanja == 2)
            glColor3f(0.3 , 0.3 , 0.3);
        else
            glColor3f(0.1 , 0.2 , 0.9);
    
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
            glColor3f(0.2 , 0.2 , 0.3);
    
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
            glColor3f(0.2 , 0.2 , 0.3);
    
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


//Ekran u slucaju neuspeha(istroseni koraci ili sudar sa preprekom)
void game_over_screen()
{
    glClearColor(0,0,0,1);
    
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
    y_kam = 5*cos(ugao_rotacije*PI/180);//*sin(ugao_rotacije);
    z_kam = 1.2*lopta.z + 30;
    
}

//Ukoliko se udje u lift na kraju hodnika
void winner_screen()
{
    glClearColor(0.1,0.9,0.1,1);
    
    //Kako ne bi bilo prelaza iz winner_screen u game_over_screen
    brojac_poteza = BROJ_DOZVOLJENIH_POTEZA/2;
    
    x_kam = lopta.x + 5;
    y_kam = lopta.y + 10;
    z_kam = lopta.z + 15;
}



void timer_movement(int value)
{
    if(value != TIMER_ID_levo && value != TIMER_ID_desno && value != TIMER_ID_napred && value != TIMER_ID_nazad)
        return;

    //Clipping ravan
    if(lopta.z > -300)//Na pocetku clipping ravan 'sporije' jede
        clip_parametar -= 11;
    else              //Kasnije clipping ravan ubrzava
        clip_parametar -= 21;
    
    if(clip_parametar <= -DUZINA_STAZE+1)
        clip_parametar = -DUZINA_STAZE+1;
    
    ugao_rotacije += 20;

    
    //skretanje desno
    if(value == TIMER_ID_desno )
    {
        if(lopta.x < 40)
        {
            on_animation_desno = 0;
        
            glutPostRedisplay();

            if(on_animation_desno)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);	
        }
        else if(lopta.x = 40)
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
        if(lopta.x > -40)
        {
            on_animation_levo = 0;
        
            glutPostRedisplay();

            if(on_animation_levo)
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);	
        }
        else if(lopta.x == -40)
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


//TODO: Trebaju mi koordinate prepreka.Ispravi ili napravi skroz novu funkciju.
void napravi_prepreke()
{
    int x_trans,y_trans,z_trans;
    x_trans = 0;
    y_trans = 0;
    z_trans = 0;
    
    for(int i = 0 ; i < 65 ; i++)
    {
        int ind  = rand() % 3;
        int ind2 = ind + 2;
        
        if(ind == 0)//0 je leva traka
            x_trans = -30;
        else if(ind == 1)//1 je srednja traka
            x_trans = 0;
        else//desna traka
            x_trans = 30;
        
        
        int size = (rand() % 15) + 6;
        
        y_trans = size;
        z_trans -= 100;
        
        
        float r,g,b;
        
        if(i % 2 == 0 && i % 5 != 0 && i % 10 != 0)
        {
            //Kocka = prepreka
            glPushMatrix();
                glDisable(GL_LIGHTING);
                
                r = (float)(rand()%10)/10;
                g = (float)(rand()%10)/10;
                b = (float)(rand()%10)/10;
                glColor3f(r,g,b);
            
                glTranslatef(x_trans,y_trans,z_trans);
                glutSolidCube(size);
                
            glPopMatrix();
        }
        
        //Cajnik = hemija
        if(i % 10 == 0)
        {
            glPushMatrix();
                glColor3f(0,0,0);
                
                glTranslatef(x_trans*(-1),y_trans,z_trans);
                glutWireTeapot(7);
            
            glPopMatrix();
        }
        
        
        //Torus = obrazac
       if(i % 5 == 0 && i % 10 != 0)
       {
            glPushMatrix();
                glColor3f(0.3,1,1);
                
                glTranslatef(x_trans*(-1),y_trans,z_trans);
                glRotatef(145, 1, 0, 0);
                glutSolidTorus(2, 5, 36, 36);
            glPopMatrix();
        }
        
        glEnable(GL_LIGHTING);
    }
    
}



void napravi_hodnik()
{
    glDisable(GL_LIGHTING);
    
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
        napravi_coveculjka(0);
    glPopMatrix();
    
    //Drugi covek u redu za STUDENTSKU SLUZBU - mase desnom rukom
    glPushMatrix();
        glColor3f(0.1,0.9,0.1);
        
        glTranslatef(40 , 0 , -3775);
        glScalef(31,41,30);
        glRotatef(-90 , 1,0,0);
        glRotatef(180 , 0,0,1);
        napravi_coveculjka(2);
    glPopMatrix();
    
    //Covek u prvoj ekspolziji - imaju kretnju
    glPushMatrix();
        glTranslatef(-119.5 , 0 , -1356);
        glScalef(25,35,30);
        glRotatef(-90 , 1,0,0);
        glRotatef( 90 , 0,0,1);
        napravi_coveculjka(1);
    glPopMatrix();
    
    //Covek u trecoj ekspolziji - imaju kretnju
    glPushMatrix();
        glTranslatef(-119.5 , 0 , -5343);
        glScalef(27,40,32);
        glRotatef(-90 , 1,0,0);
        glRotatef( 90 , 0,0,1);
        napravi_coveculjka(1);
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
        glutWireSphere(7,350,350);
        
    glPopMatrix();
    
    
    //srednja linija
    glPushMatrix();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);//prava boja
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);//odredjuje boju svetlosti
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);//isticanje materijala
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess1);//veliki odsjaj
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material1);
        
        
        glColor3f(1,1,0.3);
        glBegin(GL_POLYGON);
            glVertex3f( 30,  0,  100);
            glVertex3f(-30,  0,  100);
            glVertex3f(-30,   0, -DUZINA_STAZE);
            glVertex3f( 30,   0, -DUZINA_STAZE);
        glEnd();
        
    glPopMatrix();
    
    
    //leva linija
    glPushMatrix();
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);//prava boja
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);//odredjuje boju svetlosti
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);//isticanje materijala
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess2);//veliki odsjaj
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material2);
        
        
        glColor3f(0.5,0.7,0.5);
        glBegin(GL_POLYGON);
            glVertex3f(-30,  0,  100);
            glVertex3f(-120,  0,  100);
            glVertex3f(-120,   0, -DUZINA_STAZE);
            glVertex3f(-30,   0, -DUZINA_STAZE);
        glEnd();
        
    glPopMatrix();
    
    
    //desna linija
    glPushMatrix();
    
        glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);//prava boja
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);//odredjuje boju svetlosti
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);//isticanje materijala
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess2);//veliki odsjaj
        glMaterialfv(GL_FRONT, GL_EMISSION, no_material2);
        
        glColor3f(0.5,0.7,0.5);
        glBegin(GL_POLYGON);
            glVertex3f( 30,  0,  100);
            glVertex3f( 120,  0,  100);
            glVertex3f( 120,   0, -DUZINA_STAZE);
            glVertex3f( 30,   0, -DUZINA_STAZE);
        glEnd();

    glPopMatrix();
    
    

    napravi_prepreke();
    
    
    //Clipping ravan
    double clip_plane[] = {0,0,-1 ,clip_parametar};
    glClipPlane(GL_CLIP_PLANE0 , clip_plane);
    
    glEnable(GL_CLIP_PLANE0);
        napravi_hodnik();
    glDisable(GL_CLIP_PLANE0);
    
    //Loptica se sporije krece ako je clipping ravan prestigne
    if(clip_parametar<0 && lopta.z < 0 && clip_parametar < lopta.z)
    {
        lopta.z += 5;
    }
    
        
        
    //Pomeranje loptice napred prilikom svakog pokreta levo ili desno
    //Osim ako se udje u hack_screen
    if(lopta.z <= 10)
        lopta.z -= DUZINA_KORAKA;
    
    glTranslatef(0,0,lopta.z);
    
    char ispis[100];
    int preostali_broj_poteza = BROJ_DOZVOLJENIH_POTEZA-brojac_poteza;
    
    
    if(preostali_broj_poteza > 0 && (lopta.z < 40 && lopta.z > DUZINA_STAZE*(-1)))
    {
        sprintf(ispis, "PREOSTALI BROJ POTEZA: %d" ,preostali_broj_poteza);
    }
    else if(preostali_broj_poteza <= 0)
    {
        sprintf(ispis, "KRAJ IGRE!");
        
        game_over_screen();
    }
    else if(lopta.z >= 40 && preostali_broj_poteza > 0)
    {
        sprintf(ispis, "LIFT HEMIJSKOG");
        
        hack_screen();
    }
    else if(lopta.z <= DUZINA_STAZE*(-1) && preostali_broj_poteza >= 0)
    {
        sprintf(ispis, "USLI STE U LIFT");
        
        winner_screen();
    }
    
    if(clip_parametar<0 && lopta.z < 0 && clip_parametar < lopta.z)
    {
        
    }
    
    
    drawString(55,20,-20,ispis);
    
    
    
    glutSwapBuffers();
}



        

static void initialize(void)
{
    glClearColor(0.75,0.75,0.75,0);
    
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
    glGenTextures(5,names);
    
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
      
   
    image_done(image);
    
    
    //Vrednosti uglova koji se koriste za pomeranje kamere
    alfa = 10;
    delta_alfa = pi/90;
    
    beta = 10;
    delta_beta = pi/90;
    
    
    //Kretanje coveculjka
    arm1_angle               =  45;
    arm2_angle               = -45;
    leg1_angle               = -10;
    leg2_angle               =  10;
    
    //Clipping ravan
    clip_parametar = 11;
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
                brojac_poteza++;
                if(!on_animation_levo )
                {
                    lopta.x -= lopta.kraj;
                    if(lopta.x < -40)
                        lopta.x = -40;
                    
                    if(lopta.x < 0)
                        lopta.z -= DUZINA_KORAKA;
                    
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_levo = 1;
                    covek_aktivan     = 1;
                }
            break;
        
        //Skretanje desno
        case 'D':
        case 'd':
                brojac_poteza++;
                if(!on_animation_desno)
                {   
                    lopta.x += lopta.kraj;
                    if(lopta.x > 40)
                        lopta.x = 40;
                    
                    if(lopta.x > 0)
                        lopta.z -= DUZINA_KORAKA;
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_desno = 1;
                    covek_aktivan     = 1;
                }
            break;
        
        //Kretanje napred
        case 'W':
        case 'w':
                brojac_poteza++;
                if(!on_animation_napred)
                {
                    if(lopta.z <= 0)
                    {
                        lopta.z -= DUZINA_KORAKA;
                    }
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_napred);
                    glutTimerFunc(TIMER_INTERVAL1, timer_covek    , TIMER_ID_covek);
                    
                    on_animation_napred = 1;
                    covek_aktivan     = 1;
                }
            break;
        
        //Kretanje unazad
        case 'S':
        case 's':
                if(!on_animation_nazad)
                {
                    lopta.z += 5*DUZINA_KORAKA;
                }  
                
                glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_nazad);

                on_animation_nazad = 1;
                
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
            lopta.z = 10;
            
            brojac_poteza = 0;
            
            clip_parametar = 11; 
            
            glClearColor(0.5,0.5,0.5,1);
            
            glutPostRedisplay();
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
    
    
    //Pocetna pozicija igraca
    lopta.x = 0;
    lopta.y = 5;
    lopta.z = 10;
    lopta.kraj = 40;

    
    glutFullScreen();

    glutMainLoop();

    return 0;
}
