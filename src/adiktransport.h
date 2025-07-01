
#include "adikplayer.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>


// --- adiktransport.h ---
class AdikTransport {
private:
    std::shared_ptr<AdikPlayer> player; // Pointeur vers l'instance du AdikPlayer à contrôler

public:
    // Constructeur : Prend un shared_ptr vers l'instance de AdikPlayer
    AdikTransport(std::shared_ptr<AdikPlayer> p) : player(p) {
        if (!player) {
            std::cerr << "AdikTransport créé avec un AdikPlayer nul ! Ceci pourrait causer des problèmes." << std::endl;
        }
        std::cout << "AdikTransport initialisé." << std::endl;
    }

    // ::play: pour jouer la séquence ou le morceau courant
    void play() {
        if (player) {
            player->start();
            std::cout << "[TRANSPORT] Lecture démarrée." << std::endl;
        }
    }

    // ::pause: pour mettre en pause la séquence ou le morceau courant, au step en courant
    void pause() {
        if (player) {
            if (player->isPlaying) {
                player->isPlaying = false; // Mettre directement la variable à false
                std::cout << "[TRANSPORT] Lecture en pause au pas " << player->currentStepInSequence << "." << std::endl;
            } else {
                std::cout << "[TRANSPORT] Le lecteur n'est pas en lecture, impossible de mettre en pause." << std::endl;
            }
        }
    }

    // ::stop: pour remettre à 0 la séquence ou le morceau courant
    void stop() {
        if (player) {
            player->stop(); // Arrête la lecture
            player->currentStepInSequence = 0; // Remet le pas courant à 0
            player->currentSampleInStep = 0;   // Remet le sample courant à 0
            if (player->currentMode == AdikPlayer::SONG_MODE) {
                player->currentSequenceIndexInSong = 0; // Remet l'index de séquence du morceau à 0
            }
            std::cout << "[TRANSPORT] Lecture arrêtée et réinitialisée." << std::endl;
        }
    }

    // ::setPosition, en step, pour déplacer la séquence ou le morceau à un step donné
    void setPosition(int step) {
        if (player) {
            std::shared_ptr<AdikSequence> currentSeq = nullptr;
            if (player->currentMode == AdikPlayer::SEQUENCE_MODE) {
                if (player->selectedSequenceInPlayerIndex >= 0 && player->selectedSequenceInPlayerIndex < player->sequenceList.size()) {
                    currentSeq = player->sequenceList[player->selectedSequenceInPlayerIndex];
                }
            } else { // SONG_MODE
                if (player->currentSong && player->currentSequenceIndexInSong >= 0 && player->currentSequenceIndexInSong < player->currentSong->sequences.size()) {
                    currentSeq = player->currentSong->sequences[player->currentSequenceIndexInSong];
                }
            }

            if (currentSeq) {
                // S'assurer que le pas est dans les limites de la séquence ou du morceau
                int maxSteps = (player->currentMode == AdikPlayer::SEQUENCE_MODE) ?
                                currentSeq->lengthInSteps : player->currentSong->getTotalSteps(); // Si en mode song, on prend le total des steps du song.

                int targetStep = std::max(0, std::min(step, maxSteps - 1)); // Clamper la valeur
                
                if (player->currentMode == AdikPlayer::SONG_MODE) {
                    // Trouver la bonne séquence et le bon pas dans cette séquence pour le mode SONG
                    int songAbsoluteStep = targetStep;
                    int seqIndex = 0;
                    int stepInSeq = 0;
                    player->currentSong->getSequenceAndStepFromAbsoluteStep(songAbsoluteStep, seqIndex, stepInSeq);
                    
                    player->currentSequenceIndexInSong = seqIndex;
                    player->currentStepInSequence = stepInSeq;

                } else { // SEQUENCE_MODE
                    player->currentStepInSequence = targetStep;
                }
                
                player->currentSampleInStep = 0; // Toujours réinitialiser le sample dans le pas
                std::cout << "[TRANSPORT] Position définie au pas " << targetStep << "." << std::endl;
            } else {
                std::cerr << "[TRANSPORT] Aucune séquence ou morceau actif pour définir la position." << std::endl;
            }
        }
    }

    // ::rewind: pour reculer en step, par défaut un step
    void rewind(int stepsToRewind = 1) {
        if (player) {
            int currentAbsoluteStep = player->currentStepInSequence;
            if (player->currentMode == AdikPlayer::SONG_MODE && player->currentSong) {
                currentAbsoluteStep = player->currentSong->getAbsoluteStep(player->currentSequenceIndexInSong, player->currentStepInSequence);
            }
            
            setPosition(currentAbsoluteStep - stepsToRewind);
            std::cout << "[TRANSPORT] Reculé de " << stepsToRewind << " pas." << std::endl;
        }
    }

    // ::foward: pour avancer en step, par défaut un step
    void forward(int stepsToForward = 1) {
        if (player) {
            int currentAbsoluteStep = player->currentStepInSequence;
            if (player->currentMode == AdikPlayer::SONG_MODE && player->currentSong) {
                currentAbsoluteStep = player->currentSong->getAbsoluteStep(player->currentSequenceIndexInSong, player->currentStepInSequence);
            }

            setPosition(currentAbsoluteStep + stepsToForward);
            std::cout << "[TRANSPORT] Avancé de " << stepsToForward << " pas." << std::endl;
        }
    }

    // ::printInfo: pour afficher des infos sur la séquence ou le morceau en cours,
    // le nom, la mesure, le step courant, l'état du player: lecture, en pause, ou arrêté.
    void printInfo() const {
        if (!player) {
            std::cout << "[TRANSPORT INFO] Lecteur non disponible." << std::endl;
            return;
        }

        std::cout << "\n--- État du Transport ---" << std::endl;

        // État de la lecture
        std::string playState = "Arrêté";
        if (player->isPlaying) {
            playState = "En Lecture";
        } else if (player->currentSampleInStep > 0 || player->currentStepInSequence > 0) {
            playState = "En Pause"; // Si n'est pas en lecture mais a déjà joué ou est positionné
        }
        std::cout << "État du Player : " << playState << std::endl;

        // Infos sur le tempo
        std::cout << "Tempo : " << player->tempoBPM << " BPM" << std::endl;

        std::shared_ptr<AdikSequence> currentSeq = nullptr;
        std::string currentItemName = "N/A";
        int currentAbsoluteStep = player->currentStepInSequence;
        int currentMeasure = 0;
        int stepInMeasure = 0;
        int totalSteps = 0;
        int totalMeasures = 0;

        if (player->currentMode == AdikPlayer::SEQUENCE_MODE) {
            if (player->selectedSequenceInPlayerIndex >= 0 && player->selectedSequenceInPlayerIndex < player->sequenceList.size()) {
                currentSeq = player->sequenceList[player->selectedSequenceInPlayerIndex];
                currentItemName = currentSeq->name;
                totalSteps = currentSeq->lengthInSteps;
                totalMeasures = currentSeq->numberOfMeasures;
                currentMeasure = currentAbsoluteStep / currentSeq->stepsPerMeasure;
                stepInMeasure = currentAbsoluteStep % currentSeq->stepsPerMeasure;
            }
            std::cout << "Mode : SÉQUENCE" << std::endl;
        } else { // SONG_MODE
            if (player->currentSong && player->currentSequenceIndexInSong >= 0 && player->currentSequenceIndexInSong < player->currentSong->sequences.size()) {
                currentSeq = player->currentSong->sequences[player->currentSequenceIndexInSong];
                currentItemName = player->currentSong->name + " (Séquence courante: " + currentSeq->name + ")";
                totalSteps = player->currentSong->getTotalSteps(); // Total de steps du morceau
                totalMeasures = player->currentSong->getTotalMeasures(); // Total de mesures du morceau

                // Calculer le pas absolu dans le morceau
                currentAbsoluteStep = player->currentSong->getAbsoluteStep(player->currentSequenceIndexInSong, player->currentStepInSequence);
                
                // Calculer la mesure et le pas dans cette mesure pour le morceau entier
                // C'est un peu plus complexe car chaque séquence peut avoir un nombre de steps/mesure différent,
                // mais pour la simplicité, si tous les SPM sont les mêmes, on peut diviser par un SPM global.
                // Ici, on utilise la logique du player pour le pas dans la séquence courante.
                currentMeasure = currentAbsoluteStep / currentSeq->stepsPerMeasure; // Approx. mesure globale si SPM constants
                stepInMeasure = currentAbsoluteStep % currentSeq->stepsPerMeasure; // Pas relatif à la mesure de la séquence courante
            } else if (player->currentSong) {
                currentItemName = player->currentSong->name + " (Pas de séquence active)";
            }
            std::cout << "Mode : MORCEAU" << std::endl;
        }

        std::cout << "Élément en cours : " << currentItemName << std::endl;
        std::cout << "Pas courant (Absolu) : " << currentAbsoluteStep + 1 << " / " << totalSteps << std::endl;
        std::cout << "Mesure courante : " << currentMeasure + 1 << std::endl;
        std::cout << "Pas dans la mesure : " << stepInMeasure + 1 << std::endl; // Afficher 1-based
        // Afficher la progression dans le step actuel
        std::cout << "Progression dans le pas : "
                  << std::fixed << std::setprecision(2)
                  << (static_cast<float>(player->currentSampleInStep) / player->samplesPerStep) * 100.0f
                  << "%" << std::endl;
        std::cout << "-------------------------" << std::endl;
    }
};


