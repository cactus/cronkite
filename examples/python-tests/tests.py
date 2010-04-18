import sys
import os
import unittest
import exceptions

### setup path for load library
sys.bytecodebase = None
sys.path.append('result')

import cronkite

class TestCases(unittest.TestCase):
    def setUp(self):
        cronkite.setopt('AURURL', os.environ['CRONKITE_AURURL'])

    def test_example(self):
        """Perform a test query, and validate results"""
        x = cronkite.query('s', 'example')
        self.assertTrue(len(x) == 1)
        self.assertTrue(x[0]['name'] == 'example')

    def test_errors(self):
        """Ensure cronkite.query throws typeerror"""
        self.assertRaises(
            exceptions.TypeError, cronkite.query, ('wrong', 'example'))

    def test_badset(self):
        self.assertRaises(
            exceptions.TypeError, cronkite.setopt,
            ('BADVAL', "fffffffuuuuuuuuuuuu"))
        self.assertRaises(
            exceptions.TypeError, cronkite.setopt, ('BADVAL'))

if __name__ == '__main__':
    unittest.main()

