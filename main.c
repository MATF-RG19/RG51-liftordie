#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

static int window_width , window_height;


static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glViewport(0,0, window_width , window_height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 120, window_width/(float)window_height , 1 , 100000 );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0,10,25,
        0,0,0,
        0,1,0);
    
    
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
        
        glTranslatef(0, 0, 10);
        glColor3f(0,0.5,0.1);
        glutWireSphere(3,350,350);
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
            glVertex3f( 30,  -50,  100);
            glVertex3f(-30,  -50,  100);
            glVertex3f(-30,   10, -300000);
            glVertex3f( 30,   10, -300000);
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
            glVertex3f(-30,  -50,  100);
            glVertex3f(-120,  -50,  100);
            glVertex3f(-120,   10, -300000);
            glVertex3f(-30,   10, -300000);
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
            glVertex3f( 30,  -50,  100);
            glVertex3f( 120,  -50,  100);
            glVertex3f( 120,   10, -300000);
            glVertex3f( 30,   10, -300000);
        glEnd();
    glPopMatrix();
    
    

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
    
    glEnable(GL_DEPTH_TEST);

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
            //skreni levo
            break;
        case 'D':
        case 'd':
            //skreni desno
            break;
        case 'W':
        case 'w':
            //idi napred
            break;
        case 'S':
        case 's':
            //idi nazad
            break;
            
    }
    
}

static void on_reshape(int width , int height)
{
    window_width = width;
    window_height = height;
}


int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    
    
    initialize();
    
    
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    


    glutFullScreen();

    glutMainLoop();

    return 0;
}
