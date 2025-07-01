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

    AdikSound(const std::string& soundType) : currentSamplePosition(0) {
        // Simple simulation : générer une petite onde sinusoïdale ou une impulsion
        // pour simuler un son chargé.
        if (soundType.find("kick") != std::string::npos) {
            // Simuler une grosse caisse: onde sinusoïdale déclinante
            audioData.resize(44100 / 4); // 0.25 seconde de son
            for (size_t i = 0; i < audioData.size(); ++i) {
                float phase = 2.0f * PI * 100.0f * i / 44100.0f; // Fréquence de base
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = sin(phase) * decay * MAX_AMPLITUDE;
            }
        } else if (soundType.find("snare") != std::string::npos) {
            // Simuler une caisse claire: bruit blanc avec un peu de sinusoïde
            audioData.resize(44100 / 8); // 0.125 seconde
            for (size_t i = 0; i < audioData.size(); ++i) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f; // Bruit blanc
                float tone = sin(2.0f * PI * 400.0f * i / 44100.0f) * 0.3f; // Tonalité
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = (noise * 0.7f + tone * 0.3f) * decay * MAX_AMPLITUDE;
            }
        } else if (soundType.find("hihat") != std::string::npos || soundType.find("clap") != std::string::npos) {
            // Simuler un charley ou clap: bruit blanc court
            audioData.resize(44100 / 16); // 0.0625 seconde
            for (size_t i = 0; i < audioData.size(); ++i) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = noise * decay * MAX_AMPLITUDE;
            }
        } else {
            // Son par défaut pour les autres types
            audioData.resize(44100 / 10);
            for (size_t i = 0; i < audioData.size(); ++i) {
                audioData[i] = sin(2.0f * PI * 220.0f * i / 44100.0f) * MAX_AMPLITUDE * 0.5f;
            }
        }
        // std::cout << "AdikSound pour '" << soundType << "' créé avec " << audioData.size() << " samples." << std::endl;
    }

    // Lit les données audio dans le buffer de sortie
    void readData(float* outputBuffer, int numSamples, float finalVelocity, float finalPan, float finalPitch) {
        if (audioData.empty() || currentSamplePosition >= audioData.size()) {
            std::fill(outputBuffer, outputBuffer + numSamples, 0.0f); // Remplir de zéros si le son est terminé
            return;
        }

        // Simplification du pitch shift pour la simulation :
        // Un pitch shift réel serait plus complexe (resampling).
        // Pour cette démo, on ignore finalPitch pour ne pas trop compliquer.
        // On se concentre sur le volume et le pan.

        for (int i = 0; i < numSamples; ++i) {
            if (currentSamplePosition < audioData.size()) {
                float sample = audioData[currentSamplePosition];

                // Application du volume (velocity)
                sample *= finalVelocity;

                // Application du panoramique (très simplifié)
                // Pan de -1.0 (gauche) à +1.0 (droite)
                float leftGain = 1.0f - (finalPan > 0 ? finalPan : 0);
                float rightGain = 1.0f + (finalPan < 0 ? finalPan : 0);

                // Assuming a stereo buffer (interleaved LRLR)
                // outputBuffer[i*2] = left channel
                // outputBuffer[i*2+1] = right channel
                // For simplicity, we just add to the mono output for now
                // If it's a stereo buffer, you need to adapt.
                outputBuffer[i] += sample; // Simple addition mono. Adaptez pour le stéréo.

                currentSamplePosition++;
            } else {
                outputBuffer[i] += 0.0f; // Plus de données, ajouter zéro
            }
        }
    }

    // Réinitialise la position de lecture du son
    void resetPlayback() {
        currentSamplePosition = 0;
    }
};

#endif // ADIKSOUND_H
