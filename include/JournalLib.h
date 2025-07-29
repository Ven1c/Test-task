#ifndef JOURNAL_H
#define JOURNAL_H
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <locale>
enum Importance { Low, Medium, High }; 
class __attribute__((visibility("default"))) Message {
public:
	Message(tm timestamp, const std::string& text, Importance importance):timestamp(timestamp),text(text),importance(importance) { }
	Message(){}
	void Save(std::fstream& file);
	void ReadFromFile(std::fstream& file);
	Importance GetImportance();
	void Print();
	virtual ~Message() = default;
private:
	tm timestamp;
	std::string text;
	Importance importance;
};
class __attribute__((visibility("default"))) Journal {
public:
	Journal(Importance current, std::string filename);
	void ChangeDefaultImportance(Importance newdefault);
	void SaveMessage(Message* msg);
	std::vector<Message*> ReadJournal();
	~Journal();
private:
	Importance minLevel;
	std::string filename;
	std::fstream file;
};
extern "C" {
	
	__attribute__((visibility("default")))
		Message* FabricMessage(tm timestamp, const char* text, Importance importance);
	
	__attribute__((visibility("default")))
		Journal* FabricJournal(Importance current, const char* filename);
	
	__attribute__((visibility("default")))
		void DestroyMessage(Message* msg);

	__attribute__((visibility("default")))
		void DestroyJournal(Journal* journal);

	__attribute__((visibility("default")))
		void Journal_SaveMessage(Journal*, Message*);

	__attribute__((visibility("default")))
		std::vector<Message*>* Journal_ReadJournal(Journal* j);

	__attribute__((visibility("default")))
		void DestroyMessageVector(std::vector<Message*>* vec);

	__attribute__((visibility("default")))
		void Journal_ChangeDefaultImportance(Journal* journal, Importance newdefault);

	__attribute__((visibility("default")))
		void Message_Print(Message* msg);
}
#endif