#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "rtaudio_driver.h" // Incluez le driver spécifique (ici RtAudio)
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
    AdikPlayer* playerInstance;

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
     * @brief Initialise le moteur audio avec les paramètres spécifiés.
     * @param sampleRate Le taux d'échantillonnage (Hz).
     * @param bufferSize Le nombre de samples par buffer audio.
     * @param numChannels Le nombre de canaux audio (ex: 2 pour stéréo).
     * @param playerData Un pointeur vers l'instance de AdikPlayer à utiliser dans le callback.
     * @return True si l'initialisation réussit, False sinon.
     */
    bool init(unsigned int sampleRate, unsigned int bufferSize, unsigned int numChannels, AdikPlayer* playerData) {
        // Stocker le pointeur vers le player
        this->playerInstance = playerData;

        // Créer une instance du driver RtAudio (ou tout autre driver)
        // en utilisant un unique_ptr pour la gestion automatique de la mémoire.
        audioDriver = std::make_unique<RtAudioDriver>();

        // Ici, on ne fait que stocker les paramètres, le start() les utilisera.
        // RtAudioDriver::startStream prend ces paramètres, donc pas besoin de les stocker en membres ici.
        // Mais vous pouvez les stocker si vous voulez y accéder plus tard.
        // Pour l'instant, on les passera directement à startStream dans AudioEngine::start().

        std::cout << "AudioEngine: Initialisé avec succès." << std::endl;
        return true;
    }

    /**
     * @brief Démarre le flux audio.
     * @param sampleRate Le taux d'échantillonnage (doit être le même que celui d'init).
     * @param bufferSize Le nombre de samples par buffer (doit être le même que celui d'init).
     * @return True si le démarrage réussit, False sinon.
     */
    bool start(unsigned int sampleRate, unsigned int bufferSize) {
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
        return audioDriver->startStream(sampleRate, bufferSize, playerInstance);
    }

    /**
     * @brief Arrête le flux audio.
     */
    void stop() {
        if (audioDriver) {
            std::cout << "AudioEngine: Arrêt du flux audio..." << std::endl;
            audioDriver->stopStream(); // Assurez-vous que cette méthode existe et est publique dans RtAudioDriver
        } else {
            std::cerr << "AudioEngine: Driver audio non initialisé pour l'arrêt." << std::endl;
        }
    }

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
