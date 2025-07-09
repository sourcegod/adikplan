#ifndef ADIKSOUND_H
#define ADIKSOUND_H

#include <vector>
#include <string>
#include <iostream>
#include <numeric> // Pour std::iota
#include <cmath>
#include <algorithm> // Pour std::fill

// Constantes pour la simulation
const float PI = 3.14159265358979323846f;
const float MAX_AMPLITUDE = 0.8f;

class AdikSound {
public:
    std::vector<float> audioData;
    size_t currentSamplePosition; // Utiliser size_t pour la taille/position
    unsigned int numChannels;     // <--- NOUVEAU : Nombre de canaux du son (1 pour mono, 2 pour stéréo)
    unsigned int sampleRate;

    AdikSound() 
        : currentSamplePosition(0), 
        sampleRate(44100), numChannels(1) {
    }

    AdikSound(const std::string& soundType, unsigned int channels = 1) // <--- MODIFIÉ : Ajout du paramètre channels
        : currentSamplePosition(0), numChannels(channels),
        sampleRate(44100) {
        // Simple simulation : générer une petite onde sinusoïdale ou une impulsion.
        // La génération de données est simplifiée pour ne pas dupliquer des samples stéréo ici.
        // On suppose que les données générées sont mono pour cet exemple.
        if (soundType.find("kick") != std::string::npos) {
            audioData.resize(44100 / 4 * numChannels); // Ajuster la taille pour le nombre de canaux
            for (size_t i = 0; i < audioData.size(); i += numChannels) {
                float phase = 2.0f * PI * 100.0f * (i / numChannels) / 44100.0f; // Calculer la phase par frame
                float decay = 1.0f - (float)(i / numChannels) / (audioData.size() / numChannels);
                float sample = sin(phase) * decay * MAX_AMPLITUDE;
                for (unsigned int c = 0; c < numChannels; ++c) {
                    audioData[i + c] = sample;
                }
            }
        } else if (soundType.find("snare") != std::string::npos) {
            audioData.resize(44100 / 8 * numChannels);
            for (size_t i = 0; i < audioData.size(); i += numChannels) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
                float tone = sin(2.0f * PI * 400.0f * (i / numChannels) / 44100.0f) * 0.3f;
                float decay = 1.0f - (float)(i / numChannels) / (audioData.size() / numChannels);
                float sample = (noise * 0.7f + tone * 0.3f) * decay * MAX_AMPLITUDE;
                for (unsigned int c = 0; c < numChannels; ++c) {
                    audioData[i + c] = sample;
                }
            }
        } else if (soundType.find("hihat") != std::string::npos || soundType.find("clap") != std::string::npos) {
            audioData.resize(44100 / 16 * numChannels);
            for (size_t i = 0; i < audioData.size(); i += numChannels) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
                float decay = 1.0f - (float)(i / numChannels) / (audioData.size() / numChannels);
                float sample = noise * decay * MAX_AMPLITUDE;
                for (unsigned int c = 0; c < numChannels; ++c) {
                    audioData[i + c] = sample;
                }
            }
        } else {
            audioData.resize(44100 / 10 * numChannels);
            for (size_t i = 0; i < audioData.size(); i += numChannels) {
                float sample = sin(2.0f * PI * 220.0f * (i / numChannels) / 44100.0f) * MAX_AMPLITUDE * 0.5f;
                for (unsigned int c = 0; c < numChannels; ++c) {
                    audioData[i + c] = sample;
                }
            }
        }
        this->numChannels = channels; // Fixe le nombre de canaux
    }



    // Lit les données audio dans le buffer de sortie
    unsigned int readData(std::vector<float>& outputBuffer, unsigned int numFrames) {
      // redimensionne le buffer de sortie pour correspondre au nombre de canaux
      outputBuffer.resize(numFrames * numChannels, 0.0f);  
      if (audioData.empty() || currentSamplePosition >= audioData.size()) {
            // Remplir de zéros si le son est terminé ou vide
            outputBuffer.resize(numFrames * numChannels, 0.0f);
            return 0;
        }

        unsigned int samplesToRead = numFrames * numChannels; // Nombre total de samples (gauche + droite)
        unsigned int actualSamplesRead = 0;

        for (unsigned int i = 0; i < samplesToRead; ++i) {
            if (currentSamplePosition < audioData.size()) {
                outputBuffer[i] = audioData[currentSamplePosition];
                currentSamplePosition++;
                actualSamplesRead++;
            } else {
                outputBuffer[i] = 0.0f; // Plus de données, ajouter zéro
            }
        }
        // Si nous avons lu moins que prévu (fin du son), remplir le reste avec des zéros
        if (actualSamplesRead < samplesToRead) {
            // outputBuffer.resize(actualSamplesRead, samplesToRead, 0.0f);
            // std::fill(outputBuffer.begin() + actualSamplesRead, outputBuffer.begin() + samplesToRead, 0.0f);
        }

        // La fonction retourne le nombre de frames (pas de samples) qui ont été traités.
        return actualSamplesRead / numChannels;
    }

    // Réinitialise la position de lecture du son
    void resetPlayback() {
        currentSamplePosition = 0;
    }

    void sineWave(float freq = 440.0f, unsigned int numFrames = 44100) {
        // Définir le nombre de samples total nécessaire, en tenant compte des canaux
        // numFrames représente le nombre de points temporels, pas le nombre total d'échantillons si stéréo
        size_t totalSamples = numFrames * numChannels;

        // Redimensionner le vecteur audioData pour accueillir les nouvelles données
        audioData.resize(totalSamples);
        
        // Réinitialiser la position de lecture
        currentSamplePosition = 0;

        // Générer l'onde sinusoïdale
        for (size_t i = 0; i < numFrames; ++i) {
            // Calculer la phase pour l'échantillon actuel
            // i est l'indice de la frame (point temporel)
            float time = static_cast<float>(i) / sampleRate;
            float sampleValue = MAX_AMPLITUDE * std::sin(2.0f * PI * freq * time);

            // Remplir les canaux pour cette frame
            for (unsigned int c = 0; c < numChannels; ++c) {
                audioData[i * numChannels + c] = sampleValue;
            }
        }
        std::cout << "Génération d'une onde sinusoïdale : Fréquence = " << freq 
                  << " Hz, Durée = " << (float)numFrames / sampleRate 
                  << " secondes, Canaux = " << numChannels << std::endl;
    }

    void squareWave(float freq = 440.0f, unsigned int numFrames = 44100) {
        size_t totalSamples = numFrames * numChannels;

        // Redimensionner le vecteur audioData pour accueillir les nouvelles données
        audioData.resize(totalSamples);
        
        // Réinitialiser la position de lecture
        currentSamplePosition = 0;

        // Générer l'onde carrée
        for (size_t i = 0; i < numFrames; ++i) {
            float time = static_cast<float>(i) / sampleRate;
            // Calculer la phase modulée entre 0 et 1 (ou 0 et 2*PI, puis modulo)
            float phase = std::fmod(freq * time, 1.0f); 
            
            float sampleValue;
            if (phase < 0.5f) { // Première moitié du cycle
                sampleValue = MAX_AMPLITUDE;
            } else { // Deuxième moitié du cycle
                sampleValue = -MAX_AMPLITUDE;
            }

            // Remplir les canaux pour cette frame
            for (unsigned int c = 0; c < numChannels; ++c) {
                audioData[i * numChannels + c] = sampleValue;
            }
        }
        std::cout << "Génération d'une onde carrée : Fréquence = " << freq
                  << " Hz, Durée = " << (float)numFrames / sampleRate
                  << " secondes, Canaux = " << numChannels << std::endl;
    }


};

#endif // ADIKSOUND_H
