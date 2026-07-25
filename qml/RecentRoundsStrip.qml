/*!
 * The multipliers of the last few rounds, newest first. It is the one place in
 * the game that says what has been happening, which is what a player leans on
 * when deciding whether to sit the next round out.
 */

import QtQuick
import Ascent

Row {
  id: root

  spacing: 4

  Repeater {
    model: RecentRounds.rounds

    Rectangle {
      id: chip

      required property var modelData

      width: label.implicitWidth + 12
      height: 20
      radius: 4
      color: Qt.rgba(1, 1, 1, 0.06)

      Text {
        id: label

        anchors.centerIn: parent
        text: qsTr("%1x").arg(chip.modelData.multiplier.toFixed(2))

        // Coloured by how the round went, not by whether the player was in it:
        // the strip is the game's record, and a good round missed is still a
        // good round to know about.
        color: chip.modelData.multiplier >= 2
          ? Qt.rgba(0.55, 0.95, 0.65, 1)
          : Qt.rgba(0.85, 0.55, 0.55, 1)
        font.pixelSize: 12
      }
    }
  }
}
