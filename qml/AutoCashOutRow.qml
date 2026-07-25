/*!
 * The standing instruction: take the money at this multiplier, whether or not
 * anybody is watching the screen when it gets there.
 */

import QtQuick
import Ascent

Row {
  id: root

  spacing: 10

  GameButton {
    id: lowerButton

    text: qsTr("−")
    enabled: Rounds.engine.autoCashOutAt >= _.lowest + _.step
    onClicked: Rounds.engine.autoCashOutAt = Rounds.engine.autoCashOutAt - _.step
  }

  Text {
    id: targetLabel

    anchors.verticalCenter: parent.verticalCenter
    width: 110
    horizontalAlignment: Text.AlignHCenter

    // Off is a state worth naming. A row showing "1.00x" would read as a
    // setting that cashes out instantly.
    text: _.isOn
      ? qsTr("auto %1x").arg(Rounds.engine.autoCashOutAt.toFixed(2))
      : qsTr("auto off")
    color: _.isOn ? Qt.rgba(1, 1, 1, 1) : Qt.rgba(0.45, 0.5, 0.62, 1)
    font.pixelSize: 14
  }

  GameButton {
    id: raiseButton

    text: qsTr("+")
    enabled: Rounds.engine.autoCashOutAt < _.highest
    onClicked: Rounds.engine.autoCashOutAt = Math.max(_.lowest,
                                                      Rounds.engine.autoCashOutAt) + _.step
  }

  QtObject {
    id: _

    readonly property real step: 0.25

    // The lowest target worth offering. Below this the payout barely covers the
    // stake, and the engine treats 1.00x as off anyway.
    readonly property real lowest: 1.25

    readonly property real highest: 50

    readonly property bool isOn: Rounds.engine.autoCashOutAt > 0
  }
}
