//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 560

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_gdi.h"
#include "../ui.h"
#include "../../common.h"

#include "game.c"

static int distance_matrix[10][10];
static int my_matrix[10][10];
static int their_matrix[10][10];
static bool can_guess;
static char player_name[16];
static char winner_name[16];
static bool game_running;

void stop_game_interface()
{
    game_running = false;
}

void display_winner(char *name)
{
    strcpy(winner_name, name);
}

void set_ui_name(char *name) {
    strcpy(player_name, name);
}

void pause_turn()
{
    if (can_guess == false) return;

    can_guess = false;

    time_t current = time(0);
    time_t stop = current + 2; // Wait 2 seconds
    while (true)
    {
        current = time(0);
        if (current >= stop) break;
    }

    can_guess = true;
}

void reset_matrices()
{
    int i, j;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            my_matrix[i][j] = FIELD_STATUS_UNTOUCHED;
            their_matrix[i][j] = FIELD_STATUS_UNTOUCHED;
            distance_matrix[i][j] = 0;
        }
    }
}

void update_their_matrix(Field field, FieldStatus value)
{
    their_matrix[field.row][field.column] = value;
    distance_matrix[field.row][field.column] = field.distance;
}

void update_my_matrix(Field field, FieldStatus value)
{
    my_matrix[field.row][field.column] = value;
}

static LRESULT CALLBACK WindowProcedure(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    if (nk_gdi_handle_event(wnd, msg, wparam, lparam)) {
        return 0;
    }

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

void start_ui()
{
    GdiFont* font;
    struct nk_context *ctx;

    WNDCLASSW wc;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD ex_style = WS_EX_APPWINDOW;
    HWND wnd;
    HDC dc;

    game_running = true;
    can_guess = true;
    winner_name[0] = '\0';

    bool needs_refresh = true;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"BattleshipsWindowClass";
    RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, ex_style);

    wchar_t title[50];
    swprintf(title, 50, L"Hidden Mines - %hs", player_name);
    wnd = CreateWindowExW(ex_style, wc.lpszClassName, title,
                          style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                          rect.right - rect.left, rect.bottom - rect.top,
                          NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(wnd);

    /* GUI */
    font = nk_gdifont_create("Arial", 16);
    ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (game_running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (!needs_refresh) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                game_running = false;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = true;
        } else needs_refresh = false;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                game_running = false;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = true;
        }
        nk_input_end(ctx);

        game(ctx, my_matrix, their_matrix, distance_matrix, can_guess, winner_name);

        /* Draw */
        nk_gdi_render(nk_rgb(30, 30, 30));
    }

    nk_gdifont_del(font);
    ReleaseDC(wnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}