#ifndef ADIKINSTRUMENT_H
#define ADIKINSTRUMENT_H

#include <string>
#include <memory> // Pour std::shared_ptr si AdikSound était un pointeur
#include <iostream>
#include <vector> // Pour AdikSound::audioData

// Important : AdikSound.h DOIT être inclus avant AdikInstrument.h
// car AdikInstrument contient un membre 'AdikSound sound;'.
#include "adiksound.h" // Assurez-vous que AdikSound.h contient la définition complète de AdikSound

class AdikInstrument {
public:
    enum WaveType {
        SINE_WAVE = 0,
        SQUARE_WAVE = 1,
        WHITE_NOISE_WAVE = 2,
        COMBINED_SINE_NOISE_WAVE = 3,

        // Ajoutez d'autres types d'ondes ici si vous les implémentez dans AdikSound
        // TRIANGLE_WAVE = 2,
        // SAW_WAVE = 3
    };


    std::string id;
    std::string name;
    std::string audioFilePath; // Chemin vers le fichier audio (pour l'exemple)
    float defaultVolume;
    float defaultPan;
    float defaultPitch; // Changement de hauteur (pitch shift)


    AdikSound sound; // L'objet AdikSound qui contient les données audio

    // Constructeur
    AdikInstrument(const std::string& id_val, const std::string& name_val,
                   const std::string& path_val, unsigned int numChannels = 1) // Par défaut mono
        : id(id_val), name(name_val), audioFilePath(path_val),
          defaultVolume(1.0f), defaultPan(0.0f), defaultPitch(0.0f),
          sound(id_val, numChannels) // <--- Initialise AdikSound avec les canaux
    {
        std::cout << "Instrument '" << name << "' (" << id << ") créé, canaux: " << numChannels << std::endl;
    }

    // Nouvelle méthode pour obtenir le nombre de canaux de l'instrument
    unsigned int getNumChannels() const {
        return sound.numChannels;
    }

    // <--- MODIFIÉ : Signature de render.
    // Le buffer passé ici est un buffer temporaire pour l'instrument,
    // sa taille sera `numSamples * instrument->getNumChannels()`.
    void render(std::vector<float>& buffer, unsigned int numSamples, float finalVelocity, float finalPan, float finalPitch) {
        // Remarque: La logique de pan/pitch/velocity sera appliquée par le mixer ou la couche supérieure.
        // Ici, AdikSound::readData lit les samples bruts.
        sound.readData(buffer, numSamples); // Lire les samples directement

        // Appliquer la vélocité (volume) et le pitch (simplifié) ici, avant le pan au niveau du mixeur.
        for (unsigned int i = 0; i < numSamples * sound.numChannels; ++i) {
            buffer[i] *= defaultVolume;
            // La gestion du pitch est plus complexe et n'est pas incluse dans cette simulation simplifiée
            // car elle implique du resampling ou des algorithmes DSP.
        }
    }

    // Réinitialise la position de lecture du son de l'instrument
    void resetPlayback() {
        sound.resetPlayback();
    }

    void genTone(WaveType soundType = SINE_WAVE, float freq = 440.0f, unsigned int numFrames = 44100, float amplitude = 1.0f) {
        // Le paramètre 'amplitude' est maintenant aussi dans genTone pour passer à squareWave, sineWave, whiteNoiseWave
        // Pour COMBINED_SINE_NOISE_WAVE, nous utiliserons des ratios internes ou ajouterons d'autres paramètres si nécessaire.
        switch (soundType) {
            case SINE_WAVE:
                sound.sineWave(freq, amplitude, numFrames); // Passe l'amplitude
                name = "Sine Wave " + std::to_string(static_cast<int>(freq)) + "Hz";
                break;
            case SQUARE_WAVE:
                sound.squareWave(freq, amplitude, numFrames); // Passe l'amplitude
                name = "Square Wave " + std::to_string(static_cast<int>(freq)) + "Hz";
                break;
            case WHITE_NOISE_WAVE:
                sound.whiteNoiseWave(amplitude, numFrames); // Passe l'amplitude
                name = "White Noise";
                break;
            case COMBINED_SINE_NOISE_WAVE: // <-- NOUVEAU CASE
                // Ici, vous passez les paramètres spécifiques à combinedSineNoise.
                // Pour l'exemple, j'utilise un ratio 0.7 pour sine et 0.3 pour noise,
                // et la fréquence passée est celle de la composante sinusoïdale.
                // Vous pourriez ajouter des paramètres à genTone pour contrôler ces ratios si besoin.
                sound.combinedSineNoise(freq, 0.7f, 0.3f, numFrames);
                name = "Sine+Noise " + std::to_string(static_cast<int>(freq)) + "Hz";
                break;
            default:
                std::cerr << "Type d'onde inconnu. Génération d'une onde sinusoïdale par défaut." << std::endl;
                sound.sineWave(freq, amplitude, numFrames);
                name = "Default Sine Wave " + std::to_string(static_cast<int>(freq)) + "Hz";
                break;
        }
    }

};

#endif // ADIKINSTRUMENT_H
