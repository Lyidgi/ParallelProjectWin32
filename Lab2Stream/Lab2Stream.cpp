// Lab2Stream.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Lab2Stream.h"
#include <GL/gl.h> 
#include <GL/glu.h> 
#pragma comment (lib, "OpenGL32.lib") 
#pragma comment (lib, "Glu32.lib") 


#define M_PI	3.14159265358979323846
#define MAX_LOADSTRING 100
#define SWAPBUFFERS SwapBuffers(ghDC)

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AddCond(HWND, UINT, WPARAM, LPARAM);

Condition C;
//
int n=100;
double* x=new double[n+1];
double* y=new double[n+1];
double hx,hy;
double **u;
bool flag=false;
//
double** AlternatingDirectionMethod(double a,double b,double T);
double u0(double x, double y,double a,double b);
double* MethodProgonki (double *a,double* b,double* c,double* d,int m);
//Для Open GL
HWND  ghWnd; 
HDC   ghDC; 
HGLRC ghRC; 

BOOL bSetupPixelFormat(HDC hdc);
GLvoid initializeGL(GLsizei width, GLsizei height);
GLvoid resize( GLsizei width, GLsizei height );
GLvoid drawScene(double **u,double* x, double *y,int n,double h, bool flag);
GLvoid	DrawSystemKoord(GLvoid);
 
short chain(int j, int i);
//----------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB2STREAM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2STREAM));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2STREAM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LAB2STREAM);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	

	switch (message)
	{
	case WM_CREATE:
		ghDC = GetDC(hWnd); 
        if (!bSetupPixelFormat(ghDC)) 
            PostQuitMessage (0); 
 
        ghRC = wglCreateContext(ghDC); 
        wglMakeCurrent(ghDC, ghRC); 
        GetClientRect(hWnd, &rect); 
        initializeGL(rect.right, rect.bottom); 
		DrawSystemKoord();
        break;
    case WM_SIZE: 
        GetClientRect(hWnd, &rect); 
        resize(rect.right, rect.bottom); 
        break; 
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_MY_TASK:
			double **Rez;
			
			
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AddCond);

			Rez=AlternatingDirectionMethod(C.A,C.B,C.T);

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		// TODO: добавьте любой код отрисовки...
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE: 
        if (ghRC) 
            wglDeleteContext(ghRC); 
        if (ghDC) 
            ReleaseDC(hWnd, ghDC); 
        ghRC = 0; 
        ghDC = 0; 
        DestroyWindow (hWnd); 
        break; 
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK AddCond(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDDEFAULT)
		{
			C.A=100;
			C.B=100;
			C.T=100;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

double u0(double x, double y,double a,double b)
{
	return 50*sin(M_PI*x/a)*sin(M_PI*y/b);
}

double **AlternatingDirectionMethod(double a,double b,double T)
{
	int nxy=100;
	double hx=a/nxy;
	double hy=b/nxy;
	double ht=T/nxy;
	double* x=new double[nxy+1];
	double* y=new double[nxy+1];
	double* t1=new double[nxy+1];
	double* t12=new double[nxy];
	double **u=new double*[nxy+1];
	for (int i=0; i<=nxy; i++)
	{
		u[i]=new double[nxy+1];
		x[i]=i*hx;
		y[i]=i*hy;
		t1[i]=i*ht;
	}
	for (int i=0; i<nxy; i++)
		t12[i]=a+i*ht+ht/2;

	//начальные условия при t=0
	for (int i=0; i<=nxy; i++)
		for (int j=0; j<=nxy; j++)
			u[i][j]=u0(x[i],y[j],a,b);
	


	return u;
}

double* MethodProgonki (double *a,double* b,double* c,double* d,int m)
{
	double *alpha=new double[m-1];
	double *beta=new double[m];
	double *gamma=new double[m];
	double *x=new double[m];

	gamma[0]=b[0];
	alpha[0]=-c[0]/gamma[0];
	beta[0]=d[0]/gamma[0];
	for (int i=1; i<=m-1; i++)
	{
		gamma[i]=b[i]+a[i]*alpha[i-1];
		beta[i]=(d[i]-a[i]*beta[i-1])/gamma[i];
		alpha[i]=-c[i]/gamma[i];
	}
	gamma[m-1]=b[m-1]+a[m-1]*alpha[m-2];
	beta[m-1]=(d[m-1]-a[m-1]*beta[m-2])/gamma[m-1];
	
	//обратный ход метода
	x[m-1]=beta[m-1];
	for (int i=m-2; i>=0; i--)
		x[i]=alpha[i]*x[i+1]+beta[i];
	return x;
}

BOOL bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd, *ppfd; 
    int pixelformat; 
 
    ppfd = &pfd; 
 
    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    ppfd->nVersion = 1; 
    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  
                        PFD_DOUBLEBUFFER; 
    ppfd->dwLayerMask = PFD_MAIN_PLANE; 
    ppfd->iPixelType = PFD_TYPE_RGBA; 
    ppfd->cColorBits = 32; 
    ppfd->cDepthBits = 32; 
    ppfd->cAccumBits = 32; 
    ppfd->cStencilBits =32 ; 
 
    if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
    { 
        MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, L"SetPixelFormat failed", L"Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
} 

GLvoid initializeGL(GLsizei width, GLsizei height) 
{ 
	GLfloat aspect;
	GLenum errCode;
    const GLubyte *errString;

	GLfloat light_pos[]={300,100,600,0};//позиция источника света
	GLfloat white_light[]={1.0f,1.0f,1.0f,1.0f};
	GLfloat lmodel_ambient[]={0.3f,0.3f,0.3f,1.0f};//цвет источника света
	
    glViewport(0,0,width,height);
	aspect = (GLfloat) width / height;
 
    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity();
	gluPerspective(50,aspect,200,800);
	gluLookAt(0,100,500,0,0,0,0,1,0);
    glMatrixMode( GL_MODELVIEW );

	        // Освещение
	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	     // Проверка наличия ошибки
	if((errCode=glGetError())!=GL_NO_ERROR) {
		errString=gluErrorString(errCode);
		MessageBox(NULL, L"Ошибка при работе", L"Error", MB_OK);
	}

} 

GLvoid resize( GLsizei width, GLsizei height ) 
{ 
    GLfloat aspect; 
 
    glViewport( 0, 0, width, height ); 
 
    aspect = (GLfloat) width / height; 
 //Управление системой координат
    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity(); 
    gluPerspective(50,aspect,200,800);
	gluLookAt(0,100,300,0,0,0,0,1,0);//1-лево,право2-обзор сверху,снизу 3-удаленность от наблюдателя
    glMatrixMode( GL_MODELVIEW ); 
} 

GLvoid drawScene(double** u,double* x,double* y,int n,double hx,bool flag) 
{
	
	if (flag)
	{
	GLfloat mat_diffuse_pir[]={1.0f,0.1f,0.0f,0.0f};
	GLenum errCode;
	
	const GLubyte *errString;
	//Очиска буферов
	glClearColor(0,0,0,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse_pir);
	double hy=hx;
	
	glPushMatrix();
	glRotatef(-10,0,1,0);//1-На какой угол 2,3,4 - по какой оси Ох,Оу,Оz
	//glRotatef(90,0,1,0);
	    
	
	GLfloat *vertex=new GLfloat[(n+1)*(n+1)*3];//точки сетки
	GLfloat *normal=new GLfloat[(n+1)*(n+1)*3];//нормали
	int k=0;
	for (int i=0; i<=n; i++)
        for (int j=0; j<=n; j++)
		{
                vertex[k]=x[i];
                k++;
                vertex[k]=u[i][j];
                k++;
                vertex[k]=y[j];
                k++;
       }
		GLvoid *indexBuffer;
		// временный массив индексов
		GLuint* index;
		// 2*(n+1) - количество индексов в ленте
		// n - количество лент
		// (n-1) - добавленные индексы для связки лент 
		// размер индексного массива
		int sizeindex=2*(n+1)*n + (n-1);
		index = new GLuint[sizeindex];
		
		 k=0;
int j=0;
while (j < n) {
        // лента слева направо
        for (int i = 0; i <= n; i++) {
                index[k] = chain(j,i);
                k++;
                index[k] = chain(j+1,i);
                k++;
        }
        if (j <= n-1){
                // вставим хвостовой индекс для связки
                index[k] = chain(j+1,n);
                k++;
        }
        // переводим ряд
        j++;

        // проверяем достижение конца
        if (j < n){
                // лента справа налево
                for (int i = n; i >= 0; i--) {
                        index[k] = chain(j,i);
                        k++;
                        index[k] = chain(j+1,i);
                        k++;
                }
               if (j <= n-1){
                        // вставим хвостовой индекс для связки
                        index[k] = chain(j+1,0);
                        k++;
                }
                // переводим ряд
                j++;
        }
} 
//Расчет нормалей
//---------------------------------------
float **normalX=new float*[n+1];
float **normalY=new float*[n+1];
float **normalZ=new float*[n+1];

 for (int i=0; i<=n; i++)
 {
	 normalX[i]=new float[n+1];
	 normalY[i]=new float[n+1];
	 normalZ[i]=new float[n+1];
 }

 for (int i=0; i<n; i++)
               for (int j=0; j<n; j++) {
                        normalX [i] [j] = - ( u [i] [j+1] - u [i] [j] ) * hx;
                        normalY [i] [j] = hx * hy;
                        normalZ [i] [j] = - hx * ( u [i+1] [j] - u [i] [j] );
                }
   
        //нормаль для i=n
        for (int j=0; j<n; j++){
                normalX [j] [n] = ( u [ j ] [ n -1] - u [ j ] [ n] ) * hy;
                normalY [j] [n] = hx * hy;
                normalZ [j] [n] = - hx * ( u [ j+1 ] [ n] - u [ j ] [ n ] );
        }
        //нормаль для j=n
        for (int i=0; i<n; i++){
                normalX [n] [ i ] = - ( u [ n ] [ i+1 ] - u [ n ] [ i ] ) * hy;
                normalY [n] [ i ] = hx * hy;
                normalZ [n] [ i ] = hx * ( u [ n-1 ] [ i ] - u [ n ] [ i ] );
        }
        //нормаль для i=n и j=n
        normalX [n] [ n ]= (u [ n] [ n-1] - u [ n] [n]) * hy;
        normalY [n] [ n ] = hx * hy;
        normalZ [n] [ n ] = hx * (u [n-1] [n] - u[n ] [n]);
        //переписываем координаты вектора нормали в одномерный массив normal
         k=0;
        for (int j=0; j<=n; j++){
                for (int i=0; i<=n; i++){
                        normal[k]=normalX[j][i];
                        k++;
                        normal[k]=normalY[j][i];
                        k++;
                        normal[k]=normalZ[j][i];
                        k++;
                }
        }

		for (int i=0; i<=n; i++)
 {
	 delete []normalX[i];
	 delete []normalY[i];
	 delete []normalZ[i];
 }
		delete []normalX;
delete []normalY;
delete []normalZ;
//------------------------------------------
glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normal);
    glVertexPointer(3, GL_FLOAT, 0, vertex);

    glPushMatrix();
	glDrawElements(GL_TRIANGLES, sizeindex, GL_UNSIGNED_SHORT, index);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
   glDisableClientState(GL_NORMAL_ARRAY);

	if((errCode=glGetError())!=GL_NO_ERROR) {
		errString=gluErrorString(errCode);
		MessageBox(NULL, L"Ошибка при работе", L"Error", MB_OK);
	}
	SWAPBUFFERS;
	delete []normal;
	delete []vertex;
}

}
 short chain(int j, int i){
        return (short) (i+j*(100+1));
}

 GLvoid DrawSystemKoord(){
	 
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(1);       // ширину линии
                      // устанавливаем 1
	glBegin(GL_LINES);
	glColor3d(1,0,0);     // красный цвет
		glVertex3d(-4.5,3,0); // первая линия
		glVertex3d(-3,3,0);
		glColor3d(0,1,0);     // зеленый
		glVertex3d(-3,3.3,0); // вторая линия
		glVertex3d(-4,3.4,0);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
 }