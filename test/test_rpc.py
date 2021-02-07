#!/usr/bin/python

from common.unittests import StatsTest
from common.tlopostats import Daemon

import unittest


class TestRPC(StatsTest):
    def test_rpc(self):
        # Start the daemon:
        d = Daemon()
        d.start()

        # Initially, there should be no collector:
        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'], {})

        # Create a collector:
        result = self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')
        self.assertTrue(result['success'])

        # Let's do it again!
        result = self.doRPC('add_incremental', name='enemies_killed', event='ENEMY_KILLED')
        self.assertFalse(result['success'])
        self.assertEquals(result['error'], 'enemies_killed already exists')

        # Oops, forgot a param:
        result = self.doRPC('add_incremental', name='ships_sunk')
        self.assertFalse(result['success'])
        self.assertEquals(result['error'], "invalid or missing 'event' param")

        result = self.doRPC('add_incremental', event='SHIPS_SUNK')
        self.assertFalse(result['success'])
        self.assertEquals(result['error'], "invalid or missing 'name' param")

        # Try sending invalid params:
        result = self.doRPC('add_incremental', name='ships_sunk', event=1)
        self.assertFalse(result['success'])
        self.assertEquals(result['error'], "invalid or missing 'event' param")

        result = self.doRPC('add_incremental', name=1, event='SHIPS_SUNK')
        self.assertFalse(result['success'])
        self.assertEquals(result['error'], "invalid or missing 'name' param")

        # Make sure that the list isn't broken:
        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'],
                          {'enemies_killed':
                                {'event': 'ENEMY_KILLED',
                                 'name': 'enemies_killed',
                                 'type': 'incremental'}
                          })

        # Remove enemies_killed:
        result = self.doRPC('remove', name='enemies_killed')
        self.assertTrue(result['success'])

        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'], {})

        # Add a periodic collector:
        result = self.doRPC('add_periodic', name='shard_pop', event='SHARD_POP')
        self.assertFalse(result['success']) # Oops, forgot a param
        self.assertEquals(result['error'], "invalid or missing 'period' param")

        result = self.doRPC('add_periodic', name='shard_pop', event='SHARD_POP', period='1234 years')
        self.assertFalse(result['success']) # Good joke
        self.assertEquals(result['error'], "invalid or missing 'period' param")

        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'], {})

        # Fine, let's do it right:
        result = self.doRPC('add_periodic', name='shard_pop', event='SHARD_POP', period=5)
        self.assertTrue(result['success'])

        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'],
                          {'shard_pop':
                              {'event': 'SHARD_POP',
                               'name': 'shard_pop',
                               'period': 5,
                               'type': 'periodic'}
                          })

        # Remove shard_pop:
        result = self.doRPC('remove', name='shard_pop')
        self.assertTrue(result['success'])

        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'], {})

        # Add highscore collectors:
        result = self.doRPC('add_highscore', name='cd_wave', event='CD_WAVE', reversed=False)
        self.assertTrue(result['success'])

        result = self.doRPC('add_highscore', name='bp_time', event='BP_TIME', reversed=True)
        self.assertTrue(result['success'])

        result = self.doRPC('list')
        self.assertTrue(result['success'])
        self.assertEquals(result['result'],
                          {'cd_wave':
                              {'event': 'CD_WAVE',
                               'name': 'cd_wave',
                               'reversed': False,
                               'type': 'highscore'},
                           'bp_time':
                              {'event': 'BP_TIME',
                               'name': 'bp_time',
                               'reversed': True,
                               'type': 'highscore'}
                          })

        # Cleanup:
        d.stop()
        self.resetDatabase()


if __name__ == '__main__':
    unittest.main()
