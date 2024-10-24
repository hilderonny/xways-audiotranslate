#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "TaskBridge.h"

#pragma comment(lib, "ws2_32.lib") // Link mit der Winsock-Bibliothek

/* So muss das bei POST JSON aussehen:

------WebKitFormBoundaryGOVOdBRQNCcCuYBB
Content-Disposition: form-data; name="json"

{"type":"translate","data":{"targetlanguage":"de","texts":["Hello World","You are welcome"]}}
------WebKitFormBoundaryGOVOdBRQNCcCuYBB--



*/


void sendFile(const char* server, const char* filePath, const char* endpoint) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[4096];

    // Initialisierung von Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Fehler bei der Initialisierung von Winsock." << std::endl;
        return;
    }

    // Socket erstellen
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Fehler beim Erstellen des Sockets." << std::endl;
        WSACleanup();
        return;
    }

    // Serveradresse setzen
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80); // HTTP Port
    inet_pton(AF_INET, server, &serverAddr.sin_addr);

    // Mit dem Server verbinden
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Verbindungsfehler." << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Datei �ffnen
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler beim �ffnen der Datei." << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Dateiinhalt lesen
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Multipart-Header erstellen
    const std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW"; // Beispiel-Boundary
    std::string request = "POST " + std::string(endpoint) + " HTTP/1.1\r\n";
    request += "Host: " + std::string(server) + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    request += "Content-Length: " + std::to_string(request.size() + fileContent.size() + boundary.size() * 2 + 6) + "\r\n"; // 6 f�r \r\n\r\n

    request += "\r\n"; // Anforderungsinhalt beginnt hier

    // F�ge das erste Multipart-Teil hinzu
    request += "--" + boundary + "\r\n";
    request += "Content-Disposition: form-data; name=\"file\"; filename=\"" + std::string(filePath) + "\"\r\n";
    request += "Content-Type: application/octet-stream\r\n\r\n";
    request += fileContent; // F�ge den Dateiinhalt hinzu
    request += "\r\n"; // Abschluss des Inhalts

    // F�ge das letzte Multipart-Teil hinzu
    request += "--" + boundary + "--\r\n"; // Ende der Anfrage

    // Anfrage senden
    send(sock, request.c_str(), (int)request.size(), 0);

    // Antwort empfangen
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0'; // Nullterminator
        std::cout << buffer; // Antwort ausgeben
    }

    // Socket schlie�en und Winsock bereinigen
    closesocket(sock);
    WSACleanup();
}

// Funktion zum Extrahieren von Host, Pfad und Port aus einer URL
void parseURL(const std::string& url, std::string& host, std::string& path, std::string& port) {
    size_t schemaPos = url.find("://");
    schemaPos = (schemaPos != std::string::npos) ? schemaPos + 3 : 0;

    size_t portPos = url.find(':', schemaPos);
    size_t pathPos = url.find('/', schemaPos);

    if (portPos != std::string::npos && (pathPos == std::string::npos || portPos < pathPos)) {
        host = url.substr(schemaPos, portPos - schemaPos);
        pathPos = url.find('/', portPos);
        port = url.substr(portPos + 1, pathPos - portPos - 1);
    }
    else {
        host = url.substr(schemaPos, pathPos - schemaPos);
        port = "80"; // Standard-Port falls nicht angegeben
    }

    if (pathPos != std::string::npos) {
        path = url.substr(pathPos);
    }
    else {
        path = "/";
    }
}

// Erweiterte Funktion zum HTTP-GET mit Unterst�tzung f�r benutzerdefinierte Ports
std::string httpGET(const std::string& url) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    std::string response, host, path, port;

    // URL parsen, um Host, Pfad und Port zu extrahieren
    parseURL(url, host, path, port);

    // Winsock initialisieren
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return "";
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Adresse aufl�sen
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed." << std::endl;
        WSACleanup();
        return "";
    }

    // Socket erstellen
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return "";
    }

    // Mit dem Server verbinden
    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(sock);
        freeaddrinfo(result);
        WSACleanup();
        return "";
    }

    // HTTP GET-Anfrage erstellen
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n\r\n"; // Verbindung schlie�en nach der Antwort

    // Anfrage senden
    send(sock, request.c_str(), (int)request.length(), 0);

    // Antwort empfangen
    char buffer[4096];
    int bytesReceived;
    bool headerParsed = false;

    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        response += buffer;

        // Header �berspringen
        if (!headerParsed) {
            size_t headerEnd = response.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                response = response.substr(headerEnd + 4); // Entferne den Header
                headerParsed = true;
            }
        }
    }

    // Aufr�umen
    closesocket(sock);
    freeaddrinfo(result);
    WSACleanup();

    return response;
}

std::string translate()
{
    nlohmann::json translate_json = {
        { "type", "translate" },
        { "data", {
            { "sourcelanguage", "en" },
            { "targetlanguage", "de" },
            { "texts", {
                "Hello world",
                "you are welcome"
            }}
        }}
    };
    return translate_json.dump(0);
}
