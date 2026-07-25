/*!
 * The scene the game opens on: the title, the way into a round, and the two
 * screens that are about rounds already played.
 */

import QtQuick
import QtQuick.Layouts
import Ascent

SceneBase {
  id: root

  signal playRequested

  ColumnLayout {
    id: content

    anchors.centerIn: parent
    width: 200
    spacing: 12

    Text {
      id: title

      Layout.alignment: Qt.AlignHCenter
      text: qsTr("Ascent")
      color: Qt.rgba(1, 1, 1, 1)
      font {
        pixelSize: 48
        bold: true
      }
    }

    Text {
      id: tagline

      Layout.fillWidth: true
      Layout.bottomMargin: 16
      horizontalAlignment: Text.AlignHCenter
      text: qsTr("Cash out before it pops")
      color: Qt.rgba(0.561, 0.639, 0.816, 1)
      font.pixelSize: 14
    }

    GameButton {
      id: playButton

      Layout.fillWidth: true
      text: qsTr("Play")
      onClicked: root.playRequested()
    }

    GameButton {
      id: statsButton

      Layout.fillWidth: true
      text: qsTr("Your rounds")
      onClicked: statsPanel.visible = true
    }

    GameButton {
      id: verifyButton

      Layout.fillWidth: true
      text: qsTr("Check a round")
      onClicked: verifyPanel.visible = true
    }

    GameButton {
      id: soundButton

      Layout.fillWidth: true
      text: settings.soundEnabled ? qsTr("Sound on") : qsTr("Sound off")
      onClicked: {
        settings.soundEnabled = !settings.soundEnabled
        settings.musicEnabled = settings.soundEnabled
      }
    }

    Text {
      id: balanceLabel

      Layout.fillWidth: true
      Layout.topMargin: 16
      horizontalAlignment: Text.AlignHCenter
      text: qsTr("%1 pts").arg(PlayerWallet.balance.toFixed(2))
      color: Qt.rgba(0.561, 0.639, 0.816, 1)
      font.pixelSize: 14
    }
  }

  StatsPanel {
    id: statsPanel

    anchors.fill: parent
    visible: false
  }

  VerifyPanel {
    id: verifyPanel

    anchors.fill: parent
    visible: false
  }
}
