import logging

from .. import core

class logical_file():
    """ Logical File (LF)

    This class is the main interface for working with a single LF. A LF is
    essentially a series of Logical Records (LR). There are many different LR
    types, each designed to carry a specific piece of information. For example
    a Logical File Header Record contains static information about the file,
    while the Data Format Specification Records contain information about
    curve-data, and how that should be parsed.

    This class provides an interface for easy interaction and extraction of the
    various Logical Records within the Logical File. It is completely
    independent of other LF's, and even has it's own IO-device. It stores a
    pre-built index of all LR's for random access when reading from disk.

    Notes
    -----

    No parsed records are cached by this class. Thus it's advisable that the
    result of each record read is cached locally.
    """
    def __init__(self, path, io, index):
        self.path  = path
        self.io    = io
        self.index = index

    def close(self):
        self.io.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def __repr__(self):
        msg = 'logical_file(path="{}", io={}, index={})'
        return msg.format(self.path, self.io, self.index)

    def header(self):
        """ Logical File Header

        Reads and parses the Logical File Header _from disk_ - if present.

        Returns
        -------

        header : dlisio.core.filerecord or None
        """
        rectype = core.lis_rectype.fileheader
        info = [x for x in self.explicits if x.type == rectype]

        if len(info) > 1:
            msg =  'Multiple File Header, should only be one.'
            msg += 'Use parse_record to read them all'
            raise ValueError(msg)

        if len(info) == 0:
            logging.info("No File Header Record in Logical File")
            return None

        return self.parse_record(info[0])


    def trailer(self):
        """ Logical File Trailer

        Reads and parses the Logical File Trailer _from disk_ - if present.

        Returns
        -------

        trailer : dlisio.core.filerecord or None
        """
        rectype = core.lis_rectype.filetrailer
        info = [x for x in self.explicits if x.type == rectype]

        if len(info) > 1:
            msg =  'Multiple File Trailer, should only be one.'
            msg += 'Use parse_record to read them all'
            raise ValueError(msg)

        if len(info) == 0:
            logging.info("No File Trailer Record in Logical File")
            return None

        return self.parse_record(info[0])


    def parse_record(self, recinfo):
        # This can be C++ function (although what about the return type?)
        rec = self.io.read_record(recinfo)

        rtype = recinfo.type
        if   rtype == core.lis_rectype.format_spec: return core.parse_dfsr(rec)
        elif rtype == core.lis_rectype.fileheader:  return core.parse_file_record(rec)
        elif rtype == core.lis_rectype.filetrailer: return core.parse_file_record(rec)
        else:
            raise NotImplementedError("No parsing rule for {}".format(rtype))


    @property
    def explicits(self):
        return self.index.explicits()

    def dataformatspec(self):
        # TODO duplication checking
        rectype = core.lis_rectype.format_spec
        info = [x for x in self.explicits if x.type == rectype]
        return [self.parse_record(x) for x in info]

class physical_reel(tuple):
    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def close(self):
        for f in self:
            f.close()

    def __repr__(self):
        return 'physical_reel(logical files: {})'.format(len(self))
