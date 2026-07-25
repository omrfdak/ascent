/*!
 * The white-out at the moment of the pop. It covers the whole window rather
 * than the scene, so the flash has no visible edge on a wide display.
 */

import QtQuick

Rectangle {
  id: root

  color: Qt.rgba(1, 0.86, 0.82, 1)
  opacity: 0

  function start() {
    flash.restart()
  }

  // Up within a frame and down over a fifth of a second. Anything longer reads
  // as a screen that broke rather than a balloon that popped.
  SequentialAnimation {
    id: flash

    NumberAnimation {
      target: root
      property: "opacity"
      to: 0.5
      duration: 30
    }

    NumberAnimation {
      target: root
      property: "opacity"
      to: 0
      duration: 220
      easing.type: Easing.OutQuad
    }
  }
}
