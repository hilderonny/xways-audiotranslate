#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include "X-Tension.h"
#include "ApiUrlInput.h"

const std::string CONFIG_FILENAME = "XT-AudioTranslate.cfg";

// Funktion zum Lesen des Buffers aus einer Datei
static void ReadBufferFromFile(const std::string& fileName, CHAR* buffer, DWORD bufferLen) {
    WCHAR logBuffer[256];
    swprintf(logBuffer, 256, L"ReadBufferFromFile : buffer: %hs, bufferLen: %i", buffer, bufferLen);
    XWF_OutputMessage(logBuffer, 0);
    std::ifstream inFile(fileName, std::ios::binary);
    if (inFile) {
        inFile.read(buffer, bufferLen - 1);  // Nur so viel lesen wie der Puffer erlaubt
        buffer[inFile.gcount()] = '\0';  // Nullterminierung sicherstellen
        inFile.close();
        swprintf(logBuffer, 256, L"ReadBufferFromFile : buffer: %hs, bufferLen: %i", buffer, bufferLen);
        XWF_OutputMessage(logBuffer, 0);
    }
    else {
        // Falls Datei nicht existiert, Puffer mit Leerzeichen initialisieren
        memset(buffer, 0, sizeof(CHAR) * bufferLen);
    }
}

// Funktion zum Schreiben des Buffers in eine Datei
static int WriteBufferToFile(const std::string& fileName, const CHAR* buffer) {
    std::ofstream outFile(fileName, std::ios::binary);
    if (outFile) {
        outFile.write(buffer, strlen(buffer));
        outFile.close();
        return 0;
    }
    else {
        return 1;
    }
}

// Funktion zur Konvertierung von CHAR nach WCHAR
static void ConvertCharToWChar(const CHAR* charBuffer, WCHAR* wcharBuffer, DWORD bufferLen) {
    MultiByteToWideChar(CP_ACP, 0, charBuffer, -1, wcharBuffer, bufferLen);
}

// Funktion zur Konvertierung von WCHAR nach CHAR
static void ConvertWCharToChar(const WCHAR* wcharBuffer, CHAR* charBuffer, DWORD bufferLen) {
    WideCharToMultiByte(CP_ACP, 0, wcharBuffer, -1, charBuffer, bufferLen, NULL, NULL);
}

static int SaveApiUrl(const CHAR* apiUrl) {
    WCHAR logBuffer[256];
    swprintf(logBuffer, 256, L"SaveApiUrl : apiUrl: %hs", apiUrl);
    XWF_OutputMessage(logBuffer, 0);

    return WriteBufferToFile(CONFIG_FILENAME, apiUrl);
}

void LoadApiUrl(CHAR* buffer, DWORD bufferLen) {
    ReadBufferFromFile(CONFIG_FILENAME, buffer, bufferLen);
}

void ShowApiUrlDialog() {
    CHAR buffer[256] = { 0 };

    // Vor dem Öffnen des Dialogs: Lesen der bisherigen Eingabe aus der Datei
    LoadApiUrl(buffer, 256);

    // Nachricht für den Dialog
    LPCWSTR message = L"API URL:";

    // Temporärer Wide-Character-Puffer für die Konvertierung
    WCHAR wideBuffer[256] = { 0 };

    // Konvertiere CHAR zu WCHAR
    ConvertCharToWChar(buffer, wideBuffer, sizeof(wideBuffer) / sizeof(wideBuffer[0]));

    // Dialog öffnen und Benutzereingabe abrufen (mit WCHAR)
    INT64 result = XWF_GetUserInput((LPWSTR)message, wideBuffer, sizeof(wideBuffer), 0);

    // Nach dem Schließen des Dialogs: Konvertiere den Wide-Character-Puffer zurück zu CHAR
    if (result != -1) {
        ConvertWCharToChar(wideBuffer, buffer, sizeof(buffer));
        SaveApiUrl(buffer);
    }

}
