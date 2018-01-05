#!/usr/bin/python

from common.unittests import StatsTest
from common.tlopostats import Daemon

import unittest
import time


class TestCollectors(StatsTest):
    def test_incremental_collectors(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')

        # Send an event:
        self.sendEvent('ENEMY_KILLED', [1234], 1)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1234, 1)

        # Send the event again:
        self.sendEvent('ENEMY_KILLED', [1234], 2)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1234, 3)
        self.expectStat('total.enemies_killed', 0, 3)

        # Now, 2 players kill an enemy:
        self.sendEvent('ENEMY_KILLED', [1234, 1235], 1)
        self.expectStat('avatar.enemies_killed', 1234, 4)
        self.expectStat('avatar.enemies_killed', 1235, 1)
        self.expectStat('total.enemies_killed', 0, 4) # 4, not 5

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
        #self.expectPeriodicStat('shard_pop', 30000, 1)
        #self.expectPeriodicStat('shard_pop', 30001, 1)

        # Send more events:
        self.sendEvent('SHARD_POP', [30000, 30001], 10)
        self.sendEvent('SHARD_POP', [30000], 20)
        self.sendEvent('SHARD_POP', [30001], 15)

        # ... ZZZ ...
        time.sleep(4)

        # Should register only the last value for each doId:
        #self.expectPeriodicStat('shard_pop', 30000, 20)
        #self.expectPeriodicStat('shard_pop', 30001, 15)

        # Cleanup:
        d.stop()
        self.resetDatabase()

if __name__ == '__main__':
    unittest.main()
