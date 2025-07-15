#ifndef ADIKMIXER_H
#define ADIKMIXER_H

#include <vector>
#include <string> // Pour std::string dans displayMixerStatus
#include <iostream>
#include <memory> // Pour std::shared_ptr

// Important : AdikChannel.h DOIT être inclus avant AdikMixer.h
// car AdikMixer contient un std::vector<AdikChannel>,
// ce qui signifie qu'il a besoin de la définition complète de AdikChannel.
// De même, AdikChannel a besoin de AdikInstrument, donc AdikInstrument.h doit être inclus avant AdikChannel.h
#include "adikchannel.h"


class AdikMixer {
public:
    std::vector<AdikChannel> channelList;
    static const int NUM_MIXER_channelList = 8;
    unsigned int numOutputChannels; // Le nombre de canaux de sortie du mixeur (ex: 2 pour stéréo)
    float masterVolume; // Pour un contrôle de volume global
    std::vector<float> instruBuffer;  

    // Constructeur - maintenant prend le nombre de canaux de sortie de l'AudioEngine
    AdikMixer() : numOutputChannels(2) { // Par défaut, sortie stéréo
        // Initialiser 8 canaux par défaut
        for (int i = 0; i < 8; ++i) {
            channelList.emplace_back(i + 1);
        }
        std::cout << "AdikMixer: Constructeur appelé avec " << channelList.size() << " canaux." << std::endl;
    }

    // Acheminer le son vers un canal spécifique du mixeur
    void routeSound(int channelIndex, std::shared_ptr<AdikInstrument> instrument, float finalVelocity, float finalPan, float finalPitch) {
        if (channelIndex > 0 && channelIndex <= NUM_MIXER_channelList) {
            channelList[channelIndex - 1].receiveSound(instrument, finalVelocity, finalPan, finalPitch);
            std::cout << "routeSound: Après receiveSound\n";
        } else {
            std::cerr << "Erreur: Canal mixeur invalide: " << channelIndex << std::endl;
        }
    }

    // Afficher l'état actuel du mixeur (quels canaux sont actifs)
    void displayMixerStatus() const {
        std::cout << "Mixer Status (Master Vol: " << masterVolume << "): [";
        for (const auto& channel : channelList) {
            std::cout << (channel.isActive ? "X" : ".");
        }
        std::cout << "]" << std::endl;
    }

    // Réinitialiser l'état de lecture de tous les canaux
    void clearAllchannelListPlaybackState() {
        for (auto& channel : channelList) {
            channel.clear();
        }
        std::cout << "État de lecture de tous les canaux du mixeur réinitialisé." << std::endl;
    }
    
    // Méthode pour mixer tous les canaux actifs dans un buffer de sortie stéréo final.
    // Le outputBuffer est un buffer entrelacé (LRLR...) de la carte son.
    void mixChannels(std::vector<float>& outputBuffer, unsigned int numFrames) {
        // std::cout << "\a";  
        // std::cout << "MixChannelList: avant tout.\n";
        // Initialiser le buffer de sortie avec des zéros
        // La taille est numFrames * numOutputChannels (ex: 512 frames * 2 canaux = 1024 floats)
        outputBuffer.assign(numFrames * numOutputChannels, 0.0f);
        // std::cout << "MixchannelList: après l'initialisation du monoBuffer.\n";

        // Parcourir chaque canal du mixeur
        for (auto i =0; i < channelList.size(); i++) {
            auto& channel = channelList[i];  
            // std::cout << "mixChannels: boucle sur les channelList: " << i << ", active: " << channel.isActive << "\n";
            if (channel.isActive && channel.currentInstrument) {
                unsigned int numInstruChannels = channel.currentInstrument->getNumChannels();
                // instruBuffer.assign(numFrames * numInstruChannels, 0.0f);  
                // Demander au canal de rendre son son dans son buffer interne
                // std::cout << "mixChannels: Avant channel.render:\n ";
                // instruBuffer sera réinitialisé par la fonction readData
                channel.render(instruBuffer, numFrames);
                // std::cout << "mixchannelList: Après channel.render:\n ";


                // Appliquer le panoramique et mixer dans le buffer de sortie final (stéréo)
                for (unsigned int j =0; j < numFrames; ++j) {
                    float leftSample = 0.0f;
                    float rightSample = 0.0f;

                    if (numInstruChannels == 1) { // Son mono
                        float monoSample = instruBuffer[j];

                        // Application du panoramique (loi de puissance -3dB plus naturelle)
                        // Pan de -1 (gauche) à +1 (droite)
                        float panAngle = (channel.currentPan + 1.0f) * (PI / 4.0f); // De 0 à PI/2
                        float gainLeft = std::cos(panAngle);
                        float gainRight = std::sin(panAngle);
                        gainLeft = 0.3;
                        gainRight = 0.3;

                        leftSample = monoSample * gainLeft;
                        rightSample = monoSample * gainRight;

                    } else if (numInstruChannels == 2) { // Son stéréo
                        // Pour l'instant, on suppose que le pan n'affecte pas les sons stéréo
                        // (ce qui est une simplification courante).
                        // Un pan stéréo est plus complexe (rotation M/S ou L/R).
                        leftSample = instruBuffer[j * 2];
                        rightSample = instruBuffer[j * 2 + 1];

                        // Pour simuler un pan simple sur le stéréo, on pourrait appliquer la même loi de gain
                        // aux deux canaux, ou un cross-fade plus complexe.
                        // Pour l'instant, pas de pan sur stéréo.
                    }

                    // Ajouter les samples mixés au buffer de sortie principal (stéréo entrelacé)
                    outputBuffer[j * numOutputChannels] += leftSample;       // Canal gauche
                    outputBuffer[j * numOutputChannels + 1] += rightSample; // Canal droit
                } // End for j loop
            } // End if condition
        
        } // End for i loop
    
    }

    // Une méthode pour initialiser les paramètres si le mixeur en avait besoin.
    // Pour l'instant, numOutputChannels est défini dans le constructeur.
    void initParams(const AudioInfo& info) {
        this->numOutputChannels = info.numChannels;
        // Vous pouvez passer ces infos aux canaux si besoin
        // for (auto& ch : channelList) { ch.initParams(info); }
        std::cout << "AdikMixer: Initialisé avec " << numOutputChannels << " canaux de sortie." << std::endl;
    }


};

#endif // ADIKMIXER_H
