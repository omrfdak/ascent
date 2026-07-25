/*!
 * The playfield: shows the multiplier of the round in progress, and what the
 * round is currently doing.
 */

import QtQuick
import Ascent

Item {
  id: root

  // Applied as a transform so the scene can be shaken without any of its
  // children giving up the anchors that keep them in place.
  transform: Translate {
    x: screenShake.offsetX
    y: screenShake.offsetY
  }

  ScreenShake {
    id: screenShake
  }

  // In a lane of its own down the left. The multiplier is 56 pixels tall and
  // takes most of the width at four digits, so a balloon rising up the middle
  // ends up climbing through the number - which reads as a collision rather
  // than as altitude.
  Balloon {
    id: balloon

    x: root.width * 0.11
    y: _.groundY - _.riseProgress * (_.groundY - _.ceilingY)
    visible: Rounds.engine.state === RoundEngine.Betting
      || Rounds.engine.state === RoundEngine.Running
    riseProgress: _.riseProgress
  }

  CrashBurst {
    id: crashBurst

    x: balloon.x + balloon.width / 2
    y: balloon.y + balloon.width / 2
  }

  Column {
    id: readout

    anchors {
      centerIn: parent

      // Above the middle, because the bottom third belongs to the panel.
      verticalCenterOffset: -30
    }
    spacing: 12

    MultiplierReadout {
      id: multiplierLabel

      anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
      id: stateLabel

      anchors.horizontalCenter: parent.horizontalCenter
      text: _.stateText
      color: Qt.rgba(0.561, 0.639, 0.816, 1)
      font.pixelSize: 16
    }

  }

  // In the corner with the balance rather than under the multiplier: it is a
  // line about money, and out of the middle it can neither push the readout
  // around as it comes and goes nor reach across the balloon's lane.
  Text {
    id: outcomeLabel

    anchors {
      top: soundToggle.bottom
      right: parent.right
      topMargin: 10
      rightMargin: 12
    }
    text: _.outcomeText
    color: _.outcomeColor
    font.pixelSize: 16
    font.bold: true
  }

  Text {
    id: balanceLabel

    anchors {
      top: parent.top
      left: parent.left
      margins: 12
    }
    text: qsTr("%1 pts").arg(PlayerWallet.balance.toFixed(2))
    color: Qt.rgba(0.561, 0.639, 0.816, 1)
    font.pixelSize: 14
  }

  RecentRoundsStrip {
    id: recentRounds

    anchors {
      top: balanceLabel.bottom
      left: parent.left
      topMargin: 10
      leftMargin: 12
    }
  }

  // Felgo pauses every sound and the music from these two settings and keeps
  // them across runs, so silencing the game is a one line switch. It sits in
  // the corner until the menu arrives to give it a proper home.
  Text {
    id: soundToggle

    anchors {
      top: parent.top
      right: parent.right
      margins: 12
    }
    text: settings.soundEnabled ? qsTr("sound on") : qsTr("sound off")
    color: Qt.rgba(0.561, 0.639, 0.816, 1)
    font.pixelSize: 14

    MouseArea {
      anchors {
        fill: parent
        margins: -8
      }
      onClicked: {
        settings.soundEnabled = !settings.soundEnabled
        settings.musicEnabled = settings.soundEnabled
      }
    }
  }

  // Stacked above the round hash rather than both being pinned to the bottom
  // edge, which is how the two ended up on top of each other.
  BettingPanel {
    id: bettingPanel

    anchors {
      bottom: commitmentLabel.top
      horizontalCenter: parent.horizontalCenter
      bottomMargin: 10
    }
  }

  // The proof, on screen before anyone can bet on the round it decides. It is
  // shortened because nobody reads 64 hex characters; the full seed comes out
  // when the round ends.
  Text {
    id: commitmentLabel

    anchors {
      bottom: parent.bottom
      horizontalCenter: parent.horizontalCenter
      bottomMargin: 10
    }
    text: _.revealedSeed.length > 0
      ? qsTr("seed %1…").arg(_.revealedSeed.substring(0, 12))
      : qsTr("round hash %1…").arg(Rounds.commitment.substring(0, 12))
    color: Qt.rgba(0.35, 0.4, 0.55, 1)
    font.pixelSize: 12
  }

  QtObject {
    id: _

    // The lane the balloon has to itself: it starts clear of the panel and
    // stops clear of the strip of recent rounds, so nothing it does can put it
    // on top of something that has to stay readable.
    readonly property real groundY: bettingPanel.y - balloon.height - 12
    readonly property real ceilingY: recentRounds.y + recentRounds.height + 12

    // The multiplier has no ceiling but the scene does. Reading the height off
    // the logarithm means every scale gets its own stretch of sky: 2x is a third
    // of the way up, 10x two thirds, 1000x still climbing. A balloon pinned to
    // the top while the number keeps running would be a dead screen at exactly
    // the moment the round is worth watching.
    readonly property real riseProgress: 1 - 1 / (1 + 0.9 * Math.log(Rounds.engine.multiplier))

    readonly property color climbingColor: Qt.rgba(0.55, 0.95, 0.65, 1)
    readonly property color crashedColor: Qt.rgba(0.95, 0.35, 0.4, 1)
    readonly property color restingColor: Qt.rgba(1, 1, 1, 1)

    readonly property string stateText: {
      switch (Rounds.engine.state) {
      case RoundEngine.Betting:
        return qsTr("next round in %1s").arg((Rounds.bettingMsRemaining / 1000).toFixed(1))
      case RoundEngine.Running:
        return qsTr("cash out before it pops")
      default:
        // On a crash the engine pins the multiplier to the committed point, so
        // this is the number the round was actually worth.
        return qsTr("popped at %1x").arg(Rounds.engine.multiplier.toFixed(2))
      }
    }

    property string outcomeText: ""
    property color outcomeColor: _.restingColor
    property string revealedSeed: ""
  }

  Connections {
    target: Rounds

    function onCashOutConfirmed(payout, multiplier) {
      _.outcomeText = qsTr("+%1 pts at %2x").arg(payout.toFixed(2)).arg(multiplier.toFixed(2))
      _.outcomeColor = _.climbingColor
    }

    function onRoundCrashed(crashPoint, revealedSeed) {
      // Now that the round is over the seed proves the hash that was on screen
      // before it started - the player can check it without leaving the game.
      _.revealedSeed = revealedSeed

      // Fired from wherever the balloon had climbed to, before the binding
      // pulls it back down for the next round.
      crashBurst.start()
      screenShake.start()

      if (Rounds.engine.bet > 0 && !Rounds.engine.hasCashedOut) {
        _.outcomeText = qsTr("−%1 pts").arg(Rounds.engine.bet.toFixed(2))
        _.outcomeColor = _.crashedColor
      }
    }

    function onBettingOpened() {
      _.outcomeText = ""
      _.revealedSeed = ""
    }
  }
}
