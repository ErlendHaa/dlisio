import logging

from .. import core

class logger(core.logger):
    """ Logger

    Standard python logger wrapper
    """
    def __init__(self):
        core.logger.__init__(self)

    @staticmethod
    def log(level, msg):
        """ Overrides dl::logger::log """
        getattr(logging, level)(msg)

