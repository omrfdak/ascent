/*!
 * Temporary panel that reads the C++ core from QML, so the bridge between the
 * two can be seen on screen before there is a game to look at.
 */

import QtQuick
import Ascent

Column {
  id: root

  spacing: 8

  CrashCurve {
    id: curve
  }

  Wallet {
    id: wallet
  }

  ProvablyFair {
    id: fair
  }

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

  QtObject {
    id: _

    readonly property real previewCrashPoint: fair.crashPointFor(fair.generateSeed())
  }
}
