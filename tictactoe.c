#include <windows.h>

#define CELL_SIZE 150
#define BOARD_SIZE 3

char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
char currentPlayer = 'X';   // X starts

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DrawBoard(HDC hdc) {
    HPEN gridPen = CreatePen(PS_SOLID, 12, RGB(120, 70, 40)); // brown thick lines
    SelectObject(hdc, gridPen);

    // Draw grid lines
    for (int i = 1; i < BOARD_SIZE; i++) {
        MoveToEx(hdc, i * CELL_SIZE, 0, NULL);
        LineTo(hdc, i * CELL_SIZE, CELL_SIZE * BOARD_SIZE);

        MoveToEx(hdc, 0, i * CELL_SIZE, NULL);
        LineTo(hdc, CELL_SIZE * BOARD_SIZE, i * CELL_SIZE);
    }

    DeleteObject(gridPen);
}

void DrawX(HDC hdc, int row, int col) {
    HPEN xPen = CreatePen(PS_SOLID, 15, RGB(30, 144, 255)); // Blue X
    SelectObject(hdc, xPen);

    int x1 = col * CELL_SIZE + 20;
    int y1 = row * CELL_SIZE + 20;
    int x2 = (col + 1) * CELL_SIZE - 20;
    int y2 = (row + 1) * CELL_SIZE - 20;

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    MoveToEx(hdc, x2, y1, NULL);
    LineTo(hdc, x1, y2);

    DeleteObject(xPen);
}

void DrawO(HDC hdc, int row, int col) {
    HPEN oPen = CreatePen(PS_SOLID, 15, RGB(255, 140, 0)); // Orange O
    SelectObject(hdc, oPen);

    int x = col * CELL_SIZE + CELL_SIZE / 2;
    int y = row * CELL_SIZE + CELL_SIZE / 2;

    Ellipse(hdc,
        x - 50, y - 50,
        x + 50, y + 50
    );

    DeleteObject(oPen);
}

void DrawSymbols(HDC hdc) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == 'X')
                DrawX(hdc, r, c);
            else if (board[r][c] == 'O')
                DrawO(hdc, r, c);
        }
    }
}

char CheckWinner() {
    // Rows and columns
    for (int i = 0; i < 3; i++) {
        if (board[i][0] &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2])
            return board[i][0];

        if (board[0][i] &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i])
            return board[0][i];
    }

    // Diagonals
    if (board[0][0] &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2])
        return board[0][0];

    if (board[0][2] &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0])
        return board[0][2];

    return 0;
}

void ResetBoard() {
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = 0;

    currentPlayer = 'X';
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        int col = x / CELL_SIZE;
        int row = y / CELL_SIZE;

        if (row < 3 && col < 3 && board[row][col] == 0) {
            board[row][col] = currentPlayer;
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }

        char winner = CheckWinner();
        if (winner) {
            char msgText[50];
            wsprintf(msgText, "Player %c wins!", winner);
            MessageBox(hwnd, msgText, "Game Over", MB_OK);
            ResetBoard();
        }

        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        DrawBoard(hdc);
        DrawSymbols(hdc);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmd, int show) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "TicTacToeClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "TicTacToeClass",
        "Tic Tac Toe (GUI)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CELL_SIZE * BOARD_SIZE + 20,
        CELL_SIZE * BOARD_SIZE + 40,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, show);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}