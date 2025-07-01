#ifndef ADIKTRACK_H
#define ADIKTRACK_H

#include <string>
#include <vector>
#include <memory> // Pour std::shared_ptr si AdikEvent était un pointeur
#include <iostream>

// IMPORTANT : AdikEvent.h DOIT être inclus avant AdikTrack.h
// car AdikTrack contient un std::vector<AdikEvent>.
// Et AdikEvent.h inclut déjà AdikInstrument.h, donc l'ordre est respecté.
#include "adikevent.h"

class AdikTrack {
public:
    std::string name;
    std::vector<AdikEvent> events; // Liste des événements pour cette piste
    float volume;                  // Volume de la piste (0.0f à 1.0f)
    bool isMuted;                  // Si la piste est coupée
    bool isSoloed;                 // Si la piste est en solo
    int mixerChannelIndex;         // Index du canal du mixeur auquel cette piste est routée (1-based)

    // Constructeur
    AdikTrack(const std::string& n, int channelId)
        : name(n), volume(1.0f), isMuted(false), isSoloed(false), mixerChannelIndex(channelId) {
        // std::cout << "Piste '" << name << "' créée, routée vers le canal mixeur " << mixerChannelIndex << "." << std::endl;
    }

    // Ajoute un événement à la piste
    void addEvent(std::shared_ptr<AdikInstrument> instr, int step, float vel = 1.0f, float pan = 0.0f, float pitch = 0.0f) {
        events.emplace_back(instr, step, vel, pan, pitch);
        // std::cout << "  Ajout d'événement: Instrument '" << (instr ? instr->name : "N/A") << "' au pas " << step << " sur la piste '" << name << "'." << std::endl;
    }

    // Récupère tous les événements qui se produisent à un pas donné
    std::vector<AdikEvent*> getEventsAtStep(int step) const {
        std::vector<AdikEvent*> eventsAtStep;
        for (auto& event : events) {
            if (event.step == step) {
                eventsAtStep.push_back(const_cast<AdikEvent*>(&event)); // Nécessaire car 'events' est const dans une méthode const
            }
        }
        return eventsAtStep;
    }
};

#endif // ADIKTRACK_H
