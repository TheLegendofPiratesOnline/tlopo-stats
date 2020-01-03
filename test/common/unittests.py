from tlopostats import Daemon

import unittest
import datetime
import socket
import json
import time

import redis


class StatsTest(unittest.TestCase):
    client = None

    @classmethod
    def setUpClass(cls):
        cls.client = redis.Redis()
        cls.resetDatabase()

    @classmethod
    def tearDownClass(cls):
        cls.resetDatabase()
        cls.client = None

    @classmethod
    def resetDatabase(cls):
        prefix = Daemon.DATABASE + ':*'
        for key in cls.client.keys(prefix):
            cls.client.delete(key)

    def expectStat(self, name, type, avId, expected):
        value = 0

        # Fetch events from queue:
        ignored = []
        while True:
            data = self.client.lpop(Daemon.DATABASE + ':events')
            if not data:
                break

            item = json.loads(data)
            if item['key'] == avId and item['name'] == name and item['type'] == type:
                value += item['value']

            else:
                ignored.append(data)

        # Put ignored items back
        for data in ignored:
            self.client.rpush(Daemon.DATABASE + ':events', data)

        self.assertEquals(value, expected)

    def expectPeriodicStat(self, name, doId, expected):
        value = 0

        # Get the last event from queue:
        ignored = []
        while True:
            data = self.client.lpop(Daemon.DATABASE + ':events')
            if not data:
                break

            item = json.loads(data)
            if item['key'] == doId and item['name'] == name:
                # Most recent entry comes last
                value = item['value']

            else:
                ignored.append(data)

        # Put ignored items back
        for data in ignored:
            self.client.rpush(Daemon.DATABASE + ':events', data)

        self.assertEquals(value, expected)

    def expectDailyLeaderboard(self, name, avId, expectedRank, expectedValue):
        datestr = datetime.datetime.now().strftime('%Y%b%d')
        key = Daemon.DATABASE + ':' + name + ':' + datestr

        rank = self.client.zrevrank(key, avId)
        value = self.client.zscore(key, avId)

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectMonthlyLeaderboard(self, name, avId, expectedRank, expectedValue):
        datestr = datetime.datetime.now().strftime('%Y%b')
        key = Daemon.DATABASE + ':' + name + ':' + datestr

        rank = self.client.zrevrank(key, avId)
        value = self.client.zscore(key, avId)

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectYearlyLeaderboard(self, name, avId, expectedRank, expectedValue):
        datestr = datetime.datetime.now().strftime('%Y')
        key = Daemon.DATABASE + ':' + name + ':' + datestr

        rank = self.client.zrevrank(key, avId)
        value = self.client.zscore(key, avId)

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectOverallLeaderboard(self, name, avId, expectedRank, expectedValue):
        datestr = 'overall'
        key = Daemon.DATABASE + ':' + name + ':' + datestr

        rank = self.client.zrevrank(key, avId)
        value = self.client.zscore(key, avId)

        self.assertEquals(rank, expectedRank)
        self.assertEquals(value, expectedValue)

    def expectLeaderboard(self, name, avId, expectedRank, expectedValue):
        self.expectDailyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectMonthlyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectYearlyLeaderboard(name, avId, expectedRank, expectedValue)
        self.expectOverallLeaderboard(name, avId, expectedRank, expectedValue)

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
