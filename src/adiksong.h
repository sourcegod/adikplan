#ifndef ADIKSONG_H
#define ADIKSONG_H

#include <vector>
#include <string>
#include <memory> // Pour std::shared_ptr
#include <numeric> // Pour std::accumulate si besoin (non utilisé ici directement)
#include <stdexcept> // Pour std::out_of_range
#include <iostream>

// IMPORTANT : AdikSequence.h DOIT être inclus avant AdikSong.h
// car AdikSong contient un std::vector<std::shared_ptr<AdikSequence>>.
#include "adiksequence.h" // Assurez-vous que AdikSequence.h est défini et inclus avant

class AdikSong {
public:
    std::string name;
    // Les séquences sont des shared_ptr car elles peuvent être partagées
    // avec AdikPlayer::sequenceList et utilisées dans plusieurs endroits.
    std::vector<std::shared_ptr<AdikSequence>> sequences;

    // Constructeur
    AdikSong(const std::string& songName) : name(songName) {
        std::cout << "Morceau '" << name << "' créé." << std::endl;
    }

    // Ajoute une séquence au morceau
    void addSequence(std::shared_ptr<AdikSequence> sequenceToAdd, int numTimes = 1) {
        if (!sequenceToAdd) {
            std::cerr << "Erreur: Tentative d'ajouter une séquence nulle au morceau." << std::endl;
            return;
        }
        for (int i = 0; i < numTimes; ++i) {
            sequences.push_back(sequenceToAdd);
            std::cout << "Séquence '" << sequenceToAdd->name << "' ajoutée au morceau '" << name << "'." << std::endl;
        }
    }

    // Supprime une séquence du morceau par son index
    void deleteSequence(int index) {
        if (index >= 0 && index < sequences.size()) {
            std::cout << "Séquence '" << sequences[index]->name << "' supprimée du morceau '" << name << "'." << std::endl;
            sequences.erase(sequences.begin() + index);
        } else {
            std::cerr << "Erreur: Indice de séquence invalide pour la suppression." << std::endl;
        }
    }

    // Vide le morceau de toutes ses séquences
    void clear() {
        sequences.clear();
        std::cout << "Morceau '" << name << "' vidé de ses séquences." << std::endl;
    }

    // Obtenir le nombre total de pas dans le morceau
    int getTotalSteps() const {
        int total = 0;
        for (const auto& seq : sequences) {
            total += seq->lengthInSteps;
        }
        return total;
    }

    // Obtenir le nombre total de mesures dans le morceau (approximatif si les SPM varient)
    int getTotalMeasures() const {
        int total = 0;
        for (const auto& seq : sequences) {
            total += seq->numberOfMeasures;
        }
        return total;
    }

    // Convertit un index de séquence et un pas relatif dans cette séquence en un pas absolu dans le morceau.
    int getAbsoluteStep(int sequenceIndex, int stepInSequence) const {
        int absoluteStep = 0;
        if (sequenceIndex < 0 || sequenceIndex >= sequences.size()) {
            // Gérer l'erreur ou retourner une valeur par défaut, ex: 0
            std::cerr << "Erreur: Indice de séquence invalide dans getAbsoluteStep. (" << sequenceIndex << ")" << std::endl;
            return 0;
        }

        for (int i = 0; i < sequenceIndex; ++i) {
            absoluteStep += sequences[i]->lengthInSteps;
        }
        absoluteStep += stepInSequence;
        return absoluteStep;
    }

    // Mapper un pas absolu du morceau à un index de séquence et un pas relatif
    void getSequenceAndStepFromAbsoluteStep(int absoluteStep, int& outSequenceIndex, int& outStepInSequence) const {
        int currentAbsolutePos = 0;
        outSequenceIndex = 0;
        outStepInSequence = 0;

        int totalSongSteps = getTotalSteps();
        if (totalSongSteps == 0) { // Gérer le cas d'un morceau vide
            outSequenceIndex = 0;
            outStepInSequence = 0;
            return;
        }

        // Clamper absoluteStep aux limites du morceau
        if (absoluteStep < 0) {
            absoluteStep = 0;
        } else if (absoluteStep >= totalSongSteps) {
            absoluteStep = totalSongSteps - 1;
        }

        for (size_t i = 0; i < sequences.size(); ++i) {
            if (absoluteStep >= currentAbsolutePos && absoluteStep < currentAbsolutePos + sequences[i]->lengthInSteps) {
                outSequenceIndex = static_cast<int>(i);
                outStepInSequence = absoluteStep - currentAbsolutePos;
                return;
            }
            currentAbsolutePos += sequences[i]->lengthInSteps;
        }
        
        // Si, pour une raison quelconque, on arrive ici (ce qui ne devrait pas si les bornes sont bien gérées),
        // on retourne la dernière position valide.
        if (!sequences.empty()) {
             outSequenceIndex = static_cast<int>(sequences.size() - 1);
             outStepInSequence = sequences.back()->lengthInSteps - 1;
        }
    }
};

#endif // ADIKSONG_H
