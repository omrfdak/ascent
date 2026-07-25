/*!
 * The one button style the game uses; disabled state included, because most of
 * this UI spends its time being unavailable on purpose.
 */

import QtQuick

Rectangle {
  id: root

  property alias text: label.text

  signal clicked

  implicitWidth: label.implicitWidth + 28
  implicitHeight: 40
  radius: 8
  color: {
    if (!root.enabled)
      return Qt.rgba(0.13, 0.16, 0.26, 1)

    return tap.pressed ? Qt.rgba(0.28, 0.62, 0.45, 1) : Qt.rgba(0.22, 0.75, 0.5, 1)
  }

  Text {
    id: label

    anchors.centerIn: parent
    color: root.enabled ? Qt.rgba(0.03, 0.09, 0.07, 1) : Qt.rgba(0.42, 0.47, 0.6, 1)
    font.pixelSize: 15
    font.bold: true
  }

  TapHandler {
    id: tap

    onTapped: root.clicked()
  }
}
