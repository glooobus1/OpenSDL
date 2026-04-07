#include "SoundSystem.hpp"
#include <fstream>

SoundSystem::SoundSystem()
    : xaudio2(nullptr)
    , masteringVoice(nullptr)
    , nextId(1)
    , initialized(false) {
}

SoundSystem::~SoundSystem() {
    shutdown();
}

bool SoundSystem::init() {
    if (initialized) return true;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return false;
    }

    hr = XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    hr = xaudio2->CreateMasteringVoice(&masteringVoice);
    if (FAILED(hr)) {
        xaudio2->Release();
        xaudio2 = nullptr;
        CoUninitialize();
        return false;
    }

    initialized = true;
    return true;
}

void SoundSystem::shutdown() {
    stopAll();

    if (masteringVoice) {
        masteringVoice->DestroyVoice();
        masteringVoice = nullptr;
    }

    if (xaudio2) {
        xaudio2->Release();
        xaudio2 = nullptr;
    }

    CoUninitialize();
    initialized = false;
}

bool SoundSystem::loadWavFile(const std::string& filePath, SoundData& outData) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) return false;

    char chunkId[4];
    file.read(chunkId, 4);
    if (std::string(chunkId, 4) != "RIFF") return false;

    unsigned int chunkSize;
    file.read(reinterpret_cast<char*>(&chunkSize), 4);

    file.read(chunkId, 4);
    if (std::string(chunkId, 4) != "WAVE") return false;

    bool fmtFound = false;
    bool dataFound = false;

    ZeroMemory(&outData.format, sizeof(WAVEFORMATEX));

    while (!fmtFound || !dataFound) {
        file.read(chunkId, 4);
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (file.eof()) break;

        if (std::string(chunkId, 4) == "fmt ") {
            fmtFound = true;
            file.read(reinterpret_cast<char*>(&outData.format), chunkSize);
        }
        else if (std::string(chunkId, 4) == "data") {
            dataFound = true;
            outData.buffer.resize(chunkSize);
            file.read(reinterpret_cast<char*>(outData.buffer.data()), chunkSize);
        }
        else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    return fmtFound && dataFound;
}

bool SoundSystem::loadSound(const std::string& name, const std::string& filePath) {
    if (!initialized) return false;

    SoundData newSound;
    newSound.name = name;
    if (loadWavFile(filePath, newSound)) {
        loadedSounds[name] = newSound;
        return true;
    }

    return false;
}

int SoundSystem::play(const std::string& name, bool loop, float volume) {
    if (!initialized) return -1;

    auto it = loadedSounds.find(name);
    if (it == loadedSounds.end()) return -1;

    IXAudio2SourceVoice* sourceVoice = nullptr;
    HRESULT hr = xaudio2->CreateSourceVoice(&sourceVoice, &it->second.format);
    if (FAILED(hr)) return -1;

    XAUDIO2_BUFFER buffer = { 0 };
    buffer.AudioBytes = (unsigned int)it->second.buffer.size();
    buffer.pAudioData = it->second.buffer.data();
    buffer.LoopCount = loop ? 255 : 0;

    hr = sourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        sourceVoice->DestroyVoice();
        return -1;
    }

    sourceVoice->SetVolume(volume);
    hr = sourceVoice->Start(0);
    if (FAILED(hr)) {
        sourceVoice->DestroyVoice();
        return -1;
    }

    int id = nextId++;
    PlayingVoice pv;
    pv.voice = sourceVoice;
    pv.data = &it->second;
    pv.active = true;
    pv.name = name;  

    activeVoices[id] = pv;

    return id;
}
void SoundSystem::stopAll() {
    for (auto& pair : activeVoices) {
        if (pair.second.voice) {
            pair.second.voice->Stop(0);
            pair.second.voice->DestroyVoice();
        }
    }
    activeVoices.clear();
}

bool SoundSystem::isPlaying(int soundId) {
    auto it = activeVoices.find(soundId);
    if (it != activeVoices.end() && it->second.voice && it->second.active) {
        XAUDIO2_VOICE_STATE state;
        it->second.voice->GetState(&state);
        return state.BuffersQueued > 0;
    }
    return false;
}

void SoundSystem::update() {
    std::vector<int> toRemove;

    for (auto& pair : activeVoices) {
        if (pair.second.voice) {
            XAUDIO2_VOICE_STATE state;
            pair.second.voice->GetState(&state);
            if (state.BuffersQueued == 0) {
                pair.second.voice->DestroyVoice();
                toRemove.push_back(pair.first);
            }
        }
    }

    for (int id : toRemove) {
        activeVoices.erase(id);
    }
}
std::string SoundSystem::setPath(const std::string& relativePath) {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    std::string path = buffer;
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos) {
        path = path.substr(0, pos);
    }

    path += "\\" + relativePath;

    for (char& c : path) {
        if (c == '/') c = '\\';
    }

    return path;
}
void SoundSystem::pause(int soundId) {
    auto it = activeVoices.find(soundId);
    if (it != activeVoices.end() && it->second.voice) {
        it->second.voice->Stop(0);
        it->second.active = false;
    }
}

void SoundSystem::resume(int soundId) {
    auto it = activeVoices.find(soundId);
    if (it != activeVoices.end() && it->second.voice && !it->second.active) {
        it->second.voice->Start(0);
        it->second.active = true;
    }
}

void SoundSystem::pause(const std::string& name) {
    for (auto& pair : activeVoices) {
        if (pair.second.data && pair.second.data->name == name) {
            pause(pair.first);
        }
    }
}

void SoundSystem::resume(const std::string& name) {
    for (auto& pair : activeVoices) {
        if (pair.second.data && pair.second.data->name == name) {
            resume(pair.first);
        }
    }
}