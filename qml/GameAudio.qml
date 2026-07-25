/*!
 * Every sound the game makes. It listens to the round source itself, so no
 * other file has to remember to make a noise.
 */

import Felgo
import QtQuick
import Ascent

Item {
  id: root

  //! [background-music]
  // Eight seconds that close on themselves: every partial in the loop lasts a
  // whole number of cycles, so the wrap is silent rather than almost silent.
  // It stays a wav on purpose - an mp3 would arrive with the encoder's own
  // padding at both ends, which is exactly the gap the loop is avoiding.
  BackgroundMusic {
    id: music

    source: Qt.resolvedUrl("../assets/snd/ascent-loop.wav")
    volume: 0.3
  }
  //! [background-music]

  // One instance per sound, declared once and replayed. Loading a sound at the
  // moment it is needed is how a game ends up with a pop that arrives after the
  // balloon has already gone.
  GameSoundEffect {
    id: popSound

    source: Qt.resolvedUrl("../assets/snd/pop.wav")
  }

  GameSoundEffect {
    id: liftoffSound

    source: Qt.resolvedUrl("../assets/snd/liftoff.wav")
    volume: 0.6
  }

  GameSoundEffect {
    id: cashOutSound

    source: Qt.resolvedUrl("../assets/snd/cashout.wav")
  }

  GameSoundEffect {
    id: betSound

    source: Qt.resolvedUrl("../assets/snd/bet.wav")
    volume: 0.7
  }

  GameSoundEffect {
    id: rejectedSound

    source: Qt.resolvedUrl("../assets/snd/rejected.wav")
    volume: 0.7
  }

  Connections {
    target: Rounds

    function onRoundStarted() {
      liftoffSound.play()
    }

    function onRoundCrashed() {
      popSound.play()
    }

    function onBetAccepted() {
      betSound.play()
    }

    function onBetRejected() {
      rejectedSound.play()
    }

    function onCashOutConfirmed() {
      cashOutSound.play()
    }

    function onCashOutRejected() {
      rejectedSound.play()
    }
  }
}
