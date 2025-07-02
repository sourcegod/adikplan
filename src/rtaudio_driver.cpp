#include "rtaudio_driver.h" // Incluez le header de la classe RtAudioDriver
#include "audioengine.h"    // Incluez le header de processAudioCallback

// ============================================================================
// Implémentation du wrapper de callback de RtAudio.
// Cette fonction a la signature exacte que RtAudio attend.
// Elle est déclarée static ici pour limiter sa visibilité à ce fichier .cpp.
extern "C" { // Assure la liaison C pour la fonction de rappel
    static int rtAudioCallbackWrapper(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                                      double streamTime, RtAudioStreamStatus status, void *userData) {
        // Gérer les erreurs de statut du flux si nécessaire
        if (status) {
            std::cerr << "RtAudio callback status message: " << status << std::endl;
        }

        // Caster le buffer de sortie au type float* (assumé RTAUDIO_FLOAT32)
        float *out = static_cast<float*>(outputBuffer);

        // Appeler votre fonction processAudioCallback existante
        // C'est ici que le pont entre RtAudio et votre logique audio se fait.
        processAudioCallback(out, nFrames, userData);

        return 0; // Retourne 0 pour indiquer le succès à RtAudio
    }
}

// Implémentation de RtAudioDriver::startStream
bool RtAudioDriver::startStream(unsigned int sampleRate, unsigned int bufferSize, void* userData) {
    if (isStreamOpen) {
        std::cerr << "Erreur: Le flux audio est déjà ouvert." << std::endl;
        return false;
    }

    if (audio.getDeviceCount() < 1) {
        std::cerr << "Erreur: Aucun périphérique audio trouvé !" << std::endl;
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = 2; // Stéréo
    parameters.firstChannel = 0;

    unsigned int calculatedBufferSize = bufferSize;

    try {
        audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, sampleRate, &calculatedBufferSize, &rtAudioCallbackWrapper, userData);

        audio.startStream();
        isStreamOpen = true;
        std::cout << "Flux audio RtAudio démarré avec succès !" << std::endl;
        std::cout << "  Sample Rate: " << sampleRate << std::endl;
        std::cout << "  Buffer Size (effective): " << calculatedBufferSize << std::endl;
        std::cout << "  Output Device: " << audio.getDeviceInfo(parameters.deviceId).name << std::endl;
        return true;

    } catch (RtAudioError &e) {
        e.printMessage();
        std::cerr << "Erreur lors du démarrage du flux audio." << std::endl;
        return false;
    }
}

// Implémentation de RtAudioDriver::closeStream
void RtAudioDriver::closeStream() {
    if (isStreamOpen) {
        try {
            if (audio.isStreamRunning()) {
                audio.stopStream();
            }
            if (audio.isStreamOpen()) {
                audio.closeStream();
            }
            isStreamOpen = false;
            std::cout << "Flux audio RtAudio arrêté et fermé." << std::endl;
        } catch (RtAudioError &e) {
            e.printMessage();
            std::cerr << "Erreur lors de la fermeture du flux audio." << std::endl;
        }
    }
}


