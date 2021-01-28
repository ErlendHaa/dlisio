from .. import core

class logical_file():
    """ Logical File (LF)

    A LIS file are typically segmented into multiple logical files. LF's are
    just a way to group information that logically belongs together. LF's are
    completely independent of each other and can generally be treated as if
    they where different files on disk. In fact - that's just what dlisio does.
    Each logical file gets it's own io-device and is completly segmented from
    other LF's.
    """
    def __init__(self, io, index):
        self.io = io
        self.index = index

    def close(self):
        self.io.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def __repr__(self):
        return 'logical_file'

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
