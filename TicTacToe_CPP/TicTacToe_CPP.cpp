// TicTacToe_CPP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe_CPP.h"
#include "Windowsx.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TICTACTOECPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOECPP));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOECPP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(LTGRAY_BRUSH)); //changing window background to gray
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICTACTOECPP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

/*
Game Notes:
Player 1 = RED
Player 2 = Blue
*/

//Global variables for the game
const int cellSize = 100;
HBRUSH hbr1, hbr2;
int playerTurn = 1;
int gameBoard[9] = { 0,0,0,0,0,0,0,0,0 };
int winner = 0;
int wins[3];

BOOL GetGameBoardRect(HWND hwnd, RECT* pRect)
{
	RECT rc;	/*
				RECT Structure1
				left -- Specifies the x-coordinate of the upper-left corner of a rectangle.
				top -- Specifies the y-coordinate of the upper-left corner of a rectangle.
				right -- Specifies the x-coordinate of the lower-right corner of a rectangle.
				bottom -- Specifies the y-coordinate of the lower-right corner of a rectangle.
				RECT rctA;
				rctA.left = 20;
				rctA.top = 30;
				rctA.right  = 180;
				rctA.bottom = 230;
				RECT rctB = {20,30,180,230};
				*/
	if (GetClientRect(hwnd, &rc)) {
		int width = rc.right - rc.left; // right is width
		int height = rc.bottom - rc.top;// bottom is height
										// rc.left and rc.top would be useful when using GetWindowRect
										// find the four corners of the 3x3 table
		pRect->left = (width - cellSize * 3) / 2;
		pRect->top = (height - cellSize * 3) / 2;
		pRect->right = pRect->left + cellSize * 3;
		pRect->bottom = pRect->top + cellSize * 3;
		return TRUE;
	}
	SetRectEmpty(pRect);
	return FALSE;
}

// Draws a line given two sets of points
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	//Drawing lines
	MoveToEx(hdc, x1, y1, NULL); // The MoveToEx function updates the current position to the specified point and optionally returns the previous position.
	LineTo(hdc, x2, y2); // The LineTo function draws a line from the current position up to, but not including, the specified point.
}

int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
	RECT rc;
	POINT pt; // The POINT structure defines the x- and y-coordinates of a point
	pt.x = x; // Specifies the x-coordinate of a point
	pt.y = y; // Specifies the y-coordinate of a point
	// POINT pt = {x,y}; // Alternate ways to initialize a POINT structure

	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//user clicked inside gameboard
			//normalize (0 to 3*cellSize)
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / cellSize;
			int row = y / cellSize;

			//convert to index (0-8)
			return column + row * 3;
		}
	}
	return -1; //outside gameboard
}

//gives the coordinate values of the rectangle
int GetCellRect(HWND hWnd, int index, RECT* pRect) {

	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index>8) {
		return false;
	}

	if (GetGameBoardRect(hWnd, &rcBoard)) {
		//convert index into x,y pair
		int y = index / 3; //row number
		int x = index % 3; //col number

		pRect->left = rcBoard.left + x * cellSize + 1;
		pRect->top = rcBoard.top + y * cellSize + 1;

		pRect->right = pRect->left + cellSize - 1;
		pRect->bottom = pRect->top + cellSize - 1;

		return true;
	}
}

/*
0 1 2
3 4 5
6 7 8
Returns:
0 - empty board
1 - Player 1 wins
2 - Player 2 wins
3 - Game draw
*/

int GetWinner(int wins[3]) {
	int cells[] = { 0,1,2,3,4,5,6,7,8,0,3,6,1,4,7,2,5,8,0,4,8,2,4,6 };

	//check for winner
	for (int i = 0; i < sizeof(cells) / sizeof(cells[0]); i += 3) {
		if (0 != gameBoard[cells[i]] && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]]) {
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	for (int i = 0; i < sizeof(gameBoard) / sizeof(gameBoard[0]); i++) {
		if (0 == gameBoard[i]) {
			return 0; //at least one cell is empty; continue to play
		}
	}

	return 3; //it's a draw
}

void ShowTurn(HWND hWnd, HDC hdc) {
	RECT rc;

	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2";

	const WCHAR* pszTurnText = NULL;

	switch (winner) {
	case 0: //continue to play
		pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
		break;

	case 1: //continue to play
		pszTurnText = L"Player 1 Wins!";
		break;

	case 2: //continue to play
		pszTurnText = L"Player 1 Wins!";
		break;

	case 3: //continue to play
		pszTurnText = L"It's a draw!";
		break;

	default:
		pszTurnText = L"";
		break;
	}

	if (NULL != pszTurnText && GetClientRect(hWnd, &rc)) {
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		hbr1 = CreateSolidBrush(RGB(255, 0, 0));
		hbr2 = CreateSolidBrush(RGB(0, 0, 255));
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			//adding "New Game" menu item function
		case ID_FILE_NEWGAME:
		{
			int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
			if (IDYES == ret) {
				//reset and start a new game
				playerTurn = 1;
				int winner = 0;
				ZeroMemory(gameBoard, sizeof(gameBoard));
				//clear all drawings; force a paint message
				InvalidateRect(hWnd, NULL, TRUE); //post a WM_PAINT to window sprocess (WindowProc). It gets queued in message queue
				UpdateWindow(hWnd); // forcess immediate handling of WM_PAINT
			}
		}
		break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	// Detect mouse click
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//only handle clicks if player turn
		if (0 == playerTurn) {
			break;
		}

		int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

		//Sanity Check by printing out the index on the board
		HDC hdc = GetDC(hWnd);
		if (hdc != NULL)
		{
			WCHAR temp[100];

			//wsprintf(temp, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

			//Get cell dimension from its index
			if (index != -1) {
				RECT rcCell;
				if ((0 == gameBoard[index]) && GetCellRect(hWnd, index, &rcCell)) {
					gameBoard[index] = playerTurn;
					FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);
					//Check for winner
					winner = GetWinner(wins);
					if (winner == 1 || winner == 2) {
						MessageBox(hWnd,
							(winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
							L"You Win!",
							MB_OK | MB_ICONINFORMATION);
						playerTurn = 0;
					}
					else if (0 == winner) {
						playerTurn = (playerTurn == 2 ? 1 : 2);
					}
					else if (3 == winner) {
						MessageBox(hWnd,
							L"It's a draw!",
							L"DRAW!",
							MB_OK | MB_ICONEXCLAMATION);
					}

					//display turn
					ShowTurn(hWnd, hdc);
				}
			}

			ReleaseDC(hWnd, hdc);
		}
	}
	break;

	// Making the window size fixed
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
		pMinMax->ptMinTrackSize.x = cellSize * 5; // make sure the user doesn't resize the windows smaller than the game table
		pMinMax->ptMinTrackSize.y = cellSize * 5;
		pMinMax->ptMaxTrackSize.x = cellSize * 5;
		pMinMax->ptMaxTrackSize.y = cellSize * 5;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		//GetWindowRect: returns the size of the whole window including menu and not just a client area

		RECT rc;
		if (GetGameBoardRect(hWnd, &rc))
		{
			RECT rcClient;

			if (GetClientRect(hWnd, &rcClient)) {
				//Draw player 1 and player 2 text
				const WCHAR szPlayer1[] = L"Player 1"; //defining string arrays
				const WCHAR szPlayer2[] = L"Player 2"; //defining string arrays
				SetBkMode(hdc, TRANSPARENT);// setting text background
				SetTextColor(hdc, RGB(255, 0, 0)); //setting player 1 text to red
				TextOut(hdc, 16, 16, szPlayer1, lstrlen(szPlayer1));
				SetTextColor(hdc, RGB(0, 0, 255)); //setting player 2 text to blue
				TextOut(hdc, rcClient.right - 72, 16, szPlayer2, lstrlen(szPlayer2));

				//display turn
				ShowTurn(hWnd, hdc);
			}


			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); //Does the same thing as the line below without extra code to declare brush color
			//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); //Rectangle with a border and default background
		}

		//Draw vertical lines
		for (int i = 0; i < 4; i++) {
			DrawLine(hdc, rc.left + cellSize * i, rc.top, rc.left + cellSize * i, rc.bottom); //draws a line at x1, y1, x2, y2
		}

		//Draw horizontal lines
		for (int i = 0; i < 4; i++) {
			DrawLine(hdc, rc.left, rc.top + cellSize * i, rc.right, rc.top + cellSize * i); //draws a line at x1, y1, x2, y2
		}
		EndPaint(hWnd, &ps);

		//draw al loccupied cells
		RECT rcCell;
		for (int i = 0; i < sizeof(gameBoard) / sizeof(gameBoard[0]); i++) {
			if ((0 == gameBoard[i]) && GetCellRect(hWnd, i, &rcCell)) {
				FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);
			}
		}
	}
	break;
	case WM_DESTROY:
		DeleteObject(hbr1);
		DeleteObject(hbr2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
