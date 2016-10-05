#ifndef S_AUDIO_H
#define S_AUDIO_H

#include <fmod.hpp>
#include <conio.h>

#pragma comment(lib, "fmodex_vc.lib")
#include <string>
#include "psMath.h"
#define SAUDIO_MAXSOUNDS 100

namespace psAudio
{
	typedef FMOD::Sound Sound;
};
class S_AUDIO{
public:
	static bool createSingleton(void);
	static void destroySingleton(void);
	static S_AUDIO* getSingleton(void);

	bool init(void);
	void destroy(void);
	bool update(void);
	S_AUDIO(void);
	~S_AUDIO(void);
	void playSoundOnce(psAudio::Sound* sound);
	void playSoundOnceAtPosition(psAudio::Sound* sound, psMath::float3 position);
	void playSound(psAudio::Sound* sound);
	psAudio::Sound* createSound(std::string path);
	psAudio::Sound* create3DSound(std::string path);

	psAudio::Sound* createStream(std::string path);

	void releaseSound(psAudio::Sound* sound);
	void playGunshot(void);
	void playClick0(void);
private:
	static S_AUDIO* singleton;
	FMOD::System* system;
	FMOD::Sound* music;
	FMOD::Sound* test;
	psAudio::Sound** sounds;

	FMOD::Sound* _click0Sound;
};
#endif
