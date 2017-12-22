#!/usr/bin/python

from common.unittests import StatsTest
from common.tlopostats import Daemon

import unittest


class TestAvMgr(StatsTest):
    def test_stat_manager(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')

        # 1234 comes online:
        self.sendEvent('AV_ONLINE', [1234])

        # Send an event:
        self.sendEvent('ENEMY_KILLED', [1234], 1)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1234, 1)

        # Give 1234 a guild:
        self.sendEvent('AV_GUILD', [1234], 10000)

        # Send the event again:
        self.sendEvent('ENEMY_KILLED', [1234], 2)

        # This time, check the guild stats too:
        self.expectStat('avatar.enemies_killed', 1234, 3)
        self.expectStat('guild.enemies_killed', 10000, 2)

        # Change their guild and do it again:
        self.sendEvent('AV_GUILD', [1234], 10001)
        self.sendEvent('ENEMY_KILLED', [1234], 7)
        self.expectStat('avatar.enemies_killed', 1234, 10)
        self.expectStat('guild.enemies_killed', 10000, 2) # No change
        self.expectStat('guild.enemies_killed', 10001, 7)

        # No guild:
        self.sendEvent('AV_GUILD', [1234], 0)
        self.sendEvent('ENEMY_KILLED', [1234], 5)
        self.expectStat('avatar.enemies_killed', 1234, 15)
        self.expectStat('guild.enemies_killed', 10000, 2) # No change
        self.expectStat('guild.enemies_killed', 10001, 7) # No change

        # Back to 10000:
        self.sendEvent('AV_GUILD', [1234], 10000)
        self.sendEvent('ENEMY_KILLED', [1234], 2)
        self.expectStat('avatar.enemies_killed', 1234, 17)
        self.expectStat('guild.enemies_killed', 10000, 4)
        self.expectStat('guild.enemies_killed', 10001, 7) # No change

        # Avatar goes offline:
        self.sendEvent('AV_OFFLINE', [1234], 0)

        # I'd tell you an UDP joke, but you wouldn't get it
        self.sendEvent('ENEMY_KILLED', [1234], 5)
        self.expectStat('avatar.enemies_killed', 1234, 22)
        self.expectStat('guild.enemies_killed', 10000, 4) # No change
        self.expectStat('guild.enemies_killed', 10001, 7) # No change

        # Cleanup:
        d.stop()
        self.resetDatabase()

    def test_cache(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Create a collector:
        self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')

        # 1234 comes online:
        self.sendEvent('AV_ONLINE', [1234])

        # Give 1234 a guild:
        self.sendEvent('AV_GUILD', [1234], 10000)

        # Send an event:
        self.sendEvent('ENEMY_KILLED', [1234], 1)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1234, 1)
        self.expectStat('guild.enemies_killed', 10000, 1)

        # Oh no! The daemon died!
        d.stop()

        # Restart it:
        d.start(resetCache=False)

        # Send the event again:
        self.sendEvent('ENEMY_KILLED', [1234], 1)

        # Check the DB:
        self.expectStat('avatar.enemies_killed', 1234, 2)
        self.expectStat('guild.enemies_killed', 10000, 2)

        # Cleanup:
        d.stop()
        self.resetDatabase()

if __name__ == '__main__':
    unittest.main()
