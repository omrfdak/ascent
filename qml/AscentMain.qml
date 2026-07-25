/*!
 * Application entry point; owns the window, the scenes in it, and which one of
 * them the player is looking at.
 */

import Felgo
import QtQuick
import Ascent

//! [game-window]
GameWindow {
  id: root

  // Desktop window size. On mobile the scenes scale to the device screen
  // instead.
  screenWidth: 480
  screenHeight: 720

  MenuScene {
    id: menuScene

    onPlayRequested: root.state = "game"
  }

  GameScene {
    id: gameScene

    onBackRequested: root.state = "menu"
  }

  GameAudio {
    id: gameAudio
  }

  SavedGame {
    id: savedGame
  }

  //! [scene-states]
  // One scene visible at a time, and the window decides which. A scene asks to
  // be left and does not name its successor, so the same scene can be reached
  // from anywhere without knowing what came before it.
  //
  // The game opens on the game: rounds are already cycling, and a title screen
  // between the player and the next one is a door with nothing behind it.
  state: "game"

  states: [
    State {
      name: "menu"
      PropertyChanges { menuScene.opacity: 1 }
      PropertyChanges { root.activeScene: menuScene }
    },
    State {
      name: "game"
      PropertyChanges { gameScene.opacity: 1 }
      PropertyChanges { root.activeScene: gameScene }
    }
  ]
  //! [scene-states]

  //! [splash]
  // Not Component.onCompleted: on a free license the splash screen sits on top
  // for a few seconds, and rounds cycling behind it would mean the player walks
  // in on a bet they never had the chance to place.
  onSplashScreenFinished: Rounds.start()
  //! [splash]
}
//! [game-window]
