/*!
 * The playfield: shows the multiplier of the round in progress, and what the
 * round is currently doing.
 *
 * The scene is one vertical layout, top to bottom, and the balloon gets the
 * band that is left over. That is what keeps it out of the readout and off the
 * panel at every window size, without a single number in here having to know
 * how tall anything else turned out to be.
 */

import QtQuick
import QtQuick.Layouts
import Ascent

Item {
  id: root

  // Applied as a transform so the scene can be shaken without any of its
  // children giving up the layout that places them.
  transform: Translate {
    x: screenShake.offsetX
    y: screenShake.offsetY
  }

  ScreenShake {
    id: screenShake
  }

  ColumnLayout {
    id: content

    anchors {
      fill: parent
      margins: 12
    }
    spacing: 10

    RowLayout {
      id: topRow

      Layout.fillWidth: true

      Text {
        id: balanceLabel

        text: qsTr("%1 pts").arg(PlayerWallet.balance.toFixed(2))
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        font.pixelSize: 14
      }

      Item {
        id: topSpacer

        Layout.fillWidth: true
      }

      Text {
        id: statsToggle

        text: qsTr("stats")
        color: Qt.rgba(0.561, 0.639, 0.816, 1)
        font.pixelSize: 14

        MouseArea {
          anchors {
            fill: parent
            margins: -8
          }
          onClicked: statsPanel.visible = true
        }
      }

      // Felgo pauses every sound and the music from these two settings and
      // keeps them across runs, so silencing the game is a one line switch. It
      // sits in the corner until the menu arrives to give it a proper home.
      Text {
        id: soundToggle

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
    }

    RowLayout {
      id: historyRow

      Layout.fillWidth: true

      // Its row keeps its height while it is still empty, so the first finished
      // round does not shift everything below it.
      RecentRoundsStrip {
        id: recentRounds

        Layout.minimumHeight: 20
      }

      Item {
        id: historySpacer

        Layout.fillWidth: true
      }

      // Kept up here with the balance because it is a line about money, and out
      // of the middle it can come and go without moving anything.
      Text {
        id: outcomeLabel

        text: _.outcomeText
        color: _.outcomeColor
        font {
          pixelSize: 16
          bold: true
        }
      }
    }

    MultiplierReadout {
      id: multiplierLabel

      Layout.alignment: Qt.AlignHCenter
      Layout.topMargin: 6
    }

    Text {
      id: stateLabel

      Layout.alignment: Qt.AlignHCenter
      text: _.stateText
      color: Qt.rgba(0.561, 0.639, 0.816, 1)
      font.pixelSize: 16
    }

    // Whatever height is left after everything else has had its say. The
    // balloon rises inside it, so the layout is what guarantees it never
    // reaches the readout or the panel.
    Item {
      id: balloonLane

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.minimumHeight: balloon.height

      Balloon {
        id: balloon

        x: (balloonLane.width - width) / 2
        y: (balloonLane.height - height) * (1 - _.riseProgress)
        visible: Rounds.engine.state === RoundEngine.Betting
          || Rounds.engine.state === RoundEngine.Running
        riseProgress: _.riseProgress
      }

      CrashBurst {
        id: crashBurst

        x: balloon.x + balloon.width / 2
        y: balloon.y + balloon.width / 2
      }
    }

    BettingPanel {
      id: bettingPanel

      Layout.alignment: Qt.AlignHCenter
    }

    // The proof, on screen before anyone can bet on the round it decides. It is
    // shortened because nobody reads 64 hex characters; the full seed comes out
    // when the round ends.
    Text {
      id: commitmentLabel

      Layout.alignment: Qt.AlignHCenter
      text: _.revealedSeed.length > 0
        ? qsTr("seed %1…").arg(_.revealedSeed.substring(0, 12))
        : qsTr("round hash %1…").arg(Rounds.commitment.substring(0, 12))
      color: Qt.rgba(0.35, 0.4, 0.55, 1)
      font.pixelSize: 12
    }
  }

  // Outside the layout on purpose: it covers the scene rather than taking a row
  // in it. It moves into the menu once there is a menu to put it in.
  StatsPanel {
    id: statsPanel

    anchors.fill: parent
    visible: false
  }

  QtObject {
    id: _

    // The multiplier has no ceiling but the lane does. Reading the height off
    // the logarithm means every scale gets its own stretch of sky: 2x is a
    // third of the way up, 10x two thirds, 1000x still climbing. A balloon
    // pinned to
    // the top while the number keeps running would be a dead screen at exactly
    // the moment the round is worth watching.
    readonly property real riseProgress: 1
      - 1 / (1 + 0.9 * Math.log(Rounds.engine.multiplier))

    readonly property color climbingColor: Qt.rgba(0.55, 0.95, 0.65, 1)
    readonly property color crashedColor: Qt.rgba(0.95, 0.35, 0.4, 1)
    readonly property color restingColor: Qt.rgba(1, 1, 1, 1)

    readonly property string stateText: {
      switch (Rounds.engine.state) {
      case RoundEngine.Betting:
        return qsTr("next round in %1s")
          .arg((Rounds.bettingMsRemaining / 1000).toFixed(1))
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
      _.outcomeText = qsTr("+%1 pts at %2x")
        .arg(payout.toFixed(2)).arg(multiplier.toFixed(2))
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
