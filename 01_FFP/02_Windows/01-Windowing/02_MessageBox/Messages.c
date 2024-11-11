// Windows Header Files
#include <windows.h>

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Local Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;                                                                                                                              
	TCHAR szAppName[] = TEXT(" ABCWindow ");

	//code
	//WNDCEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	//Create Window
	hwnd = CreateWindow(szAppName,
		TEXT("Abhay B. Chilka"),
	WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	// Show The Window
	ShowWindow(hwnd, iCmdShow);

	// Paint/Re-Draw The Window
	UpdateWindow(hwnd);

	// Message-loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return((int)msg.wParam);
}


//CallBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code

	switch (iMsg)
	{
      case WM_CREATE:
        MessageBox(hwnd, TEXT("WM_CREATE Arrived"), TEXT("Message"), MB_OK);   
        break;

      case WM_SIZE:
        MessageBox(hwnd, TEXT("WM_SIZE Arrived"), TEXT ("Message"), MB_OK);
        break;
      
      case WM_KEYDOWN:
            switch (LOWORD (wParam))
            {
            case VK_ESCAPE:
                MessageBox(hwnd, TEXT("WM_KEYDOWN-VKESCAPE Arrived"), TEXT("Message"), MB_OK);
                break;
            }
            break;

      case WM_CHAR:
        switch(LOWORD (wParam))
        {
            case 'F':
            MessageBox(hwnd, TEXT("WM_CHAR_F key Press"), TEXT("Message"), MB_OK);
            break;
        }
        break;
        
      case WM_DESTROY :
		PostQuitMessage(0);
		break;
	   default :
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
