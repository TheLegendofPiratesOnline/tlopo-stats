#!/usr/bin/python

from common.unittests import StatsTest
from common.tlopostats import Daemon

import unittest
import time


class TestCollectors(StatsTest):
    def test_incremental_collector(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')

        # Send an event:
        self.sendEvent('ENEMY_KILLED', [1234], 1)

        # Check the DB:
        self.expectStat('enemies_killed', 'avatar', 1234, 1)

        # Send the event again:
        self.sendEvent('ENEMY_KILLED', [1234], 2)

        # Check the DB:
        self.expectStat('enemies_killed', 'avatar', 1234, 2)

        # Now, 2 players kill an enemy:
        self.sendEvent('ENEMY_KILLED', [1234, 1235], 1)
        self.expectStat('enemies_killed', 'avatar', 1234, 1)
        self.expectStat('enemies_killed', 'avatar', 1235, 1)

        # Cleanup:
        d.stop()
        self.resetDatabase()

    def test_periodic_collector(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_periodic', name='shard_pop', event='SHARD_POP', period=3)

        # Send an event:
        self.sendEvent('SHARD_POP', [30000, 30001], 1)

        # Wait 4 seconds:
        time.sleep(4)

        # Check the DB:
        self.expectPeriodicStat('shard_pop', 30000, 1)
        self.expectPeriodicStat('shard_pop', 30001, 1)

        # Send more events:
        self.sendEvent('SHARD_POP', [30000, 30001], 10)
        self.sendEvent('SHARD_POP', [30000], 20)
        self.sendEvent('SHARD_POP', [30001], 15)

        # ... ZZZ ...
        time.sleep(4)

        # Should register only the last value for each doId:
        self.expectPeriodicStat('shard_pop', 30000, 20)
        self.expectPeriodicStat('shard_pop', 30001, 15)

        # Cleanup:
        d.stop()
        self.resetDatabase()

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
        self.sendEvent('ENEMY_KILLED', [1002], 2)
        self.sendEvent('ENEMY_KILLED', [1002], 3)

        # Check the DB:
        self.expectStat('enemies_killed', 'avatar', 1000, 1)
        self.expectStat('enemies_killed', 'avatar', 1001, 1)
        self.expectStat('enemies_killed', 'avatar', 1002, 5)
        self.expectStat('enemies_killed', 'guild', 20000, 2)
        self.expectStat('enemies_killed', 'guild', 20001, 5)

        self.expectLeaderboard('avatar:enemies_killed', 1000, 2, 1)
        self.expectLeaderboard('avatar:enemies_killed', 1001, 1, 1)
        self.expectLeaderboard('avatar:enemies_killed', 1002, 0, 5)
        self.expectLeaderboard('guild:enemies_killed', 20000, 1, 2)
        self.expectLeaderboard('guild:enemies_killed', 20001, 0, 5)

        # Ban 1001 and 20001:
        self.doRPC('ban', id=1001)
        self.doRPC('ban', id=20001)
        time.sleep(1)

        self.expectOverallLeaderboard('avatar:enemies_killed', 1000, 1, 1)
        self.expectOverallLeaderboard('avatar:enemies_killed', 1001, None, None)
        self.expectOverallLeaderboard('avatar:enemies_killed', 1002, 0, 5)
        self.expectOverallLeaderboard('guild:enemies_killed', 20000, 0, 2)
        self.expectOverallLeaderboard('guild:enemies_killed', 20001, None, None)

        # Cleanup:
        d.stop()
        self.resetDatabase()

    def test_highscore(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_highscore', name='cd_wave', event='CD_WAVE')

        # Send an event:
        self.sendEvent('CD_WAVE', [1234, 1235], 100)

        # Check the DB:
        self.expectHighscore('cd_wave', 1234, 100)
        self.expectHighscore('cd_wave', 1235, 100)

        # 1234 sets a new highscore:
        self.sendEvent('CD_WAVE', [1234], 200)
        self.expectHighscore('cd_wave', 1234, 200)

        # Restart to ensure load_highscore_entries is working
        d.restart()

        # 1235 scores 40, but gets to keep 100 in the highscore:
        self.sendEvent('CD_WAVE', [1235], 40)
        self.expectHighscore('cd_wave', 1235, 100)

        # Cleanup:
        d.stop()
        self.resetDatabase()


if __name__ == '__main__':
    unittest.main()
