import Felgo 4.0
import QtQuick 2.0

GameWindow {
  id: gameWindow

  // Desktop window size. On mobile the scene scales to the device screen instead.
  screenWidth: 480
  screenHeight: 720

  // Portrait: the balloon rises, so vertical space is the playfield.
  Scene {
    id: scene

    width: 320
    height: 480

    // Fills the whole window, not just the scene, so no letterbox bars show through.
    Rectangle {
      anchors.fill: scene.fullWindowAnchorItem
      color: "#0b1026"
    }

    Text {
      anchors.centerIn: parent
      text: "Ascent"
      color: "white"
      font.pixelSize: 32
    }
  }
}
