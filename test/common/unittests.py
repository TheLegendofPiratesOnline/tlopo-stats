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
        cls.client = pymongo.MongoClient('localhost')
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

    def expectDailyStat(self, name, avId, expected):
        value = 0
        datestr = datetime.datetime.now().strftime('%Y%b%d')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']

        self.assertEquals(value, expected)

    def expectMonthlyStat(self, name, avId, expected):
        value = 0
        datestr = datetime.datetime.now().strftime('%Y%b')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']

        self.assertEquals(value, expected)

    def expectYearlyStat(self, name, avId, expected):
        value = 0
        datestr = datetime.datetime.now().strftime('%Y')
        r = self.db[name][datestr].find_one({'_id': avId})
        if r:
            value = r['value']

        self.assertEquals(value, expected)

    def expectStat(self, name, avId, expected):
        self.expectDailyStat(name, avId, expected)
        self.expectMonthlyStat(name, avId, expected)
        self.expectYearlyStat(name, avId, expected)

    def expectPeriodicStat(self, name, doId, expected):
        # Get the last event:
        cursor = self.db[name].find({'key': doId}).sort('date', pymongo.DESCENDING)
        try:
            value = next(cursor)['value']

        except StopIteration:
            value = 0

        self.assertEquals(value, expected)

    def sendEvent(self, event, doIds=[], value=0):
        data = json.dumps({'event': event, 'doIds': doIds, 'value': value})
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        sock.sendto(data, ('localhost', 8963))
        time.sleep(0.05)

    def doRPC(self, method, **kwargs):
        kwargs['method'] = method
        data = json.dumps(kwargs)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('localhost', 8964))
        sock.send(data + '\n')
        return json.loads(sock.recv(1024))
