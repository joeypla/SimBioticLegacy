#include "SLOG.h"



static std::ofstream symbioteLog;
void S_LOG::clear(void){

}

void S_LOG::start(void){
	symbioteLog.open("SymbioteLOG.txt", std::ios::trunc);
	symbioteLog<<"-Project Symbiote LOG-"<<std::endl;
}

void S_LOG::finish(void){
	symbioteLog.close();
}

void S_LOG::write(const char* text){
	symbioteLog<<text<<std::endl;
}