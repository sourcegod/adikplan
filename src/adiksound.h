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

    AdikSound(const std::string& soundType, unsigned int channels = 1) // <--- MODIFIÉ : Ajout du paramètre channels
        : currentSamplePosition(0), numChannels(channels) {
        // Simple simulation : générer une petite onde sinusoïdale ou une impulsion.
        // La génération de données est simplifiée pour ne pas dupliquer des samples stéréo ici.
        // On suppose que les données générées sont mono pour cet exemple.
        if (soundType.find("kick") != std::string::npos) {
            audioData.resize(44100 / 4);
            for (size_t i = 0; i < audioData.size(); ++i) {
                float phase = 2.0f * PI * 100.0f * i / 44100.0f;
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = sin(phase) * decay * MAX_AMPLITUDE;
            }
        } else if (soundType.find("snare") != std::string::npos) {
            audioData.resize(44100 / 8);
            for (size_t i = 0; i < audioData.size(); ++i) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
                float tone = sin(2.0f * PI * 400.0f * i / 44100.0f) * 0.3f;
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = (noise * 0.7f + tone * 0.3f) * decay * MAX_AMPLITUDE;
            }
        } else if (soundType.find("hihat") != std::string::npos || soundType.find("clap") != std::string::npos) {
            audioData.resize(44100 / 16);
            for (size_t i = 0; i < audioData.size(); ++i) {
                float noise = (float)rand() / RAND_MAX * 2.0f - 1.0f;
                float decay = 1.0f - (float)i / audioData.size();
                audioData[i] = noise * decay * MAX_AMPLITUDE;
            }
        } else {
            audioData.resize(44100 / 10);
            for (size_t i = 0; i < audioData.size(); ++i) {
                audioData[i] = sin(2.0f * PI * 220.0f * i / 44100.0f) * MAX_AMPLITUDE * 0.5f;
            }
        }
        // Pour la simulation, nous allons supposer que les sons générés sont mono
        // Si vous chargiez des fichiers réels, vous définiriez numChannels en fonction du fichier.
        this->numChannels = channels; // Fixe le nombre de canaux
    }

    bool isMono() const {
        return numChannels == 1;
    }

// /*
  // Lit les données audio dans le buffer de sortie
  unsigned int readData(std::vector<float>& outputBuffer, unsigned int numFrames) {
    // outputBuffer.assign(numFrames * numChannels, 0.0f);  
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
// */

    // Réinitialise la position de lecture du son
    void resetPlayback() {
        currentSamplePosition = 0;
    }
};

#endif // ADIKSOUND_H
