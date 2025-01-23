// CSCI156_instructor.cpp :

/*
This program implements the instructor client for the "Breakout Room" system.
It connects to the server, authenticates with a password, and allows the instructor to
create breakout rooms, send messages, and manage student interactions.
*/

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080               // port number used to connect to the server
#define PASSWORD "@mypassword@" // password used to authenticate the instructor client
using namespace std;

SOCKET client_fd;       // socket descriptor for the client
bool running = true;    // flag to keep the main loop running


// RECEIVE MESSAGES FROM THE SERVER
void receive_messages() {
    char buffer[1024] = { 0 };  // buffer to store received messages
    while (running) {
        int valread = recv(client_fd, buffer, sizeof(buffer) - 1, 0);   // read messages from the server
        if (valread > 0) {
            buffer[valread] = '\0';         // null-terminate the recieved string
            cout << "\r" << buffer << endl; // display the message
            cout << "You: ";
            fflush(stdout); // ensure the prompt is shown after the message
        }
        else {
            cerr << "Connection lost. Error code: " << WSAGetLastError() << endl;   // handle connection loss
            running = false;    // stop the loop
        }
    }
}

int main() {
    WSADATA wsaData;

    // INTITIALIZE WINSTOCK
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return -1;
    }

    // CREATE SOCKET
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket creation error\n";
        WSACleanup();
        return -1;
    }

    // SETUP SERVER ADDRESS
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;   // IPv4
    server_addr.sin_port = htons(PORT); // convert port number to network byte order
    const char* server_ip_address = "XXX.XXX.XXX.XXX"; // server IP

    if (inet_pton(AF_INET, server_ip_address, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid address / Address not supported\n";
        closesocket(client_fd);
        WSACleanup();
        return -1;
    }

    // CONNECT TO SERVER
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed\n";
        closesocket(client_fd);
        WSACleanup();
        return -1;
    }

    // AUTHENTICATE WITH SERVER
    send(client_fd, PASSWORD, strlen(PASSWORD), 0); // send password
    send(client_fd, "instructor", 10, 0);           // identify as an instructor

   // DISPLAY INSTRUCTOR COMMANDS
    cout << "Instructor Commands:\n";
    cout << "- /createRoom: Create a new breakout room.\n";
    cout << "- /addStudentToRoomX StudentY: Add a student to breakout room X.\n";
    cout << "- /deleteRoom: Delete the most recently created breakout room.\n";
    cout << "- /sendGlobal <Message>: Send a global message to all students.\n";
    cout << "- /sendDirect2StudentX <Message>: Send a direct message to student X.\n";
    cout << "- Type your messages to communicate in the main chat or send specific instructions.\n";
    cout << "- Type 'exit' to disconnect.\n";


    thread receiver(receive_messages);  // start thread handle incoming messages

    string message;
    while (running) {
        cout << "You: ";
        getline(cin, message);  // read input from the instructor

        if (message == "exit") {    // check if the instructor wants to exit
            running = false;
            break;
        }

        send(client_fd, message.c_str(), message.size(), 0);    // send the message to the server
    }

    receiver.join();        // wait for the message reciever thread to finish
    closesocket(client_fd); // close the socket
    WSACleanup();           // clean up the Winsock resources
    return 0;
}


