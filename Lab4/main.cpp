#define _USE_MATH_DEFINES
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <time.h>

// Poni¿ej znajduje siê deklaracja przestrzeni nazw 'std', co oznacza, ¿e korzystamy z przestrzeni nazw standardowej
using namespace std;

// Deklaracja typu 'point3' jako tablicy trójwymiarowej o elementach typu 'float'
typedef float point3[3];

// Liczba punktów wykorzystywana do generowania powierzchni jajka
int N;

// Sta³a definiuj¹ca promieñ powierzchni jajka
static GLfloat R = 10.0;

// Pozycja obserwatora w przestrzeni trójwymiarowej
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };

// Punkt obserwacji, s³u¿¹cy do ustalenia kierunku widzenia
static GLfloat punkt_obserwacji[] = { 0.0, 0.0, 0.0 };

// K¹ty obrotu dla ca³ej sceny
static GLfloat theta_x = 0.0;
static GLfloat theta_y = 0.0;

// K¹ty obrotu dla dwóch Ÿróde³ œwiat³a
static GLfloat theta_x1 = 0.0;
static GLfloat theta_y1 = M_PI;
static GLfloat theta_x2 = 0.0;
static GLfloat theta_y2 = -M_PI;

// Zmienna przechowuj¹ca przelicznik do konwersji ruchu myszy na obrót sceny
static GLfloat pix2angle;

// Status myszy (przesuniêcie lewego lub prawego przycisku)
static GLint status = 0;

// Pozycja myszy podczas poprzedniego naciœniêcia lewego przycisku
static int x_pos_old = 0;
static int delta_x = 0;
static int y_pos_old = 0;
static int delta_y = 0;
static int z_pos_old = 0;
static int delta_z = 0;

// Funkcje definiuj¹ce wektory normalne do powierzchni jajka
float wektorNormXU(float u, float v) {
    return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * cos(M_PI * v);
}

float wektorNormXV(float u, float v) {
    return M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * sin(M_PI * v);
}

float wektorNormYU(float u, float v) {
    return 640 * u * u * u - 960 * u * u + 320 * u;
}

float wektorNormYV(float u, float v) {
    return 0;
}

float wektorNormZU(float u, float v) {
    return (-450 * u * u * u * u + 900 * u * u * u - 810 * u * u + 360 * u - 45) * sin(M_PI * v);
}

float wektorNormZV(float u, float v) {
    return -M_PI * (90 * u * u * u * u * u - 225 * u * u * u * u + 270 * u * u * u - 180 * u * u + 45 * u) * cos(M_PI * v);
}

// Funkcje definiuj¹ce wektory normalne do powierzchni jajka na podstawie wczeœniej zdefiniowanych funkcji
float wektorNormX(float u, float v) {
    return wektorNormYU(u, v) * wektorNormZV(u, v) - wektorNormZU(u, v) * wektorNormYV(u, v);
}

float wektorNormY(float u, float v) {
    return wektorNormZU(u, v) * wektorNormXV(u, v) - wektorNormXU(u, v) * wektorNormZV(u, v);
}

float wektorNormZ(float u, float v) {
    return wektorNormXU(u, v) * wektorNormYV(u, v) - wektorNormYU(u, v) * wektorNormXV(u, v);
}

// Funkcje definiuj¹ce wspó³rzêdne obserwatora w przestrzeni trójwymiarowej
float observerXS(float R, float azymut, float elewacja) {
    return R * cos(azymut) * (float)cos(elewacja);
}

float observerYS(float R, float elewacja) {
    return R * sin(elewacja);
}

float observerZS(float R, float azymut, float elewacja) {
    return R * sin(azymut) * (float)cos(elewacja);
}


// Funkcja obs³uguj¹ca zdarzenia myszy
void Mouse(int btn, int state, int x, int y)
{
    // Jeœli lewy przycisk myszy zosta³ naciœniêty
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Zapisujemy aktualne pozycje myszy
    	x_pos_old = x;
        y_pos_old = y;
        // Ustawiamy status na 4, co oznacza manipulacjê œwiat³em 1
        status = 4; 
    }
    // Jeœli prawy przycisk myszy zosta³ naciœniêty
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // Zapisujemy aktualne pozycje myszy
    	x_pos_old = x;
        y_pos_old = y;
        // Ustawiamy status na 3, co oznacza manipulacjê œwiat³em 2
        status = 3; 
    }
    // Jeœli nie zosta³ naciœniêty ani lewy, ani prawy przycisk myszy
    else
        // Ustawiamy status na 0, co oznacza brak aktywnej manipulacji
        status = 0;
}

// Funkcja obs³uguj¹ca ruch myszy
void Motion(GLsizei x, GLsizei y)
{
    // Obliczamy zmiany pozycji myszy
    delta_x = x - x_pos_old;
    x_pos_old = x;
    delta_y = y - y_pos_old;
    y_pos_old = y;
    // Informujemy, ¿e nale¿y przerysowaæ scenê
    glutPostRedisplay();
}

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{
    // Definicje minimalnych i maksymalnych wspó³rzêdnych osi
    point3 x_min = { -5.0, 0.0, 0.0 };
    point3 x_max = { 5.0, 0.0, 0.0 };
    point3 y_min = { 0.0, -5.0, 0.0 };
    point3 y_max = { 0.0,  5.0, 0.0 };
    point3 z_min = { 0.0, 0.0, -5.0 };
    point3 z_max = { 0.0, 0.0,  5.0 };

    // Rysujemy oœ X kolorowym segmentem
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    // Rysujemy oœ Y kolorowym segmentem
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    // Rysujemy oœ Z kolorowym segmentem
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
}

// Funkcja rysuj¹ca jajko
void Egg(int N)
{
    // Ustawienie koloru jajka na zielony
    glColor3f(0.0f, 1.0f, 0.0f);

    // Inicjalizacja tablic punktów, wektorów normalnych i pomocniczych punktów
    point3* points = new point3[N * N];
    point3* pompoints = new point3[N * N];
    point3* vpoints = new point3[N * N];

    double u, v;
    double dlugoscwek;
    int id = 0;

    // Generowanie punktów na powierzchni jajka
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

            // Normalizacja wektora normalnego
            dlugoscwek = sqrt(vpoints[id][0] * vpoints[id][0] + vpoints[id][1] * vpoints[id][1] + vpoints[id][2] * vpoints[id][2]);
            vpoints[id][0] /= dlugoscwek;
            vpoints[id][1] /= dlugoscwek;
            vpoints[id][2] /= dlugoscwek;

            id++;
        }

    int a, b, c, d;

    // Rysowanie trójk¹tów na powierzchni jajka
    for (int i = 0; i < N; i++)
    {
        for (int j = i; j < N * N - N; j += N)
        {
            a = j;
            b = j + N;
            c = (j + 1) % (N * N);
            d = (j + 1) % (N * N) + N;

            glBegin(GL_TRIANGLES);
            glNormal3d(vpoints[a][0], vpoints[a][1], vpoints[a][2]);
            glVertex3d(points[a][0], points[a][1], points[a][2]);
            glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
            glVertex3d(points[b][0], points[b][1], points[b][2]);
            glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
            glVertex3d(points[c][0], points[c][1], points[c][2]);
            glEnd();

            glBegin(GL_TRIANGLES);
            glNormal3d(vpoints[b][0], vpoints[b][1], vpoints[b][2]);
            glVertex3d(points[b][0], points[b][1], points[b][2]);
            glNormal3d(vpoints[c][0], vpoints[c][1], vpoints[c][2]);
            glVertex3d(points[c][0], points[c][1], points[c][2]);
            glNormal3d(vpoints[d][0], vpoints[d][1], vpoints[d][2]);
            glVertex3d(points[d][0], points[d][1], points[d][2]);
            glEnd();
        }
    }

    // Zwolnienie zaalokowanej pamiêci
    delete[] points;
}

// Funkcja renderuj¹ca scenê
void RenderScene(void)
{
    // Wyczyszczenie buforów koloru i g³êbokoœci
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformacji
    glLoadIdentity();

    // Ustawienie punktu widzenia
    gluLookAt(viewer[0], viewer[1], viewer[2], punkt_obserwacji[0], punkt_obserwacji[1], punkt_obserwacji[2], 0.0, 1.0, 0.0);

    // Narysowanie osi uk³adu wspó³rzêdnych
    Axes();

    // Aktualizacja œwiate³ w zale¿noœci od statusu manipulacji mysz¹
    if (status == 3)
    {
        theta_x1 -= (delta_x * pix2angle / 100.0f);
        theta_y1 += (delta_y * pix2angle / 100.0f);
        GLfloat light_position[] = { observerXS(10.0, theta_x1, theta_y1), observerYS(10.0, theta_y1), observerZS(10.0, theta_x1, theta_y1), 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }
    else if (status == 4)
    {
        theta_x2 -= (delta_x * pix2angle / 100.0f);
        theta_y2 += (delta_y * pix2angle / 100.0f);
        GLfloat light_position[] = { observerXS(10.0, theta_x2, theta_y2), observerYS(10.0, theta_y2), observerZS(10.0, theta_x2, theta_y2), 1.0 };
        glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    }

    // Obrót obiektu
    glRotatef(theta_x, 0.0, 1.0, 0.0);
    glRotatef(theta_y, 1.0, 0.0, 0.0);

    // Ustawienie koloru na bia³y
    glColor3f(1.0f, 1.0f, 1.0f);

    // Narysowanie jajka
    Egg(N);

    // Zamiana buforów
    glutSwapBuffers();
}


// Funkcja inicjalizuj¹ca ustawienia sceny
void MyInit(void) {
    // Ustawienie koloru t³a na czarny
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

// Parametry materia³u bardziej po³yskuj¹cego
GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };  // Refleksja otoczenia (ambient reflection)
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  // Rozproszenie œwiat³a (diffuse reflection)
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // Po³ysk (specular reflection)
GLfloat mat_shininess = 100.0;                     // Wspó³czynnik po³ysku (shininess)


    // Pozycje œwiate³
    GLfloat light_position1[] = { observerXS(10.0, theta_x1, theta_y1), observerYS(10.0, theta_y1), observerZS(10.0, theta_x1, theta_y1), 1.0 };
    GLfloat light_position2[] = { observerXS(10.0, theta_x2, theta_y2), observerYS(10.0, theta_y2), observerZS(10.0, theta_x2, theta_y2), 1.0 };

    // Parametry œwiat³a
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 0.0, 0.0, 0.5 };
    GLfloat att_constant = 1.0;
    GLfloat att_linear = 0.05;
    GLfloat att_quadratic = 0.001;

    // Ustawienia materia³u
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    // Ustawienia œwiate³
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

    GLfloat light_diffuse1[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat light_specular1[] = { 0.0, 0.0, 1.0, 0.5 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);

    // Model cieniowania
    glShadeModel(GL_SMOOTH);
    
    // W³¹czenie obs³ugi oœwietlenia i testu g³êbokoœci
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

// Funkcja zmieniaj¹ca rozmiar okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Ustawienie stosu macierzy rzutowania
    pix2angle = 360.0 / (float)horizontal;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 1.0, 1.0, 30.0);

    // Ustawienie obszaru renderowania
    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

    // Ustawienie stosu macierzy modelowania
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Funkcja obs³uguj¹ca menu
void menu() {
    cout << "Podaj wartosc liczby N: ";
    cin >> N;
    while (N < 0) {
        cout << "Podana niepoprawna wartosc. Sprobuj ponownie: ";
        cin >> N;
    }
    cout << " Prawy przycisk myszy - Swiatlo 1" << endl
         << " Lewy przycisk myszy - Swiatlo 2" << endl;
    cout << endl;
}

// Funkcja main
int main(int argc, char* argv[]) {
    // Inicjalizacja menu
    menu();

    // Inicjalizacja okna GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInit(&argc, argv);
    glutCreateWindow("Rzutowanie perspektywiczne");
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);

    // Wywo³anie funkcji inicjalizuj¹cej
    MyInit();

    // W³¹czenie testu g³êbokoœci i œwiate³
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Rozpoczêcie obs³ugi pêtli GLUT
    glutMainLoop();

}

