/*!
 * Application entry point; owns the game window and the scene the game is
 * played in.
 */

import Felgo
import QtQuick

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
    Rectangle {
      id: background

      anchors.fill: scene.fullWindowAnchorItem
      color: Qt.rgba(0.043, 0.063, 0.149, 1)
    }

    CorePreview {
      id: corePreview

      anchors.centerIn: parent
    }
  }
}
//! [game-window]
