/*!
 * Where the player checks the game's word. Every recorded round is derived
 * again from its revealed seed here, in front of them, with the game's own
 * arithmetic.
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Ascent

Item {
  id: root

  MouseArea {
    id: dismissArea

    anchors.fill: parent
    onClicked: root.visible = false
  }

  Rectangle {
    id: backdrop

    anchors.fill: parent
    color: Qt.rgba(0.02, 0.03, 0.08, 0.9)
    z: -1
  }

  Rectangle {
    id: card

    anchors.centerIn: parent
    width: Math.min(parent.width - 24, 300)
    height: Math.min(parent.height - 40, rows.implicitHeight + 32)
    radius: 10
    color: Qt.rgba(0.09, 0.11, 0.2, 1)
    border {
      width: 1
      color: Qt.rgba(1, 1, 1, 0.08)
    }

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
        text: qsTr("Check a round")
        color: Qt.rgba(1, 1, 1, 1)
        font {
          pixelSize: 18
          bold: true
        }
      }

      Text {
        id: explanation

        Layout.fillWidth: true
        text: qsTr("The hash was on screen before betting opened. Hash the "
                   + "seed yourself and you get it back.")
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        wrapMode: Text.WordWrap
        font.pixelSize: 12
      }

      ListView {
        id: roundList

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 120
        clip: true
        spacing: 4
        model: RecentRounds.rounds

        delegate: Rectangle {
          id: row

          // One recorded round: multiplier, commitment and revealed seed.
          required property var modelData
          required property int index

          readonly property bool isOpen: roundList.currentIndex === row.index

          // The verdict is not stored with the round. It is worked out again
          // every time this list is drawn, from the seed and nothing else.
          readonly property bool holdsUp:
            Fairness.verify(row.modelData.seed, row.modelData.commitment,
                            row.modelData.multiplier)

          width: roundList.width
          height: rowContent.implicitHeight + 16
          radius: 6
          color: Qt.rgba(1, 1, 1, 0.05)

          ColumnLayout {
            id: rowContent

            anchors {
              fill: parent
              margins: 8
            }
            spacing: 4

            RowLayout {
              Layout.fillWidth: true
              spacing: 8

              Text {
                text: qsTr("%1x").arg(row.modelData.multiplier.toFixed(2))
                color: Qt.rgba(1, 1, 1, 1)
                font {
                  pixelSize: 14
                  bold: true
                }
              }

              Item {
                Layout.fillWidth: true
              }

              Text {
                text: row.holdsUp ? qsTr("checks out") : qsTr("does not match")
                color: row.holdsUp
                  ? Qt.rgba(0.55, 0.95, 0.65, 1)
                  : Qt.rgba(0.95, 0.35, 0.4, 1)
                font.pixelSize: 12
              }
            }

            // The full strings only when asked for: sixty-four hex characters
            // per line would turn the list into a wall.
            Text {
              Layout.fillWidth: true
              visible: row.isOpen
              text: qsTr("seed %1\nhash %2\nrecomputed %3x")
                .arg(row.modelData.seed)
                .arg(row.modelData.commitment)
                .arg(Fairness.crashPointFor(row.modelData.seed).toFixed(2))
              color: Qt.rgba(0.45, 0.5, 0.62, 1)
              wrapMode: Text.WrapAnywhere
              font.pixelSize: 10
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: roundList.currentIndex = row.isOpen ? -1 : row.index
          }
        }
      }

      Text {
        id: emptyNote

        Layout.fillWidth: true
        visible: RecentRounds.rounds.length === 0
        text: qsTr("Nothing to check yet - the first round is still running.")
        color: Qt.rgba(0.45, 0.5, 0.62, 1)
        wrapMode: Text.WordWrap
        font.pixelSize: 12
      }

      GameButton {
        id: closeButton

        Layout.fillWidth: true
        text: qsTr("Back to the game")
        onClicked: root.visible = false
      }
    }
  }
}
