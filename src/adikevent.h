#ifndef ADIKEVENT_H
#define ADIKEVENT_H

#include <memory> // Pour std::shared_ptr
#include <string>   // Pour std::string
#include <iostream> // Pour std::cout (si utilisé pour le debug)

// IMPORTANT : AdikInstrument.h DOIT être inclus avant AdikEvent.h
// car AdikEvent contient un std::shared_ptr<AdikInstrument>.
#include "adikinstrument.h" // Assurez-vous que AdikInstrument.h est défini et inclus avant

class AdikEvent {
public:
    std::shared_ptr<AdikInstrument> instrument; // L'instrument à jouer pour cet événement
    float velocity;                             // Volume de l'événement (0.0f à 1.0f)
    float pan;                                  // Panoramique (-1.0f gauche à +1.0f droite)
    float pitch;                                // Pitch shift (par exemple, 0.0f pour original, +/- pour transposer)
    int step;                                   // Le pas du séquenceur où cet événement se déclenche

    // Constructeur
    AdikEvent(std::shared_ptr<AdikInstrument> instr, int s, float vel = 1.0f, float p = 0.0f, float pt = 0.0f)
        : instrument(instr), velocity(vel), pan(p), pitch(pt), step(s) {
        // std::cout << "AdikEvent créé pour instrument " << (instrument ? instrument->name : "N/A") << " au pas " << step << std::endl;
    }
};

#endif // ADIKEVENT_H
