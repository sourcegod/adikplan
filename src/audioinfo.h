#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <cstddef> // Pour size_t
#include <iostream> // Pour d'éventuels messages de débogage

/**
 * @brief Structure pour encapsuler les informations audio communes.
 *
 * Contient les paramètres fondamentaux du flux audio utilisés par
 * plusieurs composants du moteur (Player, Mixer, AudioEngine, etc.).
 */
struct AudioInfo {
    size_t sampleRate;  // Fréquence d'échantillonnage (samples par seconde), par exemple 44100 Hz
    size_t numChannels; // Nombre de canaux audio (par exemple, 2 pour stéréo)
    size_t bitDepth;    // Profondeur de bits (bits par échantillon), par exemple 16 pour int16, 32 pour float
    size_t bufferSize;  // Taille du buffer (nombre d'échantillons par bloc de traitement), par exemple 512

    // Constructeur pour définir les valeurs par défaut
    AudioInfo(size_t sr = 44100, size_t nc = 2, size_t bd = 32, size_t bs = 512)
        : sampleRate(sr), numChannels(nc), bitDepth(bd), bufferSize(bs) {
        // Optionnel : afficher un message de débogage lors de la création
        // std::cout << "AudioInfo créé: SR=" << sampleRate << ", Ch=" << numChannels
        //           << ", BD=" << bitDepth << ", BS=" << bufferSize << std::endl;
    }

    // Méthode pour afficher les informations (utile pour le débogage)
    void display() const {
        std::cout << "AudioInfo: SampleRate=" << sampleRate
                  << ", Channels=" << numChannels
                  << ", BitDepth=" << bitDepth
                  << ", BufferSize=" << bufferSize << std::endl;
    }
};

#endif // AUDIOINFO_H
