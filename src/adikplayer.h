#ifndef ADIKPLAYER_H
#define ADIKPLAYER_H

#include "adikplan.h"
#include "adiksound.h"
#include "adikinstrument.h"
#include "adikevent.h"
#include "adiktrack.h"
#include "adikchannel.h"
#include "adikmixer.h"
#include "adiksequence.h"
#include "adiksong.h"
#include "audioengine.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <chrono>
#include <thread>

// --- AdikPlayer.h ---
// Le moteur principal de la drum machine.
// Gère tous les instruments, les morceaux, et la lecture.
class AdikPlayer {
public:
    // Enumération pour les modes de lecture
    enum PlaybackMode {
        SEQUENCE_MODE, // Lecture d'une seule séquence sélectionnée parmi les 16 disponibles du Player
        SONG_MODE      // Lecture des séquences enchaînées définies dans le AdikSong courant
    };

    static const int NUM_SEQS = 16; // Nombre fixe de séquences disponibles pour le Player

    std::map<std::string, std::shared_ptr<AdikInstrument>> globalInstruments; // Tous les instruments disponibles
    std::vector<std::shared_ptr<AdikSequence>> sequenceList;                   // La liste fixe de 16 séquences disponibles pour le Player
    std::shared_ptr<AdikSong> currentSong;                                   // Le morceau actuellement chargé

    AdikMixer mixer; // L'instance du mixeur gérée par le Player

    int tempoBPM;                       // Tempo global en BPM
    int sampleRate;                     // Taux d'échantillonnage (samples/seconde)
    int bufferSizeSamples;              // Taille du buffer audio en samples
    double samplesPerBeat;              // Nombre de samples par battement (quart de note)
    double samplesPerStep;              // Nombre de samples par pas du séquenceur

    int currentStepInSequence;          // Le pas actuel en cours de lecture dans la séquence
    long long currentSampleInStep;      // Le sample actuel dans le pas courant
    bool isPlaying;                     // Indique si le séquenceur est en lecture

    // Variables de contrôle des modes
    PlaybackMode currentMode;                   // Mode de lecture actuel
    int selectedSequenceInPlayerIndex;          // Index de la séquence sélectionnée dans sequenceList (pour SEQUENCE_MODE)
    int currentSequenceIndexInSong;             // Index de la séquence actuellement jouée dans le morceau (pour SONG_MODE)


    AdikPlayer() : tempoBPM(120), sampleRate(44100), bufferSizeSamples(512), // Taille de buffer typique
                     currentStepInSequence(0), currentSampleInStep(0), isPlaying(false),
                     currentMode(SEQUENCE_MODE), selectedSequenceInPlayerIndex(0), currentSequenceIndexInSong(0) {

        calculateTimingParameters(); // Calculer samplesPerBeat et samplesPerStep

        // Initialiser quelques instruments par défaut
        addInstrument(std::make_shared<AdikInstrument>("kick_1", "Grosse Caisse", "path/to/kick.wav"));
        addInstrument(std::make_shared<AdikInstrument>("snare_1", "Caisse Claire", "path/to/snare.wav"));
        addInstrument(std::make_shared<AdikInstrument>("hihat_closed_1", "Charley Fermé", "path/to/hihat_closed.wav"));
        addInstrument(std::make_shared<AdikInstrument>("hihat_open_1", "Charley Ouvert", "path/to/hihat_open.wav"));
        addInstrument(std::make_shared<AdikInstrument>("clap_1", "Clap", "path/to/clap.wav"));

        // Initialiser les 16 séquences fixes avec des shared_ptr
        sequenceList.reserve(NUM_SEQS);
        for (int i = 0; i < NUM_SEQS; ++i) {
            sequenceList.push_back(std::make_shared<AdikSequence>("Sequence " + std::to_string(i + 1), 1, 16));
        }

        // Créer un morceau vide au démarrage
        currentSong = std::make_shared<AdikSong>("Mon Nouveau Morceau");

        // Remplir une des 16 séquences (par exemple la première, index 0) pour la démo avec 2 mesures
        populateDemoSequence(sequenceList[0], "Intro Groove (2 Mesures)", "kick_1", "snare_1", "hihat_closed_1", "hihat_open_1", 2, 16);
        // Remplir une autre séquence (index 1) avec 1 mesure
        populateDemoSequence(sequenceList[1], "Chorus Beat (1 Mesure)", "kick_1", "snare_1", "hihat_closed_1", "clap_1", 1, 16);
    }

    // Calculer les paramètres de timing basés sur le tempo et le sample rate
    void calculateTimingParameters() {
        // 60 secondes/minute * sampleRate (samples/seconde) / tempoBPM (battements/minute) = samples/battement
        samplesPerBeat = (double)sampleRate * 60.0 / tempoBPM;
        // Un pas est un 16ème de note dans notre cas (16 pas par mesure = 4 battements, donc 1 pas = 1/4 de battement)
        samplesPerStep = samplesPerBeat / 4.0; // Si stepsPerMeasure est 16, et 4 battements/mesure
        std::cout << "Timing: Samples par battement = " << samplesPerBeat << ", Samples par pas = " << samplesPerStep << std::endl;
    }

    // Fonction utilitaire pour peupler une séquence de démonstration
    // Prend en compte le nombre de mesures et de pas par mesure.
    void populateDemoSequence(std::shared_ptr<AdikSequence> seq_ptr, const std::string& name,
                                  const std::string& kickId, const std::string& snareId,
                                  const std::string& hihatClosedId, const std::string& additionalId,
                                  int numMeasures, int spm) {
        if (!seq_ptr) return; // Sécurité

        seq_ptr->name = name;
        seq_ptr->stepsPerMeasure = spm;
        seq_ptr->numberOfMeasures = numMeasures;
        seq_ptr->lengthInSteps = seq_ptr->numberOfMeasures * seq_ptr->stepsPerMeasure;

        // Vider les événements de toutes les pistes de la séquence avant de les remplir
        for(auto& track : seq_ptr->tracks) {
            track.events.clear();
        }

        // Piste 1: Grosse Caisse (assignée au canal 1 du mixeur par défaut)
        AdikTrack& kickTrack = seq_ptr->getTrack(0);
        kickTrack.name = "Kick";
        kickTrack.mixerChannelIndex = 1; // Explicitly assign to channel 1
        for (int m = 0; m < numMeasures; ++m) {
            kickTrack.addEvent(getInstrument(kickId), m * spm + 0);  // Début de chaque mesure
            kickTrack.addEvent(getInstrument(kickId), m * spm + 8);  // Milieu de chaque mesure
        }

        // Piste 2: Caisse Claire (assignée au canal 2 du mixeur par défaut)
        AdikTrack& snareTrack = seq_ptr->getTrack(1);
        snareTrack.name = "Snare";
        snareTrack.mixerChannelIndex = 2; // Explicitly assign to channel 2
        for (int m = 0; m < numMeasures; ++m) {
            snareTrack.addEvent(getInstrument(snareId), m * spm + 4);  // 5ème pas de chaque mesure
            snareTrack.addEvent(getInstrument(snareId), m * spm + 12); // 13ème pas de chaque mesure
        }

        // Piste 3: Charley Fermé (assignée au canal 3 du mixeur par défaut)
        AdikTrack& hihatClosedTrack = seq_ptr->getTrack(2);
        hihatClosedTrack.name = "Hi-Hat Fermé";
        hihatClosedTrack.mixerChannelIndex = 3; // Explicitly assign to channel 3
        for (int m = 0; m < numMeasures; ++m) {
            for (int i = 0; i < spm; ++i) {
                hihatClosedTrack.addEvent(getInstrument(hihatClosedId), m * spm + i, 0.7f);
            }
        }
        hihatClosedTrack.volume = 0.8f;

        // Piste 4: Son additionnel (assignée au canal 4 du mixeur par défaut)
        AdikTrack& additionalTrack = seq_ptr->getTrack(3);
        additionalTrack.name = "Additional Sound";
        additionalTrack.mixerChannelIndex = 4; // Explicitly assign to channel 4
        for (int m = 0; m < numMeasures; ++m) {
            additionalTrack.addEvent(getInstrument(additionalId), m * spm + (spm - 1), 0.9f);
        }
    }


    // Ajoute un instrument à la collection globale
    void addInstrument(std::shared_ptr<AdikInstrument> instrument) {
        globalInstruments[instrument->id] = instrument;
    }

    // Récupère un instrument par son ID
    std::shared_ptr<AdikInstrument> getInstrument(const std::string& id) {
        if (globalInstruments.count(id)) {
            return globalInstruments[id];
        }
        throw std::runtime_error("Instrument avec l'ID '" + id + "' non trouvé.");
        return nullptr; // Ne devrait pas être atteint grâce à throw
    }

    // --- Méthodes de gestion des modes et séquences ---

    // Définit le mode de lecture (Sequence ou Song)
    void setPlaybackMode(PlaybackMode mode) {
        currentMode = mode;
        std::cout << "\nMode de lecture défini sur: " << (mode == SEQUENCE_MODE ? "SEQUENCE_MODE" : "SONG_MODE") << std::endl;
        // Réinitialiser le pas et le sample au début du mode
        currentStepInSequence = 0;
        currentSampleInStep = 0;
        currentSequenceIndexInSong = 0; // Pour s'assurer de commencer le song depuis le début
    }

    // Sélectionne une séquence spécifique parmi les 16 séquences du Player (mode SEQUENCE_MODE)
    void selectSequenceInPlayer(int index) {
        if (index >= 0 && index < sequenceList.size()) {
            selectedSequenceInPlayerIndex = index;
            currentStepInSequence = 0; // Réinitialiser le pas
            currentSampleInStep = 0;   // Réinitialiser le sample
            std::cout << "Séquence sélectionnée dans le Player: " << sequenceList[selectedSequenceInPlayerIndex]->name
                      << " (Longueur: " << sequenceList[selectedSequenceInPlayerIndex]->numberOfMeasures << " mesures, "
                      << sequenceList[selectedSequenceInPlayerIndex]->lengthInSteps << " pas)" << std::endl;
        } else {
            std::cerr << "Erreur: Indice de séquence invalide dans le Player (" << index << ")." << std::endl;
        }
    }

    // Sélectionne une séquence spécifique dans le morceau courant (mode SONG_MODE)
    void selectSequenceInSong(int index) {
        if (currentSong && index >= 0 && index < currentSong->sequences.size()) {
            currentSequenceIndexInSong = index;
            currentStepInSequence = 0; // Réinitialiser le pas
            currentSampleInStep = 0;   // Réinitialiser le sample
            std::cout << "Séquence sélectionnée dans le Morceau: " << currentSong->sequences[currentSequenceIndexInSong]->name << std::endl;
        } else {
            std::cerr << "Erreur: Indice de séquence invalide dans le Morceau (" << index << ")." << std::endl;
        }
    }

    // Ajoute une copie (référence partagée) d'une séquence des 16 séquences du Player au morceau courant
    void addSequenceFromPlayerToSong(int playerSequenceIndex, int numTimes = 1) {
        if (playerSequenceIndex >= 0 && playerSequenceIndex < sequenceList.size()) {
            if (currentSong) {
                currentSong->addSequence(sequenceList[playerSequenceIndex], numTimes);
            } else {
                std::cerr << "Erreur: Aucun morceau courant pour ajouter la séquence." << std::endl;
            }
        } else {
            std::cerr << "Erreur: Indice de séquence du Player invalide." << std::endl;
        }
    }

    // Supprime une séquence du morceau courant par son index
    void deleteSequenceFromCurrentSong(int indexToDelete) {
        if (currentSong) {
            currentSong->deleteSequence(indexToDelete);
        } else {
            std::cerr << "Erreur: Aucun morceau courant pour supprimer la séquence." << std::endl;
        }
    }

    // Réinitialise le morceau courant
    void clearCurrentSong() {
        if (currentSong) {
            currentSong->clear();
        }
    }

    // --- Fonctions de lecture (appelées par AdikTransport) ---
    void start() {
        if (currentMode == SONG_MODE && (!currentSong || currentSong->sequences.empty())) {
            std::cerr << "Aucun morceau ou séquence dans le morceau à jouer en mode SONG." << std::endl;
            return;
        }
        isPlaying = true;
        // On ne réinitialise plus les steps ici, car AdikTransport.stop() s'en charge.
        // start() signifie juste "commencer à jouer à la position actuelle".
        std::cout << "Lecture démarrée (interne)." << std::endl;
    }

    void stop() { // Appelé par AdikTransport
        isPlaying = false;
        // AdikTransport est responsable de la réinitialisation de la position
        std::cout << "Lecture arrêtée (interne)." << std::endl;
    }


    // Rétablit l'ancienne fonction AdikPlayer::advanceStep
    // Gère l'avancement du séquenceur, le déclenchement des événements et le bouclage.
    void advanceStep(std::shared_ptr<AdikSequence> currentPlayingSequence) {
        if (!currentPlayingSequence) return;

        // Affichage textuel pour le pas
        int currentMeasure = currentStepInSequence / currentPlayingSequence->stepsPerMeasure;
        int stepInMeasure = currentStepInSequence % currentPlayingSequence->stepsPerMeasure;

        std::cout << "Mode: " << (currentMode == SEQUENCE_MODE ? "SEQUENCE" : "SONG")
                  << " | Séquence: " << currentPlayingSequence->name
                  << " | Mesure: " << currentMeasure + 1
                  << " | Pas: " << stepInMeasure << " (Abs: " << currentStepInSequence << ")"
                  << " | Événements: ";

        bool hasPlayedSound = false;
        bool hasSoloedTrack = false;
        for (const auto& track : currentPlayingSequence->tracks) {
            if (track.isSoloed) {
                hasSoloedTrack = true;
                break;
            }
        }

        // Déclencher les événements pour ce nouveau pas
        for (auto& track : currentPlayingSequence->tracks) {
            if (track.isMuted || (hasSoloedTrack && !track.isSoloed)) {
                continue;
            }

            std::vector<AdikEvent*> trackEvents = track.getEventsAtStep(currentStepInSequence);
            for (AdikEvent* event : trackEvents) {
                if (event->instrument) {
                    float finalVelocity = event->velocity * track.volume;
                    // Route le son vers le mixeur; le mixeur gère maintenant l'instrument pendant sa durée de son
                    mixer.routeSound(track.mixerChannelIndex, event->instrument, finalVelocity, event->pan, event->pitch);
                    hasPlayedSound = true;
                }
            }
        }
        if (!hasPlayedSound) {
            std::cout << "Rien.";
        }
        std::cout << std::endl;
        mixer.displayMixerStatus(); // Affiche l'état du mixeur à chaque nouveau pas
        std::cout << std::endl;

        // Passer au pas suivant
        currentStepInSequence++;
        if (currentStepInSequence >= currentPlayingSequence->lengthInSteps) {
            currentStepInSequence = 0; // Reboucler le pas dans la séquence actuelle

            if (currentMode == SONG_MODE) {
                currentSequenceIndexInSong++; // Passer à la séquence suivante du morceau
                if (currentSequenceIndexInSong >= currentSong->sequences.size()) {
                    currentSequenceIndexInSong = 0; // Reboucler le morceau
                    std::cout << "--- Morceau bouclé ---" << std::endl;
                }
            }
        }
    }

    // Simuler le processus de lecture audio en temps réel
    void simulateRealtimePlayback(int numSecondsToSimulate) {
        start();
        int totalSamplesToSimulate = numSecondsToSimulate * sampleRate;
        std::vector<float> audioOutputBuffer(bufferSizeSamples); // Buffer pour la sortie audio

        long long samplesProcessed = 0;
        while (samplesProcessed < totalSamplesToSimulate) {
            // Appeler la fonction processAudioCallback globale, en lui passant 'this' comme userData
            ::processAudioCallback(audioOutputBuffer.data(), bufferSizeSamples, this);
            samplesProcessed += bufferSizeSamples;

            // Ici, dans une vraie application, le buffer audioOutputBuffer serait envoyé à la carte son.
            // 3. Décommenter la ligne pour ajouter un délai réaliste
            std::this_thread::sleep_for(std::chrono::milliseconds((long long)(bufferSizeSamples * 1000.0 / sampleRate)));
        }
        stop();
    }
};

/*
// --- Nouvelle fonction de rappel audio globale et statique ---
// 1. Rendre la fonction processAudioCallback indépendante et statique
// Elle prend un pointeur void* userData en paramètre, qui sera casté en AdikPlayer.
static void processAudioCallback(float* outputBuffer, int numSamples, void* userData) {
    // Caster userData en AdikPlayer*
    AdikPlayer* playerData = static_cast<AdikPlayer*>(userData);

    if (!playerData || !playerData->isPlaying) {
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f); // Remplir de silence si pas en lecture
        return;
    }

    // Récupérer la séquence en cours de lecture
    std::shared_ptr<AdikSequence> currentPlayingSequence = nullptr;
    if (playerData->currentMode == AdikPlayer::SEQUENCE_MODE) {
        if (playerData->selectedSequenceInPlayerIndex >= 0 && playerData->selectedSequenceInPlayerIndex < playerData->sequenceList.size()) {
            currentPlayingSequence = playerData->sequenceList[playerData->selectedSequenceInPlayerIndex];
        }
    } else { // SONG_MODE
        if (playerData->currentSong && playerData->currentSequenceIndexInSong >= 0 && playerData->currentSequenceIndexInSong < playerData->currentSong->sequences.size()) {
            currentPlayingSequence = playerData->currentSong->sequences[playerData->currentSequenceIndexInSong];
        }
    }

    if (!currentPlayingSequence) {
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f);
        return;
    }

    // Boucle pour remplir le buffer audio sample par sample
    for (int i = 0; i < numSamples; ++i) {
        // Vérifier si nous devons déclencher un nouvel événement (passer à un nouveau pas)
        if (playerData->currentSampleInStep >= playerData->samplesPerStep) {
            // 2. La fonction processAudioCallback appellera playerData->advanceStep
            playerData->advanceStep(currentPlayingSequence); // Passer la séquence au besoin
            playerData->currentSampleInStep = 0; // Réinitialiser le compteur de samples pour le nouveau pas
        }
        playerData->currentSampleInStep++; // Avancer le sample dans le pas actuel
    }

    // Demander au mixeur de mixer tous les canaux pour ce buffer
    playerData->mixer.mixChannels(outputBuffer, numSamples);
}
*/


#endif // ADIKPLAYER_H
