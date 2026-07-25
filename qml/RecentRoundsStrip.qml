/*!
 * The multipliers of the last few rounds, newest first. It is the one place in
 * the game that says what has been happening, which is what a player leans on
 * when deciding whether to sit the next round out.
 */

pragma ComponentBehavior: Bound

import QtQuick
import Ascent

Row {
  id: root

  spacing: 4

  Repeater {
    // Only the last few, even though more are kept. Ten chips are wider than the
    // scene, and a strip that pushes the screen sideways is worse than a short
    // one: this is a glance, and the full record is in "Check a round".
    model: RecentRounds.rounds.slice(0, 4)

    Rectangle {
      id: chip

      // One recorded round: multiplier, commitment and revealed seed. It stays
      // variant because it crosses from C++ as a plain map.
      required property var modelData

      width: label.implicitWidth + 12
      height: 20
      radius: 4
      color: Style.inset

      Text {
        id: label

        anchors.centerIn: parent
        text: qsTr("%1x").arg(chip.modelData.multiplier.toFixed(2))

        // Coloured by how the round went, not by whether the player was in it:
        // the strip is the game's record, and a good round missed is still a
        // good round to know about.
        color: chip.modelData.multiplier >= 2
          ? Style.climbing
          : Style.crashedQuiet
        font.pixelSize: Style.captionSize
      }
    }
  }
}
