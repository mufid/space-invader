// ========================================================
//           Muhammad Mufid Afif - 1006671766
//      U J I A N    T E N G A H    S E M E S T E R
//           G R A F I K A    K O M P U T E R
// ========================================================
//                         (c) 2013

#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <thread>
#include "sprite.h"

GLfloat cpuPaddle =		 175.0f;		// y awal paddle cpu
GLfloat playerPaddle =	 175.0f;		// y awal paddle player
GLfloat	ballX =			 200.0f;		// x awal bola
GLfloat ballY =			 200.0f;		// y awal bola

// How many spaces to move each frame
GLfloat step = 0.30f;
GLfloat ballStepX = -0.02f;
GLfloat ballStepY = -0.02f;

int     moveTimer;

// keep track of the window height and width
GLfloat windowWidth;
GLfloat windowHeight;

int cpuScore = 0;

char score[50];

// Informasi memori terkait dengan alien
int maxAlien;
bool alienAlive[50];
GLfloat alienPosX[50];
GLfloat alienPosY[50];
int alienType[50];

// Informasi terkait dengan tembakan sang player
bool bulletAlive;
GLfloat bulletX;
GLfloat bulletY;

// Informasi terkait dengan skor dan live player
int playerScore = 0;
int nyawaPlayer = 3;

bool gameover = false, first = true;

// Hanya fungsi biasa yang mengecek apakah val ada di antara
// x1_ dan x2_
bool antara(float val, float x1_, float x2_) {
    float x1, x2;
    if (x1_ > x2_) {
        x2 = x1_;
        x1 = x2_;
    } else {
        x1 = x1_;
        x2 = x2_;
    }
    return (x1 <= val && val <= x2);
}

void drawStringText(char *teks, int posX, int posY) {
    char *p;

	glColor3f(0.0f, 0.5f, 1.0f);
    glRasterPos2f(posX, posY);
    for (p = teks; *p; p++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p); // teks ditampilkan perkarakter
    
}

void drawIntegerText(int, int, int);

void drawScoreboard() {
    glBegin(GL_POLYGON);
        glColor3f(0.9f, 0.9f, 0.9f);
        glVertex2i(0, windowHeight - 30);
        glVertex2i(windowWidth, windowHeight - 30);
        glVertex2i(windowWidth, windowHeight - 32);
        glVertex2i(0, windowHeight - 32);
    glEnd();
    drawStringText("Skor:", 20, windowHeight - 25);
    drawIntegerText(playerScore + 100, 100, windowHeight - 25);
}

void drawIntegerText(int what, int posX, int posY) {
    char p[50];
    int residu = what;
    int printNow;
    int max = ceil(log((float) what));
    int i = max;
    while (residu > 0) {
        printNow = residu % 10;
        residu = residu / 10;
        switch (printNow) {
        case 0: p[i] = '0'; break;
        case 1: p[i] = '1'; break;
        case 2: p[i] = '2'; break;
        case 3: p[i] = '3'; break;
        case 4: p[i] = '4'; break;
        case 5: p[i] = '5'; break;
        case 6: p[i] = '6'; break;
        case 7: p[i] = '7'; break;
        case 8: p[i] = '8'; break;
        case 9: p[i] = '9'; break;
        }
        i--;
    }
    p[max+1] = 0;
    drawStringText(p, posX, posY);
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
            glCallList (displayObjects[alienType[i]]);
            glPopMatrix(); //move origin back to center
        }
    }

}

// Gambar arena
void DisplayArena() {
	glClear(GL_COLOR_BUFFER_BIT);

    // Routine alien agak rumit, jadi dipisah aja
    renderAliens();

	// Player paddle
    glColor3f(0.0f, 0.5f, 1.0f);
	glRectf(playerPaddle - 20 , 20 , playerPaddle + 20, 40);

	// Peluru, hanya digambar kalau memang
    // pelurunya ditembakkan

    if (bulletAlive) {
        glBegin(GL_POINTS);
        glColor3f(1.0, 1.0, 1.0);
        glVertex2f(bulletX, bulletY);
        glEnd();
    }
    drawScoreboard();
	glutSwapBuffers();
}

// Cek apakah ada peluru yang mengenai alien ataupun UFO
void collisionCheck() {
    // Do no collision check if there's no bullet
    if (!bulletAlive) return;

    bool collisionNotYetFound = true;
    // Iterasi setiap alien, apakah ada peluru yang mengenai mereka?
    for (int i = 0; i < maxAlien && collisionNotYetFound; i++) {
        int l = 25;
        // Ingat, ukuran alien adalah 25x25 piksel.
        // Cek apakah ada peluru yang mengenai mereka
        switch (alienType[i]) {
            // Length: 25
            case 0: l = 25; break;
            // Length: 37
            case 1: l = 37; break;
            // Length: 37
            case 2: l = 37; break;
        }

        if (antara(bulletX, alienPosX[i], alienPosX[i] + l) &&
            antara(bulletY, alienPosY[i], alienPosY[i] + 25) &&
            alienAlive[i]) {
            bulletAlive = false;
            collisionNotYetFound = false;
            alienAlive[i] = false;
            playerScore++;
            printf("Alien kena! %d\n", i);
        }
    }
}

// Menggerakkan alien, posisi player, peluru, dan kebutuhan lainnya
// yang bersifat prarender
void OnPlay() {
    moveAliens();
    collisionCheck();

    // ===========================================================
    // Bagian pengaturan bullet. Sedang ditembak, mau nembak?
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

    // ============================================================
	// Bagian pengaturan posisi pemain
    
	if(GetAsyncKeyState(VK_LEFT))
		playerPaddle -= step;
	

	if(GetAsyncKeyState(VK_RIGHT))
		playerPaddle += step;
	

    // Waktunya untuk melakukan redisplay! Spartaa!
    glutPostRedisplay(); return;
}

// Melakukan inisialisasi posisi alien dan UFO
void initAliens() {
    maxAlien = 7*4;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 7; i++) {
            int k = j*7+i;
            printf("rendering: %d\n", k);
            alienAlive[k] = true;
            alienPosX[k] = i*70.0f + 50.0f;
            alienPosY[k] = 150.0f + j * 25.0f;
            int type = (j > 1) ? j - 1 : j;
            alienType[k] = type;
        }
    }
}

void Init() {
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
	glutCreateWindow("Space Invader - Mid-term Exam Muhammad Mufid Afif (1006671766)");
    Init();
    createAlienDisplayList(); // Refactor, di sprite.h
	glutDisplayFunc(DisplayArena);
	glutReshapeFunc(ChangeSize);
    glutIdleFunc(OnPlay);
	
	glutMainLoop();

	return 0;
}
