#pragma once
#include <vector>
#include <algorithm>
#include "User.cpp"
#include "Game.cpp"

using namespace std;

class Room {
public:

	unsigned int id;
	char* name;
	unsigned int maxUsers;
	char* connectionRoute;
	vector<User*> users;
	Game* game; //TODO FOR SOME REASON, IF GAME IS NOT A POINTER, THE ROOM CLASS BECOMES NOT COPIABLE OR NOT MOVABLE (IDK WHICH ONE)

	Room(unsigned int id, const char* name, unsigned int maxUsers, const char* connectionRoute)
		: id(id), name((char*)name), maxUsers(maxUsers), connectionRoute((char*)connectionRoute){

		this->users.reserve(maxUsers);
		this->game = new Game(&users);
	}

	void addUser(User& user) {
		user.currentRoom = this;
		user.userConnection.waitConnection();
		this->users.push_back(&user);
	}

	void removeUser(User& user) {
		user.userConnection.disconnect();
		this->users.erase(std::remove(this->users.begin(), this->users.end(), &user), this->users.end()); //SHESH WTF IS THIS (JUST TO REMOVE?)
		user.currentRoom = nullptr;
	}

	bool isFull() {
		return this->users.size() >= this->maxUsers;
	}

	void sendMessageToAllUsers(const std::string& text) {
		for (int i = 0; i < this->users.size(); i++) {
			auto& userConnection = this->users.at(i)->userConnection;

			if (userConnection.status == ConnectionStatus::Connected &&
				userConnection.connection != nullptr) {

				userConnection.connection->send_text(text);
			}
		}
	}

	void notifyUsersOfNewPlayer(unsigned int userId) {
		this->sendMessageToAllUsers((std::string)"P" + to_string(userId));
	}

	vector<User*>& usersList() {
		return this->users;
	}
};