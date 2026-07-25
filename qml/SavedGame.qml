/*!
 * The game the player left behind. It reads the balance and the statistics
 * back before the first round opens, and writes them again whenever they
 * change, so closing the app is never a decision.
 */

import Felgo
import QtQuick
import Ascent

Item {
  id: root

  //! [storage]
  Storage {
    id: storage

    databaseName: "ascent"

    // Felgo's Storage answers with undefined for a key that was never written,
    // which is what a first launch looks like: the wallet keeps the balance it
    // was built with and the statistics stay empty.
    Component.onCompleted: {
      PlayerWallet.restore(storage.getValue("balance") || 0)
      PlayerStats.restore(storage.getValue("stats") || ({}))
      Rounds.engine.autoCashOutAt = storage.getValue("autoCashOutAt") || 0
    }
  }
  //! [storage]

  Connections {
    target: PlayerWallet

    function onBalanceChanged() {
      storage.setValue("balance", PlayerWallet.balance)
    }
  }

  Connections {
    target: PlayerStats

    function onStatsChanged() {
      storage.setValue("stats", PlayerStats.save())
    }
  }

  Connections {
    target: Rounds.engine

    function onAutoCashOutAtChanged() {
      storage.setValue("autoCashOutAt", Rounds.engine.autoCashOutAt)
    }
  }
}
