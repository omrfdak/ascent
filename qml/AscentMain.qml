/*!
 * Application entry point; owns the game window and the scene the game is
 * played in.
 */

import Felgo
import QtQuick
import Ascent

GameWindow {
  id: root

  // Desktop window size. On mobile the scene scales to the device screen instead.
  screenWidth: 480
  screenHeight: 720

  // Portrait: the balloon rises, so vertical space is the playfield.
  Scene {
    id: scene

    width: 320
    height: 480

    // Fills the whole window, not just the scene, so no letterbox bars show
    // through on a wider display.
    Rectangle {
      id: background

      anchors.fill: scene.fullWindowAnchorItem
      color: Qt.rgba(0.043, 0.063, 0.149, 1)
    }

    // The game logic, straight from C++. None of these draw anything.
    CrashCurve {
      id: curve
    }

    Wallet {
      id: wallet
    }

    ProvablyFair {
      id: fair
    }

    Column {
      anchors.centerIn: parent
      spacing: 8

      Text {
        id: titleLabel

        text: qsTr("Ascent")
        color: Qt.rgba(1, 1, 1, 1)
        font.pixelSize: 32
      }

      Text {
        id: curveLabel

        text: qsTr("%1x after 10s").arg(curve.multiplierAt(10000).toFixed(2))
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        font.pixelSize: 14
      }

      Text {
        id: walletLabel

        text: qsTr("%1 points, bets %2-%3")
          .arg(wallet.balance)
          .arg(wallet.minimumBet)
          .arg(wallet.maximumBet)
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        font.pixelSize: 14
      }

      Text {
        id: crashPointLabel

        text: qsTr("next round pops at %1x").arg(_.previewCrashPoint.toFixed(2))
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        font.pixelSize: 14
      }
    }

    QtObject {
      id: _

      readonly property real previewCrashPoint: fair.crashPointFor(fair.generateSeed())
    }
  }
}
