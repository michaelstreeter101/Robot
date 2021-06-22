#!/usr/bin/env/python3
# Original author: MWS
# Creation date: 2020-06-22
# Purpose: unit tests for Sergi.

import unittest
import Sergi

class TestExMethods(unittest.TestCase):

    def test_sergi(self):
        self.assertEqual(Sergi.test(), 'Sergi')

    def test_new_Sabretooth(self):
        s = Sergi.Sabretooth
        self.assertEqual( s.move(self, 1), 1)

if __name__=="__main__":
    unittest.main()
