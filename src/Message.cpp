#include "../include/JournalLib.h"
#include <string>
#include <sstream> 
Importance Message::GetImportance() {
	return importance;
}
void Message::Save(std::fstream& file) {
	if (file.is_open()) {
		file << timestamp.tm_hour << timestamp.tm_min << static_cast<int>(importance) << text << std::endl;
	}
	else {
		throw std::runtime_error("���� �� ������ ��� ������");
	}
}

void Message::ReadFromFile(std::fstream& file) {

	int i;
	if (!file.is_open()) {
		throw std::runtime_error("���� �� ������ ��� ������");
	}

	std::string line;
	if (!std::getline(file, line)) {
		throw std::runtime_error("�� ������� ��������� ������ �� �����");
	}

	std::istringstream iss(line);

	
	if (!(iss >> timestamp.tm_hour >> timestamp.tm_min)) {
		throw std::runtime_error("������ ������� ������ � ������");
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
		throw std::runtime_error("����������� ������� ��������");
		break;
	}

	// ������ ����������� ������
	std::getline(iss >> std::ws, text);  

}