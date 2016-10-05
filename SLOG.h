#ifndef S_LOG_H
#define S_LOG_H

#include <string>
#include <fstream>
class S_LOG{
public:
	static void clear(void);
	static void start(void);
	static void finish(void);
	static void write(const char*);
private:
	//static std::ofstream symbioteLog;
	S_LOG(void);
	~S_LOG(void);
};
#endif