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
    AdikInstrument(const std::string& id_val, const std::string& name_val, const std::string& path_val)
        : id(id_val), name(name_val), audioFilePath(path_val),
          defaultVolume(1.0f), defaultPan(0.0f), defaultPitch(0.0f),
          sound(id_val) // Initialise l'objet AdikSound avec l'ID de l'instrument (simule le chargement)
    {
        std::cout << "Instrument '" << name << "' (" << id << ") créé." << std::endl;
    }

    // Rend le son de l'instrument dans un buffer
    void render(float* buffer, int numSamples, float finalVelocity, float finalPan, float finalPitch) {
        sound.readData(buffer, numSamples, finalVelocity, finalPan, finalPitch);
    }

    // Réinitialise la position de lecture du son de l'instrument
    void resetPlayback() {
        sound.resetPlayback();
    }
};

#endif // ADIKINSTRUMENT_H
