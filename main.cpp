#include <dlfcn.h>
#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <time.h>
#include <locale>
#include "include/JournalLib.h"

using CreateMsgFn = Message* (*)(tm, const char*, Importance);
using DestroyMsgFn = void (*)(Message*);
using CreateJrlFn = Journal * (*)(Importance current, const char* filename);
using DestroyJrlFn = void (*)(Journal*);
using SaveMessageFn = void (*)(Journal*, Message*);
using ReadJournalFn = std::vector<Message*>* (*)(Journal*);
using DestroyVecFn = void (*)(std::vector<Message*>*);
using ChangeImportanceFn = void (*)(Journal*, Importance);
using PrintMessageFn = void (*)(Message*);

DestroyJrlFn destroy_journal;
CreateJrlFn create_journal;
DestroyVecFn destroy_msg_vec;
ReadJournalFn journal_read;
SaveMessageFn journal_save_message;
ChangeImportanceFn change_importance;
void sideThread();

std::queue<Message*> ToJournal;
std::queue<Message*> ToConsole;
std::mutex gateInit, gateW, gateR;
std::atomic<bool> endWork{false};
std::atomic<bool> flagRW{false};
std::atomic<bool> ReadComplete{false};
std::string filename; 
Importance current;
std::condition_variable cvW;
std::condition_variable cvR;
int main() {
    int i;
    std::string text;
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wcout.imbue(std::locale("ru_RU.UTF-8"));
    Message* msg;
    time_t t = time(NULL);
    void* lib = dlopen("./lib.so", RTLD_LAZY);
    if (!lib) {
        std::wcout << L"Не удалось загрузить библеотеку" << std::endl;
        return 1;
    }
    bool consoleCycle = true;
    CreateMsgFn create_msg = (CreateMsgFn)dlsym(lib, "FabricMessage");
    DestroyMsgFn destroy_msg = (DestroyMsgFn)dlsym(lib, "DestroyMessage");
    PrintMessageFn print_msg = (PrintMessageFn)dlsym(lib, "Message_Print");
    change_importance = (ChangeImportanceFn)dlsym(lib, "Journal_ChangeDefaultImportance");
    create_journal = (CreateJrlFn)dlsym(lib, "FabricJournal");
    destroy_journal = (DestroyJrlFn)dlsym(lib, "DestroyJournal");
    journal_save_message = (SaveMessageFn)dlsym(lib, "Journal_SaveMessage");
    journal_read = (ReadJournalFn)dlsym(lib, "Journal_ReadJournal");
    destroy_msg_vec = (DestroyVecFn)dlsym(lib, "DestroyMessageVector");

    std::wcout << L"Введите базовую важность сообщения 0 - Low, 1 - Medium, 2 - High" << std::endl;
    std::cin >> i;
    std::wcout << L"Введите имя файла журнала" << std::endl;
    {
        std::unique_lock<std::mutex> locker(gateInit);
        current = static_cast<Importance>(i);
        std::cin >> filename;
    }

    std::thread journalThread(sideThread);
   
    while (consoleCycle) {
        std::wcout << L"Выберете действие\n" << L"0 - Выход из программы\n" << L"1 - Запись нового сообщения\n" << L"2 - Чтение журнала\n" << L"3 - Изменение базового уровня важности\n";

        std::cin >> i;
        switch (i) {
        case 0:
            endWork.store(true, std::memory_order_release);
            cvW.notify_one();
            consoleCycle = false;
            
            break;
        case 1:
            flagRW.store(false, std::memory_order_release);
            std::wcout << L"Введите важность сообщения 0 - Low, 1 - Medium, 2 - High" << std::endl;
            std::cin >> i;
            std::wcout << L"Введите текст сообщения" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, text);
            msg = create_msg(*localtime(&t), text.c_str(), static_cast<Importance>(i));
            {
                std::unique_lock<std::mutex> locker(gateW);
                ToJournal.push(msg);
                cvW.notify_one();
            }
            break;
        case 2:
            flagRW.store(true, std::memory_order_release);
            cvW.notify_one();
            {
                //std::wcout << L"Ждём сообщений из журнала..." << std::endl;
                std::unique_lock<std::mutex> locker(gateR);
                cvR.wait(locker, [] { return !ToConsole.empty(); });
                //std::wcout << L"Получены сообщения из журнала:" << std::endl;
                while (!ToConsole.empty()) {
                    //std::wcout << L"Вызов функци вывода" << std::endl;
                    print_msg(ToConsole.front());
                    destroy_msg(ToConsole.front());
                    ToConsole.pop();
                }
            }
            break;
        case 3:
            std::wcout << L"Введите новый базовый уровень важности 0 - Low, 1 - Medium, 2 - High" << std::endl;
            std::cin >> i;
            {
                std::unique_lock<std::mutex> locker(gateInit);
                current = static_cast<Importance>(i);
            }
            cvW.notify_one();
            break;
        default:
            std::wcout << L"Нет такого действия" << std::endl;
            break;
        }
    }
    
    journalThread.join();
    dlclose(lib);
	return 0;
}

void sideThread() {
    Journal* journal;
    {
        
        std::unique_lock<std::mutex> locker(gateInit);
        journal = create_journal(current, filename.c_str());
      
        
    }

    while (!endWork.load(std::memory_order_acquire)) {
        //std::wcout << L"Начало цикла\n";
        if (flagRW.load(std::memory_order_acquire)) { //Чтение
            //std::wcout << L"Зашли на чтение\n";
            std::vector<Message*>* v = journal_read(journal);
            {
                std::unique_lock<std::mutex> locker(gateR);
                //std::wcout << L"Залочили чтение\n";
                for (const auto& e : *v) {
                    ToConsole.push(e);
                    //std::wcout << L"Толкнули сообщение в очередь\n";
                }
                destroy_msg_vec(v);
            }
            cvR.notify_one();
            //std::wcout << L"Уведомили на чтение\n";
            flagRW.store(false, std::memory_order_release);
        }
        else { //Запись
            std::unique_lock<std::mutex> locker(gateW);
                
                cvW.wait(locker, [] { return !ToJournal.empty() || endWork.load(std::memory_order_acquire) || flagRW.load(std::memory_order_acquire); });
                {
                   //std::wcout << L"Изменение уровня\n";
                    std::unique_lock<std::mutex> locker(gateInit);
                    change_importance(journal, current);
                    //std::wcout << L"успех\n";
                }
                while (!ToJournal.empty())
                {
                    journal_save_message(journal, ToJournal.front());
                    ToJournal.pop();

                }
        }

        
    }
    destroy_journal(journal);

}