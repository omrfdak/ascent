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
      color: Style.text
      font {
        pixelSize: Style.titleSize
        bold: true
      }
    }

    Text {
      id: tagline

      Layout.fillWidth: true
      Layout.bottomMargin: 16
      horizontalAlignment: Text.AlignHCenter
      text: qsTr("Cash out before it pops")
      color: Style.textMuted
      font.pixelSize: Style.labelSize
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
      color: Style.textMuted
      font.pixelSize: Style.labelSize
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
