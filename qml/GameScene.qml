/*!
 * The playfield: shows the multiplier of the round in progress, and what the
 * round is currently doing.
 */

import QtQuick
import Ascent

Item {
  id: root

  Balloon {
    id: balloon

    x: (root.width - width) / 2
    y: _.groundY - _.riseProgress * _.travel
    visible: Rounds.engine.state !== RoundEngine.Crashed
    riseProgress: _.riseProgress
  }

  Column {
    id: readout

    anchors.centerIn: parent
    spacing: 12

    Text {
      id: multiplierLabel

      anchors.horizontalCenter: parent.horizontalCenter
      text: qsTr("%1x").arg(Rounds.engine.multiplier.toFixed(2))
      color: _.multiplierColor
      font.pixelSize: 64
    }

    Text {
      id: stateLabel

      anchors.horizontalCenter: parent.horizontalCenter
      text: _.stateText
      color: Qt.rgba(0.561, 0.639, 0.816, 1)
      font.pixelSize: 16
    }
  }

  Text {
    id: balanceLabel

    anchors {
      top: parent.top
      left: parent.left
      margins: 12
    }
    text: qsTr("%1 pts").arg(PlayerWallet.balance)
    color: Qt.rgba(0.561, 0.639, 0.816, 1)
    font.pixelSize: 14
  }

  BettingPanel {
    id: bettingPanel

    anchors {
      bottom: parent.bottom
      horizontalCenter: parent.horizontalCenter
      bottomMargin: 36
    }
  }

  // The proof, on screen before anyone can bet on the round it decides. It is
  // shortened because nobody reads 64 hex characters; the full seed comes out
  // when the round ends.
  Text {
    id: commitmentLabel

    anchors {
      bottom: parent.bottom
      horizontalCenter: parent.horizontalCenter
      margins: 12
    }
    text: qsTr("round hash %1…").arg(Rounds.commitment.substring(0, 12))
    color: Qt.rgba(0.35, 0.4, 0.55, 1)
    font.pixelSize: 12
  }

  QtObject {
    id: _

    readonly property real groundY: root.height - balloon.height - 24
    readonly property real travel: _.groundY - 24

    // The multiplier has no ceiling but the scene does. Reading the height off
    // the logarithm means every scale gets its own stretch of sky: 2x is a third
    // of the way up, 10x two thirds, 1000x still climbing. A balloon pinned to
    // the top while the number keeps running would be a dead screen at exactly
    // the moment the round is worth watching.
    readonly property real riseProgress: 1 - 1 / (1 + 0.9 * Math.log(Rounds.engine.multiplier))

    readonly property color climbingColor: Qt.rgba(0.55, 0.95, 0.65, 1)
    readonly property color crashedColor: Qt.rgba(0.95, 0.35, 0.4, 1)
    readonly property color restingColor: Qt.rgba(1, 1, 1, 1)

    readonly property color multiplierColor: {
      if (Rounds.engine.state === RoundEngine.Running)
        return _.climbingColor

      return Rounds.engine.state === RoundEngine.Betting
        ? _.restingColor
        : _.crashedColor
    }

    readonly property string stateText: {
      switch (Rounds.engine.state) {
      case RoundEngine.Betting:
        return qsTr("next round starting")
      case RoundEngine.Running:
        return qsTr("cash out before it pops")
      default:
        return qsTr("popped")
      }
    }
  }
}
