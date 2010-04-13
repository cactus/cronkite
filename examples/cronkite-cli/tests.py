import sys
import os
import exceptions
from nose.tools import *
from subprocess import Popen, PIPE

class Tests:
    def setup(self):
        os.environ['CRONKITE_AURURL'] = 'http://aur.test'
        self.cmd = os.path.join(os.getcwd(),'cronkite-cli')

    def test_version(self):
        """version -- ensure version output works"""
        p = Popen(args=[self.cmd, '-version'], env=os.environ,
                  shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
        output = p.communicate()[1]
        assert_equal(p.returncode,0,"Expected 0, got %d" % p.returncode)

    def test_help(self):
        """helpoutput -- ensure help output works"""
        p = Popen(args=[self.cmd, '-help'], env=os.environ,
                  shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
        output = p.communicate()[1]
        assert_equal(p.returncode,0,"Expected 0, got %d" % p.returncode)

    def test_badarg(self):
        """badarg -- test an argument that doesn't exist"""
        p = Popen(args=[self.cmd, '-fail'], env=os.environ,
                  shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
        output = p.communicate()[1]
        assert_equal(p.returncode,1,"Expected 1, got %d" % p.returncode)

    def test_emptyresults(self):
        """emptyresults -- test an empty result set"""
        p = Popen(args=[self.cmd, '-search', 'a'],
                env={'CRONKITE_AURURL':
                     'http://aur.archlinux.org/rpc.php?type=%s&arg=%s'},
                shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
        output = p.communicate()[1]
        assert_equal(p.returncode,2,"Expected 2, got %d" % p.returncode)
        assert_equal(output.strip(),'No results found.')

    def test_badurl(self):
        """emptyresults -- test an empty result set"""
        p = Popen(args=[self.cmd, '-search', 'a'],
                env={'CRONKITE_AURURL': ''},
                shell=False, stdout=PIPE, stderr=PIPE, close_fds=True)
        output = p.communicate()[1]
        assert_equal(p.returncode,1,"Expected 2, got %d" % p.returncode)
        assert_equal(output.strip(),'URL using bad/illegal format or missing URL')

