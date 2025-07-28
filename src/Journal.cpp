#include "../include/JournalLib.h"

Journal::Journal(Importance current, std::string filename) {
	minLevel = current;
	file.open(filename, std::ios::in | std::ios::out | std::ios::app);
	if (!file.is_open()) {
		throw std::runtime_error("Не удалось открыть файл: " + filename);
	}
}
void Journal::ChangeDefaultImportance(Importance newdefault) {
	minLevel = newdefault;
}
void Journal::SaveMessage(Message msg) {
	if (msg.GetImportance() >= minLevel) {
		msg.SaveMessage(file);
	}
}
std::vector<Message> Journal::ReadJournal() {
	std::vector<Message> res;
	while (!file.eof()) {
		Message msg;
		msg.ReadMessage(file);
		res.push_back(msg);
	}
	
		
	
	return res;
}