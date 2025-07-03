#ifndef AUDIOINFO_H
#define AUDIOINFO_H

#include <cstddef> // Pour unsigned int
#include <iostream> // Pour d'éventuels messages de débogage

/**
 * @brief Structure pour encapsuler les informations audio communes.
 *
 * Contient les paramètres fondamentaux du flux audio utilisés par
 * plusieurs composants du moteur (Player, Mixer, AudioEngine, etc.).
 */
struct AudioInfo {
    unsigned int sampleRate;  // Fréquence d'échantillonnage (samples par seconde), par exemple 44100 Hz
    unsigned int numChannels; // Nombre de canaux audio (par exemple, 2 pour stéréo)
    unsigned int bitDepth;    // Profondeur de bits (bits par échantillon), par exemple 16 pour int16, 32 pour float
    unsigned int bufferSize;  // Taille du buffer (nombre d'échantillons par bloc de traitement), par exemple 512

    // Constructeur pour définir les valeurs par défaut
    AudioInfo(unsigned int sr = 44100, unsigned int nc = 2, unsigned int bd = 32, unsigned int bs = 512)
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
