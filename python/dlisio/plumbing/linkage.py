import logging

from .. import core

def obname(objtype):
    def fingerprint(obj):
        if not isinstance(obj, core.obname): raise TypeError
        return obj.fingerprint(objtype), objtype
    return fingerprint

def objref(obj):
    if not isinstance(obj, core.objref): raise TypeError
    return obj.fingerprint, obj.type

def lookup(lf, reftype, value):
    """Create a fingerprint from reftype(value) and look up corresponding
    object in the logical file."""
    try:
        fp, objtype = reftype(value)
    except TypeError:
        msg = "Unable to create object-reference to '{}'"
        logging.warning(msg.format(value))
        return None

    try:
        return lf[objtype][fp]
    except KeyError:
        msg = "Referenced object '{}' not in logical file"
        logging.warning(msg.format(fp))
        return None

def isreference(val):
    """Check if val is a rp66 reference typ"""
    # TODO: update to check repcode when repcode is back
    return (isinstance (val, core.obname) or
            isinstance (val, core.objref) or
            isinstance (val, core.attref))

def findframe(fp, logical_file):
    """Find all frames containing the channel"""
    frames = []
    for frame in logical_file.frames:
        for channel in frame.channels:
            if channel.fingerprint != fp: continue
            frames.append(frame.attic.name)

    return frames
