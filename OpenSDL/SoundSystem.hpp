#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xaudio2.h>
#include <vector>
#include <string>
#include <unordered_map>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "ole32.lib")

struct SoundData {
    std::vector<unsigned char> buffer;
    WAVEFORMATEX format;
    std::string name;
};

class SoundSystem {
public:
    SoundSystem();
    ~SoundSystem();

    bool init();
    void shutdown();
    bool loadSound(const std::string& name, const std::string& filePath);
    int play(const std::string& name, bool loop = false, float volume = 1.0f);
    void stopAll();
    bool isPlaying(int soundId);
    void update();
    std::string setPath(const std::string& relativePath);
    void pause(int soundId);
    void resume(int soundId);
    void pause(const std::string& name);
    void resume(const std::string& name);

private:
    bool loadWavFile(const std::string& filePath, SoundData& outData);

    struct PlayingVoice {
        IXAudio2SourceVoice* voice;
        SoundData* data;
        bool active;
        std::string name;
    };

    IXAudio2* xaudio2;
    IXAudio2MasteringVoice* masteringVoice;
    std::unordered_map<std::string, SoundData> loadedSounds;
    std::unordered_map<int, PlayingVoice> activeVoices;
    int nextId;
    bool initialized;
};