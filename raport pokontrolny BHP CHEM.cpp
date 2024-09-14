#include <windows.h>
#include <commdlg.h>
#include "framework.h"

#define MAX_LOADSTRING 100
#define IDC_LOAD_IMAGE    101
#define IDC_IMAGE_DESC    102
#define IDC_IMAGE_DISPLAY 103
#define IDC_SAVE_BUTTON   104
#include <iostream>
#include <iTextSharp/iTextSharp.h>

int main()
{
    // Użyj biblioteki iTextSharp do generowania pliku PDF
    using (var pdfDoc = new iTextSharp.text.Document())
    {
        using (var pdfWriter = PdfWriter.GetInstance(pdfDoc, new FileStream("plik.pdf", FileMode.Create)))
        {
            pdfDoc.Open();
            var image = iTextSharp.text.Image.GetInstance("obraz.jpg");
            image.ScaleToFit(240, 240);
            pdfDoc.Add(image);
            pdfDoc.Close();
        }
    }

    return 0;
}
// Global variables
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // Title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // Window class name
WCHAR szFileName[MAX_PATH];                     // Selected file name
WCHAR szDescription[MAX_LOADSTRING];            // Description text
HBITMAP hBitmap = NULL;                         // Bitmap handle
HWND hStaticImageDisplay = NULL;                 // Handle for the static image display
HWND hEditImageDesc = NULL;                     // Handle for the image description edit box

// Function declarations
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

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RAPORTPOKONTROLNYBHPCHEM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Initialize the application
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAPORTPOKONTROLNYBHPCHEM));

    MSG msg;

    // Main message loop
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

// Registers the window class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAPORTPOKONTROLNYBHPCHEM));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RAPORTPOKONTROLNYBHPCHEM);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Initializes the instance and creates the main window
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store the instance handle in the global variable

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

// Processes messages for the main window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Create the Load Image button
        HWND hButtonLoadImage = CreateWindowW(L"BUTTON", L"Wczytaj zdjęcie",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 120, 30, hWnd, (HMENU)IDC_LOAD_IMAGE, hInst, NULL);

        // Create the Description edit box
        hEditImageDesc = CreateWindowW(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
            150, 10, 200, 100, hWnd, (HMENU)IDC_IMAGE_DESC, hInst, NULL);

        // Create the Static Image Display
        hStaticImageDisplay = CreateWindowW(L"STATIC", NULL,
            WS_CHILD | WS_VISIBLE | SS_BITMAP,
            10, 50, 240, 240, hWnd, (HMENU)IDC_IMAGE_DISPLAY, hInst, NULL);

        // Create the Save button
        HWND hButtonSave = CreateWindowW(L"BUTTON", L"Zapisz",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 300, 120, 30, hWnd, (HMENU)IDC_SAVE_BUTTON, hInst, NULL);
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_LOAD_IMAGE:
        {
            OPENFILENAME ofn;       // Structure for the Open File dialog
            wchar_t szFile[260];    // File path
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
            ofn.lpstrFilter = L"Obrazy BMP\0*.BMP\0Wszystkie pliki\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Open the Open File dialog
            if (GetOpenFileName(&ofn))
            {
                // Load the selected image (only BMP format in this example)
                hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBitmap)
                {
                    // Set the image in the Static Image Display
                    HDC hdc = GetDC(hStaticImageDisplay);
                    int width = 240;
                    int height = 240;
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
                    BITMAP bmp;
                    GetObject(hBitmap, sizeof(bmp), &bmp);
                    StretchBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
                    SelectObject(hdcMem, hbmOld);
                    DeleteDC(hdcMem);
                    ReleaseDC(hStaticImageDisplay, hdc);

                    // Save the file name and description in global variables
                    wcscpy_s(szFileName, ofn.lpstrFile);
                    GetWindowText(hEditImageDesc, szDescription, MAX_LOADSTRING);
                }
            }
        }
        break;

        case IDC_SAVE_BUTTON:
        {
            // Save the image and description
            // Użyj biblioteki iTextSharp do generowania pliku PDF
            using (var pdfDoc = new iTextSharp.text.Document())
            {
                using (var pdfWriter = PdfWriter.GetInstance(pdfDoc, new FileStream(szFileName + ".pdf", FileMode.Create)))
                {
                    pdfDoc.Open();
                    var image = iTextSharp.text.Image.GetInstance(hBitmap);
                    image.ScaleToFit(240, 240);
                    pdfDoc.Add(image);
                    pdfDoc.Add(new iTextSharp.text.Paragraph(szDescription));
                    pdfDoc.Close();
                }
            }
        }
        break;
        }
    }
    break;

    case WM_DESTROY:
        if (hBitmap)
        {
            DeleteObject(hBitmap);  // Zwolnienie pamięci bitmapy
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for the About box
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