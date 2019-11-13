from tlopostats import Daemon

import unittest
import datetime
import pymongo
import socket
import json
import time


class StatsTest(unittest.TestCase):
    client = None
    db = None

    @classmethod
    def setUpClass(cls):
        cls.client = pymongo.MongoClient('127.0.0.1')
        cls.db = cls.client[Daemon.DATABASE]
        cls.resetDatabase()

    @classmethod
    def tearDownClass(cls):
        cls.resetDatabase()
        cls.client = None
        cls.db = None

    @classmethod
    def resetDatabase(cls):
        cls.client.drop_database(Daemon.DATABASE)

    def expectStat(self, name, type, avId, expected):
        value = 0

        # Fetch events from queue:
        for r in self.db.events.find({'key': avId, 'name': name, 'type': type}):
            value += r['value']
            self.db.events.remove({'_id': r['_id']})

        self.assertEquals(value, expected)

    def expectDailyLeaderboard(self, name, avId, expectedRank, expectedValue):
        value = 0
        rank = 0
        datestr = datetime.datetime.now().strftime('%Y%b%d')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']
            rank = r['rank']

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectMonthlyLeaderboard(self, name, avId, expectedRank, expectedValue):
        value = 0
        rank = 0
        datestr = datetime.datetime.now().strftime('%Y%b')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']
            rank = r['rank']

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectYearlyLeaderboard(self, name, avId, expectedRank, expectedValue):
        value = 0
        rank = 0
        datestr = datetime.datetime.now().strftime('%Y')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']
            rank = r['rank']

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectOverallLeaderboard(self, name, avId, expectedRank, expectedValue):
        value = 0
        rank = 0
        r = self.db[name]['overall'].find_one({'_id': avId})
        if r:
            value = r['value']
            rank = r['rank']

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectLeaderboard(self, name, avId, expectedRank, expectedValue):
        self.expectDailyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectMonthlyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectYearlyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectOverallLeaderboard(name, avId, expectedRank, expectedValue)

    def expectPeriodicStat(self, name, doId, expected):
        # Get the last event:
        cursor = self.db.events.find({'key': doId, 'name': name}).sort('date', pymongo.DESCENDING)
        try:
            value = next(cursor)['value']

        except StopIteration:
            value = 0

        # Remove events from event queue:
        self.db.events.remove({'key': doId, 'name': name})

        self.assertEquals(value, expected)

    def sendEvent(self, event, doIds=[], value=0):
        data = json.dumps({'event': event, 'doIds': doIds, 'value': value})
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        sock.sendto(data, ('127.0.0.1', 8963))
        if event.startswith('AV_'):
            time.sleep(0.1)

        else:
            time.sleep(3.0)

    def doRPC(self, method, **kwargs):
        kwargs['method'] = method
        data = json.dumps(kwargs)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('127.0.0.1', 8964))
        sock.send(data + '\n')
        return json.loads(sock.recv(1024))
