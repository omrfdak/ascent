/*!
 * Application entry point; owns the game window and the scene the game is
 * played in.
 */

import Felgo
import QtQuick
import Ascent

//! [game-window]
GameWindow {
  id: root

  // Desktop window size. On mobile the scene scales to the device screen instead.
  screenWidth: 480
  screenHeight: 720

  // Portrait: the balloon rises, so vertical space is the playfield.
  Scene {
    id: scene

    width: 320
    height: 480

    // Fills the whole window, not just the scene, so no letterbox bars show
    // through on a wider display.
    SkyBackground {
      id: background

      anchors.fill: scene.fullWindowAnchorItem
    }

    GameScene {
      id: gameScene

      anchors.fill: parent
    }

    // Above everything, and over the full window rather than the scene: the
    // flash belongs to the moment, not to the playfield.
    CrashFlash {
      id: crashFlash

      anchors.fill: scene.fullWindowAnchorItem
    }
  }

  GameAudio {
    id: gameAudio
  }

  SavedGame {
    id: savedGame
  }

  Connections {
    target: Rounds

    function onRoundCrashed() {
      crashFlash.start()
    }
  }

  //! [splash]
  // Not Component.onCompleted: on a free license the splash screen sits on top
  // for a few seconds, and rounds cycling behind it would mean the player walks
  // in on a bet they never had the chance to place.
  onSplashScreenFinished: Rounds.start()
  //! [splash]
}
//! [game-window]
