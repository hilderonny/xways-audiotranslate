#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "TaskBridgeOld.h"

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

    // Datei öffnen
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei." << std::endl;
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
    request += "Content-Length: " + std::to_string(request.size() + fileContent.size() + boundary.size() * 2 + 6) + "\r\n"; // 6 für \r\n\r\n

    request += "\r\n"; // Anforderungsinhalt beginnt hier

    // Füge das erste Multipart-Teil hinzu
    request += "--" + boundary + "\r\n";
    request += "Content-Disposition: form-data; name=\"file\"; filename=\"" + std::string(filePath) + "\"\r\n";
    request += "Content-Type: application/octet-stream\r\n\r\n";
    request += fileContent; // Füge den Dateiinhalt hinzu
    request += "\r\n"; // Abschluss des Inhalts

    // Füge das letzte Multipart-Teil hinzu
    request += "--" + boundary + "--\r\n"; // Ende der Anfrage

    // Anfrage senden
    send(sock, request.c_str(), (int)request.size(), 0);

    // Antwort empfangen
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0'; // Nullterminator
        std::cout << buffer; // Antwort ausgeben
    }

    // Socket schließen und Winsock bereinigen
    closesocket(sock);
    WSACleanup();
}

// Funktion zum Extrahieren von Host, Pfad und Port aus einer URL
static void parseURL(const std::string& url, std::string& host, std::string& path, std::string& port) {
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

static std::string sendRequest(const std::string& host, const std::string& port, const std::string& request) {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    std::string response;

    // Winsock initialisieren
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return "";
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Adresse auflösen
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

    // Anfrage senden
    send(sock, request.c_str(), (int)request.length(), 0);

    // Antwort empfangen
    char buffer[4096];
    int bytesReceived;
    bool headerParsed = false;

    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0'; // Null-Terminator für den String
        response += buffer;

        // Header überspringen
        if (!headerParsed) {
            size_t headerEnd = response.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                response = response.substr(headerEnd + 4); // Entferne den Header
                headerParsed = true;
            }
        }
    }

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "recv failed." << std::endl;
    }

    // Aufräumen
    closesocket(sock);
    freeaddrinfo(result);
    WSACleanup();

    return response;
}

// Erweiterte Funktion zum HTTP-GET mit Unterstützung für benutzerdefinierte Ports
std::string httpGET(const std::string& url) {

    // URL parsen, um Host, Pfad und Port zu extrahieren
    std::string host, path, port;
    parseURL(url, host, path, port);

    // HTTP GET-Anfrage erstellen
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Connection: close\r\n\r\n"; // Verbindung schließen nach der Antwort

    std::string response = sendRequest(host, path, request);
    return response;
}

std::string httpPOSTfile(const std::string& url, std::string& fileContent, std::string& fileName) {

    // URL parsen, um Host, Pfad und Port zu extrahieren
    std::string host, path, port;
    parseURL(url, host, path, port);

    // Multipart-Header erstellen
    const std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW"; // Beispiel-Boundary
    std::string request = "POST " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    request += "Content-Length: " + std::to_string(request.size() + fileContent.size() + boundary.size() * 2 + 6) + "\r\n"; // 6 für \r\n\r\n

    request += "\r\n"; // Anforderungsinhalt beginnt hier

    // Füge das erste Multipart-Teil hinzu
    request += "--" + boundary + "\r\n";
    request += "Content-Disposition: form-data; name=\"file\"; filename=\"" + fileName + "\"\r\n";
    request += "Content-Type: application/octet-stream\r\n\r\n";
    request += fileContent; // Füge den Dateiinhalt hinzu
    request += "\r\n"; // Abschluss des Inhalts

    // Füge das letzte Multipart-Teil hinzu
    request += "--" + boundary + "--\r\n"; // Ende der Anfrage

    std::string response = sendRequest(host, path, request);
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
