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

  // The card only; the dim and the tap that dismisses it belong to the scene.
  Rectangle {
    id: card

    anchors.centerIn: parent
    width: Math.min(parent.width - 24, 300)
    height: Math.min(parent.height - 40, rows.implicitHeight + 32)
    radius: 10
    color: Style.surface
    border {
      width: 1
      color: Style.hairline
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
        color: Style.text
        font {
          pixelSize: Style.headingSize
          bold: true
        }
      }

      Text {
        id: explanation

        Layout.fillWidth: true
        text: qsTr("The hash was on screen before betting opened. Hash the "
                   + "seed yourself and you get it back.")
        color: Style.textMuted
        wrapMode: Text.WordWrap
        font.pixelSize: Style.captionSize
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
          color: Style.inset

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
                color: Style.text
                font {
                  pixelSize: Style.labelSize
                  bold: true
                }
              }

              Item {
                Layout.fillWidth: true
              }

              Text {
                text: row.holdsUp ? qsTr("checks out") : qsTr("does not match")
                color: row.holdsUp
                  ? Style.climbing
                  : Style.crashed
                font.pixelSize: Style.captionSize
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
              color: Style.textFaint
              wrapMode: Text.WrapAnywhere
              font.pixelSize: Style.fineSize
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
        color: Style.textFaint
        wrapMode: Text.WordWrap
        font.pixelSize: Style.captionSize
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
