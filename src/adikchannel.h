#ifndef ADIKCHANNEL_H
#define ADIKCHANNEL_H

#include <memory> // Pour std::shared_ptr
#include <iostream>
#include <string> // Pour std::string dans displayStatus

// Important : AdikInstrument.h DOIT être inclus avant AdikChannel.h
// car AdikChannel contient un std::shared_ptr<AdikInstrument> et appelle des méthodes sur cet instrument.
#include "adikinstrument.h" // Assurez-vous que AdikInstrument.h contient la définition complète de AdikInstrument

class AdikChannel {
public:
    int id;
    std::shared_ptr<AdikInstrument> currentInstrument; // L'instrument actuellement routé vers ce canal
    float currentVelocity;
    float currentPan;
    float currentPitch;
    bool isActive; // Indique si ce canal est actuellement en train de jouer un son
                   //
    // Un buffer temporaire pour le son de l'instrument, avant qu'il ne soit mixé.
    // Sa taille sera ajustée dynamiquement.
    std::vector<float> instrumentBuffer;


    // Constructeur
    AdikChannel(int channelId) : id(channelId), currentVelocity(0.0f), currentPan(0.0f), currentPitch(0.0f), isActive(false) {
        std::cout << "Canal Mixeur " << id << " créé." << std::endl;
    }

    // Reçoit un événement sonore et active le canal
    void receiveSound(std::shared_ptr<AdikInstrument> instr, float vel, float pan, float pitch) {
        currentInstrument = instr;
        currentVelocity = vel;
        currentPan = pan;
        currentPitch = pitch;
        isActive = true; // Le canal est maintenant actif et devrait rendre le son
        std::cout << "receiveSound: le Canal est maintenant actif\n";
        if (currentInstrument) {
            currentInstrument->resetPlayback(); // Réinitialise la lecture de l'instrument
            std::cout << "Canal " << id << " reçoit un son de '" << currentInstrument->name << "'." << std::endl;
        }
    }

    // Réinitialise le canal (arrêt de la lecture)
    void clear() {
        isActive = false;
        currentInstrument = nullptr; // Libère l'instrument (si partagé)
        currentVelocity = 0.0f;
        currentPan = 0.0f;
        currentPitch = 0.0f;
    }

    // Affiche le statut du canal
    void displayStatus() const {
        std::cout << "Canal " << id << ": " << (isActive ? "ACTIF" : "Inactif");
        if (currentInstrument) {
            std::cout << " - Instrument: " << currentInstrument->name;
        }
        std::cout << std::endl;
    }

    // <--- MODIFIÉ : renderToBuffer
    // Cette fonction ne va plus directement dans le outputBuffer de la carte son,
    // mais elle renvoie les données de l'instrument dans son propre format.
    // Le mixeur s'occupera de la conversion finale.
    // Retourne le nombre de samples (pas de frames) traités.
    void render(std::vector<float>& outputBuffer, unsigned int numFrames) {
        if (isActive && currentInstrument) {
            unsigned int instrChannels = currentInstrument->getNumChannels();
            // Redimensionner le buffer temporaire pour contenir les données de l'instrument
            // (numFrames * nombre de canaux de l'instrument)
            // outputBuffer.assign(numFrames * instrChannels, 0.0f);

            currentInstrument->render(outputBuffer, numFrames, currentVelocity, currentPan, currentPitch);

            // Le canal est "désactivé" si le son est court (one-shot) et que le buffer a été consommé.
            // Une logique plus sophistiquée pourrait vérifier si AdikSound a encore des samples à lire.
            if (currentInstrument->sound.currentSamplePosition >= currentInstrument->sound.audioData.size()) {
                isActive = false;
                // std::cout << "currentInstrument désactivé, canal id: " << id << "\n";
                // std::cout << "\a";
            }
        } else {
            outputBuffer.clear(); // S'assurer que le buffer est vide si inactif
        }
    }

};

#endif // ADIKCHANNEL_H
