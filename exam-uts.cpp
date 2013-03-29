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

bool gameover = false, first = true;

void DisplayText(char *teks)
{
    char *p;
    glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0f, 0.5f, 1.0f);
    glRasterPos2f(windowWidth/2-180, windowHeight/2);  //posisikan teks
    for (p = teks; *p; p++)
           glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p); //teks ditampilkan perkarakter
    glutSwapBuffers();       
}

// Gambar arena
void DisplayArena()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0f, 0.5f, 1.0f);

	    // CPU paddle
	    glRectf(10, cpuPaddle, 20, cpuPaddle + 50);

	    // Player paddle
	    glRectf(windowWidth - 20, playerPaddle, windowWidth - 10, playerPaddle + 50);

	    glColor3f(1.0f, 0.0f, 0.0f);

	    // Bola
	    glRectf(ballX, ballY, ballX + 5, ballY + 5);

	glutSwapBuffers();
}

void OnPlay()
{
	// Cek key input
    
	if(playerPaddle < 350)
	{
	if(GetAsyncKeyState(VK_UP))
		playerPaddle += step;
	}

	if(playerPaddle > 0)
	{
	if(GetAsyncKeyState(VK_DOWN))
		playerPaddle -= step;
	}


	
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


void Init()
{
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
	glutInitWindowSize(400, 400);
	glutCreateWindow("Simple Pong Game");
    Init();
	glutDisplayFunc(DisplayArena);
	glutReshapeFunc(ChangeSize);
    glutIdleFunc(OnPlay);
	
	glutMainLoop();

	return 0;
}
