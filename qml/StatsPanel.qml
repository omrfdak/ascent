/*!
 * What the player has done so far, on top of the game rather than beside it.
 *
 * Everything here is read from PlayerStats. Nothing is counted twice in QML,
 * because a statistic worked out in two places is a statistic that will
 * eventually disagree with itself.
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Ascent

Item {
  id: root

  // Swallows taps so the game underneath cannot be played through the panel,
  // and closes when the player taps away from the card.
  MouseArea {
    id: dismissArea

    anchors.fill: parent
    onClicked: root.visible = false
  }

  Rectangle {
    id: backdrop

    anchors.fill: parent
    color: Style.scrim
    z: -1
  }

  Rectangle {
    id: card

    anchors.centerIn: parent
    width: Math.min(parent.width - 40, 280)
    height: rows.implicitHeight + 32
    radius: 10
    color: Style.surface
    border {
      width: 1
      color: Style.hairline
    }

    // Taps on the card are not taps away from it.
    MouseArea {
      id: cardArea

      anchors.fill: parent
    }

    ColumnLayout {
      id: rows

      anchors {
        fill: parent
        margins: 16
      }
      spacing: 8

      Text {
        id: title

        Layout.fillWidth: true
        text: qsTr("Your rounds")
        color: Style.text
        font {
          pixelSize: Style.headingSize
          bold: true
        }
      }

      GridLayout {
        id: figures

        Layout.fillWidth: true
        columns: 2
        rowSpacing: 6
        columnSpacing: 12

        Repeater {
          model: _.figures

          Text {
            id: entry

            required property string modelData
            required property int index

            // Even entries are labels in the left column, odd ones are their
            // values in the right.
            readonly property bool isValue: entry.index % 2 === 1

            Layout.fillWidth: entry.isValue
            horizontalAlignment: entry.isValue
              ? Text.AlignRight
              : Text.AlignLeft
            text: entry.modelData
            color: entry.isValue
              ? Style.text
              : Style.textMuted
            font.pixelSize: Style.labelSize
          }
        }
      }

      GameButton {
        id: closeButton

        Layout.fillWidth: true
        Layout.topMargin: 6
        text: qsTr("Back to the game")
        onClicked: root.visible = false
      }
    }
  }

  QtObject {
    id: _

    // Label and value alternating, which is the order the grid reads them in.
    readonly property list<string> figures: [
      qsTr("Rounds played"),
      PlayerStats.roundsPlayed.toString(),
      qsTr("Cashed out in time"),
      qsTr("%1 (%2%)").arg(PlayerStats.roundsWon).arg(_.winRate.toFixed(0)),
      qsTr("Best multiplier"),
      _.bestMultiplierText,
      qsTr("Current streak"),
      PlayerStats.currentStreak.toString(),
      qsTr("Best streak"),
      PlayerStats.bestStreak.toString(),
      qsTr("Points staked"),
      PlayerStats.totalWagered.toFixed(2),
      qsTr("Points returned"),
      PlayerStats.totalReturned.toFixed(2),
      qsTr("Net"),
      _.netText
    ]

    readonly property real winRate: PlayerStats.roundsPlayed > 0
      ? 100 * PlayerStats.roundsWon / PlayerStats.roundsPlayed
      : 0

    // A player who has not finished a round has no best multiplier, and showing
    // 0.00x for that would read as a round that paid nothing.
    readonly property string bestMultiplierText: PlayerStats.bestMultiplier > 0
      ? qsTr("%1x").arg(PlayerStats.bestMultiplier.toFixed(2))
      : qsTr("—")

    readonly property real net: PlayerStats.totalReturned
      - PlayerStats.totalWagered

    readonly property string netText: _.net >= 0
      ? qsTr("+%1").arg(_.net.toFixed(2))
      : qsTr("−%1").arg(Math.abs(_.net).toFixed(2))
  }
}
