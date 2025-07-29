#include "../include/JournalLib.h"
#include <string>
#include <sstream> 
#include <locale>
#include <codecvt>
Importance Message::GetImportance() {
	return importance;
}
void Message::Save(std::fstream& file) {
	if (file.is_open()) {
		file.seekg(0, std::ios::beg);
		file << timestamp.tm_hour << ' ' << timestamp.tm_min << ' ' << static_cast<int>(importance) << ' ' << text << std::endl;
	}
	else {
		throw std::runtime_error("Файл не открыт для записи");
	}
}

void Message::ReadFromFile(std::fstream& file) {

	int i;
	if (!file.is_open()) {
		throw std::runtime_error("Файл не открыт для чтения");
	}

	std::string line;
	if (!std::getline(file, line)) {
		throw std::runtime_error("Не удалось прочитать строку из файла");
	}

	std::istringstream iss(line);

	
	if (!(iss >> timestamp.tm_hour >> timestamp.tm_min)) {
		throw std::runtime_error("Ошибка формата данных в строке");
	}
	iss >> i;
	switch (i)
	{
	case 0:
		importance = Low;
		break;
	case 1:
		importance = Medium;
		break;
	case 2:
		importance = High;
		break;
	default:
		throw std::runtime_error("Неизвестный уровень важности");
		break;
	}

	// Чтение оставшегося текста
	std::getline(iss >> std::ws, text);  

}
void Message::Print() {

	std::wstring level = [this]() {
		switch (importance) {
		case Low: return L"Low";
		case Medium: return L"Medium";
		case High: return L"High";
		default: return L"Unknown";
		}
	}();
	
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wtext = converter.from_bytes(text);
	std::wcout << timestamp.tm_hour << ':' << timestamp.tm_min << ' ' << level << ' ' << wtext << std::endl;
}