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

  // The dim behind both panels. It is out here rather than inside them because
  // it covers the whole screen and they deliberately do not: the scene is
  // letterboxed on any display that is not its shape, and a dim stopping at the
  // scene edge leaves lit bars of sky around a panel that is meant to be the
  // only thing on screen.
  //
  // A tap anywhere on it closes what is open. Taps on a card land on the card's
  // own handler instead, because the panels are above this.
  Rectangle {
    id: scrim

    anchors.fill: root.fullWindowAnchorItem
    color: Style.scrim
    visible: statsPanel.visible || verifyPanel.visible

    MouseArea {
      id: dismissArea

      anchors.fill: parent
      onClicked: {
        statsPanel.visible = false
        verifyPanel.visible = false
      }
    }
  }

  // Filling the scene and not the window is what keeps the cards clear of the
  // notch and the home indicator: Felgo has already fitted the scene into the
  // part of the screen the device is not using for itself.
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
