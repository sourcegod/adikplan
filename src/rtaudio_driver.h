#ifndef RTAUDIO_DRIVER_H
#define RTAUDIO_DRIVER_H

// #include <RtAudio.h> // Incluez la bibliothèque RtAudio
#include <rtaudio/RtAudio.h> // Incluez la bibliothèque RtAudio
#include <iostream>   // Pour les messages de console
#include <vector>     // Pour gérer les buffers
#include <stdexcept>  // Pour la gestion des erreurs

// Forward declaration pour la fonction de callback globale.
// Cette fonction sera implémentée dans un .cpp séparé (e.g., audioengine.cpp)
// et sera passée à RtAudio.
extern void processAudioCallback(float* outputBuffer, int numSamples, void* userData);

/*
 * @brief Une classe utilitaire pour initialiser et gérer RtAudio.
 *
 * Cette classe simplifie l'interface avec RtAudio pour démarrer
 * un flux audio et lui associer une fonction de rappel.
 */
class RtAudioDriver {
public:
    RtAudioDriver() : audio(RtAudio::UNSPECIFIED), isStreamOpen(false) {}

    ~RtAudioDriver() {
        closeStream(); // Assure la fermeture propre du flux à la destruction de l'objet
    }
    bool startStream(unsigned int sampleRate, unsigned int bufferSize, void* userData); // Déclaration
    void stopStream();
    void closeStream(); // Déclaration


private:
    RtAudio audio;
    bool isStreamOpen;
};

#endif // RTAUDIO_DRIVER_H
