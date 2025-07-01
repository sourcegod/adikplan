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
        if (currentInstrument) {
            currentInstrument->resetPlayback(); // Réinitialise la lecture de l'instrument
            // std::cout << "Canal " << id << " reçoit un son de '" << currentInstrument->name << "'." << std::endl;
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

    // Rend le son du canal dans le buffer de sortie
    void renderToBuffer(float* outputBuffer, int numSamples) {
        if (isActive && currentInstrument) {
            // L'instrument rend ses données directement dans le buffer
            currentInstrument->render(outputBuffer, numSamples, currentVelocity, currentPan, currentPitch);
            // On désactive le canal après l'avoir rendu pour un "one-shot"
            // Dans un système audio réel, cela serait géré par la durée du son.
            // Pour cette simulation, un son est joué en une seule passe.
            isActive = false; // Désactiver après un seul rendu (simule un one-shot)
            // std::cout << "Canal " << id << " rendu." << std::endl;
        }
    }
};

#endif // ADIKCHANNEL_H
