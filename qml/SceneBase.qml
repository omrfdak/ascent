/*!
 * What every scene in the game has in common: the resolution it is laid out in,
 * the sky behind it, and the fact that it is hidden until the window's state
 * machine names it.
 *
 * Being disabled while hidden matters more than it sounds. In Qt an invisible
 * item is still enabled, so a hidden scene's buttons would keep taking taps -
 * the menu would be pressable through the game.
 */

import Felgo
import QtQuick

Scene {
  id: root

  // Every scene asks the window to leave it. Which scene comes next is the
  // window's business, not the scene's, so a scene can be opened from anywhere.
  signal backRequested

  // The resolution every scene is designed against; Felgo scales it to whatever
  // the window or the device turns out to be. Portrait, because the balloon
  // rises, and taller than the window's own ratio so the layout has room to
  // give the balloon a band of its own.
  width: 320
  height: 600

  opacity: 0
  visible: opacity > 0

  // The renderer skips invisible items, so a scene that is off screen costs
  // nothing while it waits.
  enabled: visible

  // Anchored to the window rather than to the scene: on a wider display the
  // scene is letterboxed, and the sky is what fills the bars.
  SkyBackground {
    id: sky

    anchors.fill: root.fullWindowAnchorItem
    z: -1
  }

  Behavior on opacity {
    NumberAnimation {
      duration: 220
      easing.type: Easing.InOutQuad
    }
  }
}
