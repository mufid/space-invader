// ========================================================
//           Muhammad Mufid Afif - 1006671766
//      U J I A N    T E N G A H    S E M E S T E R
//           G R A F I K A    K O M P U T E R
// ========================================================
//                         (c) 2013
//
//      Original code by Mr. Dadan Hardianto "Pong Game"

#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <thread>
#include "sprite.h"
#include <time.h>
#include <stdlib.h>

GLfloat playerPaddle =	 175.0f;		// posisi awal paddle player

// How many spaces to move each frame
// Ubah step, sesuaikan dengan performa komputer.
// Step berbanding terbalik dengan performa komputer
GLfloat step = 0.30f;

// This tick is for alien random shot
int     alienShotTickMax = 1000;
int     alienMoveTick =    5000;

// keep track of the window height and width
GLfloat windowWidth;
GLfloat windowHeight;

int cpuScore = 0;

char score[50];

// Informasi memori terkait dengan alien
int     maxAlien;
bool    alienAlive[50];
GLfloat alienPosX[50];
GLfloat alienPosY[50];
int     alienType[50];

// Informasi memori terkait dengan tembakan alien
GLfloat  alienBulletX[10];
GLfloat  alienBulletY[10];
bool     alienBulletActive[10];
bool     mati = false;
bool     movementLeft = false;

// Informasi terkait dengan tembakan sang player
bool    bulletAlive;
GLfloat bulletX;
GLfloat bulletY;

// Informasi terkai dengan objek spesial: UFO
bool     ufo  = false;
GLfloat  ufoX = -30;
int      ufoTick = 10000;

// Informasi terkait dengan skor dan live player
int playerScore = 0;
int nyawaPlayer = 3;
int highScore   = 0;
int level       = 1;

// Posisi derbis / mayat / efek mati
int     nextDerbis = 0;
bool    derbisAlive[10];
GLfloat derbisX[10];
GLfloat derbisY[10];

bool gameover = false, first = true;

void computeNextAlienShot();

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
    drawIntegerText(playerScore, 70, windowHeight - 25);
    drawStringText("High Score:", 140, windowHeight - 25);
    drawIntegerText(highScore, 220, windowHeight - 25);
    drawStringText("Nyawa:", 300, windowHeight - 25);
    drawIntegerText(nyawaPlayer, 350, windowHeight - 25);
    drawStringText("Level:", 400, windowHeight - 25);
    drawIntegerText(level, 450, windowHeight - 25);
}

void sceneGameOver() {
    drawScoreboard();
    drawStringText("GAME OVER.", 50, 300);
    drawStringText("Press SPACE to Continue.", 50, 250);
}

// Dalam satu kali level, akan ada UFO yang muncul, tetapi entah kapan UFO
// itu akan muncul..
void deployUFO(int levelTarget) {
    if (level != levelTarget) return;
    ufoX = -100;
    ufo = true;
}

void removeDerbis(int derbisNum) {
    printf("Derbis %d removed\n", derbisNum);
    derbisAlive[derbisNum] = false;
}

void addDerbis(GLfloat posX, GLfloat posY) {
    if (nextDerbis >= 10) nextDerbis = 0;
    printf("Derbis #%d added at %d, %d\n", nextDerbis, posX, posY);
    derbisX[nextDerbis]     = posX;
    derbisY[nextDerbis]     = posY;
    derbisAlive[nextDerbis] = true;
    glutTimerFunc(1000, removeDerbis, nextDerbis);
    nextDerbis++;
}

void drawIntegerText(int what, int posX, int posY) {
    char p[50];
    int residu = what;
    int printNow;
    int max = ceil(log((float) what));
    int i = max;

    if (what <= 0) {
        return drawStringText("0", posX, posY);
    }

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
// Warning: O(n), tetapi tidak masalah karena hanya
// dikerjakan beberapa saat saja
void moveAliens(int value) {
    printf("Tick from: moveAliens\n");

    // Tidak lakukan apapun jika game over
    if (gameover) {
        glutTimerFunc(1000, moveAliens, 0);
        return;
    }

    // Cek baris terkiri, cek baris terkanan
    int leftMost =    999999;
    int rightMost =  -999999;
    int bottomMost =  100000;
    for (int i = 0; i < maxAlien; i++) {
        if (alienAlive[i]) {
            if (leftMost > alienPosX[i])   leftMost   = alienPosX[i];
            if (rightMost < alienPosX[i])  rightMost  = alienPosX[i];
            if (bottomMost > alienPosY[i]) bottomMost = alienPosY[i];
        }
    }

    if (bottomMost < 40) gameover = true;

    // Cek apakah baris sudah mentok
    bool moveDown = false;
    if ((movementLeft == true && leftMost < 20) ||
        (movementLeft == false && rightMost > windowWidth - 40))
        moveDown = true;
    
    // Jika sudah mentok, ganti arah gerakan
    if (moveDown) movementLeft = !movementLeft;

    int step = 15 * (movementLeft ? -1 : 1);

    // Sparta! Gerakan semuanya sesuai dengan arahnya saat ini
    for (int i = 0; i < maxAlien; i++) {
        if (alienAlive[i]) {
            if (moveDown) alienPosY[i] -= 20;
            alienPosX[i] += step;
        }
    }

    glutTimerFunc(alienMoveTick, moveAliens, 0);
}

// Routine untuk menggambar alien
void renderAliens() {
    for (int i = 0; i < maxAlien; i++) {
        if (alienAlive[i]) {
            glPushMatrix();    
            glTranslatef(alienPosX[i], alienPosY[i], 0.0);
            glCallList (displayObjects[alienType[i]]);
            glPopMatrix(); //move origin back to center
        }
    }

    // Render UFO
    if (!ufo) return;

    glPushMatrix();    
    glTranslatef(ufoX, windowHeight - 70, 0.0);
    glCallList (displayObjects[3]);
    glPopMatrix(); //move origin back to center
}

// Gambar arena
void DisplayArena() {
	glClear(GL_COLOR_BUFFER_BIT);

    if (gameover) {
        sceneGameOver();
        glutSwapBuffers();
        return;
    }

    // Routine alien agak rumit, jadi dipisah aja
    renderAliens();

    // Render alien's bullet
    glPointSize(2.0f);
    for (int i = 0; i < 10; i++) {
        if (alienBulletActive[i]) {
            glPointSize(3.0f);
            glColor3f(1.0f, 0.5f, 0.5f);
            glBegin(GL_POINTS);
                glVertex2i(alienBulletX[i], alienBulletY[i]);
            glEnd();
        }
    }

	// Player paddle
    glColor3f(0.0f, 0.5f, 1.0f);

    glPushMatrix();    
    glTranslatef(playerPaddle-20, 5, 0.0); //translate origin (-150, 150)
    glCallList (displayObjects[4]);
    glPopMatrix(); //move origin back to center

    // Render derbis
    for (int i = 0; i < 10; i++) {
        if (derbisAlive[i]) {
            glPushMatrix();    
            glTranslatef(derbisX[i], derbisY[i], 0.0); //translate origin (-150, 150)
            glCallList (displayObjects[5]);
            glPopMatrix(); //move origin back to center
        }
    }


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
void initAliens(void);
void resetPlayer(void);
void deployUFO(int);

// Mulai game dari awal lagi
// Atau, pertama kalinya mulai game
void gameStartOver() {
    resetPlayer();
    initAliens();
    level = 1;
    nyawaPlayer = 3;
    playerScore = 0;
    alienMoveTick = 5000;
    alienShotTickMax = 1000;
    gameover = false;
    computeNextAlienShot();
    glutTimerFunc(ufoTick * ((rand() % 3) + 1), deployUFO, level);
}

// Mengecek masih ada alien yang hidup
void nextLevelCheck() {
    bool adayanghidup = false;

    for (int i = 0; i < maxAlien; i++) {
        if (alienAlive[i] && !adayanghidup) {
            adayanghidup = true;
        }
    }

    // Kalau semuanya sudah hidup, maka lanjut
    // ke level selanjutnya
    if (!adayanghidup) {
        level += 1;
        alienShotTickMax /= 2;
        alienMoveTick /=2;
        initAliens();
    }
}

void addRandomAlienBullet(int value);

// Menghitung kapan alien akan nembak
void computeNextAlienShot() {
    int alienShotTick = (float) alienShotTickMax * (float) (rand() % 10);
    glutTimerFunc(alienShotTick, addRandomAlienBullet, 0);
}

// Cek apakah ada peluru yang mengenai alien ataupun UFO
void collisionCheck() {
    // Collision detect for alien bullet
    for (int i = 0; i < 10; i++) {
        if (alienBulletActive[i] &&
            antara(alienBulletX[i], playerPaddle - 22, playerPaddle + 22) &&
            antara(alienBulletY[i], 10, 40)) {
            alienBulletActive[i] = false;
            printf("Player hit!\n");
            nyawaPlayer -= 1;
            resetPlayer();
            if (nyawaPlayer == 0) gameover = true;
        }
    }

    // Do no collision check if there's no bullet
    if (!bulletAlive) return;

    bool collisionNotYetFound = true;
    // Iterasi setiap alien, apakah ada peluru yang mengenai mereka?
    for (int i = 0; i < maxAlien && collisionNotYetFound; i++) {
        int l = 25;
        // Ingat, ukuran alien berbeda-beda
        // Cek apakah ada peluru yang mengenai mereka
        switch (alienType[i]) {
            // Length: 25
            case 0: l = 27; break;
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
            addDerbis(alienPosX[i], alienPosY[i]);
            playerScore += (alienType[i] + 1) * 10;
            printf("Alien kena! %d\n", i);
            nextLevelCheck();
        }
    }

    // Collision check for bullet targeting UFO
    if (ufo && 
        antara(bulletX, ufoX, ufoX + 52) &&
        antara(bulletY, windowHeight - 70, windowHeight - 70 + 50)) {
        ufo = false;
        bulletAlive = false;
        playerScore += rand() % 10 * 200;
    }

    // Mutakhirkan skor tertinggi, jika harus
    if (playerScore > highScore) highScore = playerScore;
}

void addRandomAlienBullet(int value) {
    bool belum = true; // Belum selesai prosesnya

    for (int z = 0; z < 10 && belum; z++) {
        if (alienBulletActive[z] == false) {
            // Lima kali random. Kalau enggak ketemu, yang pertama aja suruh nembak
            bool ketemu = false; int hasilAcak = 0;
            for (int i = 0; i < 5 && !ketemu; i++) {
                hasilAcak = rand() % maxAlien;
                if (alienAlive[hasilAcak]) ketemu = true;
            }
            printf("Adding random, got from real acak: %d\n", hasilAcak);
            for (int i = 0; i < maxAlien && !ketemu; i++) {
                if (alienAlive[i]) {
                    hasilAcak = i;
                    ketemu = true;
                }
            }
            if (ketemu) {
                alienBulletX[z] = alienPosX[hasilAcak] + 10;
                alienBulletY[z] = alienPosY[hasilAcak];
                alienBulletActive[z] = true;
            }
            
            belum = false;
        }
    }
    computeNextAlienShot();
}

// Menggerakkan alien, posisi player, peluru, dan kebutuhan lainnya
// yang bersifat prarender
void OnPlay() {
    if (gameover) {
        if (GetAsyncKeyState(VK_SPACE)) {
            gameStartOver();
        }
        glutPostRedisplay();
        return;
    }
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
    
	if(GetAsyncKeyState(VK_LEFT) && playerPaddle > 0)
		playerPaddle -= step;
	

	if(GetAsyncKeyState(VK_RIGHT) && playerPaddle < windowWidth)
		playerPaddle += step;
	
    // Cek apakah ada alien yang nembak
    for (int i = 0; i < 10; i++) {
        if (alienBulletActive[i]) {
            alienBulletY[i] -= step * .5;

            if (alienBulletY[i] < 0) {
                alienBulletActive[i] = false;
                printf("Alien bullet die: %d\n", i);
            }
        }
    }

    // UFO Part
    if (ufo) {
        float divisor = (level > 3) ? 1.0 : 1.0 / (float) (3.0 - (float) level);
        ufoX += step * divisor;
        if (ufoX > windowWidth) ufo = false;
    }

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
            alienPosX[k] = i*70.0f + 10.0f;
            alienPosY[k] = 220.0f + j * 25.0f;
            int type = (j > 1) ? j - 1 : j;
            alienType[k] = type;
        }
    }
}

void Init() {
    gameStartOver();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void resetPlayer() {
    for (int i = 0; i < 10; i++) {
        alienBulletActive[i] = false;
    }
    playerPaddle = 500;
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
    srand(time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 500);
	glutCreateWindow("Space Invader - Mid-term Exam Muhammad Mufid Afif (1006671766)");
    Init();
    createAlienDisplayList(); // Refactor, di sprite.h
	glutDisplayFunc(DisplayArena);
	glutReshapeFunc(ChangeSize);
    glutTimerFunc(alienMoveTick, moveAliens, 0);
    glutIdleFunc(OnPlay);
	
	glutMainLoop();

	return 0;
}
