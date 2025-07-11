#include "audioengine.h"
#include "adikplayer.h"
#include <iostream>      // Pour std::cout, std::cerr
#include <algorithm>     // Pour std::fill


// Votre fonction processAudioCallback existante, qui sera appelée par le wrapper.
// Elle doit toujours être non-static pour être liée globalement.
void processAudioCallback(float* outputBuffer, unsigned int numSamples, void* userData) {
    // Si votre simulateRealtimePlayback passait un int pour numSamples,
    // vous devrez ajuster sa déclaration ou faire un cast ici.
    // Ou mieux, uniformiser sur unsigned int. Je l'ai mis à unsigned int ici.

    // Caster userData en AdikPlayer*
    AdikPlayer* playerData = static_cast<AdikPlayer*>(userData);

    // std::cout << "\nprocessAudioCallback après caster playerData \n";
    if (!playerData) {
        std::fill(outputBuffer, outputBuffer + (numSamples * 2), 0.0f); // *2 pour stéréo
        return;
    }

    // Récupérer la séquence en cours de lecture
    std::shared_ptr<AdikSequence> currentPlayingSequence = nullptr;
    if (playerData->currentMode == AdikPlayer::SEQUENCE_MODE) {
        if (playerData->selectedSequenceInPlayerIndex >= 0 && playerData->selectedSequenceInPlayerIndex < playerData->sequenceList.size()) {
            currentPlayingSequence = playerData->sequenceList[playerData->selectedSequenceInPlayerIndex];
        }
    } else { // SONG_MODE
        if (playerData->currentSong && playerData->currentSequenceIndexInSong >= 0 && playerData->currentSequenceIndexInSong < playerData->currentSong->sequences.size()) {
            currentPlayingSequence = playerData->currentSong->sequences[playerData->currentSequenceIndexInSong];
        }
    }

    if (!currentPlayingSequence) {
        std::fill(outputBuffer, outputBuffer + (numSamples * 2), 0.0f); // *2 pour stéréo
        return;
    }

    // Préparer un buffer mono temporaire si votre mixeur produit du mono
    // et que le buffer de sortie de RtAudio est stéréo.
    // Si votre mixeur gère directement le stéréo, vous n'avez pas besoin de ça.
    // Supposons que votre mixeur sort du mono pour l'instant et qu'on doit le copier en stéréo.
    std::vector<float> mixedBuffer(numSamples * 2, 0.0f);
    // std::fill(monoBuffer.begin(), monoBuffer.end(), 0.0f);
    // std::cout << "processAudioCallback, préparer le monoBuffer:  \n";

    // Boucle pour remplir le buffer audio sample par sample
    // Note: Cette boucle est la logique d'avancement du séquenceur,
    // et ne doit être exécutée qu'une seule fois par appel de callback RtAudio.
    // Le `playerData->currentSampleInStep` doit être mis à jour correctement.
    // Votre logique actuelle est par sample, mais RtAudio donne des blocs.
    // Nous devons adapter la logique du Player pour qu'elle corresponde aux blocs.

    // Pour un callback orienté bloc (comme RtAudio) :
    // Avancez le séquenceur sur la base du nombre total de samples traités.
    for (unsigned int i = 0; i < numSamples; ++i) {
        if (playerData->isPlaying && currentPlayingSequence) {
            playerData->currentSampleInStep++;
            if (playerData->currentSampleInStep >= playerData->samplesPerStep) {
                playerData->advanceStep(currentPlayingSequence);
                playerData->currentSampleInStep = 0;
              // std::cout << "\a";  
            }
        }
    }

    // std::cout << "\a";
    // Demander au mixeur de mixer tous les canaux pour ce buffer (monoBuffer)
    playerData->mixer.mixChannels(mixedBuffer, numSamples);

    // Copier le buffer mono vers le buffer de sortie stéréo de RtAudio
    // Si votre mixeur produit déjà du stéréo, supprimez cette section et
    // modifiez mixChannels pour écrire directement dans 'out'.
    for (unsigned int i = 0; i < numSamples; ++i) {
        outputBuffer[i * 2] = mixedBuffer[i*2];     // Canal gauche
        outputBuffer[i * 2 + 1] = mixedBuffer[i*2 +1]; // Canal droit
    }
}

