/*!
 * Everything the player can do during a round: choose a stake, put it in, and
 * take it out again before the balloon pops.
 */

import QtQuick
import Ascent

Column {
  id: root

  spacing: 10

  Row {
    id: stakeRow

    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 10

    GameButton {
      id: lowerButton

      text: qsTr("−")
      // The limits are enforced by the wallet as well; blocking the button is
      // about not offering a move that would only be refused.
      enabled: _.canChangeStake && _.stake - _.stakeStep >= PlayerWallet.minimumBet
      onClicked: _.stake -= _.stakeStep
    }

    Text {
      id: stakeLabel

      anchors.verticalCenter: parent.verticalCenter
      width: 90
      horizontalAlignment: Text.AlignHCenter
      text: qsTr("%1 pts").arg(_.stake)
      color: Qt.rgba(1, 1, 1, 1)
      font.pixelSize: 18
    }

    GameButton {
      id: raiseButton

      text: qsTr("+")
      enabled: _.canChangeStake && _.stake + _.stakeStep <= _.highestAffordableStake
      onClicked: _.stake += _.stakeStep
    }
  }

  GameButton {
    id: actionButton

    anchors.horizontalCenter: parent.horizontalCenter
    width: 200
    text: _.actionText
    enabled: _.actionEnabled
    onClicked: _.act()
  }

  QtObject {
    id: _

    readonly property int stakeStep: 25

    // Never more than the player has, whatever the table maximum says.
    readonly property int highestAffordableStake: Math.min(PlayerWallet.maximumBet,
                                                           PlayerWallet.balance)

    property int stake: PlayerWallet.minimumBet

    readonly property bool isBettingOpen: Rounds.engine.state === RoundEngine.Betting
    readonly property bool isRoundRunning: Rounds.engine.state === RoundEngine.Running
    readonly property bool hasStakeIn: Rounds.engine.bet > 0

    readonly property bool canChangeStake: _.isBettingOpen && !_.hasStakeIn

    readonly property bool canBet: _.isBettingOpen && !_.hasStakeIn
      && _.stake <= _.highestAffordableStake

    readonly property bool canCashOut: _.isRoundRunning && _.hasStakeIn
      && !Rounds.engine.hasCashedOut

    readonly property bool actionEnabled: _.canBet || _.canCashOut

    readonly property string actionText: {
      if (_.canCashOut)
        return qsTr("Cash out %1 pts").arg(_.cashOutValue.toFixed(2))

      if (_.isRoundRunning)
        return Rounds.engine.hasCashedOut ? qsTr("Cashed out") : qsTr("Watching this one")

      if (_.hasStakeIn)
        return qsTr("%1 pts in").arg(Rounds.engine.bet.toFixed(2))

      return _.stake <= _.highestAffordableStake
        ? qsTr("Bet %1 pts").arg(_.stake)
        : qsTr("Not enough points")
    }

    // What the button pays right now, worked out the same way the wallet will.
    readonly property real cashOutValue: Math.round(Rounds.engine.bet
      * Rounds.engine.multiplier * 100) / 100

    function act(): void {
      if (_.canCashOut) {
        Rounds.requestCashOut()
        return
      }

      Rounds.requestBet(_.stake)
    }
  }

  // A stake the player could afford a moment ago may be out of reach after a
  // lost round, so it follows the balance down.
  Connections {
    target: PlayerWallet

    function onBalanceChanged() {
      _.stake = Math.max(PlayerWallet.minimumBet,
                         Math.min(_.stake, _.highestAffordableStake))
    }
  }
}
