/*!
 * The colours and the type sizes the interface is built from.
 *
 * Not called Theme: Felgo declares a Theme singleton of its own, and a name
 * that already means something else in the same scope is a name that will be
 * resolved to the wrong object exactly once, in the one place nobody looks.
 *
 * Only the interface lives here. The sky, the balloon and the sparks keep their
 * colours where they are drawn: those are pictures, and a picture's colours are
 * chosen against each other rather than against the rest of the screen.
 */

pragma Singleton

import QtQuick

QtObject {
  id: root

  // Text, from the line the player is meant to read down to the one that only
  // has to be there.
  readonly property color text: Qt.rgba(1, 1, 1, 1)
  readonly property color textMuted: Qt.rgba(0.561, 0.639, 0.816, 1)

  // Light enough to clear four and a half to one against a panel, which is what
  // the smallest text in the game is set in - the revealed seed.
  readonly property color textFaint: Qt.rgba(0.47, 0.52, 0.64, 1)

  // A panel dims the game behind it and is outlined by a hairline rather than a
  // border, so it reads as lit by the same sky.
  readonly property color scrim: Qt.rgba(0.02, 0.03, 0.08, 0.86)
  readonly property color surface: Qt.rgba(0.09, 0.11, 0.2, 1)
  readonly property color hairline: Qt.rgba(1, 1, 1, 0.08)

  // For the small things laid on a panel or on the sky: a chip, a list row.
  readonly property color inset: Qt.rgba(1, 1, 1, 0.06)

  // The action colour, and what a label on top of it has to be to stay legible.
  readonly property color accent: Qt.rgba(0.22, 0.75, 0.5, 1)
  readonly property color accentPressed: Qt.rgba(0.28, 0.62, 0.45, 1)
  readonly property color onAccent: Qt.rgba(0.03, 0.09, 0.07, 1)

  // A disabled control is still telling the player something - why they cannot
  // bet, or that the round is running without them - so its label stays
  // readable and only the colour says it cannot be pressed.
  readonly property color disabled: Qt.rgba(0.13, 0.16, 0.26, 1)
  readonly property color onDisabled: Qt.rgba(0.62, 0.66, 0.76, 1)

  // What a round can be: climbing, hot on the way up, popped.
  readonly property color climbing: Qt.rgba(0.55, 0.95, 0.65, 1)
  readonly property color hot: Qt.rgba(1, 0.78, 0.35, 1)
  readonly property color crashed: Qt.rgba(0.95, 0.35, 0.4, 1)

  // A round that ended low is not the player's loss, only a short round, so the
  // history says it in a muted red rather than the one their own money is
  // written off in.
  readonly property color crashedQuiet: Qt.rgba(0.85, 0.55, 0.55, 1)

  //! [type-scale]
  // Seven sizes, each far enough from the next that two of them beside each
  // other cannot be mistaken for one size drawn twice. Anything that needs a
  // size not on this list is either shouting or hiding.
  readonly property int displaySize: 56
  readonly property int titleSize: 48
  readonly property int headingSize: 18
  readonly property int bodySize: 16
  readonly property int labelSize: 14
  readonly property int captionSize: 12
  readonly property int fineSize: 10
  //! [type-scale]
}
