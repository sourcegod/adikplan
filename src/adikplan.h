// adikplan.h
#pragma once

/*
// Inclure toutes les classes dans l'ordre de dépendance
#include "AdikSound.h"
#include "AdikInstrument.h"
#include "AdikEvent.h"
#include "AdikChannel.h"
#include "AdikMixer.h"
#include "AdikTrack.h"
#include "AdikSequence.h"
#include "AdikSong.h"
#include "AdikPlayer.h"
*/
// #include "adiktransport.h"
#include "adikplayer.h"
// Vous pouvez également inclure des bibliothèques standards ici si elles sont utilisées globalement
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <algorithm> // Pour std::fill
// Pour le délai simulé
#include <chrono>
#include <thread>
#include <iomanip> // Pour std::fixed et std::setprecision


//
// --- Constants (peuvent être déplacées dans un fichier de configuration global si nécessaire) ---
const float PI = 3.14159265358979323846f;
const float MAX_AMPLITUDE = 0.8f; // Pour éviter la saturation lors de la génération de son simple
const int DEFAULT_SAMPLE_RATE = 44100; // Taux d'échantillonnage par défaut pour la génération de son
                                       //
// --- Déclarations anticipées ---
class AdikSound;
class AdikInstrument;
class AdikEvent;
class AdikTrack;
class AdikChannel;
class AdikMixer;
class AdikSequence;
class AdikSong;
class AdikPlayer;
class AdikTransport; // Nouvelle classe à déclarer ici

static void processAudioCallback(float* outputBuffer, int numSamples, void* userData);
/*
class AdikSound {
public:
    std::vector<float> audioData;
    int currentSamplePosition; // Pour la lecture au sein de ce buffer de son

    // Constructeur : Génère les données audio en fonction du type de son
    AdikSound(const std::string& soundType) : currentSamplePosition(0) {
        // La taille du son est fixée à 100 ms pour l'exemple
        audioData.resize(static_cast<size_t>(DEFAULT_SAMPLE_RATE * 0.1)); // 100ms de son à 44.1kHz

        if (soundType == "kick") {
            // Impulsion pour kick
            for (int i = 0; i < audioData.size(); ++i) {
                audioData[i] = MAX_AMPLITUDE * std::exp(-(float)i / (audioData.size() * 0.1f)) * std::sin(2.0f * PI * 50.0f * i / DEFAULT_SAMPLE_RATE);
            }
        } else if (soundType == "snare" || soundType == "hihat" || soundType == "clap") {
            // Bruit blanc ou transitoire court pour snare/hihat
            for (int i = 0; i < audioData.size(); ++i) {
                audioData[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.5f * MAX_AMPLITUDE * std::exp(-(float)i / (audioData.size() * 0.05f));
            }
        } else {
            // Onde sinusoïdale par défaut pour les autres
            for (int i = 0; i < audioData.size(); ++i) {
                audioData[i] = MAX_AMPLITUDE * std::sin(2.0f * PI * 220.0f * i / DEFAULT_SAMPLE_RATE);
            }
        }
    }

    // Lit les données de l'AdikSound et les ajoute au buffer de sortie
    // Cette fonction était auparavant AdikInstrument::render
    void readData(float* outputBuffer, int numSamples, float finalVelocity, float finalPan, float finalPitch) {
        // finalPitch est ignoré pour la démo, il affecterait le taux de lecture
        // finalPan est ignoré pour la démo mono, il affecterait les canaux L/R en stéréo

        for (int i = 0; i < numSamples; ++i) {
            if (currentSamplePosition < audioData.size()) {
                float sample = audioData[currentSamplePosition];
                outputBuffer[i] += sample * finalVelocity; // Additionne l'échantillon au buffer de sortie
                currentSamplePosition++;
            } else {
                // L'échantillon est terminé, nous avons lu tout ce que nous pouvions pour ce AdikSound.
                // Le reste du buffer pour cet instrument restera à 0 pour ce tour de rendu.
                break;
            }
        }
    }

    // Réinitialise la position de lecture du son
    void resetPlayback() {
        currentSamplePosition = 0;
    }
};

/// adikinstrument.h --- ///
// Représente la définition d'un son de batterie (Kick, Snare, Hi-Hat, etc.)
// C'est une ressource globale unique pour chaque son.
class AdikInstrument {
public:
    std::string id;             // ID unique de l'instrument (ex: "kick_standard")
    std::string name;           // Nom affichable (ex: "Grosse Caisse Standard")
    std::string audioFilePath;  // Chemin vers le fichier audio de l'échantillon (toujours utile pour une vraie implémentation)
    float defaultVolume;        // Volume par défaut (0.0 à 1.0)
    float defaultPan;           // Panoramique par défaut (-1.0 à 1.0)
    float defaultPitch;         // Pitch par défaut (ex: 0.0 pour l'original)

    // *** MODIFICATION MAJEURE ***
    // 3. Dans la classe AdikInstrument, nous allons ajouter une variable sound du type AdikSound
    AdikSound sound; // L'instance de AdikSound pour cet instrument

    AdikInstrument(const std::string& id, const std::string& name, const std::string& path)
        : id(id), name(name), audioFilePath(path), defaultVolume(1.0f), defaultPan(0.0f), defaultPitch(0.0f),
          // Initialisation de AdikSound en fonction de l'ID de l'instrument pour la génération du son
          sound(id.find("kick") != std::string::npos ? "kick" :
                (id.find("snare") != std::string::npos ? "snare" :
                 (id.find("hihat") != std::string::npos ? "hihat" :
                  (id.find("clap") != std::string::npos ? "clap" : "default"))))
    {
        // Le corps du constructeur peut maintenant être plus simple,
        // la logique de génération des données audio est dans AdikSound.
    }

    // Ancienne fonction AdikInstrument::render est remplacée par un appel à AdikSound::readData
    // Cette fonction sera appelée par le mixer lors du rendu audio
    void render(float* buffer, int numSamples, float finalVelocity, float finalPan, float finalPitch) {
        // Appelle la méthode readData de l'objet AdikSound interne
        sound.readData(buffer, numSamples, finalVelocity, finalPan, finalPitch);
    }

    // Réinitialise la position de lecture de l'instrument (en fait, du AdikSound interne)
    void resetPlayback() {
        sound.resetPlayback();
    }
};
*/

// --- AdikEvent.h ---
// Représente un événement unique (un "hit" sur un instrument)
// Se produit à un pas donné, avec une vélocité et fait référence à un instrument.
class AdikEvent {
public:
    std::shared_ptr<AdikInstrument> instrument; // Référence à l'instrument à jouer
    int step;           // Le pas (timing) où l'événement doit se produire (0 à patternLength-1)
    float velocity;     // La vélocité/force du hit (0.0 à 1.0, spécifique à cet événement)
    float pan;          // Panoramique spécifique à cet événement (-1.0 à 1.0)
    float pitch;        // Pitch spécifique à cet événement (ex: 0.0 pour l'original)

    AdikEvent(std::shared_ptr<AdikInstrument> instr, int s, float vel = 1.0f, float p = 0.0f, float pi = 0.0f)
        : instrument(instr), step(s), velocity(vel), pan(p), pitch(pi) {}
};

/*
// --- AdikChannel.h ---
// Représente un canal individuel sur le mixeur.
// Il simule la réception et l'affichage des informations d'un son en cours de lecture.
class AdikChannel {
public:
    int id;
    std::shared_ptr<AdikInstrument> currentInstrument; // L'instrument actuellement routé sur ce canal
    float currentVelocity;
    float currentPan;
    float currentPitch;
    bool isActive; // Indique si un son est actuellement "actif" sur ce canal

    AdikChannel(int channelId)
        : id(channelId), currentInstrument(nullptr), currentVelocity(0.0f), currentPan(0.0f), currentPitch(0.0f), isActive(false) {}

    // Met à jour l'état du canal avec un nouvel instrument et ses paramètres
    void receiveSound(std::shared_ptr<AdikInstrument> instr, float vel, float pan, float pitch) {
        currentInstrument = instr;
        currentVelocity = vel;
        currentPan = pan;
        currentPitch = pitch;
        isActive = true;
        if (currentInstrument) {
            currentInstrument->resetPlayback(); // Réinitialise la lecture de l'instrument quand il est reçu
        }
    }

    // Efface l'état du canal (simule la fin d'une note ou un canal silencieux)
    void clear() {
        currentInstrument = nullptr; // Libère l'instrument du canal
        currentVelocity = 0.0f;
        currentPan = 0.0f;
        currentPitch = 0.0f;
        isActive = false;
    }

    // Affiche le statut actuel du canal pour la simulation
    void displayStatus() const {
        if (isActive && currentInstrument) {
            std::cout << "C" << id << ": " << currentInstrument->name << " [V:" << currentVelocity << ", Pn:" << currentPan << ", Pt:" << currentPitch << "] ";
        } else {
            std::cout << "C" << id << ": (Vide) ";
        }
    }

    // Nouveau: Rend le son du canal dans le buffer de sortie
    void renderToBuffer(float* outputBuffer, int numSamples) {
        if (isActive && currentInstrument) {
            currentInstrument->render(outputBuffer, numSamples, currentVelocity, currentPan, currentPitch);
        }
    }
};

// --- AdikMixer.h ---
// Gère et route les sons vers les canaux.
// Il centralise la "sortie" audio de la drum machine.
class AdikMixer {
public:
    std::vector<AdikChannel> channels;
    static const int NUM_MIXER_CHANNELS = 8; // Nombre fixe de canaux de mixage pour cet exemple
    float masterVolume; // Volume global du mixeur

    AdikMixer() : masterVolume(0.7f) { // Volume par défaut pour éviter la saturation
        channels.reserve(NUM_MIXER_CHANNELS);
        for (int i = 0; i < NUM_MIXER_CHANNELS; ++i) {
            channels.emplace_back(i + 1); // Les canaux sont numérotés de 1 à NUM_MIXER_CHANNELS
        }
        std::cout << "AdikMixer initialisé avec " << NUM_MIXER_CHANNELS << " canaux." << std::endl;
    }

    // Route un événement sonore vers un canal spécifique du mixeur.
    void routeSound(int channelIndex, std::shared_ptr<AdikInstrument> instrument, float finalVelocity, float finalPan, float finalPitch) {
        if (channelIndex < 1 || channelIndex > NUM_MIXER_CHANNELS) {
            std::cerr << "Erreur Mixeur: Canal " << channelIndex << " hors limites. Son non routé." << std::endl;
            return;
        }
        if (!instrument) {
            std::cerr << "Erreur Mixeur: Tentative de router un instrument nul sur le canal " << channelIndex << "." << std::endl;
            return;
        }

        // Le canal du mixeur enregistre l'information du son en cours pour son état interne
        channels[channelIndex - 1].receiveSound(instrument, finalVelocity, finalPan, finalPitch);
        std::cout << "[MIXER - Canal " << channelIndex << "] " << instrument->name << " routé. ";
    }

    // Affiche l'état actuel de tous les canaux du mixeur.
    void displayMixerStatus() const {
        std::cout << "État du Mixeur: [ ";
        for (const auto& channel : channels) {
            channel.displayStatus();
        }
        std::cout << "]" << std::endl;
    }

    // Efface l'état "actif" de tous les canaux du mixeur.
    // Les instruments ne sont pas "clear" tant que leur son est en cours de lecture.
    void clearAllChannelsPlaybackState() {
        // Cette méthode sera appelée au début d'un nouveau pas pour signaler que les événements du pas précédent sont terminés.
        // Cependant, le son lui-même peut continuer de jouer pendant plusieurs buffers.
        // La gestion des samples en cours est maintenant dans AdikInstrument::render.
    }

    // NOUVEAU: Mixe les canaux actifs dans un buffer de sortie
    void mixChannels(float* outputBuffer, int numSamples) {
        // Initialiser le buffer de sortie à zéro
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f);

        // Parcourir chaque canal et lui demander de rendre son son dans le buffer
        for (auto& channel : channels) {
            if (channel.isActive) {
                channel.renderToBuffer(outputBuffer, numSamples);
                // Si l'instrument a fini de jouer, désactiver le canal
                if (channel.currentInstrument && channel.currentInstrument->sound.currentSamplePosition >= channel.currentInstrument->sound.audioData.size()) {
                    channel.clear();
                }
            }
        }

        // Appliquer le volume master et clipper les valeurs si elles dépassent les limites (-1.0 à 1.0)
        for (int i = 0; i < numSamples; ++i) {
            outputBuffer[i] *= masterVolume;
            // Clipping simple pour éviter la saturation numérique
            if (outputBuffer[i] > 1.0f) outputBuffer[i] = 1.0f;
            if (outputBuffer[i] < -1.0f) outputBuffer[i] = -1.0f;
        }
    }
};
*/


// --- AdikTrack.h ---
// Représente une piste au sein d'une séquence.
// Contient son propre "patternData" (une collection d'événements) et des contrôles de mixage.
class AdikTrack {
public:
    std::string name;                   // Nom de la piste (ex: "Kick", "Hi-Hat Groove", "Percussions")
    std::vector<AdikEvent> events;      // Le "patternData" de cette piste : liste des événements
    bool isMuted;                       // La piste est-elle mutée ?
    bool isSoloed;                      // La piste est-elle solo ?
    float volume;                       // Volume de la piste (0.0 à 1.0)
    int mixerChannelIndex;              // Le canal du mixeur auquel cette piste est assignée (1-indexé)

    AdikTrack(const std::string& n = "Nouvelle Piste", int channelIdx = 1)
        : name(n), isMuted(false), isSoloed(false), volume(1.0f), mixerChannelIndex(channelIdx) {}

    // Ajoute un événement à la piste. 'step' est un pas ABSOLU dans la séquence.
    void addEvent(std::shared_ptr<AdikInstrument> instrument, int step, float velocity = 1.0f, float pan = 0.0f, float pitch = 0.0f) {
        events.emplace_back(instrument, step, velocity, pan, pitch);
    }

    // Récupère les événements pour un pas donné sur cette piste
    std::vector<AdikEvent*> getEventsAtStep(int currentStep) {
        std::vector<AdikEvent*> currentStepEvents;
        for (AdikEvent& event : events) {
            if (event.step == currentStep) {
                currentStepEvents.push_back(&event);
            }
        }
        return currentStepEvents;
    }
};

// --- AdikSequence.h ---
// Représente une séquence musicale complète, contenant 16 pistes.
// Gère sa longueur en termes de mesures et de pas par mesure.
class AdikSequence {
public:
    std::string name;                   // Nom de la séquence (ex: "Verse 1", "Chorus", "Breakdown")
    int stepsPerMeasure;                // Nombre de pas par mesure (ex: 16 pour une mesure 4/4)
    int numberOfMeasures;               // Nombre de mesures dans la séquence (ex: 2 pour une séquence de 2 mesures)
    int lengthInSteps;                  // Longueur totale de la séquence en pas (numberOfMeasures * stepsPerMeasure)
    std::vector<AdikTrack> tracks;      // Les 16 pistes de cette séquence

    static const int NUM_TRACKS = 16; // Nombre fixe de pistes par séquence

    AdikSequence(const std::string& n = "Nouvelle Séquence", int numMeasures = 1, int spm = 16)
        : name(n), stepsPerMeasure(spm), numberOfMeasures(numMeasures) {
        lengthInSteps = numberOfMeasures * stepsPerMeasure; // Calcul de la longueur totale
        tracks.reserve(NUM_TRACKS);
        for (int i = 0; i < NUM_TRACKS; ++i) {
            // Assigner chaque piste à un canal de mixage distinct.
            // On s'assure que le channelIndex reste dans les limites de AdikMixer::NUM_MIXER_CHANNELS
            tracks.emplace_back("Piste " + std::to_string(i + 1), (i % AdikMixer::NUM_MIXER_CHANNELS) + 1);
        }
    }

    // Retourne une référence à une piste existante par index
    AdikTrack& getTrack(int index) {
        if (index >= 0 && index < NUM_TRACKS) {
            return tracks[index];
        }
        throw std::out_of_range("Indice de piste hors limites.");
    }
};


// --- AdikSong.h ---
// Représente un morceau, composé d'un enchaînement de séquences.
class AdikSong {
public:
    std::string name;                               // Nom du morceau
    std::vector<std::shared_ptr<AdikSequence>> sequences; // La liste des séquences du morceau

    AdikSong(const std::string& n = "Nouveau Morceau") : name(n) {}

    // Ajoute une séquence au morceau, le nombre de fois spécifié
    void addSequence(std::shared_ptr<AdikSequence> seq_ptr, int numTimes = 1) {
        if (!seq_ptr) {
            std::cerr << "Erreur: Tentative d'ajouter un shared_ptr de séquence nul." << std::endl;
            return;
        }
        for (int i = 0; i < numTimes; ++i) {
            sequences.push_back(seq_ptr);
        }
        std::cout << "Séquence '" << seq_ptr->name << "' ajoutée " << numTimes << " fois au morceau." << std::endl;
    }

    // Duplique une séquence existante DANS LE MORCEAU et l'ajoute à la fin
    // Cela ajoute une nouvelle référence au MÊME objet séquence
    void duplicateSequence(int indexToDuplicate) {
        if (indexToDuplicate >= 0 && indexToDuplicate < sequences.size()) {
            sequences.push_back(sequences[indexToDuplicate]);
            std::cout << "Séquence '" << sequences[indexToDuplicate]->name << "' dupliquée dans le morceau." << std::endl;
        } else {
            std::cerr << "Erreur: Index de séquence à dupliquer hors limites." << std::endl;
        }
    }

    // Supprime une séquence du morceau par son index
    void deleteSequence(int indexToDelete) {
        if (indexToDelete >= 0 && indexToDelete < sequences.size()) {
            std::string deletedSeqName = sequences[indexToDelete]->name;
            sequences.erase(sequences.begin() + indexToDelete);
            std::cout << "Séquence '" << deletedSeqName << "' supprimée du morceau (index: " << indexToDelete << ")." << std::endl;
        } else {
            std::cerr << "Erreur: Index de séquence à supprimer hors limites." << std::endl;
        }
    }

    // Supprime toutes les séquences du morceau
    void clear() {
        sequences.clear();
        std::cout << "Morceau courant vidé." << std::endl;
    }

    // Nouvelle méthode: Obtenir le nombre total de pas dans le morceau
    int getTotalSteps() const {
        int total = 0;
        for (const auto& seq : sequences) {
            total += seq->lengthInSteps;
        }
        return total;
    }

    // Nouvelle méthode: Obtenir le nombre total de mesures dans le morceau (approximatif si les SPM varient)
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
            std::cerr << "Erreur: Indice de séquence invalide dans getAbsoluteStep." << std::endl;
            return 0;
        }

        for (int i = 0; i < sequenceIndex; ++i) {
            absoluteStep += sequences[i]->lengthInSteps;
        }
        absoluteStep += stepInSequence;
        return absoluteStep;
    }

    // Nouvelle méthode: Mapper un pas absolu du morceau à un index de séquence et un pas relatif
    void getSequenceAndStepFromAbsoluteStep(int absoluteStep, int& outSequenceIndex, int& outStepInSequence) const {
        int currentAbsolutePos = 0;
        outSequenceIndex = 0;
        outStepInSequence = 0;

        for (size_t i = 0; i < sequences.size(); ++i) {
            if (absoluteStep >= currentAbsolutePos && absoluteStep < currentAbsolutePos + sequences[i]->lengthInSteps) {
                outSequenceIndex = static_cast<int>(i);
                outStepInSequence = absoluteStep - currentAbsolutePos;
                return;
            }
            currentAbsolutePos += sequences[i]->lengthInSteps;
        }

        // Si le pas absolu est en dehors des limites (peut arriver si on se positionne au-delà du morceau)
        // Revenir au début du morceau, ou à la fin si on dépasse.
        if (!sequences.empty()) {
            if (absoluteStep < 0) { // Si avant le début
                outSequenceIndex = 0;
                outStepInSequence = 0;
            } else { // Si après la fin
                outSequenceIndex = static_cast<int>(sequences.size() - 1);
                outStepInSequence = sequences.back()->lengthInSteps - 1;
            }
        }
    }

};



/*
//
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
*/


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


