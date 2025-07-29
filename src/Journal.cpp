#include "../include/JournalLib.h"

Journal::Journal(Importance current, std::string filename) {
	minLevel = current;
	file.open(filename, std::ios::in | std::ios::out | std::ios::app);
	if (!file.is_open()) {
		throw std::runtime_error("Не удалось открыть файл: " + filename);
	}
}
Journal::~Journal() {
	if (!file.is_open()) {
		file.close();
	}
}
void Journal::ChangeDefaultImportance(Importance newdefault) {
	minLevel = newdefault;
}
void Journal::SaveMessage(Message* msg) {
	if (msg->GetImportance() >= minLevel) {
		msg->Save(file);
	}
	delete msg;
}
std::vector<Message*> Journal::ReadJournal() {
	file.seekg(0, std::ios::beg);
	std::vector<Message*> res;
	while (file.peek() != EOF) {
		Message* msg = new Message();
		msg->ReadFromFile(file);
		res.push_back(msg);
	}
	
		
	
	return res;
}
extern "C" {
	Message* FabricMessage(tm timestamp, const char* text, Importance importance) {
		return new Message(timestamp, std::string(text), importance);
	}

	Journal* FabricJournal(Importance current, const char* filename) {
		return new Journal(current, std::string(filename));
	}

	void DestroyMessage(Message* msg) {
		delete msg;
	}

	void DestroyJournal(Journal* journal) {
		delete journal;
	}
	void Journal_SaveMessage(Journal* j, Message* m) {
		j->SaveMessage(m);
	}
	std::vector<Message*>* Journal_ReadJournal(Journal* j) {
		return new std::vector<Message*>(j->ReadJournal());
	}
	void DestroyMessageVector(std::vector<Message*>* vec) {
		delete vec;
	}
	void Journal_ChangeDefaultImportance(Journal* journal, Importance newdefault) {
		journal->ChangeDefaultImportance(newdefault);
	}
	void Message_Print(Message* msg) {
			msg->Print();
	}
}