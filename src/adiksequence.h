#ifndef ADIKSEQUENCE_H
#define ADIKSEQUENCE_H

#include <string>
#include <vector>
#include <memory> // Pour std::shared_ptr si AdikTrack était un pointeur
#include <iostream>

// IMPORTANT : AdikTrack.h DOIT être inclus avant AdikSequence.h
// car AdikSequence contient un std::vector<AdikTrack>.
#include "adiktrack.h" // Assurez-vous que AdikTrack.h est défini et inclus avant

class AdikSequence {
public:
    std::string name;
    std::vector<AdikTrack> tracks; // Les pistes (tracks) qui composent cette séquence
    int numberOfMeasures;          // Nombre de mesures dans la séquence
    int stepsPerMeasure;           // Nombre de pas par mesure (ex: 16 pour 16ème de notes)
    int lengthInSteps;             // Longueur totale de la séquence en pas (numberOfMeasures * stepsPerMeasure)

    // Constructeur
    AdikSequence(const std::string& seqName, int numMeasures, int spm)
        : name(seqName), numberOfMeasures(numMeasures), stepsPerMeasure(spm),
          lengthInSteps(numMeasures * spm) {
        // Initialiser avec 4 pistes par défaut
        tracks.emplace_back("Track 1", 1); // Canal mixeur 1
        tracks.emplace_back("Track 2", 2); // Canal mixeur 2
        tracks.emplace_back("Track 3", 3); // Canal mixeur 3
        tracks.emplace_back("Track 4", 4); // Canal mixeur 4
        std::cout << "Séquence '" << name << "' créée (" << numMeasures << " mesures, "
                  << spm << " pas/mesure, " << lengthInSteps << " pas total)." << std::endl;
    }

    // Accesseur pour une piste spécifique par son index
    AdikTrack& getTrack(int index) {
        if (index >= 0 && index < tracks.size()) {
            return tracks[index];
        }
        throw std::out_of_range("Indice de piste invalide.");
    }
};

#endif // ADIKSEQUENCE_H
