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
    std::string id;
    std::string name;
    std::string audioFilePath; // Chemin vers le fichier audio (pour l'exemple)
    float defaultVolume;
    float defaultPan;
    float defaultPitch; // Changement de hauteur (pitch shift)

    AdikSound sound; // L'objet AdikSound qui contient les données audio

    // Constructeur
    AdikInstrument(const std::string& id_val, const std::string& name_val,
                   const std::string& path_val, unsigned int channels = 1) // Par défaut mono
        : id(id_val), name(name_val), audioFilePath(path_val),
          defaultVolume(1.0f), defaultPan(0.0f), defaultPitch(0.0f),
          sound(id_val, channels) // <--- Initialise AdikSound avec les canaux
    {
        std::cout << "Instrument '" << name << "' (" << id << ") créé, canaux: " << channels << std::endl;
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
            // buffer[i] *= finalVelocity;
            // La gestion du pitch est plus complexe et n'est pas incluse dans cette simulation simplifiée
            // car elle implique du resampling ou des algorithmes DSP.
        }
    }

    // Réinitialise la position de lecture du son de l'instrument
    void resetPlayback() {
        sound.resetPlayback();
    }
};

#endif // ADIKINSTRUMENT_H
