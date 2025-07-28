#include <dlfcn.h>
#include <iostream>
#include "../include/JournalLib.h"
#include <thread>
using CreateMsgFn = Message* (*)(tm, const char*, Importance);
using DestroyMsgFn = void (*)(Message*);
using CreateJrlFn = Message * (*)(Importance current, const char* filename);
using DestroyJrlFn = void (*)(Journal*);
int main() {


 

	return 0;
}
void sideThread() {
    void* lib = dlopen("./lib.so", RTLD_LAZY);
    if (!lib) {
        std::cout << "Не удалось загрузить библеотеку" << std::endl;
        return 1;
    }
    CreateMsgFn create_msg = (CreateMsgFn)dlsym(lib, "FabricMessage");
    DestroyMsgFn destroy_msg = (DestroyMsgFn)dlsym(lib, "DestroyMessage");
    CreateJrlFn create_journal = (CreateMsgFn)dlsym(lib, "FabricJournal");
    DestroyJrlFn destroy_journal = (DestroyMsgFn)dlsym(lib, "DestroyJournal");


    dlclose(handle);
}