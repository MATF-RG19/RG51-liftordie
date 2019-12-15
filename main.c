#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "image.h"


#define DUZINA_STAZE       (10000)

#define TIMER_INTERVAL     (20) 
#define TIMER_ID_levo      (0)
#define TIMER_ID_desno     (1)
#define TIMER_ID_napred    (2)

#define FILENAME0          "images.bmp"
#define FILENAME1          "ormari3.bmp"

static GLuint names[2];

static int on_animation_levo = 0;
static int on_animation_desno = 0;
static int on_animation_napred = 0;

//Uglovi za rotiranje kamere
static float pi = 3.141592653589793;
static float alfa , beta; 
static float delta_alfa , delta_beta;


typedef struct{
    double x;
    double y;
    double z;
    double kraj;//daljina koju moze da predje levo i desno
}POZICIJA;



POZICIJA lopta;//igrac



void timer_movement(int value)
{
    if(value != TIMER_ID_levo && value != TIMER_ID_desno && value != TIMER_ID_napred)
        return;

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
            
}


//TODO: Trebaju mi koordinate prepreka.Ispravi ili napravi skroz novu funkciju.
void napravi_prepreke()
{
    int x_trans,y_trans,z_trans;
    x_trans = 0;
    y_trans = 0;
    z_trans = 0;
    
    for(int i = 0 ; i < 99 ; i++)
    {
        int ind  = rand() % 3;
        int ind2 = ind + 2;
        
        if(ind == 0)//0 je leva traka
            x_trans = -30;
        else if(ind == 1)//1 je srednja traka
            x_trans = 0;
        else//desna traka
            x_trans = 30;
        
        
        int size = (rand() % 15) + 8;
        
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
                
                glRotatef(145, 1, 0, 0);
                glTranslatef(x_trans*(-1),y_trans,z_trans);
                glutSolidTorus(5, 5, 36, 36);
            glPopMatrix();
        }
        
        glEnable(GL_LIGHTING);
    }
    
}



void napravi_lift()
{
    
    //LIFT
    glPushMatrix();
                glDisable(GL_LIGHTING);
                
                glColor3f(0.5,0.5,0.5);
            
                glBindTexture(GL_TEXTURE_2D, names[0]);
                glBegin(GL_POLYGON);
                    glNormal3f(0,0,1);
                
                    glTexCoord2f(0.2,0);
                    glVertex3f(-120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(120,80,-DUZINA_STAZE);
                    
                    glTexCoord2f(0.2,1);
                    glVertex3f(-120,80,-DUZINA_STAZE);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
            
                glEnable(GL_LIGHTING);
    glPopMatrix();
    
    
    //ORMARI
    glPushMatrix();
                glDisable(GL_LIGHTING);
                
                glColor3f(1,1,1);
                
                glBindTexture(GL_TEXTURE_2D, names[1]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(-120,0,-DUZINA_STAZE+300);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(-120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(-120,50,-DUZINA_STAZE);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(-120,50,-DUZINA_STAZE+300);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glBindTexture(GL_TEXTURE_2D, names[1]);
                glBegin(GL_QUADS);
                    glNormal3f(1,1,1);
                
                    glTexCoord2f(0,0);
                    glVertex3f(120,0,-DUZINA_STAZE+300);
                    
                    glTexCoord2f(1,0);
                    glVertex3f(120,0,-DUZINA_STAZE);
                    
                    glTexCoord2f(1,1);
                    glVertex3f(120,50,-DUZINA_STAZE);
                    
                    glTexCoord2f(0,1);
                    glVertex3f(120,50,-DUZINA_STAZE+300);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
                
                glEnable(GL_LIGHTING);
    glPopMatrix();
    
}




static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Kamera prati lopticu
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        7*cos(beta)*cos(alfa)-5, 7*cos(beta)*sin(alfa)+35, 7*sin(beta)+lopta.z+30,
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
    napravi_lift();
    
    //Pomeranje loptice napred prilikom svakog pokreta levo ili desno
    lopta.z -= 10;
    glTranslatef(0,0,lopta.z);
    
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
    
    //TODO: Ponavljanje teksture
    
    image = image_init(0,0);
    
    //ORMARI
    image_read(image,FILENAME1);
    glGenTextures(2,names);
    
    glBindTexture(GL_TEXTURE_2D,names[1]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);//GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);//GL_REPEAT
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
    
   
    image_done(image);
    
    
    
    alfa = 10;
    delta_alfa = pi/90;
    
    beta = 10;
    delta_beta = pi/90;
    
}





static void on_keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'A':
        case 'a':
                if(!on_animation_levo )
                {
                    lopta.x -= lopta.kraj;
                    if(lopta.x < -40)
                        lopta.x = -40;
                    
                    if(lopta.x < 0)
                        lopta.z -= 10;
                    
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_levo);
                    on_animation_levo = 1;
                }
            break;
        case 'D':
        case 'd':
                if(!on_animation_desno)
                {   
                    lopta.x += lopta.kraj;
                    if(lopta.x > 40)
                        lopta.x = 40;
                    
                    if(lopta.x > 0)
                        lopta.z -= 10;
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_desno);
                    on_animation_desno = 1;
                }
            break;
        case 'W':
        case 'w':
                if(!on_animation_napred)
                {
                    lopta.z -= 10;
                    lopta.x += 0;
                    
                    glutTimerFunc(TIMER_INTERVAL , timer_movement , TIMER_ID_napred);
                    on_animation_napred = 1;
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
            
            lopta.z += 10;
            
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
            
            lopta.z += 10;
            
            glutPostRedisplay();
            break;
        //Resetovanje kamere na pocetne koordinate
        case 'R':
        case 'r':
            alfa = 0; 
            beta = 0;
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
    
    lopta.x = 0;
    lopta.y = 5;
    lopta.z = 12;
    lopta.kraj = 40;

    glutFullScreen();

    glutMainLoop();

    return 0;
}

