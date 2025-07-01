#include "audioengine.h"
#include "adikplayer.h"

// --- Nouvelle fonction de rappel audio globale et statique ---
// 1. Rendre la fonction processAudioCallback indépendante et statique
// Elle prend un pointeur void* userData en paramètre, qui sera casté en AdikPlayer.
void processAudioCallback(float* outputBuffer, int numSamples, void* userData) {
    // Caster userData en AdikPlayer*
    AdikPlayer* playerData = static_cast<AdikPlayer*>(userData);

    if (!playerData || !playerData->isPlaying) {
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f); // Remplir de silence si pas en lecture
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
        std::fill(outputBuffer, outputBuffer + numSamples, 0.0f);
        return;
    }

    // Boucle pour remplir le buffer audio sample par sample
    for (int i = 0; i < numSamples; ++i) {
        // Vérifier si nous devons déclencher un nouvel événement (passer à un nouveau pas)
        if (playerData->currentSampleInStep >= playerData->samplesPerStep) {
            // 2. La fonction processAudioCallback appellera playerData->advanceStep
            playerData->advanceStep(currentPlayingSequence); // Passer la séquence au besoin
            playerData->currentSampleInStep = 0; // Réinitialiser le compteur de samples pour le nouveau pas
        }
        playerData->currentSampleInStep++; // Avancer le sample dans le pas actuel
    }

    // Demander au mixeur de mixer tous les canaux pour ce buffer
    playerData->mixer.mixChannels(outputBuffer, numSamples);
}


