// ========================================================
//           Muhammad Mufid Afif - 1006671766
//      U J I A N    T E N G A H    S E M E S T E R
//           G R A F I K A    K O M P U T E R
// ========================================================
//                         (c) 2013


/* Basic 2D pong with OpenGL & GLUT */


#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>


GLfloat cpuPaddle =		 175.0f;		// y awal paddle cpu
GLfloat playerPaddle =	 175.0f;		// y awal paddle player
GLfloat	ballX =			 200.0f;		// x awal bola
GLfloat ballY =			 200.0f;		// y awal bola

// How many spaces to move each frame
GLfloat step = 0.10f;
GLfloat ballStepX = -0.02f;
GLfloat ballStepY = -0.02f;

// keep track of the window height and width
GLfloat windowWidth;
GLfloat windowHeight;

int cpuScore = 0;
int playerScore = 0;
char score[50];

// Informasi memori terkait dengan alien
int maxAlien;
bool alienAlive[50];
GLfloat alienPosX[50];
GLfloat alienPosY[50];

// Informasi terkait dengan tembakan sang player
bool bulletAlive;
GLfloat bulletX;
GLfloat bulletY;

GLuint displayObjects[50];

bool gameover = false, first = true;

void createAlienDisplayList() {
    glPointSize(3.0f);
    // Buat alien jenis pertama
    // Ukuran alien: 25x25 piksel
    // Baseline di layar adalah KIRI BAWAH
    displayObjects[0] = glGenLists (1);
    glNewList (displayObjects[0], GL_COMPILE);
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin (GL_POLYGON);
        glVertex2i(0,0);
        glVertex2i(0,25);
        glVertex2i(25,25);
        glEnd ( );

        glColor3f(0.0f, 0.7f, 0.0f);
        glBegin (GL_POLYGON);
        glVertex2i(0,0);
        glVertex2i(0,10);
        glVertex2i(10,10);
        glEnd ( );

    glEndList ( );
}

void DisplayText(char *teks)
{
    char *p;
    glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0f, 0.5f, 1.0f);
    glRasterPos2f(windowWidth/2-180, windowHeight/2);         // posisikan teks
    for (p = teks; *p; p++)
           glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p); // teks ditampilkan perkarakter
    glutSwapBuffers();       
}

// Instruksi untuk alien agar bergerak tahap demi tahap
void moveAliens() {
}

// Routine untuk menggambar alien
void renderAliens() {
    for (int i = 0; i < maxAlien; i++) {
        if (alienAlive[i]) {
            glPushMatrix();    
            glTranslatef(alienPosX[i], alienPosY[i], 0.0); //translate origin (-150, 150)
            glCallList (displayObjects[0]);
            glPopMatrix(); //move origin back to center
        }
    }

}

// Gambar arena
void DisplayArena() {
	glClear(GL_COLOR_BUFFER_BIT);

    renderAliens();

	glColor3f(0.0f, 0.5f, 1.0f);

	    // Player paddle
	    glRectf(playerPaddle - 20 , 20 , playerPaddle + 20, 40);

	    glColor3f(1.0f, 0.0f, 0.0f);

	    // Bola
	    glRectf(ballX, ballY, ballX + 5, ballY + 5);
        glBegin(GL_POINTS);
        glColor3f(1.0, 1.0, 1.0);
        glVertex2f(bulletX, bulletY);
        glEnd();
	glutSwapBuffers();
}
// Cek apakah ada peluru yang mengenai alien ataupun UFO
void collisionCheck() {
}

void OnPlay() {
    // do nothing lol development mode lol
    moveAliens();
    collisionCheck();
    if (GetAsyncKeyState(VK_SPACE) & !bulletAlive) {
        bulletX = playerPaddle;
        bulletY = 20;
        bulletAlive = true;
    }
    if (bulletAlive) {
        bulletY += step;
    }
    if (bulletAlive && bulletY > windowHeight) {
        bulletAlive = false;
    }
	// Cek key input
    
	if(GetAsyncKeyState(VK_LEFT))
		playerPaddle -= step;
	

	if(GetAsyncKeyState(VK_RIGHT))
		playerPaddle += step;
	

    glutPostRedisplay(); return;
	
	//bola balik arah ketika menyentuh paddle
	if(ballX >= windowWidth-20 && ballX < windowWidth - 10)	
	{
		if(ballY >= playerPaddle && ballY <= playerPaddle + 50) // bola menyentuh posisi paddle player
		{
			ballStepX = -ballStepX;
			MessageBeep(0);
			ballStepX = ballStepX - 0.1f;	// tambah kecepatan bola
			if(ballStepY < 0)
			{
				ballStepY -= 0.01f;
			}
			else
			{
				ballStepY += 0.01f;
			}
		}
	}


	if(ballX <= 20 && ballX > 10)  // paddle cpu
	{
		if(ballY >= cpuPaddle && ballY <= cpuPaddle + 50)
			ballStepX = -ballStepX;
			MessageBeep(0);
	}


	// bola mantul
	if(ballY > windowHeight || ballY - 5 < 0)
		ballStepY = -ballStepY;

	// gerakan paddle cpu
	if(ballY < cpuPaddle + 25)
	{
		cpuPaddle -= step;
	}
	else
	{
		cpuPaddle += step;
	}

	// cek posisi bola
	if(ballX > windowWidth)
	{
        DisplayText("Permainan berakhir, Anda kalah!");
		gameover = true;
	}

	if(ballX < 0)
	{
        DisplayText("Permainan berakhir, Anda menang!");
		gameover = true;
	}


	ballX += ballStepX;
	ballY += ballStepY;

    glutPostRedisplay();
 
}

void initAliens() {
    maxAlien = 7;
    for (int i = 0; i < 7; i++) {
        alienAlive[i] = true;
        alienPosX[i] = i*70.0f + 50.0f;
        alienPosY[i] = 300.0f;
    }
}

void Init()
{
    initAliens();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}



void ChangeSize(GLsizei w, GLsizei h)
{
	if(h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w<=h)
	{
		windowHeight = 400.0f*h/w;
		windowWidth = 400.0f;
	}
	else
	{
		windowHeight = 400.0f;
		windowWidth = 400.0f*w/h;
	}

	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 1.0f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 500);
	glutCreateWindow("Simple Pong Game");
    Init();
    createAlienDisplayList();
	glutDisplayFunc(DisplayArena);
	glutReshapeFunc(ChangeSize);
    glutIdleFunc(OnPlay);
	
	glutMainLoop();

	return 0;
}
