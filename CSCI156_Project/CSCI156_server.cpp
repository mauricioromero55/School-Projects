// CSCI156_server.cpp

/*
This program implements the server for a "Breakout Room" system, which manages connections with instructor
and student clients. It handles client authentication, messaging, and manages breakout rooms for one-to-one
 or group communication. The server processes commands from the instructor to create, modify, or delete rooms
 and ensures secure and organized communication
*/

#include "pch.h"
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <algorithm>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080   // port 8080 is commonly used for custom applications and avoids conflicts with reserved system ports
#define PASSWORD "@mypassword@"
using namespace std;



// CLASS TO STORE CLIENT INFORMATION
class Client {
public:
    SOCKET client_socket;
    int client_id;
    string type; // "student" or "instructor"


    Client(SOCKET socket, int id, const string& client_type)
        : client_socket(socket), client_id(id), type(client_type) {}
};


vector<Client> all_clients;             // all connected clients
vector<Client> main_chat;               // clients in the main chat
map<string, vector<Client>> breakout_rooms; // breakout room mapping (RoomName -> List of clients)
Client* instructor_client = nullptr;    // pointer to the instructor client
mutex clients_mutex;


int client_counter = 0;   // tracks all clients
int room_counter = 0;     // unique ID generator for breakout rooms


// HELPER FUNCTION TO FIND A CLIENT BY ID
Client* find_client_by_id(int client_id) {
    for (auto& client : all_clients) {
        if (client.client_id == client_id) {
            return &client;     // return pointer to client if id matches
        }
    }
    return nullptr;     // return null if no match found
}


// BROADCAST TO ALL CLIENTS IN THE MAIN CHAT(EXCLUDING THE SENDER)
void broadcast_to_main_chat(const string& message, SOCKET sender_socket) {
    clients_mutex.lock();   // lock to ensure safe access to client lists
    for (auto& client : main_chat) {
        if (client.client_socket != sender_socket) {    // dont send to message to sender
            send(client.client_socket, message.c_str(), (int)message.size(), 0);
        }
    }
    if (instructor_client && instructor_client->client_socket != sender_socket) {
        send(instructor_client->client_socket, message.c_str(), (int)message.size(), 0);    // send to instructor
    }
    clients_mutex.unlock(); // unlock after sending messages
}


// BROADCAST TO A SPECIFIC BREAKOUT ROOM
void broadcast_to_room(const string& room_name, const string& message, SOCKET sender_socket) {
    clients_mutex.lock();   // lock for thread safety
    if (breakout_rooms.count(room_name)) {  // check if room exists
        for (auto& client : breakout_rooms[room_name]) {
            if (client.client_socket != sender_socket) {    // don't send to sender
                send(client.client_socket, message.c_str(), (int)message.size(), 0);
            }
        }
        // notify the instructor with room details
        if (instructor_client) {
            string instructor_message = "Message from " + room_name + ": " + message;
            send(instructor_client->client_socket, instructor_message.c_str(), (int)instructor_message.size(), 0);
        }
    }
    clients_mutex.unlock(); // unlock after broadcasting
}


// BROADCAST TO ALL CLIENTS (GLOBAL BROADCAST)
void broadcast_to_all_clients(const string& message) {
    clients_mutex.lock();   // lock for thread safety
    for (auto& client : all_clients) {
        send(client.client_socket, message.c_str(), (int)message.size(), 0);    // send message to all clients
    }
    clients_mutex.unlock(); // unlock after sending
}


// NOTIFY A STUDENT ABOUT BEING ADDED OR REMOVED FROM A BREAKOUT ROOM
void notify_student(Client& student, const string& message) {
    send(student.client_socket, message.c_str(), (int)message.size(), 0);   // send notification message
}



// HANDLE DIRECT MESSAGES
void handle_direct_message(const string& command, SOCKET sender_socket, int sender_id, const string& sender_type) {
    if (command.rfind("/sendDirect2Student", 0) == 0) {     // handle direct message to student
        size_t id_start = command.find("Student") + 7;      // locate student id in command
        size_t space_pos = command.find(' ', id_start);     // locate space after id
        if (space_pos == string::npos) {
            string error_message = "Invalid command format. Use /sendDirect2StudentX <Message>.\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
            return;
        }
        int student_id = stoi(command.substr(id_start, space_pos - id_start));  // extract student id
        string message = command.substr(space_pos + 1); // extract message

        clients_mutex.lock();   // lock for thread safety
        Client* recipient = find_client_by_id(student_id);  // find recipient student
        if (recipient != nullptr && recipient->type == "student") {
            string direct_message = "Direct message from " + sender_type + " " + to_string(sender_id) + ": " + message;
            send(recipient->client_socket, direct_message.c_str(), (int)direct_message.size(), 0);  // send message
        }
        else {
            string error_message = "Student " + to_string(student_id) + " not found.\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
        }
        clients_mutex.unlock(); // unlock after sending
    }
    else if (command.rfind("/sendDirect2Instructor", 0) == 0) { // handle direct message to instructor
        string message = command.substr(22);    // extract message

        clients_mutex.lock();   // lock for thread safety
        if (instructor_client != nullptr) {
            string direct_message = "Direct message from Student " + to_string(sender_id) + ": " + message;
            send(instructor_client->client_socket, direct_message.c_str(), (int)direct_message.size(), 0);  // send message
        }
        else {
            string error_message = "Instructor not found.\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
        }
        clients_mutex.unlock(); // unlock after sending
    }
    else {
        string error_message = "Invalid direct message format.\n";
        send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
    }
}



// HANDLE INSTRUCTOR COMMANDS
void handle_instructor_command(const string& command, SOCKET sender_socket) {
    stringstream ss(command);
    string cmd;
    ss >> cmd;


    if (cmd == "/createRoom") {     // create a new breakout room
        clients_mutex.lock();       // lock for thread safety
        room_counter++;             // increment room counter
        string room_name = "breakoutRoom" + to_string(room_counter);    // generate room name
        breakout_rooms[room_name] = vector<Client>();                   // create empty room
        clients_mutex.unlock();                                         // unlock after creating room


        string success_message = "Breakout room '" + room_name + "' created.\n";
        send(sender_socket, success_message.c_str(), (int)success_message.size(), 0);   // notify instructor
    }
    else if (cmd == "/deleteRoom") {    // delete the most recently created room***
        clients_mutex.lock();           // lock for thread safety
        if (!breakout_rooms.empty()) {
            auto it = --breakout_rooms.end();   // get the last room
            string room_name = it->first;


            for (auto& student : it->second) {
                main_chat.push_back(student);   // move students back to main chat
                notify_student(student, "You have rejoined the main chat.\n");  // notify students
            }
            breakout_rooms.erase(room_name);    // delete room
            room_counter--; // Adjust room counter


            string success_message = "Room '" + room_name + "' deleted. Students moved to main chat.\n";
            send(sender_socket, success_message.c_str(), (int)success_message.size(), 0);   // notify instructor
        }
        else {
            string error_message = "No breakout rooms to delete.\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
        }
        clients_mutex.unlock();     // unlock after deleting
    }
    else if (cmd.rfind("/addStudentToRoom", 0) == 0) {  // add a student to a room***
        string room_name;
        string student_str;
        ss >> room_name >> student_str; // extract the room name and student identifier


        if (room_name.rfind("breakoutRoom", 0) == 0 && student_str.rfind("Student", 0) == 0) {  // check for valid room and student identifiers
            int student_id = stoi(student_str.substr(7));       // extract student ID from the identifier
            clients_mutex.lock();                               // lock for thread safety
            Client* student = find_client_by_id(student_id);    // find the student client by ID


            if (student && breakout_rooms.count(room_name)) {   // ensure the student exists and the room is valid
                breakout_rooms[room_name].push_back(*student);  // add the student to the breakout room
                main_chat.erase(remove_if(main_chat.begin(), main_chat.end(),
                    [student](const Client& c) { return c.client_id == student->client_id; }), main_chat.end());    // remove student from main chat
                notify_student(*student, "You have been added to " + room_name + ".\n");    // notify the student


                string success_message = "Student " + to_string(student_id) + " added to room '" + room_name + "'.\n";
                send(sender_socket, success_message.c_str(), (int)success_message.size(), 0);   // notify the instructor of success
            }
            else {
                string error_message = (student ? "Room '" + room_name + "' does not exist.\n" :
                    "Student " + to_string(student_id) + " not found.\n");
                send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // notify the instructor of the error
            }
            clients_mutex.unlock();     // unlock after processing
        }
        else {
            string error_message = "Invalid command format. Use /addStudentToRoomX StudentY.\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // notify the instructor of the invalid format
        }
    }
    else if (cmd == "/sendGlobal") {    // handle global messages from instructor***
        string global_message;
        getline(ss, global_message);                                    // read the global message from the command input
        if (!global_message.empty() && global_message[0] == ' ') {
            global_message = global_message.substr(1);                  // remove leading space if present
        }
        global_message = "Global message from Instructor: " + global_message;   // format the global message
        broadcast_to_all_clients(global_message);       // send the message to all clients
    }
    else if (cmd.rfind("/sendDirect2Student", 0) == 0) {    // handle direct messages to a specific student***
        try {
            handle_direct_message(command, sender_socket, instructor_client->client_id, "Instructor");  // process the direct message
        }
        catch (const exception& e) {
            string error_message = "Error processing direct message: " + string(e.what()) + "\n";
            send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // notify instructor of error
        }
    }
    else {  // handle invalid commands
        string error_message = "Invalid command.\n";
        send(sender_socket, error_message.c_str(), (int)error_message.size(), 0);   // notify instructor of invalid command
    }
}



// HANDLE CLIENT MESSAGES AND COMMANDS
void handle_client(SOCKET client_socket, int client_id) {   // process client connections and communication
    char buffer[1024] = { 0 };


    // Require password
    int valread = recv(client_socket, buffer, sizeof(buffer) - 1, 0);   // read password from client
    buffer[valread] = '\0';     // null-terminate the password string
    if (strcmp(buffer, PASSWORD) != 0) {
        string error_message = "Incorrect password. Disconnecting.\n";  // check if password matches
        send(client_socket, error_message.c_str(), (int)error_message.size(), 0);   // send error message
        closesocket(client_socket);     // close the connection
        return;
    }


    // Receive client type
    memset(buffer, 0, sizeof(buffer));
    valread = recv(client_socket, buffer, sizeof(buffer) - 1, 0);   // read client type (instructor or student)
    string client_type(buffer);


    // Add client to appropriate list
    clients_mutex.lock();               // lock client list for thread safety
    if (client_type == "instructor") {
        if (instructor_client != nullptr) {     // ensure only one instructor is connected
            string error_message = "An instructor is already connected.\n";
            send(client_socket, error_message.c_str(), (int)error_message.size(), 0);   // notify client
            closesocket(client_socket); // close the connection
            clients_mutex.unlock();
            return;
        }
        instructor_client = new Client(client_socket, client_id, "instructor"); // add instructor to the client list
        main_chat.push_back(*instructor_client); // add instructor to the main chat
        send(client_socket, "You are Instructor.\n", 21, 0);    // send confirmation to the instructor
    }
    else if (client_type == "student") {                            // handle student connection
        Client new_student(client_socket, client_id, "student");    // create a new student client
        all_clients.push_back(new_student);                         // add student to all_clients list
        main_chat.push_back(new_student);                           // add student to the main chat
        string welcome_message = "You are Student " + to_string(client_id) + ".\n";
        send(client_socket, welcome_message.c_str(), (int)welcome_message.size(), 0);   // send confirmation to student
    }
    clients_mutex.unlock(); // unlock the client list


    // COMMUNICATION LOOP***
    while (true) {
        memset(buffer, 0, sizeof(buffer));      // clear the buffer for new input
        valread = recv(client_socket, buffer, sizeof(buffer) - 1, 0);   // read messages from client


        if (valread <= 0 || strcmp(buffer, "exit") == 0) {  // handle client disconnection
            clients_mutex.lock();   // lock for safe removal
            all_clients.erase(remove_if(all_clients.begin(), all_clients.end(),
                [client_socket](const Client& c) { return c.client_socket == client_socket; }), all_clients.end()); // remove client from all_clients
            main_chat.erase(remove_if(main_chat.begin(), main_chat.end(),
                [client_socket](const Client& c) { return c.client_socket == client_socket; }), main_chat.end());   // remove client from main chat
            if (instructor_client && instructor_client->client_socket == client_socket) {   // handle instructor disconnection
                delete instructor_client;
                instructor_client = nullptr;
            }
            clients_mutex.unlock();     // unlock after removal
            closesocket(client_socket); // close the socket
            break;                      // exit the communication loop
        }


        string message(buffer);             // convert message to string
        if (client_type == "instructor") {
            if (message.rfind("/", 0) == 0) {   // check if message is a command
                handle_instructor_command(message, client_socket);  // process instructor commands
            }
            else {
                broadcast_to_main_chat("Instructor: " + message, client_socket);    // broadcast instructor message
            }
        }
        else if (client_type == "student") {
            if (message == "/requestJoinRoom") {    // handle room join request
                string request_message = "Student " + to_string(client_id) + " is requesting a breakout room.\n";
                if (instructor_client) {
                    send(instructor_client->client_socket, request_message.c_str(), (int)request_message.size(), 0);    // notify instructor
                }
            }
            else if (message.rfind("/sendDirect2", 0) == 0) {   // handle direct messages
                handle_direct_message(message, client_socket, client_id, "Student");
            }
            else {
                string room_name;
                clients_mutex.lock();               // lock breakout rooms for access
                for (auto& pair : breakout_rooms) { // iterate through breakout rooms
                    const string& key = pair.first; // room name
                    const vector<Client>& value = pair.second;  // clients in the room
                    if (find_if(value.begin(), value.end(), [client_id](const Client& c) { return c.client_id == client_id; }) != value.end()) {
                        room_name = key;    // find the room the student is in
                        break;
                    }
                }
                clients_mutex.unlock();     // unlock breakout rooms
                if (!room_name.empty()) {
                    broadcast_to_room(room_name, "Student " + to_string(client_id) + ": " + message, client_socket);    // broadcast within room
                }
                else {
                    broadcast_to_main_chat("Student " + to_string(client_id) + ": " + message, client_socket);          // broadcast to main chat
                }
            }
        }
    }
}


// MAIN FUNCTION TO START THE SERVER
int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);   // initialize Winstock


    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0); // create server socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;           // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;   // accecpt connections from any address
    server_addr.sin_port = htons(PORT);         // set the port


    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));   // bind the socket
    listen(server_socket, SOMAXCONN);   // start listeneing for connections


    cout << "Server is running on port " << PORT << endl;       // notify that the server is running


    while (true) {      // continuously accept clients
        SOCKET client_socket = accept(server_socket, NULL, NULL);       // accept a client connection
        client_counter++;                                               // increment the client counter
        thread(handle_client, client_socket, client_counter).detach();  // handle the client in a separate thread
    }
    WSACleanup();   // clean up Winstock resources
    return 0;
}




