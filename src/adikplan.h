#include <string>
#include <vector>
#include <map>
#include <memory>   // Pour std::shared_ptr, std::make_shared
#include <iostream> // Pour les affichages de démonstration
#include <stdexcept> // Pour std::out_of_range, std::runtime_error

// --- AdikInstrument.h ---
// Représente la définition d'un son de batterie (Kick, Snare, Hi-Hat, etc.)
// C'est une ressource globale unique pour chaque son.
class AdikInstrument {
public:
    std::string id;             // ID unique de l'instrument (ex: "kick_standard")
    std::string name;           // Nom affichable (ex: "Grosse Caisse Standard")
    std::string audioFilePath;  // Chemin vers le fichier audio de l'échantillon
    float defaultVolume;        // Volume par défaut (0.0 à 1.0)
    float defaultPan;           // Panoramique par défaut (-1.0 à 1.0)
    float defaultPitch;         // Pitch par défaut (ex: 0.0 pour l'original)
    // Ajoutez d'autres propriétés spécifiques à l'instrument (enveloppe ADSR, effets, etc.)

    AdikInstrument(const std::string& id, const std::string& name, const std::string& path)
        : id(id), name(name), audioFilePath(path), defaultVolume(1.0f), defaultPan(0.0f), defaultPitch(0.0f) {}

    // Méthode pour jouer le son de l'instrument (sera appelée par le moteur de lecture)
    // La vélocité et le panoramique finaux sont calculés avant cet appel.
    void play(float finalVelocity, float finalPan, float finalPitch) const {
        // Logique de lecture de l'échantillon audio ici
        // Utiliser audioFilePath, et appliquer finalVelocity, finalPan, finalPitch
        // Note: L'implémentation réelle nécessiterait une bibliothèque audio (ex: PortAudio, SFML Audio, OpenAL)
        // Pour cet exemple, nous allons juste simuler la lecture.
        std::cout << name << " (" << id << ") [Vel: " << finalVelocity << ", Pan: " << finalPan << ", Pitch: " << finalPitch << "]";
    }
};

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

// --- AdikTrack.h ---
// Représente une piste au sein d'une séquence.
// Contient son propre "patternData" (une collection d'événements) et des contrôles de mixage.
class AdikTrack {
public:
    std::string name;               // Nom de la piste (ex: "Kick", "Hi-Hat Groove", "Percussions")
    std::vector<AdikEvent> events;  // Le "patternData" de cette piste : liste des événements
    bool isMuted;                   // La piste est-elle mutée ?
    bool isSoloed;                  // La piste est-elle solo ?
    float volume;                   // Volume de la piste (0.0 à 1.0)

    AdikTrack(const std::string& n = "Nouvelle Piste") // La longueur de piste est gérée par la séquence
        : name(n), isMuted(false), isSoloed(false), volume(1.0f) {}

    // Ajoute un événement à la piste. 'step' est un pas ABSOLU dans la séquence.
    void addEvent(std::shared_ptr<AdikInstrument> instrument, int step, float velocity = 1.0f, float pan = 0.0f, float pitch = 0.0f) {
        events.emplace_back(instrument, step, velocity, pan, pitch);
    }

    // Récupère les événements pour un pas donné sur cette piste
    std::vector<AdikEvent*> getEventsAtStep(int currentStep) {
        std::vector<AdikEvent*> currentStepEvents;
        if (isMuted) return currentStepEvents; // Si la piste est mutée, ne retourne rien

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
    std::string name;             // Nom de la séquence (ex: "Verse 1", "Chorus", "Breakdown")
    int stepsPerMeasure;          // Nombre de pas par mesure (ex: 16 pour une mesure 4/4)
    int numberOfMeasures;         // Nombre de mesures dans la séquence (ex: 2 pour une séquence de 2 mesures)
    int lengthInSteps;            // Longueur totale de la séquence en pas (numberOfMeasures * stepsPerMeasure)
    std::vector<AdikTrack> tracks; // Les 16 pistes de cette séquence

    static const int NUM_TRACKS = 16; // Nombre fixe de pistes par séquence

    AdikSequence(const std::string& n = "Nouvelle Séquence", int numMeasures = 1, int spm = 16)
        : name(n), stepsPerMeasure(spm), numberOfMeasures(numMeasures) {
        lengthInSteps = numberOfMeasures * stepsPerMeasure; // Calcul de la longueur totale
        tracks.reserve(NUM_TRACKS);
        for (int i = 0; i < NUM_TRACKS; ++i) {
            tracks.emplace_back("Piste " + std::to_string(i + 1));
        }
    }

    // Retourne une référence à une piste existante par index
    AdikTrack& getTrack(int index) {
        if (index >= 0 && index < NUM_TRACKS) {
            return tracks[index];
        }
        throw std::out_of_range("Indice de piste hors limites.");
    }

    // Retourne tous les événements à jouer pour un pas donné, en tenant compte des mutes/solos
    std::vector<std::pair<AdikEvent*, float>> getAllEventsAtStep(int currentStep) {
        std::vector<std::pair<AdikEvent*, float>> allEventsToPlay;
        bool hasSoloedTrack = false;
        
        // Vérifier s'il y a au moins une piste en solo
        for (const auto& track : tracks) {
            if (track.isSoloed) {
                hasSoloedTrack = true;
                break;
            }
        }

        for (auto& track : tracks) {
            // Si la piste est mutée, ou si une piste est en solo et que celle-ci ne l'est pas, on l'ignore
            if (track.isMuted || (hasSoloedTrack && !track.isSoloed)) {
                continue;
            }

            std::vector<AdikEvent*> trackEvents = track.getEventsAtStep(currentStep);
            for (AdikEvent* event : trackEvents) {
                // Le volume final de l'événement est le volume de l'événement multiplié par le volume de la piste
                float finalVelocity = event->velocity * track.volume;
                allEventsToPlay.push_back({event, finalVelocity});
            }
        }
        return allEventsToPlay;
    }
};

// --- AdikSong.h ---
// Représente un morceau, composé d'un enchaînement de séquences.
class AdikSong {
public:
    std::string name;                   // Nom du morceau
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
};

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
    std::vector<std::shared_ptr<AdikSequence>> sequenceList;                  // La liste fixe de 16 séquences disponibles pour le Player
    std::shared_ptr<AdikSong> currentSong;                                   // Le morceau actuellement chargé

    int tempoBPM;                   // Tempo global en BPM
    int currentStepInSequence;      // Le pas actuel en cours de lecture dans la séquence
    bool isPlaying;                 // Indique si le séquenceur est en lecture

    // Variables de contrôle des modes
    PlaybackMode currentMode;                   // Mode de lecture actuel
    int selectedSequenceInPlayerIndex;          // Index de la séquence sélectionnée dans sequenceList (pour SEQUENCE_MODE)
    int currentSequenceIndexInSong;             // Index de la séquence actuellement jouée dans le morceau (pour SONG_MODE)


    AdikPlayer() : tempoBPM(120), currentStepInSequence(0), isPlaying(false),
                   currentMode(SEQUENCE_MODE), selectedSequenceInPlayerIndex(0), currentSequenceIndexInSong(0) {

        // Initialiser quelques instruments par défaut
        addInstrument(std::make_shared<AdikInstrument>("kick_1", "Grosse Caisse", "path/to/kick.wav"));
        addInstrument(std::make_shared<AdikInstrument>("snare_1", "Caisse Claire", "path/to/snare.wav"));
        addInstrument(std::make_shared<AdikInstrument>("hihat_closed_1", "Charley Fermé", "path/to/hihat_closed.wav"));
        addInstrument(std::make_shared<AdikInstrument>("hihat_open_1", "Charley Ouvert", "path/to/hihat_open.wav"));
        addInstrument(std::make_shared<AdikInstrument>("clap_1", "Clap", "path/to/clap.wav"));

        // Initialiser les 16 séquences fixes avec des shared_ptr
        sequenceList.reserve(NUM_SEQS); // Réserver l'espace pour NUM_SEQS éléments
        for (int i = 0; i < NUM_SEQS; ++i) {
            // Créer chaque séquence avec 1 mesure et 16 pas par mesure par défaut
            sequenceList.push_back(std::make_shared<AdikSequence>("Sequence " + std::to_string(i + 1), 1, 16));
        }

        // Créer un morceau vide au démarrage
        currentSong = std::make_shared<AdikSong>("Mon Nouveau Morceau");

        // Remplir une des 16 séquences (par exemple la première, index 0) pour la démo avec 2 mesures
        populateDemoSequence(sequenceList[0], "Intro Groove (2 Mesures)", "kick_1", "snare_1", "hihat_closed_1", "hihat_open_1", 2, 16);
        // Remplir une autre séquence (index 1) avec 1 mesure
        populateDemoSequence(sequenceList[1], "Chorus Beat (1 Mesure)", "kick_1", "snare_1", "hihat_closed_1", "clap_1", 1, 16);
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
        
        // Piste 1: Grosse Caisse
        AdikTrack& kickTrack = seq_ptr->getTrack(0);
        kickTrack.name = "Kick";
        for (int m = 0; m < numMeasures; ++m) {
            kickTrack.addEvent(getInstrument(kickId), m * spm + 0);  // Début de chaque mesure
            kickTrack.addEvent(getInstrument(kickId), m * spm + 8);  // Milieu de chaque mesure
        }

        // Piste 2: Caisse Claire
        AdikTrack& snareTrack = seq_ptr->getTrack(1);
        snareTrack.name = "Snare";
        for (int m = 0; m < numMeasures; ++m) {
            snareTrack.addEvent(getInstrument(snareId), m * spm + 4);  // 5ème pas de chaque mesure
            snareTrack.addEvent(getInstrument(snareId), m * spm + 12); // 13ème pas de chaque mesure
        }

        // Piste 3: Charley Fermé (sur tous les pas de chaque mesure)
        AdikTrack& hihatClosedTrack = seq_ptr->getTrack(2);
        hihatClosedTrack.name = "Hi-Hat Fermé";
        for (int m = 0; m < numMeasures; ++m) {
            for (int i = 0; i < spm; ++i) {
                hihatClosedTrack.addEvent(getInstrument(hihatClosedId), m * spm + i, 0.7f);
            }
        }
        hihatClosedTrack.volume = 0.8f;

        // Piste 4: Son additionnel (dernier pas de chaque mesure)
        AdikTrack& additionalTrack = seq_ptr->getTrack(3);
        additionalTrack.name = "Additional Sound";
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
        // Réinitialiser le pas quand le mode change pour éviter des décalages
        currentStepInSequence = 0; 
        currentSequenceIndexInSong = 0; // Pour s'assurer de commencer le song depuis le début
    }

    // Sélectionne une séquence spécifique parmi les 16 séquences du Player (mode SEQUENCE_MODE)
    void selectSequenceInPlayer(int index) {
        if (index >= 0 && index < sequenceList.size()) {
            selectedSequenceInPlayerIndex = index;
            currentStepInSequence = 0; // Réinitialiser le pas quand la séquence change
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

    // --- Fonctions de lecture ---

    // Démarre la lecture
    void start() {
        if (currentMode == SONG_MODE && (!currentSong || currentSong->sequences.empty())) {
            std::cerr << "Aucun morceau ou séquence dans le morceau à jouer en mode SONG." << std::endl;
            return;
        }
        isPlaying = true;
        currentStepInSequence = 0; // Toujours commencer au début du pas
        if (currentMode == SONG_MODE) {
            currentSequenceIndexInSong = 0; // Commencer le morceau au début
        }
        std::cout << "Lecture démarrée." << std::endl;
    }

    // Arrête la lecture
    void stop() {
        isPlaying = false;
        std::cout << "Lecture arrêtée." << std::endl;
    }

    // Fait avancer le séquenceur d'un pas
    void advanceStep() {
        if (!isPlaying) {
            return;
        }

        std::shared_ptr<AdikSequence> currentPlayingSequence = nullptr;
        
        // Déterminer quelle séquence jouer en fonction du mode
        if (currentMode == SEQUENCE_MODE) {
            if (selectedSequenceInPlayerIndex >= 0 && selectedSequenceInPlayerIndex < sequenceList.size()) {
                currentPlayingSequence = sequenceList[selectedSequenceInPlayerIndex];
            }
        } else { // SONG_MODE
            if (currentSong && currentSequenceIndexInSong >= 0 && currentSequenceIndexInSong < currentSong->sequences.size()) {
                currentPlayingSequence = currentSong->sequences[currentSequenceIndexInSong];
            }
        }

        if (!currentPlayingSequence) {
            std::cerr << "Erreur: Aucune séquence valide à jouer." << std::endl;
            stop();
            return;
        }

        std::vector<std::pair<AdikEvent*, float>> eventsToPlay = currentPlayingSequence->getAllEventsAtStep(currentStepInSequence);

        // Calculer la mesure et le pas local pour l'affichage
        int currentMeasure = currentStepInSequence / currentPlayingSequence->stepsPerMeasure;
        int stepInMeasure = currentStepInSequence % currentPlayingSequence->stepsPerMeasure;

        std::cout << "Mode: " << (currentMode == SEQUENCE_MODE ? "SEQUENCE" : "SONG")
                  << " | Séquence: " << currentPlayingSequence->name
                  << " | Mesure: " << currentMeasure + 1 // Affichage humain (1-indexé)
                  << " | Pas: " << stepInMeasure << " (Abs: " << currentStepInSequence << ")"
                  << " | Événements: ";
        if (eventsToPlay.empty()) {
            std::cout << "Rien.";
        } else {
            for (const auto& pair : eventsToPlay) {
                AdikEvent* event = pair.first;
                float finalVelocity = pair.second;
                if (event->instrument) {
                    event->instrument->play(finalVelocity, event->pan, event->pitch);
                    std::cout << "; ";
                }
            }
        }
        std::cout << std::endl;

        // Avancer le pas
        currentStepInSequence++;
        if (currentStepInSequence >= currentPlayingSequence->lengthInSteps) {
            currentStepInSequence = 0; // Reboucler le pas

            if (currentMode == SONG_MODE) {
                currentSequenceIndexInSong++; // Passer à la séquence suivante du morceau
                if (currentSequenceIndexInSong >= currentSong->sequences.size()) {
                    currentSequenceIndexInSong = 0; // Reboucler le morceau
                    std::cout << "--- Morceau bouclé ---" << std::endl;
                }
            }
        }
    }

    // Simuler le processus de lecture (appelez cette méthode à intervalles réguliers)
    void simulatePlayback(int numStepsToSimulate) {
        start();
        for (int i = 0; i < numStepsToSimulate; ++i) {
            advanceStep();
            // Dans une vraie application, il y aurait ici un délai basé sur le tempo
            // std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsPerStep));
        }
        stop();
    }
};

 // --- Exemple d'utilisation dans main.cpp ---
int main() {
    AdikPlayer player; // Changé de DrumPlayer à AdikPlayer

    // --- DÉMONSTRATION DU MODE SÉQUENCE ---
    std::cout << "========== MODE SÉQUENCE ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SEQUENCE_MODE); // Utilise AdikPlayer::SEQUENCE_MODE

    // Jouer la première séquence (index 0) du Player, qui a 2 mesures (32 pas au total)
    player.selectSequenceInPlayer(0); // "Intro Groove (2 Mesures)"
    std::cout << "\n--- Simulation de la Séquence 'Intro Groove' (Player Index 0, 2 mesures) ---" << std::endl;
    // Note: On accède à la séquence via le shared_ptr pour obtenir sa longueur
    player.simulatePlayback(player.sequenceList[0]->lengthInSteps); 

    // Modifier la séquence actuelle (Piste 3, Charley Fermé) et rejouer
    std::cout << "\n--- Réduction du volume du Charley Fermé sur Séquence 0 du Player et relecture ---" << std::endl;
    player.sequenceList[0]->getTrack(2).volume = 0.2f; // Modification via shared_ptr
    player.simulatePlayback(player.sequenceList[0]->lengthInSteps);

    // Sélectionner et jouer la deuxième séquence (index 1) du Player, qui a 1 mesure (16 pas au total)
    std::cout << "\n--- Simulation de la Séquence 'Chorus Beat' (Player Index 1, 1 mesure) ---" << std::endl;
    player.selectSequenceInPlayer(1); // "Chorus Beat (1 Mesure)"
    player.simulatePlayback(player.sequenceList[1]->lengthInSteps); 

    // --- DÉMONSTRATION DU MODE SONG ---
    std::cout << "\n\n========== MODE SONG ==========" << std::endl;
    player.setPlaybackMode(AdikPlayer::SONG_MODE); // Utilise AdikPlayer::SONG_MODE

    // Créer un nouveau morceau
    player.clearCurrentSong();
    player.currentSong->name = "Mon Morceau Final (Demo Shared Ptr)";

    // Construire le morceau à partir des séquences du Player
    // Sequence 0: 2 mesures (32 pas)
    // Sequence 1: 1 mesure (16 pas)
    player.addSequenceFromPlayerToSong(0);     // Ajoute "Intro Groove (2 Mesures)" (1 fois par défaut)
    player.addSequenceFromPlayerToSong(1, 2);  // Ajoute "Chorus Beat (1 Mesure)" 2 fois
    player.addSequenceFromPlayerToSong(0);     // Ajoute "Intro Groove (2 Mesures)" à nouveau

    // Aperçu des séquences dans le morceau
    std::cout << "\nSéquences dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name 
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }
    // Notez que la séquence "Intro Groove" (index 0 et 3) pointe vers la même adresse mémoire,
    // montrant qu'il s'agit du même objet partagé.

    // Calculer la longueur totale du morceau pour la simulation
    int totalSongSteps = 0;
    if (player.currentSong) {
        for (const auto& seq_ptr : player.currentSong->sequences) {
            totalSongSteps += seq_ptr->lengthInSteps;
        }
    }

    std::cout << "\n--- Lecture du Morceau '" << player.currentSong->name << "' (Total " << totalSongSteps << " pas) ---" << std::endl;
    player.simulatePlayback(totalSongSteps); // Joue le morceau complet

    // Rejouer le morceau pour montrer le bouclage
    std::cout << "\n--- Relecture du Morceau (bouclage) ---" << std::endl;
    player.simulatePlayback(totalSongSteps + player.currentSong->sequences[0]->lengthInSteps); // Joue plus que le total pour voir le bouclage

    // Exemple: Muter une piste spécifique DANS la première séquence du morceau (affecte la copie dans le song, pas l'originale du player)
    // Ici, puisque les séquences sont partagées, muter la piste de la première séquence du song affectera aussi la dernière séquence du song
    // car elles pointent vers le même objet AdikSequence.
    std::cout << "\n--- Mute de la caisse claire dans la PREMIERE séquence du morceau et relecture de TOUT le morceau ---" << std::endl;
    if (!player.currentSong->sequences.empty()) {
        std::cout << "Muting snare track on sequence '" << player.currentSong->sequences[0]->name << "' (shared_ptr address: " << player.currentSong->sequences[0].get() << ")" << std::endl;
        player.currentSong->sequences[0]->getTrack(1).isMuted = true; // Mute la piste de Snare de la première séquence du morceau
    }
    player.simulatePlayback(totalSongSteps); // Relecture complète du morceau

    std::cout << "\n--- Suppression de la séquence à l'index 1 du morceau ---" << std::endl;
    player.deleteSequenceFromCurrentSong(1); // Supprime la deuxième séquence (Chorus Beat)

    std::cout << "\nSéquences restantes dans le morceau '" << player.currentSong->name << "':" << std::endl;
    for (size_t i = 0; i < player.currentSong->sequences.size(); ++i) {
        std::cout << "  Index " << i << ": " << player.currentSong->sequences[i]->name 
                  << " (Addr: " << player.currentSong->sequences[i].get() << ")" << std::endl;
    }

    std::cout << "\n--- Relecture du morceau après suppression ---" << std::endl;
    totalSongSteps = 0; // Recalculer la longueur totale après suppression
    if (player.currentSong) {
        for (const auto& seq_ptr : player.currentSong->sequences) {
            totalSongSteps += seq_ptr->lengthInSteps;
        }
    }
    player.simulatePlayback(totalSongSteps);


    return 0;
}
       
