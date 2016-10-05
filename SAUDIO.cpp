#include "SAUDIO.h"

S_AUDIO* S_AUDIO::singleton;
bool S_AUDIO::createSingleton(void){
	singleton = new S_AUDIO();
	if (!singleton) return false;
	return true;
}

void S_AUDIO::destroySingleton(void){
	delete singleton;
}

S_AUDIO* S_AUDIO::getSingleton(void){
	return singleton;
}

S_AUDIO::S_AUDIO(void){}
S_AUDIO::~S_AUDIO(void){}

bool S_AUDIO::init(void){
	FMOD::System_Create(&system);
	if (!system) return false;
	system->init(1024, FMOD_INIT_NORMAL, 0);
	//system->createSound("music.wav", FMOD_HARDWARE, 0, &music);
	system->createSound("../Resources/Audio/Sounds/gunshot.wav", FMOD_HARDWARE, 0, &test);
	system->createSound("../Resources/Audio/Sounds/click0.wav", FMOD_HARDWARE, 0, &_click0Sound);

	//music->setMode(FMOD_LOOP_NORMAL);
	test->setMode(FMOD_LOOP_OFF);
	//system->playSound(FMOD_CHANNEL_FREE, music, false, 0);

	system->set3DSettings(1.0f, 1.0f, 1.0f);
	
	return true;
}

void S_AUDIO::destroy(void){
	music->release();
	system->close();
	system->release();
}

#include "SRender.h"
bool S_AUDIO::update(void){
	//Now this is some strong coupling, but we always want to hear sound based on where the camera is
	SCamera* cam = SRender::getSingleton()->getCurrentCamera();
	FMOD_VECTOR listenerVelocity;
	listenerVelocity.x = 0.0f;
	listenerVelocity.y = 0.0f;
	listenerVelocity.z = 0.0f;

	FMOD_VECTOR listenerPosition;
	listenerPosition.x = cam->x;
	listenerPosition.y = cam->y;
	listenerPosition.z = cam->z;

	FMOD_VECTOR listenerForward;
	listenerForward.x = cam->dx;
	listenerForward.y = cam->dy;
	listenerForward.z = cam->dz;

	FMOD_VECTOR listenerUp;
	listenerUp.x = cam->ux;
	listenerUp.y = cam->uy;
	listenerUp.z = cam->uz;

	system->set3DListenerAttributes(0, &listenerPosition, &listenerVelocity, &listenerForward, &listenerUp);
	system->update();
	return true;
}

void S_AUDIO::playGunshot(void){
	system->playSound(FMOD_CHANNEL_FREE, test, false, 0);
}

void S_AUDIO::playClick0(void)
{
	static float randomPitch;
	randomPitch = rand() % 100 - 50;
	FMOD::Channel* channel;
	system->playSound(FMOD_CHANNEL_FREE, _click0Sound, true, &channel);
	channel->setVolume(1.0f);
	channel->setLowPassGain(randomPitch);
	channel->setPaused(false);
}
void S_AUDIO::playSoundOnce(psAudio::Sound* sound)
{
	
	sound->setMode(FMOD_LOOP_OFF);
	system->playSound(FMOD_CHANNEL_FREE, sound, false, 0);
	system->update();
}

void S_AUDIO::playSoundOnceAtPosition(psAudio::Sound* sound, psMath::float3 position)
{
	FMOD::Channel* channel;
	sound->set3DMinMaxDistance(5.0f, 50.0f);
	system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
	FMOD_VECTOR pos;
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;

	FMOD_VECTOR velocity;
	velocity.x = 0.0f;
	velocity.y = 0.0f;
	velocity.z = 0.0f;

	channel->set3DAttributes(&pos, &velocity);
	channel->setVolume(1.0f);
	channel->setPaused(false);
}
void S_AUDIO::playSound(psAudio::Sound* sound)
{
	
	system->playSound(FMOD_CHANNEL_FREE, sound, false, 0);
	system->update();
}
psAudio::Sound* S_AUDIO::createSound(std::string path)
{
	psAudio::Sound* sound;
	system->createSound(path.c_str(), FMOD_HARDWARE, 0, &sound);
	return sound;
}

psAudio::Sound* S_AUDIO::create3DSound(std::string path)
{
	psAudio::Sound* sound;
	system->createSound(path.c_str(), FMOD_3D, 0, &sound);
	return sound;
}
psAudio::Sound* S_AUDIO::createStream(std::string path)
{
	FMOD::Sound *audioStream;
	system->createStream(path.c_str(), FMOD_DEFAULT, 0, &audioStream);
	return audioStream;
}
void S_AUDIO::releaseSound(psAudio::Sound* sound)
{
}

