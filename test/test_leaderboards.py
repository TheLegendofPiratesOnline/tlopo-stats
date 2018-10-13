#!/usr/bin/python

from common.unittests import StatsTest
from common.tlopostats import Daemon

import unittest
import time


class TestLeaderboards(StatsTest):
    def test_leaderboards(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')

        # Assign guilds:
        self.sendEvent('AV_GUILD', [1000], 20000)
        self.sendEvent('AV_GUILD', [1001], 20000)
        self.sendEvent('AV_GUILD', [1002], 20001)

        # Send a few events:
        self.sendEvent('ENEMY_KILLED', [1000], 1)
        self.sendEvent('ENEMY_KILLED', [1001], 1)
        self.sendEvent('ENEMY_KILLED', [1002], 5)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1000, 1)
        self.expectStat('avatar.enemies_killed', 1001, 1)
        self.expectStat('avatar.enemies_killed', 1002, 5)
        self.expectStat('guild.enemies_killed', 20000, 2)
        self.expectStat('guild.enemies_killed', 20001, 5)

        # Restart the daemon (forces leaderboard flush):
        d.restart()

        # Check the DB:
        self.expectLeaderboard('ld.avatar.enemies_killed', 1000, 2, 1)
        self.expectLeaderboard('ld.avatar.enemies_killed', 1001, 2, 1)
        self.expectLeaderboard('ld.avatar.enemies_killed', 1002, 1, 5)
        self.expectLeaderboard('ld.guild.enemies_killed', 20000, 2, 2)
        self.expectLeaderboard('ld.guild.enemies_killed', 20001, 1, 5)

        # Cleanup:
        d.stop()
        self.resetDatabase()

if __name__ == '__main__':
    unittest.main()
