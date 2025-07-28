#ifndef JOURNAL_H
#define JOURNAL_H
#include <iostream>
#include <vector>
#include <fstream>

enum Importance { Low, Medium, High }; 
class __attribute__((visibility("default"))) Message {
public:
	Message(tm timestamp, std::string& text, Importance importance):timestamp(timestamp),text(text),importance(importance) { }
	Message(){}
	void SaveMessage(std::fstream& file);
	void ReadMessage(std::fstream& file);
	Importance GetImportance();

private:
	tm timestamp;
	std::string text;
	Importance importance;
};
class __attribute__((visibility("default"))) Journal {
public:
	Journal(Importance current, std::string filename);
	void ChangeDefaultImportance(Importance newdefault);
	void SaveMessage(Message msg);
	std::vector<Message> ReadJournal();
private:
	Importance minLevel;
	std::string filename;
	std::fstream file;
};

#endif