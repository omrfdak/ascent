/*!
 * Everything the player can do during a round: choose a stake, choose where to
 * be taken out automatically, put the stake in, and take it out by hand before
 * the balloon pops.
 *
 * The two steppers and the action button share one grid, so the columns line up
 * because they are the same columns - not because two rows were given matching
 * widths and kept in step by hand.
 */

import QtQuick
import QtQuick.Layouts
import Ascent

GridLayout {
  id: root

  columns: 3
  rowSpacing: 8
  columnSpacing: 10

  GameButton {
    id: lowerStakeButton

    Layout.preferredWidth: _.stepperWidth
    text: qsTr("−")
    // The limits are enforced by the wallet as well; blocking the button is
    // about not offering a move that would only be refused.
    enabled: _.canChangeStake
      && _.stake - _.stakeStep >= PlayerWallet.minimumBet
    onClicked: _.stake -= _.stakeStep
  }

  Text {
    id: stakeLabel

    Layout.fillWidth: true
    Layout.minimumWidth: _.labelWidth
    horizontalAlignment: Text.AlignHCenter
    text: qsTr("%1 pts").arg(_.stake)
    color: Style.text
    font.pixelSize: Style.headingSize
  }

  GameButton {
    id: raiseStakeButton

    Layout.preferredWidth: _.stepperWidth
    text: qsTr("+")
    enabled: _.canChangeStake
      && _.stake + _.stakeStep <= _.highestAffordableStake
    onClicked: _.stake += _.stakeStep
  }

  GameButton {
    id: lowerTargetButton

    Layout.preferredWidth: _.stepperWidth
    text: qsTr("−")
    enabled: Rounds.engine.autoCashOutAt >= _.lowestTarget + _.targetStep
    onClicked: Rounds.engine.autoCashOutAt = Rounds.engine.autoCashOutAt
      - _.targetStep
  }

  Text {
    id: targetLabel

    Layout.fillWidth: true
    Layout.minimumWidth: _.labelWidth
    horizontalAlignment: Text.AlignHCenter

    // Off is a state worth naming. A row showing "1.00x" would read as a
    // setting that cashes out instantly.
    text: _.isAutoOn
      ? qsTr("auto %1x").arg(Rounds.engine.autoCashOutAt.toFixed(2))
      : qsTr("auto off")
    color: _.isAutoOn ? Style.text : Style.textFaint
    font.pixelSize: Style.labelSize
  }

  GameButton {
    id: raiseTargetButton

    Layout.preferredWidth: _.stepperWidth
    text: qsTr("+")
    enabled: Rounds.engine.autoCashOutAt < _.highestTarget
    onClicked: Rounds.engine.autoCashOutAt = _.targetStep
      + Math.max(_.lowestTarget, Rounds.engine.autoCashOutAt)
  }

  GameButton {
    id: actionButton

    Layout.columnSpan: 3
    Layout.fillWidth: true
    text: _.actionText
    enabled: _.actionEnabled
    onClicked: _.act()
  }

  QtObject {
    id: _

    readonly property real stepperWidth: 48
    readonly property real labelWidth: 110

    readonly property int stakeStep: 25

    // Never more than the player has, whatever the table maximum says.
    readonly property int highestAffordableStake:
      Math.min(PlayerWallet.maximumBet, PlayerWallet.balance)

    property int stake: PlayerWallet.minimumBet

    readonly property real targetStep: 0.25

    // The lowest target worth offering. Below this the payout barely covers the
    // stake, and the engine treats 1.00x as off anyway.
    readonly property real lowestTarget: 1.25

    readonly property real highestTarget: 50

    readonly property bool isAutoOn: Rounds.engine.autoCashOutAt > 0

    readonly property bool isBettingOpen:
      Rounds.engine.state === RoundEngine.Betting

    readonly property bool isRoundRunning:
      Rounds.engine.state === RoundEngine.Running

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
        return Rounds.engine.hasCashedOut
          ? qsTr("Cashed out")
          : qsTr("Watching this one")

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
