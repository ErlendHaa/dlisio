import os

from . import core
from .file import physicalfile, logicalfile
from .errors import ErrorHandler

def open(path):
    """ Open a file

    Open a low-level file handle. This is not intended for end-users - rather,
    it's an escape hatch for very broken files that dlisio cannot handle.

    Parameters
    ----------
    path : str_like

    Returns
    -------
    stream : dlisio.core.stream

    See Also
    --------
    dlisio.load
    """
    return core.open(str(path))

def load(path, error_handler = None):
    """ Loads a file and returns one filehandle pr logical file.

    Load does more than just opening the file. A DLIS file has no random access
    in itself, so load scans the entire file and creates its own index to
    enumlate random access.

    DLIS-files segmented into into Logical Files, see :class:`physical_file`
    and :class:`logical_file`. The partitioning into Logical Files also happens
    at load.

    Parameters
    ----------

    path : str_like

    error_handler : dlisio.errors.ErrorHandler, optional
            Error handling rules. Default rules will apply if none supplied.
            Handler will be added to all the logical files, so users may modify
            the behavior at any time.

    Returns
    -------

    dlis : dlisio.physicalfile

    Notes
    -----

    It's not uncommon that DLIS files are stored with different file extensions
    than `.DLIS`. For example `.TIF`. Load does not care about file extension at
    all. As long as the content adheres to the Digital Log Interchange
    Standard, load will read it as such.

    Examples
    --------

    Load is designed to work with python's ``with``-statement:

    >>> with dlisio.load(filename) as files:
    ...     for f in files:
    ...         pass

    Automatically unpack the first logical file and store the remaining logical
    files in tail

    >>> with dlisio.load(filename) as (f, *tail):
    ...     pass

    Note that the parentheses are needed when unpacking directly in the with-
    statement.  The asterisk allows an arbitrary number of extra logical files
    to be stored in tail. Use len(tail) to check how many extra logical files
    there are.
    """
    if not error_handler:
        error_handler = ErrorHandler()

    sulsize = 80
    tifsize = 12
    lfs = []

    def rewind(offset, tif):
        """Rewind offset to make sure not to miss VRL when calling findvrl"""
        offset -= 4
        if tif: offset -= 12
        return offset

    path = str(path)
    if not os.path.isfile(path):
        raise OSError("'{}' is not an existing regular file".format(path))
    stream = open(path)
    try:
        offset = core.findsul(stream)
        sul = stream.get(bytearray(sulsize), offset, sulsize)
        offset += sulsize
    except:
        offset = 0
        sul = None

    try:
        stream.seek(0)
        tm = core.read_tapemark(stream)
        tapemarks = core.valid_tapemark(tm)
        offset = core.findvrl(stream, offset)

        # Layered File Protocol does not currently offer support for re-opening
        # files at the current position, nor is it able to precisly report the
        # underlying tell. Therefore, dlisio has to manually search for the
        # VRL to determine the right offset in which to open the new filehandle
        # at.
        #
        # Logical files are partitioned by core.findoffsets and it's required
        # [1] that new logical files always start on a new Visible Record.
        # Hence, dlisio takes the (approximate) tell at the end of each Logical
        # File and searches for the VRL to get the exact tell.
        #
        # [1] rp66v1, 2.3.6 Record Structure Requirements:
        #     > ... Visible Records cannot intersect more than one Logical File.
        while True:
            if tapemarks: offset -= tifsize
            stream.seek(offset)
            if tapemarks: stream = core.open_tif(stream)
            stream = core.open_rp66(stream)

            explicits, implicits, broken = core.findoffsets(stream, error_handler)
            hint = rewind(stream.ptell, tapemarks)

            recs  = core.extract(stream, explicits, error_handler)
            sets  = core.parse_objects(recs, error_handler)
            pool  = core.pool(sets)
            fdata = core.findfdata(stream, implicits, error_handler)

            lf = logicalfile(stream, pool, fdata, sul, error_handler)
            lfs.append(lf)

            if len(broken):
                # do not attempt to recover or read more logical files
                # if the error happened in findoffsets
                # return all logical files we were able to process until now
                break

            stream = core.open(path)

            try:
                offset = core.findvrl(stream, hint)
            except RuntimeError:
                if stream.eof():
                    stream.close()
                    break
                raise

        return physicalfile(lfs)
    except:
        stream.close()
        for f in lfs:
            f.close()
        raise
