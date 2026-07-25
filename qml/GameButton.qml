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
      return Style.disabled

    return tap.pressed ? Style.accentPressed : Style.accent
  }

  Text {
    id: label

    anchors.centerIn: parent
    color: root.enabled ? Style.onAccent : Style.onDisabled
    font {
      pixelSize: Style.bodySize
      bold: true
    }
  }

  TapHandler {
    id: tap

    onTapped: root.clicked()
  }
}
