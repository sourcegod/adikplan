#ifndef ADIKMIXER_H
#define ADIKMIXER_H

#include "adikchannel.h"
// Important : AdikChannel.h DOIT être inclus avant AdikMixer.h
// car AdikMixer contient un std::vector<AdikChannel>,
// ce qui signifie qu'il a besoin de la définition complète de AdikChannel.
// De même, AdikChannel a besoin de AdikInstrument, donc AdikInstrument.h doit être inclus avant AdikChannel.h

#include <vector>
#include <string> // Pour std::string dans displayMixerStatus
#include <iostream>
#include <memory> // Pour std::shared_ptr

class AdikMixer {
public:
    std::vector<AdikChannel> channels;
    static const int NUM_MIXER_CHANNELS = 8;
    float masterVolume;

    // Constructeur
    AdikMixer() : masterVolume(0.7f) {
        channels.reserve(NUM_MIXER_CHANNELS);
        for (int i = 0; i < NUM_MIXER_CHANNELS; ++i) {
            channels.emplace_back(i + 1); // Crée un canal avec un ID de 1 à NUM_MIXER_CHANNELS
        }
        std::cout << "Mixer initialisé avec " << NUM_MIXER_CHANNELS << " canaux." << std::endl;
    }

    // Acheminer le son vers un canal spécifique du mixeur
    void routeSound(int channelIndex, std::shared_ptr<AdikInstrument> instrument, float finalVelocity, float finalPan, float finalPitch) {
        if (channelIndex > 0 && channelIndex <= NUM_MIXER_CHANNELS) {
            channels[channelIndex - 1].receiveSound(instrument, finalVelocity, finalPan, finalPitch);
        } else {
            std::cerr << "Erreur: Canal mixeur invalide: " << channelIndex << std::endl;
        }
    }

    // Afficher l'état actuel du mixeur (quels canaux sont actifs)
    void displayMixerStatus() const {
        std::cout << "Mixer Status (Master Vol: " << masterVolume << "): [";
        for (const auto& channel : channels) {
            std::cout << (channel.isActive ? "X" : ".");
        }
        std::cout << "]" << std::endl;
    }

    // Réinitialiser l'état de lecture de tous les canaux
    void clearAllChannelsPlaybackState() {
        for (auto& channel : channels) {
            channel.clear();
        }
        std::cout << "État de lecture de tous les canaux du mixeur réinitialisé." << std::endl;
    }

    // Mélanger le son de tous les canaux actifs dans le buffer de sortie
    void mixChannels(float* outputBuffer, int numSamples) {
        // Initialiser le buffer de sortie à zéro
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f);

        for (auto& channel : channels) {
            if (channel.isActive) {
                // Rendre le son de ce canal et l'ajouter au buffer de sortie
                channel.renderToBuffer(outputBuffer, numSamples);
            }
        }

        // Appliquer le volume master
        for (int i = 0; i < numSamples; ++i) {
            outputBuffer[i] *= masterVolume;
        }
    }
};

#endif // ADIKMIXER_H
