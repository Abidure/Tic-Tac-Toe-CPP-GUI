// TicTacToe_CPP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe_CPP.h"
#include "Windowsx.h"
#include "CTicTacToe.h"

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

//Globals
CTicTacToe tictactoe;
int cellSize = tictactoe.getCellSize();
HBRUSH hbr1, hbr2;
int playerTurn = tictactoe.getPlayerTurn();
int winner = tictactoe.getWinner();

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
				ZeroMemory(tictactoe.gameBoard, sizeof(tictactoe.gameBoard));
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

		int index = tictactoe.GetCellNumberFromPoint(hWnd, xPos, yPos);

		//Sanity Check by printing out the index on the board
		HDC hdc = GetDC(hWnd);
		if (hdc != NULL)
		{
			//WCHAR temp[100];

			//wsprintf(temp, L"Index = %d", index);
			//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

			//Get cell dimension from its index
			if (index != -1) {
				RECT rcCell;
				if ((0 == tictactoe.gameBoard[index]) && tictactoe.GetCellRect(hWnd, index, &rcCell)) {
					tictactoe.gameBoard[index] = playerTurn;
					FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);
					//Check for winner
					winner = tictactoe.GetWinner(tictactoe.wins);
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
					tictactoe.ShowTurn(hWnd, hdc);
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
		if (tictactoe.GetGameBoardRect(hWnd, &rc))
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
				tictactoe.ShowTurn(hWnd, hdc);
			}


			FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); //Does the same thing as the line below without extra code to declare brush color
			//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); //Rectangle with a border and default background
		}

		//Draw vertical lines
		for (int i = 0; i < 4; i++) {
			tictactoe.DrawLine(hdc, rc.left + cellSize * i, rc.top, rc.left + cellSize * i, rc.bottom); //draws a line at x1, y1, x2, y2
		}

		//Draw horizontal lines
		for (int i = 0; i < 4; i++) {
			tictactoe.DrawLine(hdc, rc.left, rc.top + cellSize * i, rc.right, rc.top + cellSize * i); //draws a line at x1, y1, x2, y2
		}
		EndPaint(hWnd, &ps);

		//draw al loccupied cells
		RECT rcCell;
		for (int i = 0; i < sizeof(tictactoe.gameBoard) / sizeof(tictactoe.gameBoard[0]); i++) {
			if ((0 == tictactoe.gameBoard[i]) && tictactoe.GetCellRect(hWnd, i, &rcCell)) {
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