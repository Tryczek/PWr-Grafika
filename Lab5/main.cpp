// Definicje umo¿liwiaj¹ce u¿ycie wartoœci matematycznych z biblioteki M_PI itp.
#define _USE_MATH_DEFINES

// Wy³¹cza ostrze¿enia zwi¹zane z u¿ywaniem niebezpiecznych funkcji CRT w systemie Windows
#define _CRT_SECURE_NO_WARNINGS

// Nag³ówki bibliotek do obs³ugi grafiki OpenGL oraz standardowe biblioteki C++
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <time.h>

// Przestrzeñ nazw standardowa
using namespace std;

// Definicja typu tablicy trójwymiarowej punktów
typedef float point3[3];

// Domyœlna liczba punktów do generowania dla powierzchni jajka
int N = 100;

// Zmienna przechowuj¹ca aktualny model do wyœwietlenia
int model;

// Flagi w³¹czaj¹ce/wy³¹czaj¹ce œwiat³a
bool light0 = false;
bool light1 = false;

// Domyœlny promieñ obiektu
static GLfloat R = 10.0;

// Wspó³rzêdne widza
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };

// Punkt obserwacji (punkt, na który patrzy widz)
static GLfloat punkt_obserwacji[] = { 0.0, 0.0, 0.0 };

// K¹ty obrotu obiektu wokó³ osi x i y
static GLfloat theta_x = 0.0;
static GLfloat theta_y = 0.0;

// Wspó³czynniki obracania obiektu wokó³ osi x i y
static GLfloat theta_x1 = 0.0;
static GLfloat theta_y1 = 0.0;

// Rozdzielczoœæ jednostkowa k¹ta (piksele na stopieñ)
static GLfloat pix2angle;

// Flagi kontroluj¹ce widocznoœæ poszczególnych œcian obiektu
bool sciana1_show = true;
bool sciana2_show = true;
bool sciana3_show = true;
bool sciana4_show = true;
bool sciana5_show = true;

// Stan interakcji mysz¹ (0 - brak interakcji, 1 - obrót, 2 - przesuniêcie w pionie)
static GLint status = 0;

// Zmienne przechowuj¹ce poprzednie po³o¿enia myszy i ich zmiany
static int x_pos_old = 0;
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;
static int z_pos_old = 0;
static int delta_z = 0;


// Funkcja do wczytywania obrazów w formacie TGA
GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{
    // Struktura nag³ówka pliku TGA
    #pragma pack(1)           
    typedef struct
    {
        GLbyte    idlength;
        GLbyte    colormaptype;
        GLbyte    datatypecode;
        unsigned short    colormapstart;
        unsigned short    colormaplength;
        unsigned char     colormapdepth;
        unsigned short    x_orgin;
        unsigned short    y_orgin;
        unsigned short    width;
        unsigned short    height;
        GLbyte    bitsperpixel;
        GLbyte    descriptor;
    } TGAHEADER;
    #pragma pack(8)

    // Deklaracje zmiennych i wskaŸników
    FILE* pFile;
    TGAHEADER tgaHeader;
    unsigned long lImageSize;
    short sDepth;
    GLbyte* pbitsperpixel = NULL;

    // Inicjalizacja wartoœci zmiennych wyjœciowych
    *ImWidth = 0;
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;

    // Otwarcie pliku do odczytu binarnego
    pFile = fopen(FileName, "rb");
    if (pFile == NULL) {
        cout << "B³¹d odczytu pliku" << endl;
        return NULL;
    }

    // Odczyt nag³ówka pliku TGA
    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

    // Przypisanie szerokoœci, wysokoœci i g³êbokoœci pikseli obrazu
    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;

    // Sprawdzenie poprawnoœci g³êbokoœci pikseli
    if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

    // Obliczenie rozmiaru obrazu
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Alokacja pamiêci dla danych pikseli
    pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

    // Sprawdzenie poprawnoœci alokacji pamiêci
    if (pbitsperpixel == NULL)
        return NULL;

    // Odczyt danych pikseli z pliku
    if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
    {
        free(pbitsperpixel);
        return NULL;
    }

    // Ustalenie formatu i sk³adowych obrazu w zale¿noœci od g³êbokoœci pikseli
    switch (sDepth)
    {
    case 3:
        *ImFormat = GL_BGR_EXT;
        *ImComponents = GL_RGB8;
        break;

    case 4:
        *ImFormat = GL_BGRA_EXT;
        *ImComponents = GL_RGBA8;
        break;

    case 1:
        *ImFormat = GL_LUMINANCE;
        *ImComponents = GL_LUMINANCE8;
        break;
    };

    // Zamkniêcie pliku
    fclose(pFile);

    // Zwrócenie wskaŸnika na dane pikseli obrazu
    return pbitsperpixel;
}

// Funkcje obliczaj¹ce pochodne cz¹stkowe wektora normalnego powierzchni obiektu na podstawie parametrów u i v.
float wektorNormXU(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej X wzglêdem u
    return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * cos(M_PI * v);
}

float wektorNormXV(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej X wzglêdem v
    return M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * sin(M_PI * v);
}

float wektorNormYU(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej Y wzglêdem u
    return 640 * u * u * u - 960 * u * u + 320 * u;
}

float wektorNormYV(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej Y wzglêdem v
    return 0;
}

float wektorNormZU(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej Z wzglêdem u
    return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * sin(M_PI * v);
}

float wektorNormZV(float u, float v) 
{
    // Obliczenie pochodnej cz¹stkowej Z wzglêdem v
    return -M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * cos(M_PI * v);
}

// Funkcje obliczaj¹ce sk³adowe wektora normalnego powierzchni obiektu.
float wektorNormX(float u, float v)
{
    // Obliczenie sk³adowej X wektora normalnego
    return wektorNormYU(u, v) * wektorNormZV(u, v) - wektorNormZU(u, v) * wektorNormYV(u, v);
}

float wektorNormY(float u, float v)
{
    // Obliczenie sk³adowej Y wektora normalnego
    return wektorNormZU(u, v) * wektorNormXV(u, v) - wektorNormXU(u, v) * wektorNormZV(u, v);
}

float wektorNormZ(float u, float v)
{
    // Obliczenie sk³adowej Z wektora normalnego
    return wektorNormXU(u, v) * wektorNormYV(u, v) - wektorNormYU(u, v) * wektorNormXV(u, v);
}

// Funkcje obliczaj¹ce wspó³rzêdne punktu obserwatora w uk³adzie wspó³rzêdnych sferycznych.
float observerXS(float R, float azymut, float elewacja)
{
    // Obliczenie wspó³rzêdnej X punktu obserwatora
    return R * cos(azymut) * (float)cos(elewacja);
}

float observerYS(float R, float elewacja)
{
    // Obliczenie wspó³rzêdnej Y punktu obserwatora
    return R * sin(elewacja);
}

float observerZS(float R, float azymut, float elewacja)
{
    // Obliczenie wspó³rzêdnej Z punktu obserwatora
    return R * sin(azymut) * (float)cos(elewacja);
}

// Funkcja obs³uguj¹ca zdarzenie myszy (klikniêcia i ruchy).
void Mouse(int btn, int state, int x, int y)
{
    // Obs³uga lewego przycisku myszy (GLUT_LEFT_BUTTON) w momencie naciœniêcia (GLUT_DOWN).
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;        // Zapisanie aktualnej pozycji kursora myszy.
        y_pos_old = y;
        status = 1;           // Ustawienie statusu na 1, co oznacza aktywacjê ruchu obiektu.
    }
    // Obs³uga prawego przycisku myszy (GLUT_RIGHT_BUTTON) w momencie naciœniêcia (GLUT_DOWN).
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        y_pos_old = y;        // Zapisanie aktualnej pozycji kursora myszy.
        status = 2;           // Ustawienie statusu na 2, co oznacza aktywacjê zmiany k¹ta widzenia.
    }
    else
        status = 0;           // Jeœli ¿aden przycisk nie jest naciœniêty, ustawienie statusu na 0.
}

// Funkcja obs³uguj¹ca ruch myszy (motion).
void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;     // Obliczenie ró¿nicy po³o¿enia kursora myszy w osi X.
    x_pos_old = x;               // Zapisanie aktualnej pozycji kursora myszy.
    delta_y = y - y_pos_old;     // Obliczenie ró¿nicy po³o¿enia kursora myszy w osi Y.
    y_pos_old = y;               // Zapisanie aktualnej pozycji kursora myszy.
    glutPostRedisplay();         // Wywo³anie funkcji do ponownego rysowania sceny.
}

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych.
void Axes(void)
{
    // Okreœlenie punktów koñcowych dla ka¿dej z osi.
    point3  x_min = { -5.0, 0.0, 0.0 };
    point3  x_max = { 5.0, 0.0, 0.0 };
    point3  y_min = { 0.0, -5.0, 0.0 };
    point3  y_max = { 0.0,  5.0, 0.0 };
    point3  z_min = { 0.0, 0.0, -5.0 };
    point3  z_max = { 0.0, 0.0,  5.0 };

    // Rysowanie osi X w kolorze czerwonym.
    glColor3f(1.0f, 0.0f, 0.0f); 
    glBegin(GL_LINES);
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();
    
    // Rysowanie osi Y w kolorze zielonym.
    glColor3f(0.0f, 1.0f, 0.0f); 
    glBegin(GL_LINES);  
    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();
    
    // Rysowanie osi Z w kolorze niebieskim.
    glColor3f(0.0f, 0.0f, 1.0f);  
    glBegin(GL_LINES); 
    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
}


// Funkcja rysuj¹ca ostrza³up sk³adaj¹cy siê z piêciu œcian.
void rysujOstroslup()
{
    // Rysowanie pierwszej œciany (QUADS) jeœli flaga sciana1_show jest ustawiona na true.
    if (sciana1_show)
    {
        glBegin(GL_QUADS);
        glNormal3f(-5.0, -5.0, 0.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-5.0, -5.0, 0.0);
        glNormal3f(5.0, -5.0, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(5.0, -5.0, 0.0);
        glNormal3f(5.0, 5.0, 0.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(5.0, 5.0, 0.0);
        glNormal3f(-5.0, 5.0, 0.0);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-5.0, 5.0, 0.0);
        glEnd();
    }

    // Rysowanie drugiej œciany (TRIANGLES) jeœli flaga sciana2_show jest ustawiona na true.
    if (sciana2_show)
    {
        glBegin(GL_TRIANGLES);
        glNormal3f(5.0, -5.0, 0.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(5.0, -5.0, 0.0);
        glNormal3f(-5.0, -5.0, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-5.0, -5.0, 0.0);
        glNormal3f(0.0, 0.0, -10.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0, 0.0, -10.0);
        glEnd();
    }

    // Rysowanie trzeciej œciany (TRIANGLES) jeœli flaga sciana3_show jest ustawiona na true.
    if (sciana3_show)
    {
        glBegin(GL_TRIANGLES);
        glNormal3f(5.0, 5.0, 0.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(5.0, 5.0, 0.0);
        glNormal3f(5.0, -5.0, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(5.0, -5.0, 0.0);
        glNormal3f(0.0, 0.0, -10.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0, 0.0, -10.0);
        glEnd();
    }

    // Rysowanie czwartej œciany (TRIANGLES) jeœli flaga sciana4_show jest ustawiona na true.
    if (sciana4_show)
    {
        glBegin(GL_TRIANGLES);
        glNormal3f(-5.0, 5.0, 0.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-5.0, 5.0, 0.0);
        glNormal3f(5.0, 5.0, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(5.0, 5.0, 0.0);
        glNormal3f(0.0, 0.0, -10.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0, 0.0, -10.0);
        glEnd();
    }

    // Rysowanie pi¹tej œciany (TRIANGLES) jeœli flaga sciana5_show jest ustawiona na true.
    if (sciana5_show)
    {
        glBegin(GL_TRIANGLES);
        glNormal3f(-5.0, -5.0, 0.0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-5.0, -5.0, 0.0);
        glNormal3f(-5.0, 5.0, 0.0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-5.0, 5.0, 0.0);
        glNormal3f(0.0, 0.0, -10.0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0, 0.0, -10.0);
        glEnd();
    }
}

// Funkcja rysuj¹ca jajo (Egg) w zale¿noœci od wybranej opcji modelu.
void Egg(int N)
{
    // Ustawienie koloru na zielony.
    glColor3f(0.0f, 1.0f, 0.0f);
    
    // Inicjalizacja tablic punktów, pomocniczych punktów i wektorów punktów.
    point3* points = new point3[N * N];
    point3* pompoints = new point3[N * N];
    point3* vpoints = new point3[N * N];
    
    double u, v;
    double dlugoscwek;
    int id = 0;

    // Wype³nienie tablicy punktów na podstawie parametrycznych wzorów jaja.
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            u = i / ((double)(N - 1) * 2.0);
            v = j / ((double)N / 2.0);
            points[id][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
            points[id][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5);
            points[id][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
            vpoints[id][0] = wektorNormX(u, v);
            vpoints[id][1] = wektorNormY(u, v);
            vpoints[id][2] = wektorNormZ(u, v);
            dlugoscwek = sqrt(vpoints[id][0]* vpoints[id][0] + vpoints[id][1] * vpoints[id][1] + vpoints[id][2] * vpoints[id][2]);
            vpoints[id][0] /= dlugoscwek;
            vpoints[id][1] /= dlugoscwek;
            vpoints[id][2] /= dlugoscwek;
            id++;
        }

    // Rysowanie jaja w zale¿noœci od wybranej opcji modelu.
    if (model == 1)
    {
        glBegin(GL_POINTS);
        for (int id = 0; id < N * N; id++)
            glVertex3d(points[id][0], points[id][1], points[id][2]);
        glEnd();
    }
    else if (model == 2)
    {
        for (int i = 0; i < N * N; i += N)
        {
            for (int j = i; j < i + N - 1; j++)
            {
                glBegin(GL_LINES);
                glVertex3d(points[j][0], points[j][1], points[j][2]);
                glVertex3d(points[j + 1][0], points[j + 1][1], points[j + 1][2]);
                glEnd();
            }
            glBegin(GL_LINES);
            glVertex3d(points[i + N - 1][0], points[i + N - 1][1], points[i + N - 1][2]);
            glVertex3d(points[i][0], points[i][1], points[i][2]);
            glEnd();
        }

        for (int i = 0; i < N; i++)
        {
            for (int j = i; j < N * N - N; j += N)
            {
                glBegin(GL_LINES);
                glVertex3d(points[j][0], points[j][1], points[j][2]);
                glVertex3d(points[j + N][0], points[j + N][1], points[j + N][2]);
                glEnd();
            }
        }
    }
    else if (model == 3)
    {
        int a, b, c, d;
        double t_u = 0.0;
        double t_v = 0.0;
        double t_d = 1.0 / (N - 1);

        // Rysowanie jaja w trybach TRIMESH i TRIANGLE_STRIP.
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N * N - N; j += N)
            {
                a = j + i;
                b = j + i + N;
                c = (j + (i + 1) % N);
                d = (j + (i + 1) % N) + N;

                glBegin(GL_TRIANGLES);
                glNormal3d(vpoints[a][0], vpoints[a][1], vpoints[a][2]);
                glTexCoord2d(t_u, t_v);
                glVertex3d(points[a][0], points[a][1], points[a][2]);
                glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
                glTexCoord2d(t_u, t_v + t_d);
                glVertex3d(points[b][0], points[b][1], points[b][2]);
                glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
                glTexCoord2d(t_u + t_d, t_v);
                glVertex3d(points[c][0], points[c][1], points[c][2]);
                glEnd();
                glBegin(GL_TRIANGLES);
                glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
                glTexCoord2d(t_u, t_v + t_d);
                glVertex3d(points[b][0], points[b][1], points[b][2]);
                glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
                glTexCoord2d(t_u + t_d, t_v);
                glVertex3d(points[c][0], points[c][1], points[c][2]);
                glNormal3d(vpoints[d][0], vpoints[d][1], vpoints[d][2]);
                glTexCoord2d(t_u + t_d, t_v + t_d);
                glVertex3d(points[d][0], points[d][1], points[d][2]);
                glEnd();

                t_v += t_d;
            }
            t_u += t_d;
        }
    }
    delete[] points;

    // Rysowanie czajnika jeœli wybrany model to 4.
    if (model == 4)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glutSolidTeapot(3.0);
    }

    // Rysowanie ostros³upa jeœli wybrany model to 5.
    if (model == 5)
        rysujOstroslup();
}


// Funkcja rysuj¹ca scenê.
void RenderScene(void)
{
    // Czyszczenie buforów koloru i g³êbokoœci.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset macierzy modelview.
    glLoadIdentity();
    
    // Ustawienie punktu widzenia kamery.
    gluLookAt(viewer[0], viewer[1], viewer[2], punkt_obserwacji[0], punkt_obserwacji[1], punkt_obserwacji[2], 0.0, 1.0, 0.0);
    
    // Rysowanie osi wspó³rzêdnych.
    Axes();
    
    // Obs³uga obrotów i przemieszczeñ obiektu w zale¿noœci od trybu i statusu myszy.
    if (status == 1)
    {
        theta_x += delta_x * pix2angle;
        theta_y += delta_y * pix2angle;
    }
    else if (status == 2)
    {
        if (delta_y > 0)
        {
            viewer[2] += 0.1;
            punkt_obserwacji[2] += 0.25;
        }
        else
        {
            viewer[2] -= 0.1;
            punkt_obserwacji[2] -= 0.25;
        }
    }

    // Obrót obiektu.
    glRotatef(theta_x, 0.0, 1.0, 0.0);
    glRotatef(theta_y, 1.0, 0.0, 0.0);

    // Ustawienie koloru na bia³y i rysowanie jaja zale¿nie od wybranej opcji modelu.
    glColor3f(1.0f, 1.0f, 1.0f);
    Egg(N);

    // Wys³anie poleceñ do bufora.
    glFlush();
    
    // Zamiana buforów.
    glutSwapBuffers();
}

// Funkcja obs³uguj¹ca klawisze klawiatury.
void keys(unsigned char key, int x, int y)
{
    // Obs³uga klawiszy od 'p' do '7' oraz '1' i '2' do w³¹czania i wy³¹czania œwiate³ i œcian.
    if (key == 'p')
        model = 1;
    if (key == 'w')
        model = 2;
    if (key == 's')
        model = 3;
    if (key == 't')
        model = 4;
    if (key == 'o')
        model = 5;
    if (key == '1')
    {
        light0 = !light0;
        if (light0)
            glEnable(GL_LIGHT0);
        else
            glDisable(GL_LIGHT0);
    }
    if (key == '2')
    {
        light1 = !light1;
        if (light1)
            glEnable(GL_LIGHT1);
        else
            glDisable(GL_LIGHT1);
    }
    if (key == '3')
        sciana1_show = !sciana1_show;
    if (key == '4')
        sciana2_show = !sciana2_show;
    if (key == '5')
        sciana3_show = !sciana3_show;
    if (key == '6')
        sciana4_show = !sciana4_show;
    if (key == '7')
        sciana5_show = !sciana5_show;

    // Ponowne rysowanie sceny po zmianie.
    RenderScene();
}

// Funkcja inicjalizuj¹ca ustawienia programu.
void MyInit(void)
{
    // Wczytanie tekstury z pliku TGA i ustawienie jej parametrów.
    GLbyte* pBytes;
    GLint ImWidth, ImHeight, ImComponents;
    GLenum ImFormat;
    pBytes = LoadTGAImage("tekstury/N2_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
    free(pBytes);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Ustawienie koloru t³a na czarny.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Ustawienia materia³ów i œwiate³.
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 20.0 };
    GLfloat light_position[] = { 0.0, -10.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_specular[] = { 0.0, 0.0, 0.0, 0.5 };
    GLfloat att_constant = { 1.0 };
    GLfloat att_linear = { 0.05 };
    GLfloat att_quadratic = { 0.001 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
    GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat light_specular1[] = { 0.0, 0.0, 1.0, 0.5 };
    GLfloat light_position1[] = { 0.0, 10.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

// Funkcja zmieniaj¹ca rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Obliczenie wspó³czynnika przeskalowania.
    pix2angle = 360.0 / (float)horizontal;

    // Ustawienie macierzy projekcji i viewportu.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 1.0, 1.0, 30.0);

    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Funkcja wyœwietlaj¹ca menu.
void menu()
{
    // Wyœwietlenie informacji o poprawnym wprowadzeniu wartoœci N.
    while (N < 0)
    {
        cout << "Podana niepoprawna wartosc. Sprobuj ponownie: ";
        cin >> N;
    }

    // Wyœwietlenie menu z klawiszami i ich funkcjami.
    cout
        << " P - jajko punktowe " << endl
        << " W - jajko siatki" << endl
        << " S - jajko pelne" << endl
        << " T - czajniczek" << endl
        << " O - ostroslup" << endl
        << " 1 - swiatlo 1" << endl
        << " 2 - swiatlo 2" << endl
        << " 3-6 - Zmiana scian" << endl;
    cout << endl;
}

// Funkcja g³ówna programu.
int main(int argc, char* argv[])
{
    // Wywo³anie funkcji wyœwietlaj¹cej menu.
    menu();
    
    // Inicjalizacja biblioteki GLUT i ustawienia trybu okna.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInit(&argc, argv);
    
    // Utworzenie okna i przypisanie funkcji RenderScene do obs³ugi wyœwietlania.
    glutCreateWindow("Rzutowanie perspektywiczne");
    glutDisplayFunc(RenderScene);
    
    // Przypisanie funkcji ChangeSize do obs³ugi zmiany rozmiaru okna.
    glutReshapeFunc(ChangeSize);
    
// Ustawienie funkcji obs³uguj¹cej zdarzenia myszy (np. klikniêcie przycisku myszy)
glutMouseFunc(Mouse);

// Ustawienie funkcji obs³uguj¹cej ruch myszy
glutMotionFunc(Motion);

// Inicjalizacja w³asnych ustawieñ lub zmiennych
MyInit(); 

// W³¹czenie mechanizmu testowania g³êbokoœci (do obs³ugi trójwymiarowej grafiki)
glEnable(GL_DEPTH_TEST);

// Ustawienie funkcji obs³uguj¹cej klawiaturê
glutKeyboardFunc(keys);

// Rozpoczêcie g³ównej pêtli programu (czeka na zdarzenia i obs³uguje je)
glutMainLoop();


}
