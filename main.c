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
    gluPerspective( 120, window_width/(float)window_height , 1 , 1000 );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0,1,25,
        0,0,0,
        0,1,0);
    
    //srednja linija
    glColor3f(1,1,0.3);
    glBegin(GL_POLYGON);
        glVertex3f( 30,  -50,  100);
        glVertex3f(-30,  -50,  100);
        glVertex3f(-30,   10, -300);
        glVertex3f( 30,   10, -300);
    glEnd();
    
    
    //leva linija
    glColor3f(0.5,0.7,0.5);
    glBegin(GL_POLYGON);
        glVertex3f(-30,  -50,  100);
        glVertex3f(-90,  -50,  100);
        glVertex3f(-90,   10, -300);
        glVertex3f(-30,   10, -300);
    glEnd();
    
   //desna
    glColor3f(0.5,0.7,0.5);
    glBegin(GL_POLYGON);
        glVertex3f( 30,  -50,  100);
        glVertex3f( 90,  -50,  100);
        glVertex3f( 90,   10, -300);
        glVertex3f( 30,   10, -300);
    glEnd();
    
    
    glPushMatrix();
        glColor3f(0,0.5,0.1);
        glutWireSphere(3,300,300);
    glPopMatrix();
    

    glutSwapBuffers();
}

static void initialize(void)
{
    glClearColor(0.75,0.75,0.75,0);
 
    
    
    glEnable(GL_DEPTH_TEST);

}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
       	    exit(0);
            break;
        case 'a':
            //skreni levo
            break;
        case 'd':
            //skreni desno
            break;
        case 'w':
            //idi napred
            break;
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
    
    glutReshapeFunc(on_reshape);
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    
    glutFullScreen();
    
    initialize();
    
    glutMainLoop();

    return 0;
}
