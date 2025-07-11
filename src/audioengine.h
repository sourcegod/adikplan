#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "rtaudio_driver.h" // Incluez le driver spécifique (ici RtAudio)
#include "audioinfo.h"      // Inclure la nouvelle structure AudioInfo
#include <memory>           // Pour std::unique_ptr
#include <iostream>         // Pour les messages de débogage

// Forward declaration de AdikPlayer (si nécessaire, ici car on passe son pointeur)
class AdikPlayer;

// Déclaration de la fonction de rappel principale, toujours visible globalement.
// Elle sera l'interface entre le driver audio et votre logique de mixage.
extern void processAudioCallback(float* outputBuffer, unsigned int numSamples, void* userData);

class AudioEngine {
public:
    // Pointeur intelligent vers le driver audio.
    // On utilise unique_ptr car l'AudioEngine "possède" ce driver.
    std::unique_ptr<RtAudioDriver> audioDriver;

    // Pointeur vers l'instance de AdikPlayer que le callback utilisera
    std::shared_ptr<AdikPlayer> playerInstance;

    AudioInfo audioInfo; // Stocke les paramètres audio pour référence
    bool _running = false;

    // Constructeur
    AudioEngine() : playerInstance(nullptr) {
        std::cout << "AudioEngine: Constructeur appelé." << std::endl;
    }

    // Destructeur
    ~AudioEngine() {
        close(); // S'assurer que le driver est fermé quand l'AudioEngine est détruit
        std::cout << "AudioEngine: Destructeur appelé." << std::endl;
    }
    
    /**
     * @brief Initialise le moteur audio avec les paramètres et l'instance du player.
     * @param info Les paramètres audio encapsulés dans AudioInfo.
     * @param playerData Un pointeur vers l'instance de AdikPlayer.
     * @return True si l'initialisation réussit, False sinon.
     */
    // bool init(const AudioInfo& info, AdikPlayer* playerData) {
    bool init(const AudioInfo& info, std::shared_ptr<AdikPlayer> playerData) {
        if (!playerData) {
            std::cerr << "Erreur: Le shared_ptr de AdikPlayer fourni est nul." << std::endl;
            return false;
        }

        this->audioInfo = info; // Copie les paramètres dans la variable membre
        this->playerInstance = playerData;

        // Créer l'instance du driver audio
        audioDriver = std::make_unique<RtAudioDriver>();

        std::cout << "AudioEngine: Initialisé avec succès." << std::endl;
        audioInfo.display(); // Pour confirmation
        return true;
    }


    /**
     * @brief Démarre le flux audio.
     * @param sampleRate Le taux d'échantillonnage (doit être le même que celui d'init).
     * @param bufferSize Le nombre de samples par buffer (doit être le même que celui d'init).
     * @return True si le démarrage réussit, False sinon.
     */
    bool start() {
        if (!audioDriver) {
            std::cerr << "AudioEngine: Driver audio non initialisé. Appelez init() d'abord." << std::endl;
            return false;
        }

        if (!playerInstance) {
            std::cerr << "AudioEngine: Instance de AdikPlayer non fournie à init()." << std::endl;
            return false;
        }

        std::cout << "AudioEngine: Démarrage du flux audio..." << std::endl;
        // Appelez la méthode startStream du driver RtAudio, en passant le playerInstance comme userData.
        _running = true;
        return audioDriver->startStream(audioInfo.sampleRate, audioInfo.bufferSize, playerInstance.get());
    }

    /**
     * @brief Arrête le flux audio.
     */
    void stop() {
        if (audioDriver) {
            std::cout << "AudioEngine: Arrêt du flux audio..." << std::endl;
            audioDriver->stopStream(); // Assurez-vous que cette méthode existe et est publique dans RtAudioDriver
            _running = false;
        } else {
            std::cerr << "AudioEngine: Driver audio non initialisé pour l'arrêt." << std::endl;
        }
    }

    /**
     * @brief Test si le driver audio est lancé.
    */
    bool isRunning() { return _running; }

     /**
     * @brief Ferme le driver audio et libère ses ressources.
     */

     void close() {
        if (audioDriver) {
            std::cout << "AudioEngine: Fermeture du driver audio..." << std::endl;
            audioDriver->closeStream();
            audioDriver.reset(); // Libère le unique_ptr et détruit l'objet RtAudioDriver
            playerInstance = nullptr; // Réinitialise le pointeur aussi
        } else {
            std::cerr << "AudioEngine: Driver audio déjà fermé ou non initialisé." << std::endl;
        }
     }

};

#endif // AUDIOENGINE_H
